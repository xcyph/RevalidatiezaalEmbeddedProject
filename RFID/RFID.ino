#include "MFRC522.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

#define RST_PIN 0 // RST-PIN for RC522 - RFID - SPI - Modul GPIO5
#define SS_PIN 15 // SDA-PIN for RC522 - RFID - SPI - Modul GPIO4
#define LED_PIN 16

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Set WiFi credentials
#define WIFI_SSID "revalidatieD-wifi"
#define WIFI_PASS ""
const int serverPort = 8080;

WiFiServer server(serverPort);

int cardRead = 0;
unsigned long readTime;
char UID[10];

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Start de socketserver
  server.begin();
  Serial.println("Server started");
}

void loop() {
  if (cardRead == 1 && millis() - readTime > 1000) {
    cardRead = 0;
    digitalWrite(LED_PIN, 0);
  }
  if (cardRead == 0) {
    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent()) {
      // Select one of the cards
      if (mfrc522.PICC_ReadCardSerial()) {
        // Print UID van de kaart
        byteToCharArray(mfrc522.uid.uidByte, mfrc522.uid.size, UID);
        Serial.print(F("Card UID: "));
        Serial.println(UID);

        cardRead = 1;
        digitalWrite(LED_PIN, 1);
        readTime = millis();
      }
    }
  }

  // Wacht op een nieuwe client
  WiFiClient client = server.available();

  // Stuur data naar client
  if (client) {
    Serial.println("New client connected");

    if (cardRead == 0) {
      client.println("Geen Data");
      Serial.println("Data verstuurd: Geen Data");
    }
    else {
      client.println(UID);
      Serial.print(F("Data verstuurd: "));
      Serial.println(UID);
    }

    // Sluit de verbinding met de client
    client.stop();
    Serial.println("Client disconnected");
  }
}

// Zet een byte array om naar een char array [ChatGPT]
void byteToCharArray(byte *byteArray, byte arraySize, char *charArray) {
  for (byte i = 0; i < arraySize; i++) {
    // Elke byte omzetten naar twee karakters in het hexadecimale formaat
    sprintf(charArray + i * 2, "%02X", byteArray[i]);
  }
  // Voeg een nulterminator toe aan het einde van de char-array
  charArray[arraySize * 2] = '\0';
}
