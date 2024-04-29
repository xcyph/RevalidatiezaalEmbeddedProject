#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void collectDataFromWemos(const char* wemosIP, const char* filePath) {
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

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
    
    FILE* fp = fopen(filePath, "a");
    if (fp == NULL) {
        perror("Failed to open file");
        //close(client_fd);
        return;
    }

    if (read(client_fd, buffer, sizeof(buffer) - 1) > 0) {
        fprintf(fp, "%s", buffer);
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

int main(int argc, char const* argv[]) {
    char* file1 = "Pulsoxy_Encoder_data.txt";
    char* file2 = "RFID.txt";
    char* ipRasp = "10.42.0.1";
    char* ipwemos1 = "10.42.0.122";//RFID
    char* ipwemos2 = "10.42.0.94";//pulsoxy
    
    while (1){
    printf("Start wemos\n");
    collectDataFromWemos(ipwemos1, file2);
    printf("\nStart rasp pi\n\n");
    communicateWithServer(ipRasp, file1);
}
    return 0;
}
