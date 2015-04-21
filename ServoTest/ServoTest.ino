#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 10;    // variable to store the servo position 
int val = 10;
 
void setup() 
{ 
  Serial.begin(9600);
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object 
} 
 
 
void loop() 
{ 
  for(pos = 65; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  /*if (Serial.available()) {
    val = Serial.parseInt();
    Serial.println(val);
    if (val < 180 && val >= 10) {
      pos = val;
    }
  }
  
  myservo.write(pos);
  delay(20);*/
}
