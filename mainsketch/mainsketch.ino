#include <CheapStepper.h>
#include <arduino-timer.h>
#include <EEPROM.h>

// Define the two steppers on different pins:
CheapStepper stepperX (8,9,10,11); 
CheapStepper stepperY (2,3,4,5);
//Richtungszuweisung des Motors 
boolean moveClockwise = true;
int tag = 1;
int position = 0;
int XOffset = 0;
int YOffset = 0;
int XPos = 0;
int YPos = 0;
int AdventX[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
int AdventY[24] = {0, 10, 20, 30, 40, 50, 100, 200, 300, 500, 1000, 1500, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

auto timer = timer_create_default(); // create a timer with default settings

bool move_mirrors(void *) {
  tag = tag+1;
  Serial.println("Tag: "); Serial.print(tag);
  
    position = AdventX[tag];
    stepperX.moveTo (moveClockwise, OffsetPositionX(position));
    EEPROM.write(0,position);

    position = AdventY[tag];
    stepperY.moveTo (moveClockwise, OffsetPositionY(position));
    EEPROM.write(1,position);

    Serial.print("Es ist der "); Serial.print(tag); Serial.println(". Dezember");
    Serial.print("Die Spiegel stehen auf X: "); Serial.print(OffsetPositionX(position));
    Serial.print(" und Y: "); Serial.println(OffsetPositionY(position));

    delay(1000);
    stepperX.off();
    stepperY.off();

  return true; // keep timer active? true
}

int OffsetPositionX(int currentposition){
  int offsetvalue;
  offsetvalue = currentposition + XOffset;
  if (offsetvalue > 4096) {
    offsetvalue = offsetvalue - 4096;
    XOffset = XOffset - 4096;
  }
  else if (offsetvalue < 0){
    offsetvalue = offsetvalue + 4096;
    XOffset = XOffset + 4096;
  }
  return offsetvalue;
}

int OffsetPositionY(int currentposition){
  int offsetvalue;
  offsetvalue = currentposition + YOffset;
  if (offsetvalue > 4096) {
    offsetvalue = offsetvalue - 4096;
    YOffset = YOffset - 4096;
  }
  else if (offsetvalue < 0){
    offsetvalue = offsetvalue + 4096;
    YOffset = YOffset + 4096;
  }
  return offsetvalue;
}

void setup() {
  Serial.begin(9600);
  Serial.println("28BYJ-48 bereit.");
  
  // set Stepper
  // ideal range: 10RPM (safe, high torque) - 22RPM (fast, low torque)
  stepperX.setRpm(10); 
  stepperY.setRpm(10);

  // call the  move mirrors function every 24 hours (86400000 millis)
  timer.every(86400000, move_mirrors);

  // Lese Positionen der Spiegel seit letztem Power-Off aus dem EEPROM
  XOffset = EEPROM.read(0) - stepperX.getStep();
  YOffset = EEPROM.read(1) - stepperY.getStep();
}

void loop() {

  // Solange keine Eingabe erfolgt wird die Spiegelposition angepasst:
  while (Serial.available() == 0) {
    timer.tick(); // tick the timer
  
  }

  int val = Serial.read() - '0';
  if (val == 1) { // set current position to offset 0
        position = stepperX.getStep();
        XOffset = position;
        EEPROM.write(0,position);
        position = stepperY.getStep();
        YOffset = position;
        EEPROM.write(1,position);
        Serial.println("Current step position set to reference (0;0) ");
     }
  
  else if (val == 0) // test for command 0 then turn off LED
  {  
    //nothing
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

  else if (val == 3) // manuelle Einstellung X-Richtung
  {
    Serial.println("Bewegung in X-Richtung. Auf welche Position soll der Spiegel fahren (0-4096)?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    stepperX.moveTo (moveClockwise, OffsetPositionX(position));
    delay(1000);
    EEPROM.write(0,position);
    stepperX.off();
  }

  else if (val == 4) // manuelle Einstellung X-Richtung
  {
    Serial.println("Bewegung in Y-Richtung. Auf welche Position soll der Spiegel fahren (0-4096)?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    Serial.print("Moving to: "); Serial.println(OffsetPositionY(position));
    stepperY.moveTo (moveClockwise, OffsetPositionY(position));
    delay(1000);
    EEPROM.write(1,position);
    stepperY.off();
  }

  else if (val == 5) // manuelle Einstellung X-Richtung
  {
    Serial.println("Scannerpositionen: ");
    position = stepperX.getStep();
    Serial.print("X: ");Serial.print(OffsetPositionX(position));
    Serial.print("; X Offset: ");Serial.print(XOffset);
    position = stepperY.getStep();
    Serial.print(" | Y: ");Serial.print(OffsetPositionY(position));
    Serial.print("; Y Offset: ");Serial.println(YOffset);
    
  }

  else if (val == 6) // fast forward
  {
    for (int i=1; i<25; i++) {

      Serial.print("Es ist der "); Serial.print(i); Serial.println(". Dezember");

      position = AdventX[i];
      stepperX.moveTo (moveClockwise, OffsetPositionX(position));
      Serial.print("Die Spiegel stehen auf X: "); Serial.print(OffsetPositionX(position));

      position = AdventY[i];
      stepperY.moveTo (moveClockwise, OffsetPositionY(position));
      Serial.print(" und Y: "); Serial.println(OffsetPositionY(position));
     
      delay(1000);
    }
  stepperX.off();
  stepperY.off();
  }
  delay(500);
}
