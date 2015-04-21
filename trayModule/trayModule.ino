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
#define FLUXSTEP 200
#define TRAYSTEP 100
#define WIRESTEP 200
#define ALIGNSTEP 100
#define ARMEND 300

const int steps = 200;

int limitSwitch = 5;
int gateSensor = 4;
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
  tray.setSpeed(150);
  arm.setSpeed(180);
  pinMode(gateSensor,INPUT);
  pinMode(limitSwitch,INPUT);
  Serial.begin(9600);
  Wire.begin(5);
  Wire.onReceive(recieveEvent);
  Wire.onRequest(requestEvent);
  delay(3000); //wait for config from master
  startUp(int(query));
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
	Serial.println("Waiting");
    wait();
    for(int j = 0; j < 5; j++) {
	  Serial.println("At gate");
	  while(digitalRead(gateSensor) == LOW) {
        Serial.print(analogRead(gateSensor));
        Serial.println("Waiting for gate");
      }
      Serial.println("past gate");
	  arm.step(ALIGNSTEP); //get aligned with wire & flux
	  Serial.println("Waiting");
	  wait();
      moveToFlux();
      wait();
      //delay(500);
      moveToWire();
      wait();
      //delay(500);
	  moveToArm();
      //delay(500);
	  int pieceStep = ARMEND - j*20;
      arm.step(pieceStep); //need to play with this number
      //move arm back to home
      delay(500);
      arm.step(-(pieceStep + ALIGNSTEP));
      response = 'Y';
	}
    //move tray
    Serial.print(" Moving Tray ");
    tray.step(TRAYSTEP);
  }
  Serial.print("Done");
  //make sure to never leave this function
  for(;;) {
    }
}

void moveToArm() {
	tray.step(-(FLUXSTEP + WIRESTEP));
	response = 'Y';
}

void moveToFlux() {
  tray.step(FLUXSTEP);
  response = 'Y';
}

void moveToWire() {
  tray.step(WIRESTEP);
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
  if(query=="startUp") {
	  return;
  }
  go = 1;
  response = 'W';
}
  
void requestEvent() {
  Wire.write(response);
}

void startUp(int switchVal) {
  switch(switchVal){
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}
  
  
