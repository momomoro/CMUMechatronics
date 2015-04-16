#include <Servo.h>
#include <Wire.h>

//constants
const int ID = 3;
const int GATE_THRESHOLD = 100;
const int ORIENT_SERVO_DELAY = 750;
const int HOPPER_SERVO_DELAY = 600;
const int HOPPER_SERVO_EXTEND = 90;
const int HOPPER_SERVO_RETRACT = 180;
const int ORIENT_SERVO_REST = 97;
const int ORIENT_SERVO_LEFT = 0;
const int ORIENT_SERVO_RIGHT = 180;
const char ORIENT_REQUEST = 'C';
const char READY_QUERY = 'R';
const int WAIT_THRESHOLD = 1000; //ms

//query

char query[2];

//pins
int orientGatePin = A0;
int hopperGatePin = A1;
int orientServoPin = 5;
int hopperServoPin = 6;

//dejamming
long hopperDispatchTime = 0;

//servos
Servo orient_servo;
Servo hopper_servo;

//flags
boolean hopperCheck = 0;
boolean hopperServoGo = 0;
boolean hopperDone = 0;
boolean orientGo = 0;
boolean hopperLatch = 0;
char moduleDone = 'N';

//orientation values
char orientation = '?';
const char RIGHT_SHARP = 'A';
const char RIGHT_ROUND = 'B';
const char LEFT_SHARP = 'C';
const char LEFT_ROUND = 'D';


void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(orientGatePin, INPUT);
  pinMode(orientServoPin, OUTPUT);
  pinMode(hopperGatePin, INPUT);
  pinMode(hopperServoPin, OUTPUT);
  Wire.begin(ID);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //init servos
  orient_servo.attach(orientServoPin);
  hopper_servo.attach(hopperServoPin);
  orient_servo.write(ORIENT_SERVO_REST);
  hopper_servo.write(HOPPER_SERVO_RETRACT);


}

void loop() {

  //check for piece at hopper and turn servo
  if (hopperCheck) {
    if (analogRead(hopperGatePin) >= GATE_THRESHOLD) {
      Serial.println("Jam Detected. Dejamming...");
      hopperLatch = 0;
      int servoLast = millis();
      int servoTime = millis() - servoLast;
      while(servoTime < HOPPER_SERVO_DELAY) {
        //read pin
        if (analogRead(hopperGatePin) < GATE_THRESHOLD) {
          Serial.println("PIECE GOTTEN");
          hopperLatch = 1;
        }
        //move stepper
        hopper_servo.write(HOPPER_SERVO_EXTEND);
        delay(15);
        servoTime = millis() - servoLast;
      }
      servoLast = millis();
      servoTime = millis() - servoLast;
      while(servoTime < HOPPER_SERVO_DELAY) {
        //read pin
        if (analogRead(hopperGatePin) < GATE_THRESHOLD) {
          Serial.println("PIECE GOTTEN");
          hopperLatch = 1;
        }
        //move stepper
        hopper_servo.write(HOPPER_SERVO_RETRACT);
        delay(15);
        servoTime = millis() - servoLast;
      }
      Serial.println("...Done");
      delay(1000);
      int ogateval = analogRead(orientGatePin);
      Serial.print("Orient Gate: ");
      Serial.println(ogateval);
      if(ogateval < GATE_THRESHOLD) {
        Serial.print("Seen Gate: ");
        Serial.println(ogateval);
        Serial.println("Piece received by Orientator!");
        hopperLatch = 0;
        hopperCheck = 0;
        hopperDone = 1;
      }
    }
    else {
      hopperLatch = 1;
    }
    
    if (hopperLatch) {
      Serial.println("Hopper Piece Seen");
      hopperDispatchTime = millis();
      hopperServoGo = 1;
      hopperCheck = 0;
    }
    else {
      //hopper_servo.write(HOPPER_SERVO_EXTEND);
      //delay(HOPPER_SERVO_DELAY);
      hopper_servo.write(HOPPER_SERVO_RETRACT);
      //delay(HOPPER_SERVO_DELAY*2);
    }
  }

  //dispense piece
  if (hopperServoGo) {
    hopper_servo.write(HOPPER_SERVO_EXTEND);
    delay(HOPPER_SERVO_DELAY);
    hopperDone = 1;
    hopperServoGo = 0;
  }
  else if (!hopperCheck) {
    hopper_servo.write(HOPPER_SERVO_RETRACT);
  }

  //check for piece at orient and flag ready
  if (hopperDone) {
    int waitTime = millis() - hopperDispatchTime;
    int ogateVal = analogRead(orientGatePin);
    if (ogateVal < GATE_THRESHOLD) {
      Serial.println("Orient Piece Seen");
      Serial.print("Orient Gate: ");
      Serial.println(ogateVal);
      moduleDone = 'Y';
      hopperDone = 0;
    }
    else if (waitTime > WAIT_THRESHOLD) {
      hopper_servo.write(HOPPER_SERVO_EXTEND);
      delay(HOPPER_SERVO_DELAY);
      hopper_servo.write(HOPPER_SERVO_RETRACT);
      delay(HOPPER_SERVO_DELAY);
    }
  }

  //needs orientGo signal from Master
  if (orientGo) {
    Serial.print("Moving to: ");
    Serial.println(orientation);
    if (orientation == RIGHT_SHARP ||
        orientation == LEFT_ROUND) {
      orient_servo.write(ORIENT_SERVO_LEFT);
      delay(ORIENT_SERVO_DELAY);

    }
    else {
      orient_servo.write(ORIENT_SERVO_RIGHT);
      delay(ORIENT_SERVO_DELAY);
    }
    moduleDone = 'Y';
    orientGo = 0;
  }
  else {
    orient_servo.write(ORIENT_SERVO_REST);
  }


}

void requestEvent() {
  Serial.println(moduleDone);
  Wire.write(moduleDone);
}

void receiveEvent(int maxCount) {
  int i = 0;
  while (Wire.available()) //loop through Query
  {
    char c = Wire.read();
    Serial.print(c);
    query[i] = c;
    i++;
  }

  Serial.println("");
  if (query[0] == READY_QUERY) {
    Serial.println("Ready?");
    moduleDone = 'N';
    hopperCheck = 1;
  }
  if (query[0] == ORIENT_REQUEST) {
    Serial.println("Orient!");
    moduleDone = 'N';
    orientation = query[1];
    Serial.println(orientation);
    orientGo = 1;
  }
}




