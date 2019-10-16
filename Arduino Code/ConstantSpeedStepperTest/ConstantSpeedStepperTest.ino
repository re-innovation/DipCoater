// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(1, 8, 9); // Stepper using a driver (1) with Pulses on pin D10 and Direction on pin D9. Enable is D8


void setup()
{  
  //Stepper setup
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);             // STEP_ENA is active LOW
  
   stepper.setMaxSpeed(5000);
   stepper.setSpeed(4000);
   // Set Stepper enabled
   digitalWrite(10, LOW); 	
}

void loop()
{  
  
   stepper.runSpeed();
}
