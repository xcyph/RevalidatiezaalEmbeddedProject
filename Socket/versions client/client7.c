#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void collectDataFromWemos1(const char* wemosIP, const char* filePath) {
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    const char * message = "start meting";
    const char * message2 = "stop meting";
    char* woord = "end";

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, wemosIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        //close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        close(client_fd);
        return;
    }
    else{
        printf("Connected to %s.\n", wemosIP);
    }
    
    FILE* fp = fopen(filePath, "w");
    if (fp == NULL) {
        perror("Failed to open file");
        //close(client_fd);
        return;
    }
    send(client_fd, message, strlen(message), 0);
    while(1){
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0){
            buffer[bytes]= '\0';
            fprintf(fp, "%s", buffer);
            printf("%s", buffer);
        //memset(buffer, 0, sizeof(buffer));
            if (strstr(buffer, "end") !=NULL){
                break;
            }
        } 
        else {
            break;
        }
    }
    fclose(fp);
    close(client_fd);
}

void collectDataFromWemos2(const char* wemosIP, const char* filePath, char* iets) {
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    const char * message = "start meting";
    const char * message2 = "stop meting";


    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, wemosIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        //close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        close(client_fd);
        return;
    }
    else{
        printf("Connected to %s.\n", wemosIP);
    }
    
    if (read(client_fd, buffer, sizeof(buffer) - 1) > 0) {
        memcpy(iets, buffer, 9);
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
   else {
    fclose(fp);
    close(client_fd);
    }
}

void communicateWithServer(const char* serverIP, const char* filePath) {
    int client_fd;
    struct sockaddr_in serv_addr;
    FILE* fp = fopen(filePath, "r");
    FILE* rf = fopen("RFID.txt", "r");
    char buffer[1024] = { 0 };
    //char sendBuffer[1100] = { 0 }; // Extra space for the message label

    if (fp == NULL) {
        perror("Failed to open file");
        return;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\n Socket creation error \n");
        fclose(fp);
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        fclose(fp);
        close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        fclose(fp);
        close(client_fd);
        return;
    }
    else{
        printf("Connected to %s.\n", serverIP);
    }

    // Assume data from the file is plain and needs to be labeled before sending
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("Sending\n");
        //snprintf(sendBuffer, sizeof(sendBuffer), "Hartslag %s Zuurstof %s Encoder %s", buffer, buffer, buffer); // Modify this line based on actual format and data separation
        send(client_fd, buffer, strlen(buffer), 0);
    }

    fclose(fp);
    close(client_fd);
}

void sendStringToServer(const char* serverIP, char* data, int dataLength) {
    int client_fd;
    struct sockaddr_in serv_addr;
    
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("\n Socket creation error \n");
        fclose(fp);
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        fclose(fp);
        close(client_fd);
        return;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        fclose(fp);
        close(client_fd);
        return;
    }
    else {
        printf("Connected to %s.\n", serverIP);
    }
    
    printf("Sending\n");
    send(client_fd, buffer, strlen(buffer), 0);

    fclose(fp);
    close(client_fd);
}

int main(int argc, char const* argv[]) {
    char* file1 = "Pulsoxy_Encoder_data.txt";
    char* file2 = "RFID.txt";
    char* ipRasp = "127.0.0.1";//10.42.0.1
    char* ipWemosRFID = "10.42.0.122";
    char* ipwemos2 = "10.42.0.94";//pulsoxy
    char rfid[9];
    int rfidSize = 9;
    
    while (1) {
        printf("Start wemos\n");
        collectDataFromWemos2(ipWemosRFID, file2, rfid);
        printf("\nStart rasp pi\n\n");
        communicateWithServer(ipRasp, file1);
        //communicateWithServer(ipRasp, file2);
        printf("rfid: %s\n", rfid);
        usleep(1000000);
    }
    return 0;
}
