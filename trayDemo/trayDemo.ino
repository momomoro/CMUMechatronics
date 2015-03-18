#inclue <Stepper.h>

#define STEPS 200

int pieceReady = 0;

int stepPin = 8;
int dirPin = 9;
int gate = 3;

Stepper stepper(STEPS, stepPin, dirPin);

void setup() {
  pinMode(gate, INPUT);
  stepper.setSpeed(120);
  Serial.begin(9600);
  Serial.println("Begin");
}

void loop() {
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 5; j++) {
      while(digitalRead(gate)) {
        Serial.print(".");
      }
      Serial.println("Go!");
      stepper.step(j*30);
    }
  }
}
