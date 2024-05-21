#include <Wire.h>
#include <Adafruit_SGP30.h>

// Maak een instantie van de SGP30 sensor
Adafruit_SGP30 sgp;

void setup() {
  Serial.begin(115200); // Start de seriële communicatie met 115200 baud

  // Initialiseer de SGP30 sensor
  if (!sgp.begin()) {
    Serial.println("SGP30 sensor niet gevonden!");
    while (1) delay(1); // Blijf in een lus als de sensor niet wordt gevonden
  } else {
    Serial.println("SGP30 sensor gevonden!");
  }

  // Bereid de meting voor
  if (!sgp.IAQinit()) {
    Serial.println("SGP30 sensor kon niet worden geïnitialiseerd!");
    while (1) delay(1);
  }
}

void loop() {
  // Lees de luchtkwaliteit van de SGP30 sensor
  if (!sgp.IAQmeasure()) {
    Serial.println("Fout bij het lezen van de SGP30 sensor!");
  } else {
    Serial.print("eCO2: "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
