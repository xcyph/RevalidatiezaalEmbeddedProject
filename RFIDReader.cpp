#include "RFIDReader.h"
#include <SPI.h>

RFIDReader::RFIDReader(byte ssPin, byte rstPin) : _ssPin(ssPin), _rstPin(rstPin), _mfrc522(ssPin, rstPin), _server(8080), _cardRead(0) {}

void RFIDReader::setup() {
    pinMode(16, OUTPUT);
    Serial.begin(115200);
    SPI.begin();
    _mfrc522.PCD_Init();

    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    _server.begin();
    Serial.println("Server started");
}

void RFIDReader::loop() {
    if (_cardRead == 1 && millis() - _readTime > 1000) {
        _cardRead = 0;
        digitalWrite(16, 0);
    }
    if (_cardRead == 0) {
        if (_mfrc522.PICC_IsNewCardPresent() && _mfrc522.PICC_ReadCardSerial()) {
            byteToCharArray(_mfrc522.uid.uidByte, _mfrc522.uid.size, _UID);
            Serial.print(F("Card UID: "));
            Serial.println(_UID);

            _cardRead = 1;
            digitalWrite(16, 1);
            _readTime = millis();
        }
    }

    WiFiClient client = _server.available();
    if (client) {
        Serial.println("New client connected");
        if (_cardRead == 0) {
            client.println("Geen Data");
            Serial.println("Data verstuurd: Geen Data");
        } else {
            client.println(_UID);
            Serial.print(F("Data verstuurd: "));
            Serial.println(_UID);
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}

void RFIDReader::byteToCharArray(byte *byteArray, byte arraySize, char *charArray) {
    for (byte i = 0; i < arraySize; i++) {
        sprintf(charArray + i * 2, "%02X", byteArray[i]);
    }
    charArray[arraySize * 2] = '\0';
}
#include "RFIDReader.h"
#include <SPI.h>

RFIDReader::RFIDReader(byte ssPin, byte rstPin) : _ssPin(ssPin), _rstPin(rstPin), _mfrc522(ssPin, rstPin), _server(8080), _cardRead(0) {}

void RFIDReader::setup() {
    pinMode(16, OUTPUT);
    Serial.begin(115200);
    SPI.begin();
    _mfrc522.PCD_Init();

    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    _server.begin();
    Serial.println("Server started");
}

void RFIDReader::loop() {
    if (_cardRead == 1 && millis() - _readTime > 1000) {
        _cardRead = 0;
        digitalWrite(16, 0);
    }
    if (_cardRead == 0) {
        if (_mfrc522.PICC_IsNewCardPresent() && _mfrc522.PICC_ReadCardSerial()) {
            byteToCharArray(_mfrc522.uid.uidByte, _mfrc522.uid.size, _UID);
            Serial.print(F("Card UID: "));
            Serial.println(_UID);

            _cardRead = 1;
            digitalWrite(16, 1);
            _readTime = millis();
        }
    }

    WiFiClient client = _server.available();
    if (client) {
        Serial.println("New client connected");
        if (_cardRead == 0) {
            client.println("Geen Data");
            Serial.println("Data verstuurd: Geen Data");
        } else {
            client.println(_UID);
            Serial.print(F("Data verstuurd: "));
            Serial.println(_UID);
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}

void RFIDReader::byteToCharArray(byte *byteArray, byte arraySize, char *charArray) {
    for (byte i = 0; i < arraySize; i++) {
        sprintf(charArray + i * 2, "%02X", byteArray[i]);
    }
    charArray[arraySize * 2] = '\0';
}
