// Will Milner
// Team L Nothing to Flux With
//
// AccelStepper Library for arduino is needed to run this code
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <Stepper.h>
#include <Wire.h>

#define NUM_BYTES 20
#define STEPSIZE 200
#define FLUXSTEP 280
#define TRAYSTEP 200
#define WIRESTEP 1500
#define ALIGNSTEP 650
#define ARMEND 200
#define TRAY_SETUP_STEP 1900
#define COLOR_THRESHOLD 250

const char SETUP_REQUEST = 'S';

/*
int ARM_STEP = 20;
int ARM_STEPS_TO_APP = 100;
int TRAY_MAX_STEP_TO_FLUX = 100;
int TRAY_STEP_OFFSET = 20;
int TRAY_STEP_TO_WIRE = 100;
int TRAY_STEP_NEXT_ROW = 20;
*/
const int steps = 200;

int IRsensor = A0;

int limitSwitch = 5;
int gateSensor = 4;
int go = 0;
char query[NUM_BYTES + 1]; //Array to hold query from master
char response;
char currMode = 'W';

int trayStepPin = 8;
int trayDirPin = 9;
int armStepPin = 6;
int armDirPin = 7;

// Define a stepper and the pins it will use
Stepper tray(steps, trayStepPin, trayDirPin);
Stepper arm(steps, armStepPin, armDirPin);

void setup()
{
  tray.setSpeed(150);
  arm.setSpeed(180);
  pinMode(IRsensor, INPUT);
  pinMode(gateSensor, INPUT);
  pinMode(limitSwitch, INPUT);
  Serial.begin(9600);
  Wire.begin(5);
  Wire.onReceive(recieveEvent);
  Wire.onRequest(requestEvent);
  delay(1000); //wait for config from master
  Serial.println("Starting...");
  startUp(int(query));
  Serial.println("Done");
}

void loop()
{
  placePart();
}


void placePart() {
  for (int i = 0; i < 10; i++) {
    Serial.println("Waiting");
    for (int j = 0; j < 2; j++) {
      wait();
      Serial.println("At gate");
      /*while (digitalRead(gateSensor) == LOW) {
        Serial.print(analogRead(gateSensor));
        Serial.println("Waiting for gate");
      }*/
      delay(1000);
      Serial.println("past gate");
      align();
      //arm.step(950); //get aligned with wire & flux
      Serial.println("Waiting");
      //wait();
      //delay(1000);
      //move offset
      tray.step(-TRAYSTEP * i); //I don't think we want this

      moveToFlux();
      wait();
      //wait(1000);
      //delay(1000);
      moveToWire();
      wait();
      //delay(1000);
      moveToArm();
      delay(500);
      //tray return offset
      int pieceStep = ARMEND - j * 20;
      arm.step(pieceStep); //need to play with this number
      //move arm back to home
      arm.step(-pieceStep);
      moveArmToHome();
      tray.step(TRAYSTEP * i);
      
      response = 'Y';
    }
    //move tray
    Serial.print(" Moving Tray ");
    tray.step(TRAYSTEP);
    delay(500);
  }
  Serial.print("Done");
  //make sure to never leave this function
  for (;;) {
  }
}

void moveArmToHome() {
  arm.step(-300);
}

void align() {
  while(analogRead(IRsensor) > COLOR_THRESHOLD) {
    arm.step(1);
  }
}

void moveToArm() {
  tray.step((FLUXSTEP + WIRESTEP));
  response = 'Y';
}

void moveToFlux() {
  tray.step(-FLUXSTEP);
  response = 'Y';
}

void moveToWire() {
  tray.step(-WIRESTEP);
  response = 'Y';
}

void wait() {
  Serial.println("Starting wait");
  while (!go) {
    delay(100);
  }
  Serial.println("Done waiting");
  go = 0;
}

void recieveEvent(int numBytes) {
  int i = 0;
  while (Wire.available())
  {
    char c = Wire.read();
    Serial.print(c);
    query[i] = c;
    i++;
  }
  Serial.print(query[0]);
  if (query[0] == SETUP_REQUEST) {
    currMode = query[1];
    /*switch(currMode) {
      case(MODE_A):
        mode = 1;
        break;
      case(MODE_B):
        mode = 2;
        break;
      case(MODE_C):
        mode = 2;
        break;
      case(MODE_D):
        mode = 2;
        break;
      default:
        mode = 1;
        break;
    }*/
    Serial.print("mode: ");
    Serial.println(currMode);

  }
  else {
    go = 1;
  }
  response = 'W';
}

void requestEvent() {
  Wire.write(response);
}

void startUp(int switchVal) {
  while (digitalRead(limitSwitch)) { //run until you hit lmit switch
    tray.step(-1);
  }
  tray.step(TRAY_SETUP_STEP);

}


