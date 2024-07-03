#include <Wire.h>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();

//Misschien weghalen 
Wire.begin(D4,D5); //Initialiseer I2C op de juiste pinnnen

void setup() {
  Serial.begin(115200);

  if (!sht31.begin(0x44)) { // Start de sensor op het standaard I2C-adres 0x44
    Serial.println("Kon SHT31 sensor niet vinden!");
    while (1) delay(1); // Blijf in een lus als de sensor niet wordt gevonden
  } else {
    Serial.println("SHT31 sensor gevonden!");
  }
}

void loop() {
  float temperature = sht31.readTemperature(); // Lees de temperatuur

  if (!isnan(temperature)) {
    Serial.print("TEMP:");
    Serial.println(temperature); // Print de temperatuur met 2 decimalen
  } else {
    Serial.println("Fout bij het lezen van de SHT31 sensor!");
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
