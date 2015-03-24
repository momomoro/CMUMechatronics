#include <Stepper.h>

#define THRESHOLD .002
#define STEPSIZE 30
#define ARMEND 120

const int steps = 200;

int pieceReady = 0;

int trayStepPin = 8;
int trayDirPin = 9;
int armStepPin = 6;
int armDirPin = 7;
int gate = A0;

//loop parameters
int i;
int j;

int pieceNotReady; //Will be arduino communication variable later

Stepper trayStepper(steps, trayStepPin, trayDirPin);
Stepper armStepper(steps, armStepPin, armDirPin);

void setup() {
  pinMode(gate, INPUT);
  trayStepper.setSpeed(250);
  armStepper.setSpeed(250);
  Serial.begin(9600);
  Serial.println("Begin");
}

void loop() {
  for(i = 0; i < 5; i++) {
    for(j = 0; j < 5; j++) {
      Serial.println("Wait");
      while(pieceNotReady) {
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
}
