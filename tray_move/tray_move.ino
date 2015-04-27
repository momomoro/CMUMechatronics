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
#define STEPSIZE 30
#define FLUXSTEP 50
#define WIRESTEP 50

const int steps = 200;

int limitSwitch = 5;
int gateSensor = 8;
int go = 0;
char query[NUM_BYTES+1]; //Array to hold query from master
char response;

int trayStepPin = 8;
int trayDirPin = 9;
int armStepPin = 6;
int armDirPin = 7;

// Define a stepper and the pins it will use
Stepper tray(steps, trayStepPin, trayDirPin);
Stepper arm(steps, armStepPin, armDirPin);

void setup()
{  
  tray.setSpeed(250);
  arm.setSpeed(250);
  pinMode(gateSensor,INPUT);
  pinMode(limitSwitch,INPUT);
  Serial.begin(9600);
  Wire.begin(5);
  Wire.onReceive(recieveEvent);
  Wire.onRequest(requestEvent);
}

void loop()
{  
  placePart();
}


void placePart() {
  while(digitalRead(limitSwitch)) { //run until you hit lmit switch
    tray.step(-1);
  }
  for(int i = 0; i < 4; i++) { 
    //Arm movement
    for(int j = 0; j < 5; j++) {
      Serial.println("Waiting");
      wait();
      Serial.println("At gate");
      while(digitalRead(gateSensor) == LOW) {
        Serial.print(analogRead(gateSensor));
        Serial.println("Waiting for gate");
      }
      Serial.println("past gate");
      moveToFlux();
      wait();
      moveToWire();
      wait();
      Serial.print(" Moving Arm ");
      int pieceStep = 100 - j*20;
      arm.step(pieceStep); //need to play with this number
      //move arm back to home
      delay(500);
      arm.step(-(pieceStep + FLUXSTEP + WIRESTEP));
      response = 'Y';
    }
    //move tray
    Serial.print(" Moving Tray ");
    tray.step(100);
  }
  Serial.print("Done");
  //make sure to never leave this function
  for(;;) {
    }
}

void moveToFlux() {
  arm.step(FLUXSTEP);
  response = 'Y';
}

void moveToWire() {
  arm.step(FLUXSTEP);
  response = 'Y';
}

void wait() {
  Serial.println("Starting wait");
  while(!go) {
    delay(100);
    }
  Serial.println("Done waiting");
  go = 0;
}

void recieveEvent(int numBytes) {
    while(Wire.available())
    {
      int i = 0;
      char c = Wire.read();
      Serial.print(c);
      query[i] = c;
      i++;
  }
  go = 1;
  response = 'W';
}
  
void requestEvent() {
  Wire.write(response);
}
