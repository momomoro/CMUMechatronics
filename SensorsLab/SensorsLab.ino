#include <Servo.h> 

// GUI Variables
int width = 0;
int graphValue = 0;
int numPieces = 0;
int deposit = 0;
int oldMode = 1;
int fromHW = 0;

//sensor variables
int IR = 0;
int PotReading = 0;
int GateReading = 0;
int POT_BASE = 515;

//GUI IDs
int motorDirID = 0;
int motorButtonID = 0;
int motorRunning = 0;
int pieceCountID = 0;
int modeStateID = 0;
int stepperDirToggleID = 0;
int stepperButtonID = 0;
int GUILabelID = 0;
int motorRunningLabel = 0;
int motorButtonDepId = 0;
int permissionLabel = 0;
int servoID = 0;
int rotaryServo = 0;

// Smoothing variables
const int numReadings = 10;
int readings[numReadings];      
int potReadings[numReadings];
int potIndex = 0;
int readIndex = 0; 
int potTotal = 0;
int total= 0;    
int potAverage = 0;
int average = 0;

//Servo variables

Servo myservo; 
unsigned long servoDur = 0;
unsigned long servoLast = 0;
int servoAngle_GUI = 0;
int servoAngle_POT = 0;

//encoder variables
volatile int encoderPin_ACurr;
volatile int encoderPin_BCurr;
volatile int encoderPin_ALast;
volatile int encoderReading = 0;
unsigned long encoderDur = 0;
unsigned long encoderLast = 0;
int motorSpeed_GUI = 0;
int motorSpeed_IR = 0;
int motorAngle = 0;
int motorGo = 0;
int motorDir = 1;
int angleTurned = 0;
int THRESHOLD = 1;
int maxDistance = 2000;
int minDistance = 0;


//stepper variables
int stepCount = 0;
long stepperDur = 0;
long stepperLast = 0;
int SLOT_STEPS = 25; //200 steps, 8 slots
int stepperReady = 1;
int dropPart = 0;
int input_steps = 0;
int stepperGo = 0;
int stepDir = 0;

//control modes
int GUImode = 0;

//pins
int GUI_Pin = 13;
int potPin = A1;
int gatePin = 4;
int IRPin = A4;
int servoPin = 5;
int stepPin = 7;  
int dirPin = 6;
int L3Pin = 9;
int L4Pin = 10;
int enMotorPin = 11;
int encoderPin_B = 2;
int encoderPin_A = 3;

void setup(){
  pinMode(GUI_Pin, INPUT);
  pinMode(potPin, INPUT);
  pinMode(IRPin, INPUT);
  pinMode(gatePin, INPUT);
  pinMode(L3Pin, OUTPUT);
  pinMode(L4Pin, OUTPUT);
  pinMode(enMotorPin, OUTPUT);
  pinMode(encoderPin_A, INPUT);
  pinMode(encoderPin_B, INPUT);
  //  attachInterrupt(1, updateEncoders, FALLING);
  attachInterrupt(1, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);   
  Serial.begin(4800);        
  myservo.attach(servoPin); 

  GUImode = digitalRead(GUI_Pin);
  if (GUImode == 1)
  {
    gUpdateLabel(GUILabelID,"GUI CONTROL MODE");
  }
  else {
    gUpdateLabel(GUILabelID,"HARDWARE CONTROL MODE");
  }
  // Start the guino dashboard interface.
  // The number is your personal key for saving data. This should be unique for each sketch
  // This key should also be changed if you change the gui structure. Hence the saved data vill not match.
  gBegin(34236); 
}


