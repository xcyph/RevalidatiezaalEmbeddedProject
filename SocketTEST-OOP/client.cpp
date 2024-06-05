#include "Client.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

// Constructor om de client in te stellen
Client::Client() : client_fd_(-1), port_(0) {}

// Destructor om de client te sluiten
Client::~Client() {
    if (client_fd_ != -1) {
        close(client_fd_);
    }
}

// Verbind met de server
void Client::connectToServer(const std::string& serverIp, int port) {
    struct sockaddr_in serv_addr;
    serverIp_ = serverIp;
    port_ = port;

    if ((client_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, serverIp_.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed\n");
        exit(EXIT_FAILURE);
    }
}

// Verzend data naar de server
void Client::sendData(const std::string& data) {
    send(client_fd_, data.c_str(), data.length(), 0);
}

// Ontvang data van de server
std::string Client::receiveData() {
    char buffer[1024] = {0};
    int valread = read(client_fd_, buffer, 1024 - 1);
    buffer[valread] = '\0';
    return std::string(buffer);
}
