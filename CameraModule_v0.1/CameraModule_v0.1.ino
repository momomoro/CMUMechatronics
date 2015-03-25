#include <Pixy.h>
#include <SPI.h> 
#include <Wire.h>

//cam variables
int camGo = 0;
Pixy pixy;
const int LEFT_SIG = 1;
const int RIGHT_SIG = 2;

const int NUM_READINGS = 6;
const int MAX_READINGS = 20;
const int LEFT_THRESHOLD_A = 35;
const int RIGHT_THRESHOLD_A = 35;
const int LEFT_THRESHOLD_B = 51;
const int RIGHT_THRESHOLD_B = 55;
const int LEFT_THRESHOLD_C = 51;
const int RIGHT_THRESHOLD_C = 55;
const int LEFT_THRESHOLD_D = 51;
const int RIGHT_THRESHOLD_D = 55;

int LEFT_THRESHOLD = LEFT_THRESHOLD_A;
int RIGHT_THRESHOLD = RIGHT_THRESHOLD_A;

const char RIGHT_SHARP = 'A';
const char RIGHT_ROUND = 'B';
const char LEFT_SHARP = 'C';
const char LEFT_ROUND = 'D';

const char MODE_A = 'A';
const char MODE_B = 'B';
const char MODE_C = 'C';
const char MODE_D = 'D';

char orientation = '?';
char isDone = 'N';
char mode = MODE_A;

int refreshCount = 0;

//cam smoothing
int leftAverage = 0;
int leftReadings[NUM_READINGS];
int leftReadIndex = 0;
int leftTotal = 0;

int rightAverage = 0;
int rightReadings[NUM_READINGS];      
int rightReadIndex = 0;
int rightTotal = 0;
int leftIndex = 0;
int rightIndex = 0;

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
  switch(mode) {
    case(MODE_A): 
      LEFT_THRESHOLD = LEFT_THRESHOLD_A;
      RIGHT_THRESHOLD = RIGHT_THRESHOLD_A;
      break;
    case(MODE_B): 
      LEFT_THRESHOLD = LEFT_THRESHOLD_B;
      RIGHT_THRESHOLD = RIGHT_THRESHOLD_B;
      break;
    case(MODE_C): 
      LEFT_THRESHOLD = LEFT_THRESHOLD_C;
      RIGHT_THRESHOLD = RIGHT_THRESHOLD_C;
      break;
    case(MODE_D): 
      LEFT_THRESHOLD = LEFT_THRESHOLD_D;
      RIGHT_THRESHOLD = RIGHT_THRESHOLD_D;
      break;
    default:
      LEFT_THRESHOLD = 0;
      RIGHT_THRESHOLD = 0;
      break;
  }
  if(camGo) {
    //Serial.println("Camera Go");
    //Serial.print("REFRESH: ");
    //Serial.println(refreshCount);
    //Serial.println("Scanning...");
    static int i = 0;
    int j;
    uint16_t blocks;
    char buf[32]; 
    leftIndex = 0;
    rightIndex = 0;
    //get orientation
    blocks = pixy.getBlocks();
    if (blocks) {
      //Serial.println("Blocks Found...");
      i++;

      // do this (print) every 50 frames because printing every
      // frame would bog down the Arduino
      if (i%30==0)
      {
        if(refreshCount <= NUM_READINGS &&
        refreshCount <= MAX_READINGS) {
          refreshCount += 1;
          sprintf(buf, "Detected %d:\n", blocks);
          Serial.print(buf);
          for (j=0; j<blocks; j++)
          {
            sprintf(buf, "  block %d: ", j);
            Serial.print(buf); 
            pixy.blocks[j].print();
            if(pixy.blocks[j].signature == LEFT_SIG) 
            {
              leftIndex = j;
            }
            else if(pixy.blocks[j].signature == RIGHT_SIG)
            {
              rightIndex = j;
            }
          }
          int leftHeight = leftSmooth(pixy.blocks[leftIndex].height);
          int rightHeight = rightSmooth(pixy.blocks[rightIndex].height);
          Serial.println(leftHeight);
          Serial.println(rightHeight);
          if(leftHeight > rightHeight){
            Serial.println("Corner on the right");
            if(leftHeight < RIGHT_THRESHOLD) {
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
            if(rightHeight < LEFT_THRESHOLD) {
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
  mode = c;
  isDone = 'N';
}

void getOrientation() {
  message[0] = isDone;
  message[1] = orientation;
  //Serial.print("Message: ");
  //Serial.println(message);
  Wire.write(message);
}

int leftSmooth(int height) {
  // subtract the last reading:
  leftTotal= leftTotal - leftReadings[leftReadIndex];         
  // read from the sensor:  
  leftReadings[leftReadIndex] = height;
  // add the reading to the total:
  leftTotal= leftTotal + leftReadings[leftReadIndex];       
  // advance to the next position in the array:  
  leftReadIndex = leftReadIndex + 1;                    

  // if we're at the end of the array...
  if (leftReadIndex >= NUM_READINGS)              
    // ...wrap around to the beginning: 
    leftReadIndex = 0;                           

  // calculate the average:
  leftAverage = leftTotal / NUM_READINGS;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return leftAverage; 
}

int rightSmooth(int height) {
  // subtract the last reading:
  rightTotal= rightTotal - rightReadings[rightReadIndex];         
  // read from the sensor:  
  rightReadings[rightReadIndex] = height;
  // add the reading to the total:
  rightTotal= rightTotal + rightReadings[rightReadIndex];       
  // advance to the next position in the array:  
  rightReadIndex = rightReadIndex + 1;                    

  // if we're at the end of the array...
  if (rightReadIndex >= NUM_READINGS)              
    // ...wrap around to the beginning: 
    rightReadIndex = 0;                           

  // calculate the average:
  rightAverage = rightTotal / NUM_READINGS;         
  // send it to the computer as ASCII digits
  //Serial.println(average);   
  return rightAverage; 
}