void loop(){
  // **** Main update call for the guino
  guino_update();

  //read GUI switch
  GUImode = digitalRead(GUI_Pin);
  if (oldMode != GUImode) {
    noInterrupts();
    angleTurned = 0;
    encoderReading = 0;
    interrupts();
    if (GUImode == 1)
    {
      gUpdateLabel(GUILabelID,"GUI CONTROL MODE");
      fromHW = 1;
    }
    else {
      gUpdateLabel(GUILabelID,"HARDWARE CONTROL MODE");
    }
    oldMode = GUImode;
  }

  // Read potentiometer
  PotReading = smoothPot(potPin); //attached to a voltage divider so reading is 1/2 of pot

  // Read IR sensor
  IR = smooth(IRPin);
  motorSpeed_IR = map(IR, 0, 1023, 90, 255);

  //read gate sensor
  GateReading = 1 - digitalRead(gatePin); //Gate is high when not broken
  if(deposit && GateReading && stepperReady) {
    dropPart = 1;
    stepperReady = 0;
    stepCount = 0;
  }

  //servo angle
  servoAngle_POT = map(PotReading, 0, POT_BASE, 0, 180);//convertToServoAngle(PotReading, POT_BASE);

  // Send the value to the gui.
  //gUpdateValue(&encoderReading);
  gUpdateValue(&angleTurned);
  gUpdateValue(&PotReading);
  gUpdateValue(&servoAngle_POT);
  gUpdateValue(&graphValue);
  gUpdateValue(&GUImode);
  gUpdateValue(&GateReading);
  gUpdateValue(&numPieces);
  gUpdateValue(&stepCount);
  gUpdateValue(&input_steps);
  gUpdateValue(&IR);

  //update durations
  unsigned long currTime = millis();
  servoDur = currTime - servoLast;
  encoderDur = currTime - encoderLast;
  stepperDur = currTime - stepperLast;

  //run motors

  //GUI mode
  if (GUImode) {
    //servo
    if(servoDur > 15) {
      myservo.write(servoAngle_GUI);
      servoLast = currTime;
    }

    //DC motor
    if(!motorRunning && motorGo) {
      motorRunning = 1;
    }
    if(fromHW) {
      digitalWrite(enMotorPin, HIGH);
      digitalWrite(L3Pin, LOW);
      digitalWrite(L4Pin, LOW);
      fromHW = 0;
    }
    else if(motorRunning) {

      analogWrite(enMotorPin, motorSpeed_GUI);
      angleTurned = encoderReading;
      if (motorAngle - angleTurned > THRESHOLD){ //angle is less than target
        turnCCW();
      }
      else if (motorAngle - angleTurned < -THRESHOLD) { //angle is greater than target
        turnCW();
      }
      else {
        stopMotor();
      }

    }

    //stepper
    if(stepDir) {
      digitalWrite(dirPin, HIGH);
    }
    else {
      digitalWrite(dirPin, LOW);
    }
    if (stepperGo) {
      if (stepCount < input_steps) {
        stepCount += stepper_stepOnce(stepperDur, currTime);
      }
      else {
        digitalWrite(stepPin, LOW);
        stepperReady = 1;
        stepperGo = 0;
      }
    }
    else {
      digitalWrite(stepPin, LOW);
    }
  }
  //hardware mode
  else {    
    //servo
    if(servoDur > 15) {
      myservo.write(servoAngle_POT);
      servoLast = currTime;
    }

    //encoder
    angleTurned = encoderReading;
    analogWrite(enMotorPin, motorSpeed_IR);    
    if(motorDir) {
      digitalWrite(L3Pin, HIGH);
      digitalWrite(L4Pin, LOW);
    }
    else {
      digitalWrite(L3Pin, LOW);
      digitalWrite(L4Pin, HIGH);
    } 

    if(angleTurned >= 2000 && motorDir == 1) {
      motorDir = 0;
    }
    else if (angleTurned <= 0 && motorDir == 0) {
      motorDir = 1;
    }

    //stepper
    digitalWrite(dirPin, HIGH);
    if (dropPart) {
      if (stepCount < SLOT_STEPS) {
        stepCount += stepper_stepOnce(stepperDur, currTime);
      }
      else {
        digitalWrite(stepPin, LOW);
        stepCount = 0;
        stepperReady = 1;
        dropPart = 0;
        numPieces += 1;
      }
    }
    else {
      digitalWrite(stepPin, LOW);
    }
  }

}

// This is where you setup your interface 
void gInit()
{
  GUILabelID = gAddLabel("GUI CONTROL MODE",1);
  gAddSpacer(1);

  gAddLabel("Dispenser Control",1);
  gAddSpacer(1);
  motorButtonDepId = gAddToggle("Deposit Part", &deposit);
  permissionLabel = gAddLabel("WAIT",2);
  motorRunningLabel = gAddLabel("Motor Status",2);
  gAddSpacer(1);
  pieceCountID = gAddSlider(0,100,"Pieces",&numPieces);
  gAddSlider(0,200,"Steps Taken:",&stepCount);
  gAddSpacer(1);
  gAddLabel("Servo/Motor Control",1);
  gAddSpacer(1);
  gAddSlider(0,200,"Step",&input_steps);
  stepperDirToggleID = gAddToggle("Servo Direc.", &stepDir);
  stepperButtonID = gAddButton("STEPPER: GO");
  gAddSpacer(1);
  rotaryServo = gAddRotarySlider(0,180,"SRV:",&servoAngle_GUI);
  gAddRotarySlider(0,255,"MS",&motorSpeed_GUI);
  gAddRotarySlider(-360,360,"MA",&motorAngle);
  gAddSpacer(1);  
  gAddSlider(-360,360,"Degrees Turned:",&angleTurned);
  //gAddToggle("Motor Direc.", &motorDir);
  motorButtonID = gAddButton("MOTOR: GO");
  gAddColumn();

  gAddLabel("SENSORS",1);
  gAddSpacer(1);

  // Last parameter in moving graph defines the size 10 = normal

  gAddMovingGraph("distance",0,1023, &IR, 10);
  gAddSlider(0,1023,"infared",&IR);
  gAddSpacer(1);
  gAddMovingGraph("Pot",0,1023,&PotReading,10);
  gAddSlider(0,1023,"Pot",&PotReading);
  gAddColumn();
  gAddLabel("MOTORS",1);
  gAddSpacer(1);
  gAddMovingGraph("encoder",minDistance,maxDistance,&angleTurned,10);
  motorDirID = gAddLabel("MOTOR DIR: ",2);
  gAddSlider(0,180,"Servo Angle",&servoAngle_POT);

  gAddSlider(0,1,"Gate sensor",&GateReading);
  gAddSlider(0,1,"GUI switch",&GUImode);

  //servoID = gAddSlider(0,180,"Servo",&servoAngle);
}

