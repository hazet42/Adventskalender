#include <CheapStepper.h>
CheapStepper stepper; 
//Richtungszuweisung des Motors 
boolean moveClockwise = true;

void setup() {
  Serial.begin(9600);
  Serial.println("28BYJ-48 bereit.");
}

void loop() {
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


 for (int s=0; s<4096; s++){
   //Eine volle Umdrehung beinhaltet 4096 Schritte
   stepper.step(moveClockwise);
   //Ausgabe der aktuellen Motorposition in der Konsole
   int nStep = stepper.getStep(); 
   if (nStep%64==0)
   {
     Serial.print("current step position: "); Serial.print(nStep);
     Serial.println(); 
    }
  } 
  delay(1000);
  //Richtungswechsel nach vollständiger Umdrehung 
  moveClockwise = !moveClockwise;
}
