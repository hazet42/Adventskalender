#include <CheapStepper.h>
CheapStepper stepper; 
//Richtungszuweisung des Motors 
boolean moveClockwise = true;

void setup() {
  Serial.begin(9600);
  Serial.println("28BYJ-48 bereit.");
}

void loop() {
  // Example for Serial Input - Must now be adapted to Arduino Case!
  int val = Serial.read() - '0';
  if (val == 1) { // test for command 1 then turn on LED
    //Serial.println("RELAY on");
    digitalWrite(RELAY1, LOW); // turn on LED
    Serial.println("led off");

  }
  else if (val == 0) // test for command 0 then turn off LED
  {
    // Serial.println("RELAY OFF");
    digitalWrite(RELAY1, HIGH); // turn off LED
    Serial.println("led ON");
  }
  Serial.println("Arduino program");
  delay(500);
}
