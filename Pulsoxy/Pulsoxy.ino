/*!
  @file Pulsoxy.ino
  @date 3 may 2024
  @author Ruben van Eijken

  Leest de pulsoxymeter uit vanaf een Wemos D1 mini en kan dit doorsturen via een socket-verbinding.
*/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

#define MAX_BRIGHTNESS 255

MAX30105 particleSensor;

uint8_t eersteMetingVoltooid = 0;
char sendBuffer[26];

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

byte pulseLED = 12; //Must be on PWM pin


// Buzzer pin
const int buzzerPin = 14; // GPIO 14 (D5)

// Set WiFi credentials
#define WIFI_SSID "revalidatieD-wifi"
#define WIFI_PASS ""
const int serverPort = 8080;
WiFiServer server(serverPort);


 

/*! Wordt eenmalig uitgevoerd om de Wemos te verbinden met de hotspot van Raspberry Pi */
void wifiInit()
{
  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Start de socketserver
  server.begin();
  Serial.println("Server started");
}

/*! Wordt eenmalig uitgevoerd om de sensor in te stellen met de juiste parameters */
void sensorInit()
{
  // Use default I2C port, 400kHz speed
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
}

/*! Wordt eenmalig aan het begin van een metingsequentie uitgevoerd */
void readFirst100Samples()
{
  Serial.println("Test 2");
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    // Serial.print(F("red="));
    // Serial.print(redBuffer[i], DEC);
    // Serial.print(F(", ir="));
    // Serial.println(irBuffer[i], DEC);
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

/*! Wordt voor elke individuele meting uitgevoerd om de sensor uit te lezen */
void readSensor()
{
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

  // Dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 0; i < 75; i++) {
    redBuffer[i] = redBuffer[i + 25];
    irBuffer[i] = irBuffer[i + 25];
    Serial.println("Test 1");
  }

  // Take 25 new sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++) {
    Serial.println("Test 3");
    while (particleSensor.available() == false) // Do we have new data?
      particleSensor.check(); // Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // We're finished with this sample so move to next sample

    // Send samples and calculation result to terminal program through UART
    // Serial.print(F("red="));
    // Serial.print(redBuffer[i], DEC);
    // Serial.print(F(", ir="));
    // Serial.print(irBuffer[i], DEC);

    // Serial.print(F(", HR="));
    // Serial.print(heartRate, DEC);

    // Serial.print(F(", HRvalid="));
    // Serial.print(validHeartRate, DEC);

    // Serial.print(F(", SPO2="));
    // Serial.print(spo2, DEC);

    // Serial.print(F(", SPO2Valid="));
    // Serial.println(validSPO2, DEC);
  }
  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    


}

/*! Wordt eenmalig uitgevoerd bij het opstarten van de Wemos */
void setup()
{
  // Setup serial port
  Serial.begin(115200);
  Serial.println();

  sensorInit();
  wifiInit();
  // Initialize buzzer pin
  pinMode(buzzerPin, OUTPUT);
}

/*! Wordt continue doorlopen zolang de Wemos aan staat */
void loop()
{
  // Check voor een nieuwe client
  WiFiClient client = server.available();

  if (client) {
    while (client.connected()) {
      if (client.available()) {
        // Lees de data van de client
        String data = client.readStringUntil('\n');
        Serial.println("Received data: " + data);

        if (data == "start meting") {
          if (eersteMetingVoltooid == 0) {
            readFirst100Samples();
            eersteMetingVoltooid = 1;
          }
          readSensor();
          sprintf(sendBuffer, "Hartslag %3d Zuurstof %3d", (uint32_t)heartRate, spo2);
          // Serial.print("Data verzonden: ");
          // Serial.println(sendBuffer);
          client.println(sendBuffer);
        }
        else if (data == "stop meting") {
          eersteMetingVoltooid = 0;
        }
        // Stuur een end message terug naar de client
        Serial.println("Data verzonden: end");
        //client.println("end");
        // Check if heart rate exceeds threshold and activate buzzer if it does
          if (heartRate > 100) { // You can adjust the threshold value as needed
            tone(buzzerPin, 1000); // Send 1KHz sound signal... // Turn on the buzzer
          } else {
            noTone(buzzerPin);     // Stop sound... // Turn off the buzzer
          }
      }
    }
    // Sluit de verbinding met de client
    client.stop();
    Serial.println("Client disconnected");
  }
  //else Serial.println("Geen client...");
}
