//Example master I2C serial com, sends out commands and reads back status

#include <Wire.h>

#define NUM_BYTES 20
#define HOPPER 1
#define ORIENTATOR 2
#define WIRE_APP 3
#define FLUX 4
#define PIECE_PLACE 5

char response[NUM_BYTES+1]; //String array to hold response from slave

void setup() {
  Wire.begin();  //Start I2c bus
  Serial.begin(9600);
}


void loop() {
  for(int i = 0; i < 6; i++) {
    SendQuery(i,"begin");
    Recieve(i,NUM_BYTES);
    Serial.println(response); //Check for response here
    delay(500);
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

