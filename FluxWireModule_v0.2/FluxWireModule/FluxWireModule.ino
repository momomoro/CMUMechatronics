#include <Stepper.h>

#define STEPS 200
#define GATE_THRESHOLD 200
#define FLUX_STEP 200  
#define WIRE_STEP 400  

//control flow vars
int go = 1;
int fluxReady = 1;
int wireReady = 1;
int done = 0;
int signal_latch = 0;

//pin numbers

int fluxStepPin = 9; 
int fluxDirPin = 8;
int wireStepPin = 6; 
int wireDirPin = 7;
int gatePin = A0;

int commOutPin = 3;
int commInPin = 2;

//motor vars
Stepper fluxStepper(STEPS, fluxStepPin, fluxDirPin);
Stepper wireStepper(STEPS, wireStepPin, wireDirPin);

void setup()
{
  pinMode(commOutPin, OUTPUT);
  pinMode(commInPin, INPUT);
  pinMode(gatePin, INPUT);
  fluxStepper.setSpeed(200);
  wireStepper.setSpeed(200);
  digitalWrite(commOutPin, LOW);
  Serial.begin(9600);
  Serial.print("Starting...\n");
}

void loop() 
{
  int signal = digitalRead(commInPin);

  if(!signal && done && signal_latch) {
    signal_latch = 0;
  }

  if (signal && !signal_latch && !go) {
    go = 1;
    done = 0;
    signal_latch = 1;
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
      Serial.println("Flux Done");
      fluxStepper.step(FLUX_STEP);
      delay(1000);
      fluxReady = 0;
      wireReady = 1;
    }

    if(wireReady) {
      Serial.println("Wire Done");
      wireStepper.step(WIRE_STEP);
      delay(100);
      wireStepper.step(-WIRE_STEP);
      wireReady = 0;
      done = 1;
    }

    if(done) {
      digitalWrite(commOutPin, HIGH);
      delay(500);
      digitalWrite(commOutPin, LOW);
      go = 0;
    }
  }
}


