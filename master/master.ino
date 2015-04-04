//Example master I2C serial com, sends out commands and reads back status

#include <Wire.h>

#define NUM_BYTES 20
#define ORIENT 0
#define CAMERAMODULE 2
#define ORIENTATOR 3
#define FLUX 4
#define WIREAPP 4
#define PIECEPLACE 5

#define START 6

char response[NUM_BYTES+1]; //String array to hold response from slave
char query[NUM_BYTES+1]; //String array to send query to slave

int state;

void setup() {
  Wire.begin();  //Start I2c bus
  Serial.begin(9600);
  state = START;
}


void loop() {
  switch(state) {
    case CAMERAMODULE:
      Serial.println("In Camera");
      SendQuery(CAMERAMODULE,"A");
      delay(500);
      while(response[0] != 'Y') {
        Recieve(CAMERAMODULE,NUM_BYTES);
      }
      state = ORIENT;
      query[0] = 'C';
      query[1] = response[1];
      break;
    case ORIENT:
      Serial.println("ORient");
      Serial.println(query);
      SendQuery(ORIENTATOR,query);
      delay(1000);
      Recieve(ORIENTATOR,NUM_BYTES);
      if(response[0] == 'Y') {
        state = 1;
      }
      break;
    case 1:
      SendQuery(PIECEPLACE,"Move to Flux");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      SendQuery(FLUX,"F");
      while(response[0] != 'Y') {
        Recieve(FLUX,NUM_BYTES);
      }
      state = WIREAPP;
      break;
    case WIREAPP:
      Serial.println("WireAPP");
      SendQuery(PIECEPLACE,"Move to Wire");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      SendQuery(WIREAPP,"W");
      while(response[0] != 'Y') {
        Recieve(WIREAPP,NUM_BYTES);
      }
      state = PIECEPLACE;
      break;
   case PIECEPLACE:
      Serial.println("Place");
      SendQuery(PIECEPLACE,"Move Part");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      state = START;
      break;
   case START:
      Serial.println("Beginning process");
      SendQuery(ORIENTATOR,"R");
      while(response[0] != 'Y') {
        Recieve(ORIENTATOR,NUM_BYTES);
        Serial.println("Response is : ");
        Serial.println(response);
      }
      state = CAMERAMODULE;
      break;
   default:
     SendQuery(ORIENTATOR,"Hello");
     break;
  }  
}

/* function to send data to certain device,
device : device number to send message to (set up in that devices code
message : message to send to device 
*/

void SendQuery(int device, char message[]) {
  Wire.beginTransmission(device);
  Serial.println(message);
  Wire.write(message);
  Wire.endTransmission();
  delay(500);
  response[0] = 'N';
}


/* function to read response from device
device : device to query
numBytes : number of bytes to recieve

return value is stored in response variable
*/

void Recieve(int device, int numBytes) {
  Wire.requestFrom(device,NUM_BYTES);
  int i = 0;
  while(Wire.available()) //slave may send less than requested
  {
    char c = Wire.read();
    Serial.print(c);
    response[i] = c;
    i++;
  }
  Serial.println();
  delay(500);
}

