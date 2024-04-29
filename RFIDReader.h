#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <ESP8266WiFi.h>
#include <MFRC522.h>

class RFIDReader {
public:
    RFIDReader(byte ssPin, byte rstPin);
    void setup();
    void loop();
    void byteToCharArray(byte *byteArray, byte arraySize, char *charArray);

private:
    byte _ssPin, _rstPin;
    MFRC522 _mfrc522;
    WiFiServer _server;
    unsigned long _readTime;
    char _UID[10];
    int _cardRead;
};

#endif
