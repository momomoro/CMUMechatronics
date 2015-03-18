// Will Milner
// Team L Nothing to Flux With 
//
// AccelStepper Library for arduino is needed to run this code
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

int gateSensor = 8;

// Define a stepper and the pins it will use
AccelStepper stepper;
AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

void setup()
{  
  stepper.setMaxSpeed(50);
  stepper.setAcceleration(20);
  stepper2.setMaxSpeed(50);
  stepper2.setAcceleration(20);
  pinMode(gateSensor,INPUT);
  Serial.begin(9600);
}

void loop()
{  //Tray movement
  for(int i = 0; i < 4; i++) { 
    Serial.print(" Moving Tray ");
    //Arm movement
    for(int j = 0; j < 5; j++) {
      Serial.print(" Moving Arm ");
      //part acknowledged 
      if (digitalRead(gateSensor) == LOW) {
        stepper2.moveTo(100 - j*18); //need to play with this number
      }
      stepper2.run();
      //move arm back to home
      stepper2.moveTo(0);
      stepper2.run();
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
