//Example master I2C serial com, sends out commands and reads back status

#include <Wire.h>

#define NUM_BYTES 20
#define ORIENTOR 0
#define CAMERAMODULE 2
#define ORIENTATOR 3
#define FLUX 4
#define WIREAPP 4
#define PIECEPLACE 5

#define START 1
#define IDENTIFY 2
#define ORIENT 3
#define APPLY_FLUX 4
#define APPLY_WIRE 5
#define MOVE_PART 6
#define DONE 7

const char PIECE_A = 'A';
const char PIECE_B = 'B';
const char PIECE_C = 'C';
const char PIECE_D = 'D';

char response[NUM_BYTES+1]; //String array to hold response from slave
char query[NUM_BYTES+1]; //String array to send query to slave
char mode = PIECE_A;

int state;
int switch1 = 6;
int switch2 = 7; /* needs to be adjusted */
int count = 0;

void setup() {
  pinMode(switch1,INPUT);
  pinMode(switch2,INPUT);
  Wire.begin();  //Start I2c bus
  startUp();
  count = 0;
  Serial.begin(9600);
  state = START;
}


void loop() {
  switch(state) {
    case IDENTIFY:
      Serial.println("In Camera");
      char cameraMSG[1];
      cameraMSG[0] = mode;
      SendQuery(CAMERAMODULE,cameraMSG);
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
        state = APPLY_FLUX;
      }
      break;
    case APPLY_FLUX:
      SendQuery(PIECEPLACE,"Move to Flux");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      SendQuery(FLUX,"F");
      while(response[0] != 'Y') {
        Recieve(FLUX,NUM_BYTES);
      }
      state = APPLY_WIRE;
      break;
    case APPLY_WIRE:
      Serial.println("WireAPP");
      SendQuery(PIECEPLACE,"Move to Wire");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      SendQuery(WIREAPP,"W");
      while(response[0] != 'Y') {
        Recieve(WIREAPP,NUM_BYTES);
      }
      state = MOVE_PART;
      break;
   case MOVE_PART:
      Serial.println("Place");
      SendQuery(PIECEPLACE,"Move Part");
      while(response[0] != 'Y') {
        Recieve(PIECEPLACE,NUM_BYTES);
      }
      count = count + 1;
      Serial.print("Count: ");
      Serial.println(count);
      if(count >= 20) {
        state = DONE;
      }
      else {
        state = START;
      }
      break;
   case START:
      Serial.println("Beginning process");
      SendQuery(ORIENTATOR,"R");
      while(response[0] != 'Y') {
        Recieve(ORIENTATOR,NUM_BYTES);
        Serial.println("Response is : ");
        Serial.println(response);
      }
      state = IDENTIFY;
      break;
   case DONE:
     Serial.println("Done.");
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

void startUp() {
  bool switch1Val = digitalRead(switch1);
  bool switch2Val = digitalRead(switch2);
  
  if(switch1Val && switch2Val) {
    mode = PIECE_D;
  }
  else if (switch1Val && !switch2Val) {
    mode = PIECE_C;
  }
  else if (!switch1Val && switch2Val) {
    mode = PIECE_B;
  }
  else {
    mode = PIECE_A;
  }
  
  char setupMsg[2];
  setupMsg[0] = 'S';
  setupMsg[1] = mode;
  
  SendQuery(WIREAPP,setupMsg);
  SendQuery(PIECEPLACE,setupMsg);
}

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

