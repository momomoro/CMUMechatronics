/*sample sketch on how to setup slave for I2C comm,
this example assumes we are the hopper module */

#include <Wire.h>

#define NUM_BYTES 20
#define HOPPER 1
#define ORIENTATOR 2
#define WIRE_APP 3
#define FLUX 4
#define PIECE_PLACE 5

char query[NUM_BYTES+1]; //Array to hold query from master
int recieved; //flag if query has been recieved

void setup() {
  Wire.begin(HOPPER); //Join i2c bus on address #1 (hopper address)
  Wire.onReceive(recieveEvent); //register event
  Wire.onRequest(requestEvent);
  recieved = 0;
  Serial.begin(9600);

}

void loop() {
  if(recieved) {
    noInterrupts();
    //do stuff
    interrupts();
  }
  recieved = 0;
}

//executes whenever data is recieve from master
void recieveEvent(int numBytes) {
  while(Wire.available()) //loop through Query
  {
    int i = 0;
    char c = Wire.read();
    Serial.print(c);
    query[i] = c;
    i++;
  }
  recieved = 1;
}

int x = 0; //this is only a test

//function that executes whenever data is requested by master
void requestEvent()
{
  Wire.write("Hello");
  x++; //just a test
}
