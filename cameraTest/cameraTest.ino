/*
* Spring 2015
* Will Milner    CMU mechatronics team L
* 
* Simple hello world program for use with pixymon and the CMUcam5
* detects all blocks in a given frame and turns a motor based on
* the block detected
*/


#include <SPI.h>  
#include <Pixy.h>

Pixy pixy;
const int numReadings = 10;
const int LEFT_THRESHOLD = 40;
const int RIGHT_THRESHOLD = 40;

int greenAverage = 0;
int greenReadings[numReadings];
int greenReadIndex = 0;
int greenTotal = 0;

int yellowAverage = 0;
int yellowReadings[numReadings];      
int yellowReadIndex = 0;
int yellowTotal = 0;


void setup()
{

  Serial.begin(9600);
  Serial.print("Starting...\n");

  pixy.init();
}

void loop()
{
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32]; 
  int greenIndex = 0;
  int yellowIndex = 0;
  
  blocks = pixy.getBlocks();
  
  if (blocks)
  {
    i++;
    
    if (i%50==0)
    {
      sprintf(buf, "Detected %d:\n", blocks);
      Serial.print(buf);
      for (j=0; j<blocks; j++)
      {
        sprintf(buf, "  block %d: ", j);
        Serial.print(buf); 
        pixy.blocks[j].print();
        if(pixy.blocks[j].signature == 1) 
        {
          //turnMotorRight
          greenIndex = j;
        }
        else if(pixy.blocks[j].signature == 2)
        {
          //turnMotorLeft
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
        }
        else {
          Serial.println("   Round");
        }
      }
      else {
        Serial.println("Corner on the left");
        if(yellowHeight < LEFT_THRESHOLD) {
          Serial.println("   Sharp");
        }
        else {
          Serial.println("   Round");
        }
      }
      
    }
  }  
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
  if (greenReadIndex >= numReadings)              
    // ...wrap around to the beginning: 
    greenReadIndex = 0;                           

  // calculate the average:
  greenAverage = greenTotal / numReadings;         
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
  if (yellowReadIndex >= numReadings)              
    // ...wrap around to the beginning: 
    yellowReadIndex = 0;                           

  // calculate the average:
  yellowAverage = yellowTotal / numReadings;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return yellowAverage; 
}

