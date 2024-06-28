#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <WiFi.h>

const char* ssid = "revalidatieD-wifi";
const char* password = "";
const char* server_ip = "192.168.178.226"; // IP van de Raspberry Pi server
const int server_port = 9999;

WiFiClient client;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbinden met WiFi...");
  }
  Serial.println("Verbonden met WiFi");

  if (!sht31.begin(0x44)) { // Start de sensor op het standaard I2C-adres 0x44
    Serial.println("Kon SHT31 sensor niet vinden!");
    while (1) delay(1); // Blijf in een lus als de sensor niet wordt gevonden
  } else {
    Serial.println("SHT31 sensor gevonden!");
  }
}

void loop() {
  if (client.connect(server_ip, server_port)) {
    // Stuur identificerende boodschap
    client.println("TEMP_SENSOR");

    float temperature = sht31.readTemperature(); // Lees de temperatuur

    if (!isnan(temperature)) {
      Serial.print("Temperatuur: "); Serial.print(temperature); Serial.println(" °C");
      String tempString = "TEMP:" + String(temperature);
      client.println(tempString);
      Serial.println("Verzonden: " + tempString);
    } else {
      Serial.println("Fout bij het lezen van de SHT31 sensor!");
    }

    client.stop();
  } else {
    Serial.println("Verbinding met server mislukt");
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
