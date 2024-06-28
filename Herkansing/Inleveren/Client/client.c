#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#define PORT 8080

int connectToServer(const char* ip) {
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
    printf("Connected to %s.\n", ip);
    return client_fd;
}

void sendDataToServer(const char* serverIp, const char* data) {
    int client_fd = connectToServer(serverIp);
    if (client_fd < 1) return;

    send(client_fd, data, strlen(data), 0);
    printf("Verzonden: %s\n", data);
    close(client_fd);
}

void readSensorData(char* buffer, int bufferSize) {
    int serial_fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY);
    if (serial_fd < 0) {
        printf("Error opening serial port\n");
        return;
    }

    int bytesRead = read(serial_fd, buffer, bufferSize - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
    } else {
        strcpy(buffer, "GeenData");
    }
    close(serial_fd);
}

int main() {
    const char* serverIp = "10.42.0.1";
    char sensorData[50];
    int dataSize = 50;

    while (1) {
        printf("Temperatuur data opvragen...\n");
        readSensorData(sensorData, dataSize);
        if (strcmp(sensorData, "GeenData") != 0) {
            sendDataToServer(serverIp, sensorData);
        }

        sleep(2);
    }
    return 0;
}
