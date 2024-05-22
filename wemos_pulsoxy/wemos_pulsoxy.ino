/*!
  @file wemos_pulsoxy.ino
  @date 21 may 2024
  @author Ruben van Eijken

  Leest de pulsoxymeter uit vanaf een Wemos D1 mini en kan dit doorsturen via een socket-verbinding.
*/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

#define WIFI_SSID "revalidatieD-wifi"
#define WIFI_PASS ""
#define MAX_BRIGHTNESS 255
#define TXBUFFER_SIZE 39

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data
uint16_t irBuffer[100];
uint16_t redBuffer[100];
#else
uint32_t irBuffer[100];
uint32_t redBuffer[100];
#endif

MAX30105 pulsoxySensor;

char txBuffer[TXBUFFER_SIZE];

// Encoder
const byte encoderS1 = 13;
volatile int encoderCount = 0;

// Buzzer
const byte buzzerPin = 12;

/*! Variables voor het opslaan van de data van de pulsoxymeter */
int32_t pulsoxyBufferLength = 100; //data length
int32_t __attribute__((aligned(4))) spo2; //SPO2 value
int8_t validSPO2 = 0; //indicator to show if the SPO2 calculation is valid
int32_t __attribute__((aligned(4))) heartRate; //heart rate value
int8_t validHeartRate = 0; //indicator to show if the heart rate calculation is valid

// Wifi poort instellen
const int serverPort = 8080;
WiFiServer server(serverPort);

/*! Maakt verbinding met een wifi-netwerk, wordt eenmalig uitgevoerd bij het opstarten */
void wifiStart();

/*! Wordt eenmalig uitgevoerd om de pulsoxymeter te initialiseren */
void pulsoxyInit();

/*! Neemt 100 samples om de signaal range te bepalen, vult de volledige buffer */
void pulsoxyCalibrate();

/*! Neemt 25 nieuwe samples en berekent nieuwe hartslag/spo2 */
void pulsoxyRead();

/*! Wordt uitgevoerd bij een interrupt van de encoder */
void ICACHE_RAM_ATTR encoderInt();

/*! Wordt eenmalig uitgevoerd bij het opstarten van de Wemos */
void setup() {
  Serial.begin(115200);
  wifiStart();
  pinMode(buzzerPin, OUTPUT);
  pinMode(encoderS1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderS1), encoderInt, CHANGE);
  pulsoxyInit();
  pulsoxyCalibrate();
}

/*! Wordt continue doorlopen zolang de Wemos aan staat */
void loop() {
  pulsoxyRead();
  // Controleer op te hoge hartslag
  if (heartRate > 160) tone(buzzerPin, 1000);
  else noTone(buzzerPin);
  // Zet data in string voor Pi
  int16_t hr = (int16_t)heartRate;
  int16_t sp = (int16_t)spo2;
  if (hr < 0) hr *= -1;
  if (sp < 0) sp *= -1;
  if (hr == 999) hr = 220;
  if (sp == 999) sp = 100;
  snprintf(txBuffer, TXBUFFER_SIZE, "Hartslag %3d Zuurstof %3d Encoder %4d", hr, sp, encoderCount);
  // Check voor een nieuwe client
  WiFiClient client = server.available();
  // Als er een client verbonden is:
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        // Lees de data van de client
        // String data = client.readStringUntil('\n');
        // Serial.println("Received data: " + data);
        client.println(txBuffer);
      }
    }
    // Sluit de verbinding met de client
    client.stop();
    Serial.println("Client verbinding verbroken");
  }
}

void wifiStart() {
  // Verbinden met wifi...
  Serial.print("Verbinding maken met: ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // Loop zolang er geen wifi connectie is
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  // Verbonden met wifi
  Serial.println();
  Serial.print("Verbonden! IP adres: ");
  Serial.println(WiFi.localIP());
  // Start de socketserver
  server.begin();
  Serial.println("Socketserver gestart");
}

void pulsoxyInit() {
  // Gebruik standaard I2C poort, 400kHz speed
  if (!pulsoxySensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  byte ledBrightness = 45; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  // Configureer sensor met deze parameters
  pulsoxySensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void pulsoxyCalibrate() {
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < pulsoxyBufferLength ; i++)
  {
    while (pulsoxySensor.available() == false) //do we have new data?
      pulsoxySensor.check(); //Check the sensor for new data

    redBuffer[i] = pulsoxySensor.getRed();
    irBuffer[i] = pulsoxySensor.getIR();
    pulsoxySensor.nextSample(); //We're finished with this sample so move to next sample
    // Print naar de console
    // Serial.print(F("red="));
    // Serial.print(redBuffer[i], DEC);
    // Serial.print(F(", ir="));
    // Serial.println(irBuffer[i], DEC);
  }
  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, pulsoxyBufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

void pulsoxyRead() {
  //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 100; i++)
  {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }
  //take 25 sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++)
  {
    while (pulsoxySensor.available() == false) //do we have new data?
      pulsoxySensor.check(); //Check the sensor for new data

    redBuffer[i] = pulsoxySensor.getRed();
    irBuffer[i] = pulsoxySensor.getIR();
    pulsoxySensor.nextSample(); //We're finished with this sample so move to next sample
    //send samples and calculation result to terminal program through UART
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
  maxim_heart_rate_and_oxygen_saturation(irBuffer, pulsoxyBufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

void encoderInt() {
  encoderCount++;
}
