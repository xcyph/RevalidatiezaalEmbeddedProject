#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address,
        sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
while (1) {
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int valread = read(new_socket, buffer, 1024 - 1);
        buffer[valread] = '\0'; // Null-terminate de buffer

        if (valread > 0) {
            int hartslag;
            double zuurstof;
            int encoder;
            // Scan de buffer voor verwachte data
            if (sscanf(buffer, "Hartslag %d Zuurstof %lf Encoder %d", &hartslag, &zuurstof, &encoder) == 3) {
                printf("Hartslag: %d\n", hartslag);
                printf("Zuurstof: %.1f\n", zuurstof);
                printf("Encoder: %d\n", encoder);
                // Hier zou je de waarden kunnen opslaan of verder verwerken
            }
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
