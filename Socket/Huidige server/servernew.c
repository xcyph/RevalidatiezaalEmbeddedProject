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
#define TX_BUFFERSIZELCD 8
//#define RX_BUFFERSIZE 2

char bufferDeur[5] = "open";
char bufferLCD[8];

void stuurnaarLCD(char hartslag, char zuurstof){
    int fd = open("/dev/i2c-1",O_RDWR);
    ioctl(fd,I2C_SLAVE,I2C_ADDR_LCD);
    sprintf(bufferLCD, "%d %d", hartslag, zuurstof);
    write(fd, bufferLCD, TX_BUFFERSIZELCD);
    printf("LCD sturen\n");
}

void openDeur() {
    int a = 0;
	int fd = open("/dev/i2c-1",O_RDWR);
	ioctl(fd,I2C_SLAVE,I2C_ADDR_DEUR);
    write(fd,bufferDeur,TX_BUFFERSIZEDEUR);
    printf("Deur open\n");
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address,
        sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
while (1) {
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
    }

    int valread = read(new_socket, buffer, 1024 - 1);
    buffer[valread] = '\0'; // Null-terminate de buffer

    if (valread > 0) {
        int hartslag;
            int zuurstof;
            int encoder;
        printf("%s\n", buffer);
        if (sscanf(buffer, "Hartslag %d Zuurstof %d", &hartslag, &zuurstof) == 2)
        stuurnaarLCD(hartslag, zuurstof);
            //data naat txtfile
        if (strstr(buffer, "Hartslag") != NULL){
            FILE *fl = fopen("Data.txt", "w");
            if (fl == NULL){
                perror("File openen mislukt.");
                exit(EXIT_FAILURE);
            }
            fprintf(fl, "%s", buffer);
            fflush(fl);
            
            //int hartslag;
            //double zuurstof;
            //int encoder;
            // Scan de buffer voor verwachte data
            if (sscanf(buffer, "Hartslag %d Zuurstof %lf Encoder %d", &hartslag, &zuurstof, &encoder) == 3) {
                printf("Hartslag: %d\n", hartslag);
                printf("Zuurstof: %.1f\n", zuurstof);
                printf("Encoder: %d\n", encoder);
                
            // Hier zou je de waarden kunnen opslaan of verder verwerken
            }
            fclose(fl);
        } 
        //else if(strstr(buffer, "RFID"){//afhandeling RFID
            else if (strstr(buffer, "F7A1061B")){
               // strcpy(bufferDeur, "open");
                openDeur();
            }
           /* int RFID;
            if (sscanf(buffer, "RFID %d ", &RFID) == 1){
                if (strstr(RFID, "F7A1061B"){  //F7A1061B
                    bufferDeur = "open"
                    opendeur();
                    bufferDeur = 
                }
            }
        */
        
        }
       

        close(new_socket);
    }
    
    close(server_fd);
    return 0;
}
