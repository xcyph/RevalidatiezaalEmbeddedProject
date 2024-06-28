#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

double upper_threshold = 27.0;
double lower_threshold = 26.0;

void *handle_client(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    free(client_socket_ptr);

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        close(client_socket);
        pthread_exit(NULL);
    }
    buffer[bytes_received] = '\0';

    char client_type[20] = "Onbekend";
    if (strstr(buffer, "TEMP") != NULL) {
        strcpy(client_type, "Temperatuursensor");
    } else if (strstr(buffer, "SERVO") != NULL) {
        strcpy(client_type, "Servo");
    }

    printf("%s verbonden van socket %d\n", client_type, client_socket);

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Ontvangen van %s: %s\n", client_type, buffer);

        if (strncmp(buffer, "TEMP:", 5) == 0) {
            double temp = atof(buffer + 5);
            char response[BUFFER_SIZE];

            if (temp > upper_threshold) {
                strcpy(response, "SERVO:180\n");
            } else if (temp < lower_threshold) {
                strcpy(response, "SERVO:0\n");
            } else {
                strcpy(response, "SERVO:NEUTRAL\n");
            }

            send(client_socket, response, strlen(response), 0);
        }
    }

    if (bytes_received < 0) {
        perror("recv");
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server luistert op poort %d\n", PORT);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, client_socket_ptr) != 0) {
            perror("pthread_create");
            close(client_socket);
            free(client_socket_ptr);
        }

        pthread_detach(client_thread); // Zorg ervoor dat resources worden vrijgegeven na thread beëindiging
    }

    close(server_socket);
    return 0;
}
