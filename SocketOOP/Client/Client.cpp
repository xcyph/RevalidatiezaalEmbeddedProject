#include "ClientHeader.h"
#include <iostream>
#include <cstring>

// Constructor om het IP-adres in te stellen en de sockaddr_in structuur voor te bereiden
Client::Client(const std::string& ip) : ip_(ip), client_fd_(-1) {
    memset(&serv_addr_, 0, sizeof(serv_addr_));
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(PORT);
}

// Verbind met de server
bool Client::connectToServer() {
    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd_ < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    if (inet_pton(AF_INET, ip_.c_str(), &serv_addr_.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        close(client_fd_);
        return false;
    }

    if (connect(client_fd_, (struct sockaddr*)&serv_addr_, sizeof(serv_addr_)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        close(client_fd_);
        return false;
    }
    std::cout << "Connected to " << ip_ << std::endl;
    return true;
}

// Lees data van de server
bool Client::readData(char* data, int dataSize) {
    int bytes = read(client_fd_, data, dataSize - 1);
    if (bytes > 0) {
        data[bytes] = '\0';
        return true;
    }
    return false;
}

// Stuur data naar de server
void Client::sendData(const char* data, int dataSize) {
    send(client_fd_, data, dataSize, 0);
    std::cout << "Data sent" << std::endl;
}

// Sluit de verbinding
void Client::closeConnection() {
    close(client_fd_);
}

// Functie om temperatuur- en vochtigheidsdata te lezen
void readTempVocht(Client& client, char* data, int dataSize) {
    if (client.connectToServer()) {
        while (true) {
            if (client.readData(data, dataSize)) {
                break;
            }
        }
        std::cout << data << std::endl;
        client.closeConnection();
    }
}

// Functie om RFID-gegevens te lezen
void readRFID(Client& client, char* dest, int destSize) {
    if (client.connectToServer()) {
        if (client.readData(dest, destSize)) {
            dest[8] = '\0';
            std::cout << dest << std::endl;
        }
        client.closeConnection();
    }
}

// Functie om pulsoxymetrie-gegevens te lezen
void readPulsoxy(Client& client, char* data, int dataSize) {
    if (client.connectToServer()) {
        while (true) {
            if (client.readData(data, dataSize)) {
                break;
            }
        }
        std::cout << data << std::endl;
        client.closeConnection();
    }
}

// Functie om een string naar de server te sturen
void sendStringToServer(Client& client, char* data, int dataSize) {
    if (client.connectToServer()) {
        client.sendData(data, dataSize);
        client.closeConnection();
    }
}
