#include <Pixy.h>
#include <SPI.h> 
#include <Wire.h>

//cam variables
int camGo = 0;
Pixy pixy;
const int NUM_READINGS = 6;
const int LEFT_THRESHOLD = 39;
const int RIGHT_THRESHOLD = 43;

const char RIGHT_SHARP = 'A';
const char RIGHT_ROUND = 'B';
const char LEFT_SHARP = 'C';
const char LEFT_ROUND = 'D';

char orientation = '?';
char isDone = 'N';

int refreshCount = 0;

//cam smoothing
int greenAverage = 0;
int greenReadings[NUM_READINGS];
int greenReadIndex = 0;
int greenTotal = 0;

int yellowAverage = 0;
int yellowReadings[NUM_READINGS];      
int yellowReadIndex = 0;
int yellowTotal = 0;
int greenIndex = 0;
int yellowIndex = 0;

char message[2]; 


void setup(){
  pixy.init();

  Wire.begin(2);
  Wire.onReceive(startEvent);
  Wire.onRequest(getOrientation);
  Serial.begin(9600);
  Serial.print("Starting...\n");

}

void loop() {  
  if(camGo) {
    //Serial.println("Camera Go");
    //Serial.print("REFRESH: ");
    //Serial.println(refreshCount);
    //Serial.println("Scanning...");
    static int i = 0;
    int j;
    uint16_t blocks;
    char buf[32]; 
    greenIndex = 0;
    yellowIndex = 0;
    //get orientation
    blocks = pixy.getBlocks();
    if (blocks) {
      //Serial.println("Blocks Found...");
      i++;

      // do this (print) every 50 frames because printing every
      // frame would bog down the Arduino
      if (i%30==0)
      {
        if(refreshCount <= NUM_READINGS) {
          refreshCount += 1;
          sprintf(buf, "Detected %d:\n", blocks);
          Serial.print(buf);
          for (j=0; j<blocks; j++)
          {
            sprintf(buf, "  block %d: ", j);
            Serial.print(buf); 
            pixy.blocks[j].print();
            if(pixy.blocks[j].signature == 2) 
            {
              greenIndex = j;
            }
            else if(pixy.blocks[j].signature == 1)
            {
              yellowIndex = j;
            }
          }
          int greenHeight = greenSmooth(pixy.blocks[greenIndex].height);
          int yellowHeight = yellowSmooth(pixy.blocks[yellowIndex].height);
          Serial.println(greenHeight);
          Serial.println(yellowHeight);
          if(greenHeight > yellowHeight){
            Serial.println("Corner on the right");
            if(greenHeight < RIGHT_THRESHOLD) {
              Serial.println("   Sharp");
              orientation = RIGHT_SHARP;
            }
            else {
              Serial.println("   Round");
              orientation = RIGHT_ROUND;
            }
          }
          else {
            Serial.println("Corner on the left");
            if(yellowHeight < LEFT_THRESHOLD) {
              Serial.println("   Sharp");
              orientation = LEFT_SHARP;
            }
            else {
              Serial.println("   Round");
              orientation = LEFT_ROUND;
            }
          }
        }
        else {
              camGo = 0;
              refreshCount = 0;
              isDone = 'Y';
              Serial.println("==== DONE ====");
        }
      }
    }


  }
}

void startEvent(int maxCount) {
  char c = Wire.read();
  Serial.print("START: ");
  Serial.println(c);
  camGo = 1;
  refreshCount = 0;
  isDone = 'N';
}

void getOrientation() {
  message[0] = isDone;
  message[1] = orientation;
  //Serial.print("Message: ");
  //Serial.println(message);
  Wire.write(message);
}

int greenSmooth(int height) {
  // subtract the last reading:
  greenTotal= greenTotal - greenReadings[greenReadIndex];         
  // read from the sensor:  
  greenReadings[greenReadIndex] = height;
  // add the reading to the total:
  greenTotal= greenTotal + greenReadings[greenReadIndex];       
  // advance to the next position in the array:  
  greenReadIndex = greenReadIndex + 1;                    

  // if we're at the end of the array...
  if (greenReadIndex >= NUM_READINGS)              
    // ...wrap around to the beginning: 
    greenReadIndex = 0;                           

  // calculate the average:
  greenAverage = greenTotal / NUM_READINGS;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return greenAverage; 
}

int yellowSmooth(int height) {
  // subtract the last reading:
  yellowTotal= yellowTotal - yellowReadings[yellowReadIndex];         
  // read from the sensor:  
  yellowReadings[yellowReadIndex] = height;
  // add the reading to the total:
  yellowTotal= yellowTotal + yellowReadings[yellowReadIndex];       
  // advance to the next position in the array:  
  yellowReadIndex = yellowReadIndex + 1;                    

  // if we're at the end of the array...
  if (yellowReadIndex >= NUM_READINGS)              
    // ...wrap around to the beginning: 
    yellowReadIndex = 0;                           

  // calculate the average:
  yellowAverage = yellowTotal / NUM_READINGS;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return yellowAverage; 
}















