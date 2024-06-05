#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <string>

class Server {
public:
    Server(int port);
    ~Server();
    void run();

private:
    int server_fd_;
    int new_socket_;
    struct sockaddr_in address_;
    char buffer_[1024];
    char bufferDeur_[5];
    char bufferLCD_[13];
    char RX_Buffer_[3];

    void setupServer();
    void handleClient();
    void stuurnaarLCD(int hartslag, int zuurstof, int encoder);
    void openDeur();
};

#endif // SERVER_H
