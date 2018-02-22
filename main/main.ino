#include "DualVNH5019MotorShield.h"
#include "PinChangeInt.h"

#define TEST_TIME 2.0
#define FRONT_RIGHT A0
#define FRONT_CENTER A1
#define FRONT_LEFT A2
#define LEFT A3
#define RIGHT A4
#define BACK A5 
#define READ_TIMES 7
#define encoderRPinA 3
#define encoderRPinB 5
#define encoderLPinA 11
#define encoderLPinB 13

int encoderRPos = 0;
int encoderRPinALast = LOW;

int encoderLPos = 0;
int encoderLPinALast = LOW;

int n = LOW;
int counter = 0;
DualVNH5019MotorShield md;
long long Time;

void setup() {
  pinMode (encoderLPinA, INPUT);
  pinMode (encoderLPinB, INPUT);
  pinMode (encoderRPinA, INPUT);
  pinMode (encoderRPinB, INPUT);
  
  Serial.begin (9600);
  // attachInterrupt(1, doEncoder, RISING); 
  PCintPort::attachInterrupt(encoderLPinA, &doEncoderLeft, RISING);
  PCintPort::attachInterrupt(encoderRPinA, &doEncoderRight, RISING);
  md.init();
}

void loop() {
  char command;
  if (Serial.available()) {
    command = Serial.read();
    switch(command) {
      case 'f': md.setSpeed(200, 185);
                break;
      case 'l': md.setSpeed(-200, 185);
                break;
      case 'r': md.setSpeed(200, -185);
                break;
      case 'b': md.setSpeed(-200, -185);
                break;
      case 's': md.setSpeed(0, 0);
                md.setBrakes(400, 400);
    }
  }
  //md.setSpeed(200,185);
  Serial.println(readRpmWithInterrupt(true));
  Serial.println(readRmpWithInterrupt(false));
}

float readRpmWithInterrupt(bool isLeftWheel) {
//  Time = micros();
//  md.setSpeeds(200, 185);
//  if (Time > TEST_TIME * 1000000) {
//    if (counter == 0) {
//      int encoderPos = isLeftWheel * encoderLPos + (1 - isLeftWheel) * encoderRPos;
//      float rpm = 60 / (562.25 / (encoderPos / TEST_TIME));
//      Serial.print(rpm);
//    }
//    counter = 1;
//  }

  delay(TEST_TIMES * 1000000);
  if (isLeftWheel) {
    float rpm = 60 / (562.25 / (encoderLPos / TEST_TIME));
    encoderLPos = 0;
    return rpm;
  } else {
    float rpm = 60 / (562.25 / (encoderRPos / TEST_TIME));
    encoderRPos = 0;
    return rpm;
  }
}

float readRpmWithoutInterrupt(bool isLeftWheel) {
//  Time = micros();
//  md.setSpeeds(400,400);
//  if (Time <= 2000000) {
//    n = digitalRead(encoderRPinA);
//    if ((encoderRPinALast == LOW) && (n == HIGH)) {
//      if (digitalRead(encoderRPinB) == LOW) {
//        encoderRPos--;
//      } else {
//        encoderRPos++;
//      }
//    }
//    encoderRPinALast = n;
//  } else {
//    if (counter == 0)
//      Serial.print(encoderRPos);
//    counter = 1;
//  }
}

void doEncoderLeft() {
  if (digitalRead(encoderLPinA) == digitalRead(encoderLPinB)) {
    encoderLPos++;
  } else {
    encoderLPos--;
  }
}

void doEncoderRight() {
  if (digitalRead(encoderRPinA) == digitalRead(encoderRPinB)) {
    encoderRPos++;
  } else {
    encoderRPos--;
  }
}

float readSingleSensor(int sensorNumber) {
  // read the pin 7 times to get median value
  int sensorVal[READ_TIMES];
  int sortedSensorVal[READ_TIMES];
  for (int i = 0; i < READ_TIMES; i++) {
    sensorVal[i] = analogRead(sensorNumber);         
    delay(10);
  }
  sortedSensorVal[0] = sensorVal[0];
  for (int i = 1; i < READ_TIMES; i++) {
    int id = 0;
    while (id < i && sortedSensorVal[id] <= sensorVal[i])
      id++;
    if (id == i) {
      sortedSensorVal[i] = sensorVal[i];
    } else {
      for (int j = i - 1; j >= id; j--) {
        sortedSensorVal[j+1] = sensorVal[j]; 
      } 
      sortedSensorVal[id] = sensorVal[i]; 
    }
  }
  int readValue = sortedSensorVal[READ_TIMES/2];

  // deduce distance from value read from analog pin
  switch (sensorNumber) {
    case FRONT_RIGHT: 
      return 8544.11/readValue - 7.29;
    case FRONT_LEFT: 
      return 8068.98/readValue - 5.63;
    case LEFT:
      return 8766.22/readValue - 7.35;
    case RIGHT:
      return 7904.38/readValue - 5.62;
    case BACK:
      return 8068.41/readValue - 6.77;
  }
  
}


