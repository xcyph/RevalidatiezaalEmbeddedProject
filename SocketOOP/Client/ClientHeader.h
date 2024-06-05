#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <string>
#include <unistd.h>

#define PORT 8080

class Client {
public:
    Client(const std::string& ip); // Constructor om het IP-adres in te stellen
    bool connectToServer(); // Verbind met de server
    bool readData(char* data, int dataSize); // Lees data van de server
    void sendData(const char* data, int dataSize); // Stuur data naar de server
    void closeConnection(); // Sluit de verbinding

private:
    std::string ip_; // IP-adres van de server
    int client_fd_; // Bestandsdescriptor voor de socket
    struct sockaddr_in serv_addr_; // Struct voor serveradres
};

// Functies om verschillende soorten gegevens op te halen en te verzenden
void readTempVocht(Client& client, char* data, int dataSize); // Lees temperatuur- en vochtigheidsdata
void readRFID(Client& client, char* dest, int destSize); // Lees RFID-gegevens
void readPulsoxy(Client& client, char* data, int dataSize); // Lees pulsoxymetrie-gegevens
void sendStringToServer(Client& client, char* data, int dataSize); // Stuur een string naar de server

#endif // CLIENT_H
