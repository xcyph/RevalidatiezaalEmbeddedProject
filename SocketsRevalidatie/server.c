#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
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

/*! Stuurt een string (hartslag + zuurstof) naar STM32 bordje met LCD */
void stuurnaarLCD(char hartslag, char zuurstof, char encoder) {
	sprintf(bufferLCD, "%d %d %d", hartslag, zuurstof, encoder);
	printf("Naar LCD verzenden...\n");
	int fd = open("/dev/i2c-1", O_RDWR);
	ioctl(fd, I2C_SLAVE, I2C_ADDR_LCD);
	write(fd, bufferLCD, TX_BUFFERSIZELCD);
}

/*! Stuurt een string ("open") naar STM32 bordje met servo */
void openDeur() {
	printf("Deur openen...\n");
	int a = 0;
	int fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0){
		perror("bus niet geopend\n");
		exit(1);
	}
	if (ioctl(fd, I2C_SLAVE, I2C_ADDR_DEUR) < 0) {
		perror("niet verbinding kunnen maken met 0x20\n");
		exit(1);
	}
	if (write(fd, bufferDeur, TX_BUFFERSIZEDEUR) != TX_BUFFERSIZEDEUR){
		perror("niet geschreven\n");
	}
	read(fd,RX_Buffer,RX_BUFFERSIZE);
	printf("%s\n",RX_Buffer);
}

int main(int argc, char const* argv[]) {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = { 0 };

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed\n");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt\n");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed\n");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
		if (listen(server_fd, 3) < 0) {
			perror("listen\n");
			exit(EXIT_FAILURE);
		}

		if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
			perror("accept\n");
			exit(EXIT_FAILURE);
		}

		int valread = read(new_socket, buffer, 1024 - 1);
		buffer[valread] = '\0'; // Null-terminate de buffer
		printf("Data ontvangen: %s\n", buffer); // Print de buffer

		// Kijk wat er is ontvangen
		if (valread > 0) {
			// RFID data
			if (strstr(buffer, "F7A1061B")) {
				openDeur();
				printf("\n");
			}
			
			int hartslag, zuurstof, encoder;
			// Pulsoxy en encoder data
			if (strncmp("Hartslag", buffer, 4) == 0) {
				sscanf(buffer, "Hartslag %d Zuurstof %d Encoder %d", &hartslag, &zuurstof, &encoder);
				stuurnaarLCD(hartslag, zuurstof, encoder);
				printf("\n");
			}
		}
		close(new_socket);
	}
	close(server_fd);
	return 0;
}
