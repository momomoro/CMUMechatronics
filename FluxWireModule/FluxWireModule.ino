#include <Stepper.h>

#define STEPS 200
#define GATE_THRESHOLD 200
#define FLUX_STEP 50  

//control flow vars
int go = 0;
int fluxReady = 0;
int done = 0;

//pin numbers

int fluxStepPin = 9; 
int fluxDirPin = 8;
int gatePin = A0;

int commOutPin = 3;
int commInPin = 2;

//motor vars
Stepper fluxStepper(STEPS, fluxStepPin, fluxDirPin);

void setup()
{
  pinMode(commOutPin, OUTPUT);
  pinMode(commInPin, INPUT);
  pinMode(gatePin, INPUT);
  fluxStepper.setSpeed(120);
    digitalWrite(commOutPin, LOW);
  Serial.begin(9600);
  Serial.print("Starting...\n");
}

void loop() 
{
  int signal = digitalRead(commInPin);
  if (signal && !go) {
    go = 1;
    Serial.println("Signal received! Waiting for Part...");
  }
  if(go) {
    int gateValue = analogRead(gatePin);
    //Serial.print("Gate Reading: ");
    //Serial.println(gateValue);
    if(gateValue > GATE_THRESHOLD && !fluxReady) {
      Serial.println("Piece Seen!");
      fluxReady = 1;
    }
    if(fluxReady) {
      Serial.print("Done: ");
      Serial.println(done);
      fluxStepper.step(FLUX_STEP);
      fluxReady = 0;
      go = 0;
      digitalWrite(commOutPin, HIGH);
      delay(500);
      digitalWrite(commOutPin, LOW);
    }
  }
}

