#include <ESP8266WiFi.h>
#include <Servo.h>

const char* ssid = "revalidatieD-wifi";
const char* password = "";
const char* server_ip = "SERVER_RASPBERRY_PI_IP";  // Vervang met het IP-adres van de server Raspberry Pi
const int server_port = 8080;

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
          } else if (response == "SERVO:NEUTRAL") {
            myservo.write(90); // Zet servo in neutrale positie
            Serial.println("Servo draait naar neutrale positie (90 graden)");
          } else {
            Serial.println("Onbekend servo commando");
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
