#include "Server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define TX_BUFFERSIZEDEUR 5
#define TX_BUFFERSIZELCD 13
#define RX_BUFFERSIZE 3
#define I2C_ADDR_LCD 0x10
#define I2C_ADDR_DEUR 0x20

Server::Server(int port) {
    memset(buffer_, 0, sizeof(buffer_));
    memset(bufferDeur_, 0, sizeof(bufferDeur_));
    memset(bufferLCD_, 0, sizeof(bufferLCD_));
    memset(RX_Buffer_, 0, sizeof(RX_Buffer_));

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port);

    setupServer();
}

Server::~Server() {
    close(server_fd_);
}

void Server::setupServer() {
    int opt = 1;

    if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd_, (struct sockaddr*)&address_, sizeof(address_)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd_, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::run() {
    socklen_t addrlen = sizeof(address_);
    while (1) {
        if ((new_socket_ = accept(server_fd_, (struct sockaddr*)&address_, &addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        handleClient();
        close(new_socket_);
    }
}

void Server::handleClient() {
    int valread = read(new_socket_, buffer_, 1024 - 1);
    buffer_[valread] = '\0';
    std::cout << "Data ontvangen: " << buffer_ << std::endl;

    if (valread > 0) {
        if (strstr(buffer_, "F7A1061B")) {
            openDeur();
        } else {
            int hartslag, zuurstof, encoder;
            if (sscanf(buffer_, "Hartslag %d Zuurstof %d Encoder %d", &hartslag, &zuurstof, &encoder) == 3) {
                stuurnaarLCD(hartslag, zuurstof, encoder);
            }
        }
    }
}

void Server::stuurnaarLCD(int hartslag, int zuurstof, int encoder) {
    sprintf(bufferLCD_, "%d %d %d", hartslag, zuurstof, encoder);
    std::cout << "Naar LCD verzenden..." << std::endl;
    int fd = open("/dev/i2c-1", O_RDWR);
    ioctl(fd, I2C_SLAVE, I2C_ADDR_LCD);
    write(fd, bufferLCD_, TX_BUFFERSIZELCD);
    close(fd);
}

void Server::openDeur() {
    std::cout << "Deur openen..." << std::endl;
    int fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        perror("Bus niet geopend");
        exit(1);
    }
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR_DEUR) < 0) {
        perror("Niet verbinding kunnen maken met 0x20");
        exit(1);
    }
    if (write(fd, bufferDeur_, TX_BUFFERSIZEDEUR) != TX_BUFFERSIZEDEUR) {
        perror("Niet geschreven");
    }
    read(fd, RX_Buffer_, RX_BUFFERSIZE);
    std::cout << RX_Buffer_ << std::endl;
    close(fd);
}
