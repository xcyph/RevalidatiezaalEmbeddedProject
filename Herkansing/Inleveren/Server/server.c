#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int connectToWeMos(const char* ip) {
    int client_fd;
    struct sockaddr_in serv_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\nSocket creation error\n");
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported\n");
        close(client_fd);
        return 0;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return 0;
    }
    printf("Connected to WeMos at %s.\n", ip);
    return client_fd;
}

void sendCommandToWeMos(const char* weMosIp, const char* command) {
    int client_fd = connectToWeMos(weMosIp);
    if (client_fd < 1) return;

    send(client_fd, command, strlen(command), 0);
    printf("Command sent to WeMos: %s\n", command);
    close(client_fd);
}

void handleTemperatureData(const char* data, const char* weMosIp) {
    float temperature;
    sscanf(data, "TEMP:%f", &temperature);
    if (temperature > 27.0) {
        sendCommandToWeMos(weMosIp, "SERVO:180");
    } else if (temperature < 26.0) {
        sendCommandToWeMos(weMosIp, "SERVO:0");
    } else {
        sendCommandToWeMos(weMosIp, "SERVO:90"); // Neutrale positie
    }
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    const char* weMosIp = "10.42.0.122";  // Vervang met het IP-adres van de WeMos D1 Mini

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        int valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread > 0) {
            buffer[valread] = '\0'; // Null-terminate de buffer
            printf("Data received: %s\n", buffer); // Print de buffer

            if (strncmp(buffer, "TEMP:", 5) == 0) {
                handleTemperatureData(buffer, weMosIp);
            }
        }
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
