#include <Servo.h>

// Maak een Servo object aan
Servo myservo;

void setup() {
  Serial.begin(115200); // Start seriële communicatie op 115200 baud
  myservo.attach(D4);   // Verbind de servo met pin D4 op de WeMos
  Serial.println("Servo klaar");
}

void loop() {
  // Draai de servo naar 0 graden
  myservo.write(0);
  Serial.println("Servo op 0 graden");
  delay(1000); // Wacht 1 seconde

  // Draai de servo naar 90 graden
  myservo.write(90);
  Serial.println("Servo op 90 graden");
  delay(1000); // Wacht 1 seconde

  // Draai de servo naar 180 graden
  myservo.write(180);
  Serial.println("Servo op 180 graden");
  delay(1000); // Wacht 1 seconde
}
