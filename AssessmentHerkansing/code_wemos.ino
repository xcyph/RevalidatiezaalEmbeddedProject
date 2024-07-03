#include <ESP8266WiFi.h>
#include <Servo.h>

const char* ssid = "revalidatieD-wifi";
const char* password = "";

Servo myservo;
const int tempThresholdHigh = 23; // Drempelwaarde voor hoge temperatuur
const int tempThresholdLow = 22;  // Drempelwaarde voor lage temperatuur

void setup() {
  Serial.begin(115200); // Seriële communicatie met STM32L432KC
  myservo.attach(D4);   // Verbind de servo met pin D4 op de WeMos D1 Mini

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbinden met WiFi...");
  }
  Serial.println("Verbonden met WiFi");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.println("Ontvangen: " + data);

    if (data.startsWith("TEMP:")) {
      float temperature = data.substring(5).toFloat();
      Serial.print("Gelezen temperatuur: ");
      Serial.println(temperature);

      if (temperature >= tempThresholdHigh) {
        myservo.write(180);
        Serial.println("Servo draait naar 180 graden");
      } else if (temperature <= tempThresholdLow) {
        myservo.write(0);
        Serial.println("Servo draait naar 0 graden");
      }
    } else {
      Serial.println("Onbekend commando ontvangen");
    }
  } else {
    Serial.println("Geen seriële gegevens beschikbaar");
  }

  delay(1000); // Kleine vertraging om seriële buffer te verwerken
}
