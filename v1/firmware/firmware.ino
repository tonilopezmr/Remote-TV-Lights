#include <IRremote.h>

const int RECV_PIN = 4;
const int TURN_ON_OFF = 11;

long codes[5] = {0x11CC1A2B, 0x7577A252, 0xB08F934B, 0xA7F1EAA6, 0x20DF8D72};

boolean stateLights;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(TURN_ON_OFF, OUTPUT);
  
  stateLights = false;
}

void loop() {
  if(irrecv.decode(&results)) {       
    long value = results.value;
    if(value != 0xFFFFFFFF && match(value)) {
      if(stateLights) {
        stateLights = false; 
        digitalWrite(TURN_ON_OFF, LOW);
      } else {
        stateLights = true;     
        digitalWrite(TURN_ON_OFF, HIGH);
      }
    }     
    irrecv.resume();
  }
}

boolean match(long remoteValue) {
  for (int i = 0; i < 5; i = i + 1) {
    if(remoteValue == codes[i]) {
      return true;
    }
  }
 
  return false;
}
