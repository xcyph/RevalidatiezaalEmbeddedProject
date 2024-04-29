#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void communicateWithServer(const char* serverIP, const char* message) {
    int client_fd, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    printf("Connected to %s.\n", serverIP);
    send(client_fd, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    while (1) {
        valread = read(client_fd, buffer, 1023); // Laat ruimte voor null-terminator
        buffer[valread] = '\0'; // Zorg ervoor dat de string correct wordt afgesloten

        if (valread <= 0 || strcmp(buffer, "end message") == 0) {
            break; // Stop met luisteren als er geen data is of het "end message" ontvangen is
        }

        printf("%s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    close(client_fd);
}

int main(int argc, char const* argv[]) {
    char* hello = "Requesting data \n";
    char* ipRasp = "10.42.0.1";
    char* ipwemos1 = "10.42.0.122";

    // Eerst communiceren met ipwemos1
    communicateWithServer(ipwemos1, hello);

    // Dan communiceren met ipRasp
    communicateWithServer(ipRasp, hello);

    return 0;
}
