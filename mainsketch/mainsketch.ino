#include <CheapStepper.h>
CheapStepper stepper; 
//Richtungszuweisung des Motors 
boolean moveClockwise = true;

void setup() {
  Serial.begin(9600);
  Serial.println("28BYJ-48 bereit.");
}

void loop() {
 for (int s=0; s<4096; s++){
   //Eine volle Umdrehung beinhaltet 4096 Schritte
   stepper.step(moveClockwise);
   //Ausgabe der aktuellen Motorposition in der Konsole
   int nStep = stepper.getStep(); if (nStep%64==0)
   {
     Serial.print("current step position: "); Serial.print(nStep);
     Serial.println(); 
    }
  } 
  delay(1000);
  //Richtungswechsel nach vollständiger Umdrehung 
  moveClockwise = !moveClockwise;
}
