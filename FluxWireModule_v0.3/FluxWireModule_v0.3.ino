#include <Stepper.h>
#include <Wire.h>

#define STEPS 200
#define GATE_THRESHOLD 200
#define FLUX_STEP 140
#define WIRE_STEP 400

//constants
const int ID = 4;
const char GATE_REQUEST = 'G';
const char FLUX_REQUEST = 'F';
const char WIRE_REQUEST = 'W';

//query
char query[2];

//pin numbers
int fluxStepPin = 9; 
int fluxDirPin = 8;
int wireStepPin = 6; 
int wireDirPin = 7;
int gatePin = A0;

//motor vars
Stepper fluxStepper(STEPS, fluxStepPin, fluxDirPin);
Stepper wireStepper(STEPS, wireStepPin, wireDirPin);

//flags
boolean wireReady = 0;
boolean fluxReady = 0;
boolean checkGate = 0;
char moduleDone = 'N';

void setup() {
  pinMode(gatePin, INPUT);
  fluxStepper.setSpeed(50);
  wireStepper.setSpeed(200);

  Wire.begin(ID);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
  Serial.print("Starting...\n");
}

void loop()  {
  //check gate
  if(checkGate) {
    int gateValue = analogRead(gatePin);
    Serial.print("Gate Reading: ");
    Serial.println(gateValue);
    if(gateValue > GATE_THRESHOLD) {
      Serial.println("Piece Seen!");
      moduleDone = 'Y';
      checkGate = 0;
    }
  }


  //apply flux

  if(fluxReady) {
    Serial.println("Flux Done");
    fluxStepper.step(FLUX_STEP);
    delay(100);
    fluxReady = 0;
    moduleDone = 'Y';
  }
  //apply wire
  if(wireReady) {
    Serial.println("Wire Done");
    wireStepper.step(WIRE_STEP);
    delay(100);
    wireStepper.step(-WIRE_STEP);
    delay(100);
    wireReady = 0;
    moduleDone = 'Y';
  }
}

void requestEvent() {
  Serial.println(moduleDone);
  Wire.write(moduleDone);
}

void receiveEvent(int maxCount) {
  noInterrupts();
  int i = 0;
  while(Wire.available()) //loop through Query
  {
    char c = Wire.read();
    Serial.println(c);
    query[i] = c;
    i++;
  }

  if(query[0] == GATE_REQUEST) {
    Serial.println("Gate");
    moduleDone = 'N';
    checkGate = 1;
  }
  
  if(query[0] == FLUX_REQUEST) {
    Serial.println("Flux");
    moduleDone = 'N';
    fluxReady = 1;
  }
  if(query[0] == WIRE_REQUEST) {
    Serial.println("Wire");
    moduleDone = 'N';
    wireReady = 1;
  }
  interrupts();
}


