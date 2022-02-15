#include "heltec.h"
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <U8g2lib.h>
#include <Arduino.h>
#include <stdio.h>
#include "Adafruit_seesaw.h"
#include "pitches.h"
#include "numberOne.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

//LoRa
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

//definitions for temperature sensor
#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

#define SEALEVELPRESSURE_HPA (1013.25)

//define pins used in this project
int dhtPin = 10; //wired
int buzzerPin = 23; //wired
int servoPin = 4;
int servoRelayPin = 12;
int solenoidpin = 25;

Servo myServo;

//sensor and timing variables
int interval =  30000;
unsigned long previousMillis = 0;
TempAndHumidity newValues;
DHTesp dht;

//soil sensor
Adafruit_seesaw ss;

//pressure sensor
Adafruit_BMP3XX bmp;

//LoRa variables
String outgoing;              // outgoing message
int code = -1;                //which action to take

byte localAddress = 0xBC;     // address of this device
byte destination = 0xAB;      // destination to send to

byte msgCount = 0;            // count of outgoing messages
long lastSendTime = 0;        // last send time
//int interval = 2000;          // interval between sends

//DHT11 variables
bool getTemperature();
ComfortState cf;

bool getTemperature() {
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
  float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);
  float cr = dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);

  String comfortStatus;
  switch (cf) {
    case Comfort_OK:
      comfortStatus = "Comfort_OK";
      break;
    case Comfort_TooHot:
      comfortStatus = "Comfort_TooHot";
      break;
    case Comfort_TooCold:
      comfortStatus = "Comfort_TooCold";
      break;
    case Comfort_TooDry:
      comfortStatus = "Comfort_TooDry";
      break;
    case Comfort_TooHumid:
      comfortStatus = "Comfort_TooHumid";
      break;
    case Comfort_HotAndHumid:
      comfortStatus = "Comfort_HotAndHumid";
      break;
    case Comfort_HotAndDry:
      comfortStatus = "Comfort_HotAndDry";
      break;
    case Comfort_ColdAndHumid:
      comfortStatus = "Comfort_ColdAndHumid";
      break;
    case Comfort_ColdAndDry:
      comfortStatus = "Comfort_ColdAndDry";
      break;
    default:
      comfortStatus = "Unknown:";
      break;
  };

  //Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
  return true;
}

void setup()
{
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.begin(115200);
  Serial.println("Greenhouse Project");

  // initialize DHT11 and attach to dhtPin
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  LoRa.setSpreadingFactor(8);

  //buzzer
  ledcAttachPin(buzzerPin, pwmChannel);

  //pressure sensor
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);


  //initialize soil sensor
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while (1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  // initialize servo
  pinMode(servoRelayPin, OUTPUT);
  digitalWrite(servoRelayPin, HIGH);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 1000, 2000);

  //initialize solenoid
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, HIGH);
}

uint16_t capread;
void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    //read sensors
    getTemperature();
    //float tempC = ss.getTemp();

    capread = ss.touchRead(0);

    Serial.print("Temperature: "); Serial.println(newValues.temperature);
    Serial.print("Humidity: "); Serial.println(newValues.humidity);
    Serial.print("Moisture: "); Serial.println(capread);

    if (! bmp.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }

    Serial.print("Pressure = ");
    Serial.print(bmp.pressure / 100.0);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    double alt = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print(alt);
    Serial.println(" m");

    //LoRa stuff
    char buffer[50];
    sprintf(buffer, "%g, %g, %d, %g, %g", newValues.temperature, newValues.humidity, capread, bmp.pressure, alt);
    sendMessage(buffer);
    Serial.println("Sending message");

  }

  if (code == 0) {
    code = -1;
    //solenoid stuff goes here
    digitalWrite(solenoidPin, LOW);
  }

  if (code == 1) {
    code = -1;
    //song stuff goes here
    play_a_song();
  }

  if (code == 2) {
    code = -1;
    //open window here
    digitalWrite(relayPin, LOW);
    myServo.attach(servoPin, 1000, 2000);
    myServo.write(1800);
    myServo.detach();
    digitalWrite(relayPin, HIGH);
  }

  if (code == 3) {
    code = -1;
    //close window here
    digitalWrite(relayPin, LOW);
    myServo.attach(servoPin, 1000, 2000);
    myServo.write(1200);
    myServo.detach();
    digitalWrite(relayPin, HIGH);
  }

  // parse for a packet, and call onReceive with the result:
  int temp = onReceive(LoRa.parsePacket());
  if (temp != -1) {
    code = temp;
  }
}

void sendMessage(String outgoing)
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

int onReceive(int packetSize)
{
  if (packetSize == 0) return -1;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  { // check length for error
    Serial.println("error: message length does not match length");
    return -1;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return -1;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  return incoming.toInt();
}

void play_a_song() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    ledcWriteNote(pwmChannel, melody[thisNote], octave[thisNote]);
    delay(noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    ledcWriteTone(pwmChannel, 0);
    delay(15);
  }
}
