#include "heltec.h"
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <U8g2lib.h>
#include <Arduino.h>
#include <stdio.h>
#include "Adafruit_seesaw.h"
#include "pitches.h"
#include "numberOne.h"

//LoRa
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

//definitions for temperature sensor
#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

//definitions for OLED display
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//create OLED display object
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

//define pins used in this project
int dhtPin = 10; //wired
int buzzerPin = 23; //wired
int servopin = 9;
int servoRelayPin = 12;
int solenoidpin = 25;
const int pwmChannel = 0;

//sensor and timing variables
int interval = 2000;
unsigned long previousMillis = 0;
TempAndHumidity newValues;
DHTesp dht;

//soil sensor
Adafruit_seesaw ss;

//LoRa variables
String outgoing;              // outgoing message

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
  switch(cf) {
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
  
  //LoRa.setSpreadingFactor(8);

  //buzzer
  ledcAttachPin(buzzerPin, pwmChannel);
  
  //u8g2.begin();

  //initialize soil sensor
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }

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
    
//Add LoRa stuff here later
    char buffer[50];
//    sprintf(buffer, "%g", newValues.temperature);
//    //sendMessage(buffer);
//    //Serial.println("Sending message");

    //send data to OLED display - commented out b/c it conflicts with the soil sensor
//    u8g2.clearBuffer();          // clear the internal memory
//    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
//    sprintf(buffer, "Temperature: %g °C", newValues.temperature);
//    u8g2.drawStr(0,10, buffer);  // write something to the internal memory
//    sprintf(buffer, "Humidity: %d", newValues.humidity);
//    u8g2.drawStr(0,30, buffer);  // write something to the internal memory
//    sprintf(buffer, "Moisture: %d", capread);
//    u8g2.drawStr(0,50, buffer);  // write something to the internal memory
//    u8g2.sendBuffer(); 
  }

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
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
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

void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // if there's no packet, return

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
  {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
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
}
