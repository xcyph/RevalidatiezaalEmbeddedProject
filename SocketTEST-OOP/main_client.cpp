#include "client.h"
using namespace

int main() {
    Client client;
    client.connectToServer("127.0.0.1", 8080);
    client.sendData("Hartslag 75 Zuurstof 98 Encoder 2");
    std::string response = client.receiveData();
    std::cout << "Response from server: " << response << std::endl;
    return 0;
}
