// Client side C program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Pulsoxy\n";
	char* ipRasp = "10.42.0.1";
	char* ipwemos1 = "10.42.0.122"; 
	char buffer[1024] = { 0 };
		
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "10.42.0.122", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	printf("Connected.\n\r" );
	while(1){	
	send(client_fd, hello, strlen(hello), 0);
	printf("Requesting data\n");
	valread = read(client_fd, buffer, 1024 - 1); // subtract 1 for the null
	// terminator at the end
	if (valread <= 0){
		break;
	}
	printf("%s", buffer);
	memset(buffer, 0, sizeof(buffer));
	//sleep(1);
}
	// closing the connected socket
	close(client_fd);
	return 0;
}
