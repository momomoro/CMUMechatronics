#include <Servo.h> 
#include <Wire.h>

//pins
int gatePin = A0;
int hopperGatePin_A = A1;
int hopperGatePin_B = A2;
int orientServoPin = 5;
int hopperServoPin = 6;
int commOutPin = 8;
int commInPin = 9;
int commErrPin = 2;
int statePin_0 = 4;
int statePin_1 = 3;
int stateOutPin_A = 10;
int stateOutPin_B = 11;

//gate vars
int GATE_THRESHOLD = 60;

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
boolean dispenserReady = 0;
boolean go = 1;
boolean done = 1;
boolean scanReady = 0;
boolean hopperGo = 0;
boolean orientGo = 0;
boolean typeSwitch_lower = 0;
boolean typeSwitch_higher = 0;
char mode = 'G';

//stepper variables
int stepCount = 0;
int TURN_COUNT = 25;

//timing variables
int hopperLast = 0;
int orientorLast = 0;

//servo variables
Servo orient_servo; 
Servo hopper_servo;
int servoLast = 0;
int SERVO_LEFT = 0;
int SERVO_RIGHT = 180;
int SERVO_REST = 97;

//Code Startid 
void setup(){
  pinMode(gatePin, INPUT);
  pinMode(orientServoPin, OUTPUT);
  pinMode(hopperServoPin, OUTPUT);
  pinMode(stateOutPin_A, OUTPUT);
  pinMode(stateOutPin_B, OUTPUT);
  pinMode(commOutPin, OUTPUT);
  pinMode(commInPin, INPUT);
  pinMode(statePin_0, INPUT);
  pinMode(statePin_1, INPUT);
  pinMode(hopperGatePin_A, INPUT);
  pinMode(hopperGatePin_B, INPUT);
  Wire.begin();

  //init timing vars
  servoLast = millis();

  orient_servo.attach(orientServoPin); 
  hopper_servo.attach(hopperServoPin); 
  orient_servo.write(SERVO_REST);
  hopper_servo.write(90);

  Serial.begin(9600);
  Serial.print("Starting...\n");

}

void loop(){
  //loop start
  unsigned long currTime = millis();
  unsigned long servoDur = currTime - servoLast;

  typeSwitch_higher = digitalRead(statePin_1);
  typeSwitch_lower = digitalRead(statePin_0);    
  digitalWrite(stateOutPin_B, typeSwitch_higher); 
  digitalWrite(stateOutPin_A, typeSwitch_lower);
  
  int switchVal = typeSwitch_higher*2 + typeSwitch_lower;
  
  switch(switchVal) {
    case(0): 
      mode = 'A';
      break;
    case(1): 
      mode = 'B';
      break;
    case(2): 
      mode = 'C';
      break;
    case(3): 
      mode = 'D';
      break;
    default:
      mode = 'G';
      break;
  }

  //if go
  if(go) {
    //wait for module not busy
    //Serial.println("Loop Start");
    if(dispenserReady) {
      delay(500);
      //set module busy
      Serial.println("Dispenser Ready. Hopper GO!");
      dispenserReady = 0;
      hopperGo = 1;
      done = 0;
    }
    else if (done) {
      int hgateVal_A = analogRead(hopperGatePin_A);
      if(hgateVal_A < GATE_THRESHOLD) {
        dispenserReady = 1;
      }
    }

    //scanning and orientation
    //gate sensor sees piece
    int gateValue = analogRead(gatePin);
    if(gateValue < GATE_THRESHOLD && 
       !camGo &&
       hopperGo) {
      Serial.print("Gate Value: ");
      Serial.println(gateValue);
      Serial.println("Piece Seen!");
      camGo = 1;
      hopperGo = 0;
    }

    if(camGo) {
      if(scanSent == 0) {
        Serial.print("REQUEST SCAN - MODE: ");
        Serial.println(mode);
        Wire.beginTransmission(2);
        Wire.write(mode);
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
          orientGo = 1;
          scanSent = 0;
        }  
      }


      //scan piece
      //Serial.print("Servo Go :");
      //Serial.println(servoGo);
      if(orientGo) {
        Serial.println("Orienting Piece...");
        Serial.println("Waiting for handshake...");
        //send handshake
        digitalWrite(commOutPin, HIGH);
        orientGo = 1;
        camGo = 0;
      }
      else {
        digitalWrite(commOutPin, LOW);
      }
    }
    //turn servos
    if(servoDur > 15) {
      //dispense piece 
      if(hopperGo) {
        hopper_servo.write(30);
      }
      else {
        hopper_servo.write(90);
      }

      if(orientGo) {
        if(orientation == RIGHT_SHARP ||
          orientation == LEFT_ROUND) {
          //Serial.println("LEFT");
          orient_servo.write(SERVO_LEFT);
        }
        else {
          //Serial.println("RIGHT");
          orient_servo.write(SERVO_RIGHT);
        }
      }
      else {
        orient_servo.write(SERVO_REST);
      }
      servoLast = currTime;

    }

    //confirm reply
    int reply = digitalRead(commInPin);
    if(orientGo && reply) {
      Serial.println("...Piece Received by FW_module.");
      Serial.println("Ready for next Piece");
      Serial.println("Resetting...");
      orientGo = 0;
      done = 1;
      digitalWrite(commOutPin, LOW);
    }
  }

}


