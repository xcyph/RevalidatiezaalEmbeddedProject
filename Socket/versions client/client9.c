#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int connectToServer(const char* ip) {
    int client_fd;
    struct sockaddr_in serv_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\nSocket creation error\n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported\n");
        close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return;
    }
    printf("Connected to %s.\n", ip);
    return client_fd;
}

void readData(const char* ip, char* data, int dataSize) {
    int client_fd = connectToServer(ip);
    if (client_fd < 0) return;

    while (1) {
        int bytes = read(client_fd, data, dataSize - 1);
        if (bytes > 0) {
            data[bytes] = '\0';
            break;
        }
    }
    printf("%s\n", data);
    close(client_fd);
}

void sendData(const char* ip, const char* data, int dataSize) {
    int client_fd = connectToServer(ip);
    if (client_fd < 0) return;

    send(client_fd, data, dataSize, 0);
    printf("Verzonden\n");
    close(client_fd);
}

void handleSensorData(const char* sensorIp, const char* serverIp, char* data, int dataSize) {
    readData(sensorIp, data, dataSize);
    if (strcmp(data, "GeenData")) {
        sendData(serverIp, data, dataSize);
    }
    printf("\n");
}

int main() {
    const char* ipServer = "10.42.0.1";
    const char* ipWemosRFID = "10.42.0.122";
    const char* ipWemosTemp = "10.42.0.94";
    const char* ipWemosPulsoxy = "10.42.0.73";
    char rfid[9]; // RFID is saved here
    int rfidSize = 9;
    char pulsoxy[39];
    int pulsoxySize = 39;
    char tempVocht[49];
    int tempVochtSize = 49;

    while (1) {
        printf("RFID data opvragen...\n");
        handleSensorData(ipWemosRFID, ipServer, rfid, rfidSize);

        printf("Pulsoxy data opvragen...\n");
        handleSensorData(ipWemosPulsoxy, ipServer, pulsoxy, pulsoxySize);

        printf("Temp vocht CO2 opvragen...\n");
        handleSensorData(ipWemosTemp, ipServer, tempVocht, tempVochtSize);

        usleep(1000000);
    }
    return 0;
}
