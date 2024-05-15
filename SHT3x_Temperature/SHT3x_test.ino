#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT3x.h>

ArtronShop_SHT3x sht3x(0x44, &Wire); // ADDR: 0 => 0x44, ADDR: 1 => 0x45

const int ledPin = D0; 

void setup() 
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  Wire.begin();
  while (!sht3x.begin()) {
    Serial.println("SHT3x not found !");
    delay(1000);
  }
}

void loop() 
{
  if (sht3x.measure()) {
    float temperature = sht3x.temperature();
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" *C\tHumidity: ");
    Serial.print(sht3x.humidity(), 1);
    Serial.print(" %RH");
    Serial.println();
    
    if (temperature >= 27 && temperature < 30) {
      analogWrite(ledPin, 64); // 25% brightness
    } else if (temperature >= 30) {
      analogWrite(ledPin, 255); // 100% brightness
    } else {
      analogWrite(ledPin, 0); // LED off
    }
  } else {
    Serial.println("SHT3x read error");
  }
  delay(1000);
}
