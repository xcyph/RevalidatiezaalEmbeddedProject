#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT3x.h>
#include <Adafruit_SGP30.h>

ArtronShop_SHT3x sht3x(0x44, &Wire); // ADDR: 0 => 0x44, ADDR: 1 => 0x45

Adafruit_SGP30 sgp30;

const int ledPin = D0; 
const int CO2_THRESHOLD = 1200;

void setup() 
{
  Serial.begin(9600); 
  Wire.begin();

  pinMode(ledPin, OUTPUT);
  
  while (!sht3x.begin()) {
    Serial.println("SHT3x sensor niet gevonden!");
    delay(1000);
  }

  while (!sgp30.begin()) {
    Serial.println("SGP30 sensor niet gevonden!");
    delay(1000);
  } 

  // if (!sgp30.IAQinit()) {
  //   Serial.println("SGP30 sensor kon niet worden geïnitialiseerd!");
  //   while (1) delay(1);
  // }
}

void loop() 
{  
  float temperature = sht3x.temperature();
  float humidity = sht3x.humidity();

  if (!sht3x.measure()) {
    Serial.println("SHT3x read error");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" *C\tHumidity: ");
    Serial.print(humidity, 1);
    Serial.print(" %RH\t");
  }

  if (!sgp30.IAQmeasure()) {
    Serial.println("SGP30 read error");
  } else {    
    Serial.print("eCO2: ");
    Serial.print(sgp30.eCO2); 
    Serial.print(" PPM");
    Serial.println();
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

  delay(1000);
}
