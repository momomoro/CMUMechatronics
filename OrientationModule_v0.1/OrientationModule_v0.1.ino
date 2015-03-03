#include <Servo.h> 
#include <Wire.h>

//pins
int gatePin = 8;
int servoPin = 5;
int stepPin = 6;
int dirPin = 7;

//cam vars
int camGo = 0;
int refreshCount = 0;
int NUM_READINGS = 10;
boolean scanSent = 0;
char isDone = 'N';

//orientation vars
char orientation = 'A';
const char RIGHT_SHARP = 'A';
const char RIGHT_ROUND = 'B';
const char LEFT_SHARP = 'C';
const char LEFT_ROUND = 'D';

//status variables
boolean dispenserReady = 1;
boolean go = 1;
boolean scanReady = 0;
boolean stepperGo = 0;

//stepper variables
int stepCount = 0;
int TURN_COUNT = 25;

//timing variables
int stepperLast = 0;
int servoLast = 0;

//servo variables
Servo myservo; 
int servoGo = 0;
int SERVO_LEFT = 0;
int SERVO_RIGHT = 180;
int SERVO_REST = 90;

//Code Start
void setup(){
  pinMode(gatePin, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(gatePin, INPUT);
  pinMode(dirPin, OUTPUT);
  Wire.begin();

  //init timing vars
  stepperLast = millis();
  servoLast = millis();

  myservo.attach(servoPin); 
  myservo.write(SERVO_REST);
  digitalWrite(dirPin, HIGH);

  Serial.begin(9600);
  Serial.print("Starting...\n");

}

void loop(){
  //loop start
  unsigned long currTime = millis();
  unsigned long stepperDur = currTime - stepperLast;
  unsigned long servoDur = currTime - servoLast;

  //if go
  if(go) {
    //wait for module not busy
    //Serial.println("Loop Start");
    if(dispenserReady) {
      //set module busy
      Serial.println("Dispenser Ready. Stepper GO!");
      dispenserReady = 0;
      stepperGo = 1;
    }

    //dispense piece
    if (stepperGo) {
      if (stepCount < TURN_COUNT) {
        stepCount += stepper_stepOnce(stepperDur, currTime);
        //Serial.print("Step Count: ");
        //Serial.println(stepCount);
      }
      else {
        digitalWrite(stepPin, LOW);
        stepperGo = 0;
        scanReady = 1;
        stepCount = 0;
      }
    }
    else {
      digitalWrite(stepPin, LOW);
    }

    //scanning and orientation
    if(scanReady) {
      //gate sensor sees piece
      if(digitalRead(gatePin) == LOW && camGo == 0) {
        Serial.println("Piece Seen!");
        camGo = 1;
        
      }
      
      if(camGo) {
        if(scanSent == 0) {
          Serial.println("REQUEST SCAN");
          Wire.beginTransmission(2);
          Wire.write('G');
          Wire.endTransmission();
          scanSent = 1;
          isDone = 'N';
        }
        else {
          delay(500);
          Serial.println("REQUEST ORIENT");
          Wire.requestFrom(2, 2);
            Serial.println("REQUEST SENT");
          while(Wire.available()) {
            Serial.println("READING...");
            isDone = Wire.read();
            orientation = Wire.read();
            Serial.println(isDone);
            Serial.println(orientation);
          }
          if(isDone == 'Y') {
            servoGo = 1;
            scanSent = 0;
          }  
        }
      }
      
      //scan piece
      //Serial.print("Servo Go :");
      //Serial.println(servoGo);
      if(servoGo) {
        Serial.print("Servo Go!");
        servoGo = 1;
        camGo = 0;
        scanReady = 0;
      }
    }
    //turn servo
    if(servoDur > 15) {
      if(servoGo) {
        if(orientation == RIGHT_SHARP ||
          orientation == LEFT_ROUND) {
          //Serial.println("LEFT");
          myservo.write(SERVO_LEFT);
          }
        else {
          //Serial.println("RIGHT");
          myservo.write(SERVO_RIGHT);
          }
      }
      else {
        myservo.write(SERVO_REST);
      }
      servoLast = currTime;

    }

    //send handshake
    //confirm reply
    //pieceCount++
  }

}

int stepper_stepOnce(long stepperDur, long currTime) {
  int stepDone = 0;

  if(stepperDur <= 10) {
    digitalWrite(stepPin, HIGH);
  }
  else {
    digitalWrite(stepPin, LOW);
  }
  if (stepperDur > 20) {
    stepperLast = currTime;
    stepDone = 1;
  }
  return stepDone;
}

