int stepPin = 9;  
int dirPin = 8;
/*volatile int encoderPin_ACurr = LOW;
volatile int encoderPin_BCurr = LOW;
int L3Pin = 9;
int L4Pin = 10;
int enMotorPin = 11;
int encoderPin_B = 2;
int encoderPin_A = 3;

volatile int encoderPin_ALast = LOW;
volatile int encoderPos = 0;*/
int count = 0;
int MAX = 1000;

long lastTime = 0;
long enlastTime = 0;

void setup()
{
  //pinMode(L3Pin, OUTPUT);
  //pinMode(L4Pin, OUTPUT);
  //pinMode(enMotorPin, OUTPUT);
  //pinMode(encoderPin_B, INPUT);
  //pinMode(encoderPin_A, INPUT);
  //attachInterrupt(0, updateEncoders, CHANGE);
  //attachInterrupt(1, updateEncoders, CHANGE);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);   
  digitalWrite(dirPin, HIGH);
  Serial.begin(9600);          //  setup serial
}

//rotates a quarter, then pauses, then rotates again
void loop()
{


  //send pulses every 0.01s
  //each pulse is a rotation
  if(count < 10) {    
    //encoder motor controls
/*    int angle = encoderPos;
    digitalWrite(enMotorPin, HIGH);
    digitalWrite(L3Pin, HIGH);
    digitalWrite(L4Pin, LOW);  
    Serial.println(angle);
    Serial.println("A B");
    if(encoderPin_ACurr == HIGH) {
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
    Serial.print(" ");
    if(encoderPin_BCurr == HIGH) {
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
    Serial.println("");*/
    long dur = millis() - lastTime;
    //stepper motor controls
    count += stepper_stepOnce(dur, millis());
    //delay(20);
  }
  else { //at 50 rotations, pause for a second
    digitalWrite(stepPin, LOW);
    delay(1000);
    count = 0;
  }
}

int stepper_stepOnce(long stepperDur, long currTime) {
  int stepDone = 0;

  if(stepperDur <= 100) {
    digitalWrite(stepPin, HIGH);
  }
  else {
    digitalWrite(stepPin, LOW);
  }
  if (stepperDur > 200) {
    lastTime = currTime;
    stepDone = 1;
  }
  return stepDone;
}

/*void updateEncoders() {
  encoderPin_ACurr = digitalRead(encoderPin_A);
  encoderPin_BCurr = digitalRead(encoderPin_B);
  if(encoderPin_ACurr == HIGH &&
    encoderPin_ALast == LOW) {
    if(encoderPin_BCurr == HIGH) {
      encoderPos++;
    }
    else {
      encoderPos--;
    }
  }
  encoderPin_ALast = encoderPin_ACurr;
}
*/

