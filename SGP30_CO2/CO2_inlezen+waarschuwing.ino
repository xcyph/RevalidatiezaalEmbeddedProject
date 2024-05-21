#include <Wire.h>
#include <Adafruit_SGP30.h>

// Maak een instantie van de SGP30 sensor
Adafruit_SGP30 sgp;

const int ledPin = D0; // Pin waar de LED op is aangesloten
const int CO2_THRESHOLD = 400; // Drempelwaarde voor CO2 in ppm

void setup() {
  Serial.begin(115200); // Start de seriële communicatie met 115200 baud
  
  pinMode(ledPin, OUTPUT); // Stel de LED pin in als output

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

    // Controleer of de eCO2 waarde boven de drempelwaarde komt
    if (sgp.eCO2 > CO2_THRESHOLD) {
      Serial.println("Waarschuwing: CO2-niveau is te hoog!");
      digitalWrite(ledPin, HIGH); // Zet de LED aan
    } else {
      digitalWrite(ledPin, LOW); // Zet de LED uit
    }
  }

  delay(2000); // Wacht 2 seconden voor de volgende meting
}
