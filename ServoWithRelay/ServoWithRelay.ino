#include <ESP32Servo.h>
#include <analogWrite.h>
//#include <tone.h>
#include <ESP32Tone.h>
#include <ESP32PWM.h>

//#include "Arduino.h"
#include "heltec.h"
int servoPin = 4;
int relayPin = 12;
const int pwmChannel = 0;
Servo myServo;

void setup() {
  // put your setup code here, to run once:
  Heltec.begin(false /*Display Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  pinMode(relayPin, OUTPUT);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 1000, 2000);
  digitalWrite(relayPin, LOW);
  myServo.write(1500);  
  Serial.println("first delay");
  delay(1000);
  myServo.write(1200);
  Serial.println("second delay");
  delay(1000);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println("top of loop");
  digitalWrite(relayPin, HIGH);
  myServo.detach();
  delay(1000);
  digitalWrite(relayPin, LOW);
  myServo.attach(servoPin, 1000, 2000);
  myServo.write(1950);
  delay(1000);
  
  Serial.println("middle of the loop");
  digitalWrite(relayPin, HIGH);
  myServo.detach();
  delay(1000);
  digitalWrite(relayPin, LOW);
  myServo.attach(servoPin, 1000, 2000);
  myServo.write(1000);
  delay(1000);
}
