//Example master I2C serial com, sends out commands and reads back status

#include <Wire.h>

#define NUM_BYTES 20
#define CAMERAMODULE 1
#define ORIENTATOR 2
#define FLUX 3
#define WIREAPP 4
#define PIECEPLACE 5

char response[NUM_BYTES+1]; //String array to hold response from slave
char query[NUM_BYTES+1]; //String array to send query to slave

void setup() {
  Wire.begin();  //Start I2c bus
  Serial.begin(9600);
}


void loop() {
  switch(response[0]) {
    case 'C':
      SendQuery(ORIENTATOR,"Camera module message");
      delay(1000);
      Recieve(ORIENTATOR,NUM_BYTES);
      break;
    case 'O':
      SendQuery(PIECEPLACE,"distance to move");
      delay(1000);
      SendQuery(FLUX,"distance to step");
      delay(1000);
      Recieve(FLUX,NUM_BYTES);
      break;
    case 'F':
      SendQuery(PIECEPLACE,"distance to move");
      delay(1000);
      SendQuery(WIREAPP,"distance to step");
      delay(1000);
      Recieve(WIREAPP,NUM_BYTES);
      break;
   case 'W':
      SendQuery(PIECEPLACE,"distance to move");
      break;
   default:
      SendQuery(CAMERAMODULE,"Begin process");
      delay(5000);
      Recieve(CAMERAMODULE,NUM_BYTES);
  }  
}

/* function to send data to certain device,
device : device number to send message to (set up in that devices code
message : message to send to device 
*/

void SendQuery(int device, char message[]) {
  Wire.beginTransmission(device);
  Wire.write(message);
  Wire.endTransmission();
  delay(500);
}


/* function to read response from device
device : device to query
numBytes : number of bytes to recieve

return value is stored in response variable
*/

void Recieve(int device, int numBytes) {
  Wire.requestFrom(device,NUM_BYTES);
  while(Wire.available()) //slave may send less than requested
  {
    int i = 0;
    char c = Wire.read();
    Serial.print(c);
    response[i] = c;
    i++;
  }
  delay(500);
}

