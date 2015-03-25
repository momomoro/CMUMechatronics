// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

#define WIREHOLE 160

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL2WIRE,8,9); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
int commOutPin = 3;
int commInPin = 2;


void setup()
{  
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(20);
  stepper.moveTo(0);
}

void loop()
{
    while(digitalRead(commInPin)) { //wait for arduino com
    }
    delay(2000); //allow time for wires to settle
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(WIREHOLE);
    stepper.run();
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-WIREHOLE);
    stepper.run();
    delay(2000); //allow time for wires to settle
}
