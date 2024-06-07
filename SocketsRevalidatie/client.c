#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void readTempVocht(const char* ip, char* data, int dataSize) {
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
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return;
    }
    else printf("Connected to %s.\n", ip);
    
    while(1) {
        int bytes = read(client_fd, data, dataSize - 1);
        if (bytes > 0) {
            data[bytes]= '\0';
            break;
        }
    }
    printf("%s\n", data);
    close(client_fd);
}

/*! Leest de RFID data van de betreffende Wemos uit en sluit vervolgens de connectie */
void readRFID(const char* ip, char* dest, int destSize) {
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
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return;
    }
    else printf("Connected to %s.\n", ip);
    
    if (read(client_fd, dest, destSize - 1) > 0) {
        dest[8] = '\0';
        printf("%s\n", dest);
    }
    else {
        close(client_fd);
    }
}

/*! Leest de Pulsoxy data van de betreffende Wemos uit en sluit vervolgens de connectie */
void readPulsoxy(const char* ip, char* data, int dataSize) {
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
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return;
    }
    else printf("Connected to %s.\n", ip);

    while(1) {
        int bytes = read(client_fd, data, dataSize - 1);
        if (bytes > 0) {
            data[bytes]= '\0';
            break;
        }
    }
    
    printf("%s\n", data);
    close(client_fd);
}

/*! Stuurt een string naar de server en sluit vervolgens de connectie */
void sendStringToServer(const char* ip, char* data, int dataSize) {
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
        printf("\nInvalid address / Address not supported \n");
        close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        close(client_fd);
        return;
    }
    else printf("Connected to %s.\n", ip);
    
    send(client_fd, data, dataSize, 0);
    printf("Verzonden\n");
    
    close(client_fd);
}

int main() {
    char* ipServer = "10.42.0.1";
    char* ipWemosRFID = "10.42.0.122";
    char* ipWemosTemp = "10.42.0.94";
    char* ipWemosPulsoxy = "10.42.0.73";
    char rfid[9]; // RFID is saved here
    int rfidSize = 9;
    char pulsoxy[39];
    int pulsoxySize = 39;
    char tempVocht[49];
    int tempVochtSize = 49;
    
    while (1) {
        printf("RFID data opvragen...\n");
        readRFID(ipWemosRFID, rfid, rfidSize);
        if (strcmp(rfid, "GeenData")) {
            printf("RFID code naar server sturen...\n");
            sendStringToServer(ipServer, rfid, rfidSize);
        }
        printf("\n");
        
        printf("Pulsoxy data opvragen...\n");
        readPulsoxy(ipWemosPulsoxy, pulsoxy, pulsoxySize);
        if (strcmp(pulsoxy, "GeenData")) {
            printf("Pulsoxy data naar server sturen...\n");
            sendStringToServer(ipServer, pulsoxy, pulsoxySize);
        }
        printf("\n");
        
        printf("Temp vocht CO2 opvragen...\n");
        readTempVocht(ipWemosTemp, tempVocht, tempVochtSize);
        if (strcmp(tempVocht, "GeenData")) {
            printf("Temperatuur, vochtigheid en CO2 naar server sturen...\n");
            sendStringToServer(ipServer, tempVocht, tempVochtSize);
        }
        printf("\n");
        usleep(1000000);
    }
    return 0;
}
