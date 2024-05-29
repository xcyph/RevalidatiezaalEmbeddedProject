#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT3x.h>
#include <Adafruit_SGP30.h>
#include <ESP8266WiFi.h>

#define TXBUFFER_SIZE 49
#define WIFI_SSID "revalidatieD-wifi"
#define WIFI_PASS ""

ArtronShop_SHT3x sht3x(0x44, &Wire); // ADDR: 0 => 0x44, ADDR: 1 => 0x45

Adafruit_SGP30 sgp30;

const int ledPin = D0;
const int CO2_THRESHOLD = 1200;
char txBuffer[TXBUFFER_SIZE];
int tijd = 0;

int co2Val = 0;
float temperature = 0, humidity = 0;

// Wifi poort instellen
const int serverPort = 8080;
WiFiServer server(serverPort);

/*! Maakt verbinding met een wifi-netwerk, wordt eenmalig uitgevoerd bij het opstarten */
void wifiStart();

/*! Lees de co2 sensor uit */
void co2Read();

/*! Lees de temperatuur en luchtvochtigheid sensor */
void tempHumRead();

void setup() 
{
  Serial.begin(115200); 
  Wire.begin();
  wifiStart();

  pinMode(ledPin, OUTPUT);
  
  while (!sht3x.begin()) {
    Serial.println("SHT3x sensor niet gevonden!");
    delay(1000);
  }
  while (!sgp30.begin()) {
    Serial.println("SGP30 sensor niet gevonden!");
    delay(1000);
  }
  tijd = millis();
}

void loop() 
{
  if ((millis() - tijd) > 100) {
    co2Read();
    tempHumRead();
    tijd = millis();
  }
  snprintf(txBuffer, TXBUFFER_SIZE, "Temperatuur %2.1f Luchtvochtigheid %2.1f eCO2 %4d", temperature, humidity, co2Val);
  // Check voor een nieuwe client
  WiFiClient client = server.available();
  // Als er een client verbonden is:
  if (client) {
    client.println(txBuffer);
    // Sluit de verbinding met de client
    client.stop();
    Serial.println("Client verbinding verbroken");
  }
  // if ((temperature >= 27 && temperature < 30) || (humidity >= 60 && humidity < 70)) {
  //   analogWrite(ledPin, 64); // 25% brightness
  // } else if ((temperature >= 30) || (humidity >= 70)) {
  //   analogWrite(ledPin, 255); // 100% brightness
  // } else if (sgp30.eCO2 > CO2_THRESHOLD) {
  //   Serial.println("Waarschuwing: CO2-niveau is te hoog!");
  //   analogWrite(ledPin, 255); // LED aan
  // } else {
  //   analogWrite(ledPin, 0); // LED uit
  // }
}

void co2Read() {
  co2Val = sgp30.eCO2;
  if (!sgp30.IAQmeasure()) {
    Serial.println("SGP30 read error");
  }
}

void tempHumRead() {
  temperature = sht3x.temperature();
  humidity = sht3x.humidity();
  if (!sht3x.measure()) {
    Serial.println("SHT3x read error");
  }
}

void wifiStart() {
  // Verbinden met wifi...
  Serial.print("Verbinding maken met: ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // Loop zolang er geen wifi connectie is
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  // Verbonden met wifi
  Serial.println();
  Serial.print("Verbonden! IP adres: ");
  Serial.println(WiFi.localIP());
  // Start de socketserver
  server.begin();
  Serial.println("Socketserver gestart");
}
