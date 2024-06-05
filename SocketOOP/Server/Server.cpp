#include "ServerHeader.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

// Constructor om de server in te stellen
Server::Server() {
    memset(bufferDeur_, 0, TX_BUFFERSIZEDEUR);
    strcpy(bufferDeur_, "open");
    memset(bufferLCD_, 0, TX_BUFFERSIZELCD);
    memset(RX_Buffer_, 0, RX_BUFFERSIZE);
}

// Destructor om de server te sluiten
Server::~Server() {
    close(server_fd_);
}

// Stel de server in
void Server::setupServer() {
    int opt = 1;
    socklen_t addrlen = sizeof(address_);

    if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt\n");
        exit(EXIT_FAILURE);
    }

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(PORT);

    if (bind(server_fd_, (struct sockaddr*)&address_, sizeof(address_)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);
    }
}

// Start de server
void Server::run() {
    setupServer();

    while (true) {
        if (listen(server_fd_, 3) < 0) {
            perror("listen\n");
            exit(EXIT_FAILURE);
        }

        if ((new_socket_ = accept(server_fd_, (struct sockaddr*)&address_, &addrlen)) < 0) {
            perror("accept\n");
            exit(EXIT_FAILURE);
        }

        handleClient();
        close(new_socket_);
    }
}

// Verwerk inkomende client verbindingen
void Server::handleClient() {
    int valread = read(new_socket_, buffer_, 1024 - 1);
    buffer_[valread] = '\0'; // Null-terminate de buffer
    std::cout << "Data ontvangen: " << buffer_ << std::endl; // Print de buffer

    // Kijk wat er is ontvangen
    if (valread > 0) {
        // RFID data
        if (strstr(buffer_, "F7A1061B")) {
            openDeur();
            std::cout << std::endl;
        }
        
        int hartslag, zuurstof, encoder;
        // Pulsoxy en encoder data
        if (strncmp("Hartslag", buffer_, 4) == 0) {
            sscanf(buffer_, "Hartslag %d Zuurstof %d Encoder %d", &hartslag, &zuurstof, &encoder);
            stuurnaarLCD(hartslag, zuurstof, encoder);
            std::cout << std::endl;
        }
    }
}

// Stuur data naar LCD
void Server::stuurnaarLCD(int hartslag, int zuurstof, int encoder) {
    snprintf(bufferLCD_, TX_BUFFERSIZELCD, "%d %d %d", hartslag, zuurstof, encoder);
    std::cout << "Naar LCD verzenden..." << std::endl;
    int fd = open("/dev/i2c-1", O_RDWR);
    ioctl(fd, I2C_SLAVE, I2C_ADDR_LCD);
    write(fd, bufferLCD_, TX_BUFFERSIZELCD);
    close(fd);
}

// Open de deur
void Server::openDeur() {
    std::cout << "Deur openen..." << std::endl;
    int fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        perror("bus niet geopend\n");
        exit(1);
    }
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR_DEUR) < 0) {
        perror("niet verbinding kunnen maken met 0x20\n");
        exit(1);
    }
    if (write(fd, bufferDeur_, TX_BUFFERSIZEDEUR) != TX_BUFFERSIZEDEUR) {
        perror("niet geschreven\n");
    }
    read(fd, RX_Buffer_, RX_BUFFERSIZE);
    std::cout << RX_Buffer_ << std::endl;
    close(fd);
}
