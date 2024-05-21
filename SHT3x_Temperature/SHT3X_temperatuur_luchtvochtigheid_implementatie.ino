#include <Wire.h>
#include <Adafruit_SHT31.h>

// Maak een instantie van de SHT31 sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const int ledPin = D0; // Definieer de LED pin

void setup() {
  Serial.begin(115200); // Start de seriële communicatie met 115200 baud
  pinMode(ledPin, OUTPUT); // Stel de LED pin in als output

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
    
    // LED logica gebaseerd op temperatuur en luchtvochtigheid
    if ((temperature >= 27 && temperature < 30) || (humidity >= 60 && humidity < 70)) {
      analogWrite(ledPin, 64); // 25% helderheid
    } else if (temperature >= 30 || humidity >= 70) {
      analogWrite(ledPin, 255); // 100% helderheid
    } else {
      analogWrite(ledPin, 0); // LED uit
    }
  } else {
    Serial.println("Fout bij het lezen van de SHT31 sensor!");
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
