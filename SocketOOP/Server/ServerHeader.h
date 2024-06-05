#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <string>

#define PORT 8080
#define I2C_ADDR_LCD 0x10
#define I2C_ADDR_DEUR 0x20
#define TX_BUFFERSIZEDEUR 5
#define TX_BUFFERSIZELCD 13
#define RX_BUFFERSIZE 3

class Server {
public:
    Server(); // Constructor om de server in te stellen
    ~Server(); // Destructor om de server te sluiten
    void run(); // Start de server

private:
    int server_fd_, new_socket_;
    struct sockaddr_in address_;
    char buffer_[1024];
    char bufferDeur_[TX_BUFFERSIZEDEUR];
    char bufferLCD_[TX_BUFFERSIZELCD];
    char RX_Buffer_[RX_BUFFERSIZE];
    
    void setupServer(); // Stel de server in
    void handleClient(); // Verwerk inkomende client verbindingen
    void stuurnaarLCD(int hartslag, int zuurstof, int encoder); // Stuur data naar LCD
    void openDeur(); // Open de deur
};

#endif // SERVER_H