// Method called everytime a button has been pressed in the interface.
void gButtonPressed(int id)
{  
  if (stepperButtonID == id) {
    stepperGo = 1;
    stepCount = 0;
  }
  if (motorButtonID == id) {
    motorGo = 1;
  }
}

void gItemUpdated(int id)
{

  //update status
  if (motorButtonDepId == id) {
    if(deposit == 1) {
      gUpdateLabel(permissionLabel,"Permission Granted!");
    }
    else {
      gUpdateLabel(permissionLabel,"WAIT");
    }
  }

  if (pieceCountID == id) {
    if (stepperReady == 0) {
      gUpdateLabel(motorRunningLabel,"Stepping busy!");
    }
    else {
      gUpdateLabel(motorRunningLabel,"Ready for next part!");
    }
  }

}

int smooth(int inputPin) {
  // subtract the last reading:
  total= total - readings[readIndex];         
  // read from the sensor:  
  readings[readIndex] = analogRead(inputPin); 
  // add the reading to the total:
  total= total + readings[readIndex];       
  // advance to the next position in the array:  
  readIndex = readIndex + 1;                    

  // if we're at the end of the array...
  if (readIndex >= numReadings)              
    // ...wrap around to the beginning: 
    readIndex = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return average; 
}

int smoothPot(int inputPin) {
  // subtract the last reading:
  potTotal= potTotal - potReadings[potIndex];         
  // read from the sensor:  
  potReadings[potIndex] = analogRead(inputPin); 
  // add the reading to the total:
  potTotal= potTotal + potReadings[potIndex];     
  // advance to the next position in the array:  
  potIndex = potIndex + 1;                    

  // if we're at the end of the array...
  if (potIndex >= numReadings)              
    // ...wrap around to the beginning: 
    potIndex = 0;                           

  // calculate the average:
  potAverage = potTotal / numReadings;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return potAverage; 
}

void turnCCW() {
  gUpdateLabel(motorDirID,"Motor Dir: CCW");
  digitalWrite(L3Pin, HIGH); //more neg
  digitalWrite(L4Pin, LOW); 
}

void turnCW() {
  gUpdateLabel(motorDirID,"Motor Dir: CW");
  digitalWrite(L3Pin, LOW); //more pos
  digitalWrite(L4Pin, HIGH); 
}

void stopMotor() {
  motorRunning = 0;
  motorGo = 0;
  //analogWrite(enMotorPin, motorSpeed);    
  digitalWrite(L3Pin, LOW);
  digitalWrite(L4Pin, LOW); 
}

int stepper_stepOnce(long stepperDur, long currTime) {
  int stepDone = 0;

  if(stepperDur <= 50) {
    digitalWrite(stepPin, HIGH);
  }
  else {
    digitalWrite(stepPin, LOW);
  }
  if (stepperDur > 100) {
    stepperLast = currTime;
    stepDone = 1;
  }
  return stepDone;
}

void updateEncoders() {
  encoderPin_ACurr = digitalRead(encoderPin_A);
  encoderPin_BCurr = digitalRead(encoderPin_B);
  //if(encoderPin_ACurr == HIGH &&
  // encoderPin_ALast == LOW) {
  if(encoderPin_BCurr == HIGH) {
    encoderReading++;
  }
  else {
    encoderReading--;
  }
  //}
  //encoderPin_ALast = encoderPin_ACurr;
}

void doEncoderA(){

  // look for a low-to-high on channel A
  if (digitalRead(encoderPin_A) == HIGH) { 
    // check channel B to see which way encoder is turning
    if (digitalRead(encoderPin_B) == LOW) {  
      encoderReading = encoderReading + 1;         // CW
    } 
    else {
      encoderReading = encoderReading - 1;         // CCW
    }
  }
  else   // must be a high-to-low edge on channel A                                       
  { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoderPin_B) == HIGH) {   
      encoderReading = encoderReading + 1;          // CW
    } 
    else {
      encoderReading = encoderReading - 1;          // CCW
    }
  }          
  // use for debugging - remember to comment out
}

void doEncoderB(){

  // look for a low-to-high on channel B
  if (digitalRead(encoderPin_B) == HIGH) {   
    // check channel A to see which way encoder is turning
    if (digitalRead(encoderPin_A) == HIGH) {  
      encoderReading = encoderReading + 1;         // CW
    } 
    else {
      encoderReading = encoderReading - 1;         // CCW
    }
  }
  // Look for a high-to-low on channel B
  else { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoderPin_A) == LOW) {   
      encoderReading = encoderReading + 1;          // CW
    } 
    else {
      encoderReading = encoderReading - 1;          // CCW
    }
  }
}







