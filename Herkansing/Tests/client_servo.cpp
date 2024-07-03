#include <Servo.h>
#include <ESP8266WiFi.h>

const char* ssid = "revalidatieD-wifi";
const char* password = "";
const char* server_ip = "192.168.178.226"; // IP van de Raspberry Pi server
const int server_port = 8088;

WiFiClient client;
Servo myservo;

void setup() {
  Serial.begin(115200);
  myservo.attach(D4);   // Verbind de servo met pin D4 op de WeMos D1 Mini

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbinden met WiFi...");
  }
  Serial.println("Verbonden met WiFi");
}

void loop() {
  if (client.connect(server_ip, server_port)) {
    // Wacht op commando van de server
    while (client.connected()) {
      if (client.available() > 0) {
        String response = client.readStringUntil('\n');
        Serial.println("Ontvangen: " + response);

        if (response.startsWith("SERVO:")) {
          if (response == "SERVO:180") {
            myservo.write(180);
            Serial.println("Servo draait naar 180 graden");
          } else if (response == "SERVO:0") {
            myservo.write(0);
            Serial.println("Servo draait naar 0 graden");
          } else {
            Serial.println("Servo blijft in de huidige positie");
          }
        }
      }
    }
    client.stop();
  } else {
    Serial.println("Verbinding met server mislukt");
  }

  delay(2000); // Wacht 2 seconden voor de volgende poging
}
