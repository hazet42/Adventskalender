#include <CheapStepper.h>
#include <arduino-timer.h>

CheapStepper stepper; 
//Richtungszuweisung des Motors 
boolean moveClockwise = true;
int tag = 1;

auto timer = timer_create_default(); // create a timer with default settings

bool move_mirrors(void *) {
  tag = tag+1;
  Serial.println("Tag: "); Serial.print(tag);
  // Add repositioning of mirrors here
  return true; // keep timer active? true
}

void setup() {
  Serial.begin(9600);
  Serial.println("28BYJ-48 bereit.");
  
  // call the  move mirrors function every 24 hours (86400000 millis)
  timer.every(86400000, move_mirrors);
}

void loop() {

  // Solange keine Eingabe erfolgt wird die Spiegelposition angepasst:
  while (Serial.available() == 0) {
    timer.tick(); // tick the timer

  }

  int val = Serial.read() - '0';
  if (val == 1) { // test for command 1 then turn on LED
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
  }
  else if (val == 0) // test for command 0 then turn off LED
  {  
    //Richtungswechsel nach vollständiger Umdrehung 
    moveClockwise = !moveClockwise;
  }

  else if (val == 2) // test for command 2 adjust "tag"
    {  

      Serial.println("Gib den aktuellen Tag ein:");
      while (Serial.available() == 0) {
        // wait for Input
      } 
      tag = Serial.parseInt();
      Serial.print("Neu eingestellter Tag: ");Serial.print(tag);Serial.print("\n");
    }

  delay(500);
}
