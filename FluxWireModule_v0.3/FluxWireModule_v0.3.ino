#include <Stepper.h>
#include <Wire.h>

#define STEPS 200
#define GATE_THRESHOLD 200
#define FLUX_STEP -15
#define WIRE_FULL 800
#define WIRE_HALF 400

//constants
const int ID = 4;
const char GATE_REQUEST = 'G';
const char FLUX_REQUEST = 'F';
const char WIRE_REQUEST = 'W';
const char SETUP_REQUEST = 'S';


const char MODE_A = 'A';
const char MODE_B = 'B';
const char MODE_C = 'C';
const char MODE_D = 'D';

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
int mode = 1;

void setup() {
  pinMode(gatePin, INPUT);
  fluxStepper.setSpeed(10);
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
    //fluxStepper.step(40);
    fluxStepper.step(FLUX_STEP);
    //delay(100);
    //fluxStepper.step(-40);
    fluxReady = 0;
    moduleDone = 'Y';
    Serial.println("Flux Done");
    //delay(2000);
    //wireReady = 1;
  }
  //apply wire
  if(wireReady) {
    if (mode == 1) {
      wireStepper.step(WIRE_FULL);
      delay(100);
    }
    else if (mode == 2) {
      wireStepper.step(WIRE_FULL);
      delay(100);
      wireStepper.step(-WIRE_FULL);
      delay(100);
      wireStepper.step(-WIRE_HALF);
      delay(100);
      wireStepper.step(WIRE_HALF);
      delay(100);
    }
    wireReady = 0;
    moduleDone = 'Y';
    Serial.println("Wire Done");
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
  
  if(query[0] == SETUP_REQUEST) {
      char currMode = query[1];
      switch(currMode) {
        case(MODE_A):
          mode = 1;
          break;
        case(MODE_B):
          mode = 2; 
          break;
        case(MODE_C):
          mode = 2; 
          break;
        case(MODE_D):
          mode = 2; 
          break;
        default:
          mode = 1;
          break;
      }
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


