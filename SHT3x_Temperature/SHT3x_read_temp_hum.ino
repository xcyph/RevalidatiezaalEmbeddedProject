#include <Wire.h>
#include <Adafruit_SHT31.h>

// Maak een instantie van de SHT31 sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void setup() {
  Serial.begin(115200); // Start de seriële communicatie met 115200 baud
  if (!sht31.begin(0x44)) { // Start de sensor op het standaard I2C-adres 0x44
    Serial.println("Kon SHT31 sensor niet vinden!");
    while (1) delay(1); // Blijf in een lus als de sensor niet wordt gevonden
  } else {
    Serial.println("SHT31 sensor gevonden!");
  }
}

void loop() {
  float temperature = sht31.readTemperature(); // Lees de temperatuur
  float humidity = sht31.readHumidity(); // Lees de luchtvochtigheid

  if (!isnan(temperature) && !isnan(humidity)) { // Controleer of de waarden geldig zijn
    Serial.print("Temperatuur: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Luchtvochtigheid: "); Serial.print(humidity); Serial.println(" %");
  } else {
    Serial.println("Fout bij het lezen van de SHT31 sensor!");
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
