#include <Stepper.h>

#define THRESHOLD 200
#define STEPSIZE 30
#define ARMEND 120

const int steps = 200;

int commOutPin = 3;
int commInPin = 2;

int pieceReady = 0;

int trayStepPin = 8;
int trayDirPin = 9;
int armStepPin = 6;
int armDirPin = 7;

//Control logic
int limitSwitch = 5;
int gate = A0;

//loop parameters
int i;
int j;

int pieceNotReady; //Will be arduino communication variable later

Stepper trayStepper(steps, trayStepPin, trayDirPin);
Stepper armStepper(steps, armStepPin, armDirPin);

void setup() {
  pinMode(commOutPin, OUTPUT);
  pinMode(commInPin, INPUT);
  pinMode(gate, INPUT);
  pinMode(limitSwitch, INPUT);
  trayStepper.setSpeed(250);
  armStepper.setSpeed(250);
  Serial.begin(9600);
  Serial.println("Begin");
}

void loop() {
  while(digitalRead(commInPin)) { //wait for arduino com
  }
  while(digitalRead(limitSwitch)) { //run until you hit limit switch
    trayStepper.step(-1);
  }
  for(i = 0; i < 5; i++) {
    for(j = 0; j < 5; j++) {
      Serial.println("Wait");
      while(analogRead(gate) < THRESHOLD) { //wait for piece
      }
      Serial.println("Go!");
      Serial.println("Arm Move!");
      armStepper.step(ARMEND - j*STEPSIZE);
      delay(2000);
    }
    Serial.println("Tray Move!");
    trayStepper.step(STEPSIZE);
    delay(500);
  }
  digitalWrite(commOutPin,HIGH);
  delay(1000);
  digitalWrite(commOutPin,LOW);
}
