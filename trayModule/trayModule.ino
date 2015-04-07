// Will Milner
// Team L Nothing to Flux With 
//
// AccelStepper Library for arduino is needed to run this code
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>
#include <Wire.h>

#define NUM_BYTES 20

int gateSensor = 8;
int go = 0;
char query[NUM_BYTES+1]; //Array to hold query from master
char response;

// Define a stepper and the pins it will use
AccelStepper stepper;
AccelStepper stepper2(AccelStepper::FULL4WIRE, 2, 3, 4, 5);

void setup()
{  
  stepper.setMaxSpeed(50);
  stepper.setAcceleration(20);
  stepper2.setMaxSpeed(50);
  stepper2.setAcceleration(20);
  pinMode(gateSensor,INPUT);
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
   for(int i = 0; i < 4; i++) { 
    Serial.print(" Moving Tray ");
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
      stepper2.moveTo(100 - j*18); //need to play with this number
      stepper2.run();
      //move arm back to home
      stepper2.moveTo(0);
      stepper2.run();
      response = 'Y';
    }
    //move tray
    stepper.move(100);
    stepper.run();
  }
  Serial.print("Done");
  //make sure to never leave this function
  for(;;) {
    }
}

void moveToFlux() {
  stepper2.moveTo(50);
  response = 'Y';
}

void moveToWire() {
  stepper2.moveTo(100);
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
