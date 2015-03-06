// Will Milner
// Team L Nothing to Flux With 
//
// AccelStepper Library for arduino is needed to run this code
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

void setup()
{  
  stepper.setMaxSpeed(50);
  stepper.setAcceleration(20);
  stepper2.setMaxSpeed(50);
  stepper2.setAcceleration(20);
}

void loop()
{  //Tray movement
  for(int i = 0; i < 4; i++) { 
    //Arm movement
    for(int j = 0; j < 5; j++) {
      //part acknowledged 
      if (gateSensor) {
        stepper.moveTo(100 - j*18); //need to play with this number
      }
      stepper.run();
      //move arm back to home
      stepper.moveTo(0);
      stepper.run();
    }
    //move tray
    stepper2.move(10);
    stepper.run()
  }
}
