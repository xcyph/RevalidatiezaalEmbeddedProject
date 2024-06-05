#include "ClientHeader.h"
#include <iostream>
#include <cstring>

int main() {
    // IP-adressen van de servers
    const std::string ipServer = "10.42.0.1";
    const std::string ipWemosRFID = "10.42.0.122";
    const std::string ipWemosTemp = "10.42.0.94";
    const std::string ipWemosPulsoxy = "10.42.0.73";

    char rfid[9]; // RFID wordt hier opgeslagen
    int rfidSize = 9;
    char pulsoxy[39]; // Pulsoxy data wordt hier opgeslagen
    int pulsoxySize = 39;
    char tempVocht[49]; // Temperatuur en vochtigheidsdata wordt hier opgeslagen
    int tempVochtSize = 49;

    // Client-objecten voor elke server
    Client serverClient(ipServer);
    Client wemosRFIDClient(ipWemosRFID);
    Client wemosTempClient(ipWemosTemp);
    Client wemosPulsoxyClient(ipWemosPulsoxy);

    while (true) {
        // Vraag RFID data op
        std::cout << "RFID data opvragen..." << std::endl;
        readRFID(wemosRFIDClient, rfid, rfidSize);
        if (strcmp(rfid, "GeenData")) {
            std::cout << "RFID code naar server sturen..." << std::endl;
            sendStringToServer(serverClient, rfid, rfidSize);
        }
        std::cout << std::endl;

        // Vraag Pulsoxy data op
        std::cout << "Pulsoxy data opvragen..." << std::endl;
        readPulsoxy(wemosPulsoxyClient, pulsoxy, pulsoxySize);
        if (strcmp(pulsoxy, "GeenData")) {
            std::cout << "Pulsoxy data naar server sturen..." << std::endl;
            sendStringToServer(serverClient, pulsoxy, pulsoxySize);
        }
        std::cout << std::endl;

        // Vraag temperatuur- en vochtigheidsdata op
        std::cout << "Temp vocht CO2 opvragen..." << std::endl;
        readTempVocht(wemosTempClient, tempVocht, tempVochtSize);
        if (strcmp(tempVocht, "GeenData")) {
            std::cout << "Temperatuur, vochtigheid en CO2 naar server sturen..." << std::endl;
            sendStringToServer(serverClient, tempVocht, tempVochtSize);
        }
        std::cout << std::endl;
        
        // Wacht 1 seconde voordat de volgende cyclus begint
        usleep(1000000);
    }

    return 0;
}
