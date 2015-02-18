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
        }
        else if(pixy.blocks[j].signature == 2)
        {
          //turnMotorLeft
        }
      }
    }
  }  
}

