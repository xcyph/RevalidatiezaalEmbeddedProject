#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080
#define I2C_ADDR_LCD 0x10
#define I2C_ADDR_DEUR 0x20
#define TX_BUFFERSIZEDEUR 5
#define TX_BUFFERSIZELCD 13
#define RX_BUFFERSIZE 3

char bufferDeur[TX_BUFFERSIZEDEUR] = "open";
char bufferLCD[TX_BUFFERSIZELCD];
char RX_Buffer[RX_BUFFERSIZE];

/*! Open I2C bus en stel het slave-adres in */
int openI2CBus(const char* bus, int addr) {
    int fd = open(bus, O_RDWR);
    if (fd < 0) {
        perror("Bus niet geopend");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("Niet verbinding kunnen maken");
        close(fd);
        exit(EXIT_FAILURE);
    }
    return fd;
}

/*! Stuurt een string (hartslag + zuurstof) naar STM32 bordje met LCD */
void stuurnaarLCD(int fd, char hartslag, char zuurstof, char encoder) {
    snprintf(bufferLCD, TX_BUFFERSIZELCD, "%d %d %d", hartslag, zuurstof, encoder);
    printf("Naar LCD verzenden...\n");
    if (write(fd, bufferLCD, TX_BUFFERSIZELCD) != TX_BUFFERSIZELCD) {
        perror("Schrijven naar LCD mislukt");
    }
}

/*! Stuurt een string ("open") naar STM32 bordje met servo */
void openDeur(int fd) {
    printf("Deur openen...\n");
    if (write(fd, bufferDeur, TX_BUFFERSIZEDEUR) != TX_BUFFERSIZEDEUR) {
        perror("Niet geschreven");
    }
    if (read(fd, RX_Buffer, RX_BUFFERSIZE) > 0) {
        printf("%s\n", RX_Buffer);
    }
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    int i2c_fd_lcd = openI2CBus("/dev/i2c-1", I2C_ADDR_LCD);
    int i2c_fd_deur = openI2CBus("/dev/i2c-1", I2C_ADDR_DEUR);

    while (1) {
        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        int valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread > 0) {
            buffer[valread] = '\0'; // Null-terminate de buffer
            printf("Data ontvangen: %s\n", buffer); // Print de buffer

            if (strstr(buffer, "F7A1061B")) {
                openDeur(i2c_fd_deur);
            }
            else if (strncmp("Hartslag", buffer, 4) == 0) {
                int hartslag, zuurstof, encoder;
                sscanf(buffer, "Hartslag %d Zuurstof %d Encoder %d", &hartslag, &zuurstof, &encoder);
                stuurnaarLCD(i2c_fd_lcd, hartslag, zuurstof, encoder);
            }
        }
        close(new_socket);
    }

    close(i2c_fd_lcd);
    close(i2c_fd_deur);
    close(server_fd);
    return 0;
}
