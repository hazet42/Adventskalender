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
int XNow = 0;
int YNow = 0;
int XOffset = 0;
int YOffset = 0;
byte lb = 0;
byte hb = 0;
int XPos = 0;
int YPos = 0;
int AdventX[25] = {4075, 4075, 4090, 10,   3,    4075, 4075, 4075, 4070, 4076, 4063, 4063, 5,   35, 10, 4068, 4068,    5, 4075, 4075, 4075, 4077, 4085, 0, 0};
int AdventY[25] = {0,    4030, 3750, 3850, 3800,   10,   40, 4020, 3950, 4060,   60,   68, 0, 3750, 18, 3880,  320, 3880, 3970, 4050,   20,   37, 3800, 0, 0};

auto timer = timer_create_default(); // create a timer with default settings

bool move_mirrors(void *) {
  tag = tag+1;
  EEPROM.write(4,tag);
  Serial.println("Tag: "); Serial.print(tag);
  
    position = AdventX[tag-1];
    OffsetPositionX(position);
        
    position = AdventY[tag-1];
    OffsetPositionY(position);

    Serial.print("Es ist der "); Serial.print(tag); Serial.println(". Dezember");
    Serial.print("Die Spiegel stehen auf X: "); Serial.print(XNow);
    Serial.print(" und Y: "); Serial.println(YNow);

    delay(1000);
    stepperX.off();
    stepperY.off();

  return true; // keep timer active? true
}

void OffsetPositionX(int newposition){
  int offsetvalue;
  offsetvalue = newposition - XNow;
  if (offsetvalue > 4096) {
    offsetvalue = offsetvalue - 4096;
  }
  else if (offsetvalue < 0){
    offsetvalue = offsetvalue + 4096;
  }
  stepperX.move(moveClockwise, offsetvalue);
        XNow = newposition;
        lb = lowByte(newposition);
        hb = highByte(newposition);
        EEPROM.write(0,lb); 
        EEPROM.write(1,hb);
}

void OffsetPositionY(int newposition){
  int offsetvalue;
  offsetvalue = newposition - YNow;
  if (offsetvalue > 4096) {
    offsetvalue = offsetvalue - 4096;
  }
  else if (offsetvalue < 0){
    offsetvalue = offsetvalue + 4096;
  }
  stepperY.move(moveClockwise,offsetvalue);
        YNow = newposition;
        lb = lowByte(newposition);
        hb = highByte(newposition);
        EEPROM.write(0,lb); 
        EEPROM.write(1,hb);
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
  XNow = EEPROM.read(1)*256 + EEPROM.read(0);
  YNow = EEPROM.read(3)*256 + EEPROM.read(2);
  tag = EEPROM.read(4);
}

void loop() {

  // Solange keine Eingabe erfolgt wird die Spiegelposition angepasst:
  while (Serial.available() == 0) {
    timer.tick(); // tick the timer
  
  }

  int val = Serial.read() - '0';
  if (val == 1) { // set current position to 
        XNow = 0;
        lb = lowByte(XNow);
        hb = highByte(XNow);
        EEPROM.write(0,lb); 
        EEPROM.write(1,hb);
        YNow = 0;
        lb = lowByte(YNow);
        hb = highByte(YNow);
        EEPROM.write(2,lb);
        EEPROM.write(3,hb);
        Serial.println("Current step position set to reference (0;0) ");
     }
  
  else if (val == 0) // Set EEPROM to Startvalue
  { 
    position = 4075;
    lb = lowByte(position);
    hb = highByte(position);
    EEPROM.write(0,lb); 
    EEPROM.write(1,hb);
    EEPROM.write(2,0);
    EEPROM.write(3,0);
    Serial.println("Reset EEPROM. ");
    XNow = 4075;
    YNow = 0;
  }

  else if (val == 2) // test for command 2 adjust "tag"
    {  
      bool success;
      Serial.println("Gib den aktuellen Tag ein:");
      while (Serial.available() == 0) {
        // wait for Input
      } 
      tag = Serial.parseInt();
      EEPROM.write(4,tag);
      Serial.print("Neu eingestellter Tag: ");Serial.print(tag);Serial.print("\n");
       
      position = AdventX[tag-1];
      OffsetPositionX(position);
        
      position = AdventY[tag-1];
      OffsetPositionY(position);

      Serial.print("Es ist der "); Serial.print(tag); Serial.println(". Dezember");
      Serial.print("Die Spiegel stehen auf X: "); Serial.print(XNow);
      Serial.print(" und Y: "); Serial.println(YNow);

      delay(1000);
      stepperX.off();
      stepperY.off();

    }

  else if (val == 3) // manuelle Einstellung X-Richtung
  {
    Serial.println("Bewegung in X-Richtung. Auf welche Position soll der Spiegel fahren (0-4096)?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    Serial.print("Moving to: "); Serial.println(position);
    OffsetPositionX(position);
    delay(1000);
    stepperX.off();
  }

  else if (val == 4) // manuelle Einstellung X-Richtung
  {
    Serial.println("Bewegung in Y-Richtung. Auf welche Position soll der Spiegel fahren (0-4096)?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    Serial.print("Moving to: "); Serial.println(position);
    OffsetPositionY(position);
    delay(1000);
    stepperY.off();
  }

  else if (val == 5) // Anzeige aktuelle Einstellung X-Richtung
  {
    Serial.println("Scannerpositionen: ");
    position = stepperX.getStep();
    Serial.print("X: ");Serial.print(XNow);
    position = stepperY.getStep();
    Serial.print(" | Y: ");Serial.println(YNow);    
  }

  else if (val == 6) // fast forward
  {
    for (int i=1; i<26; i++) {

      Serial.print("Es ist der "); Serial.print(i); Serial.println(". Dezember");

      position = AdventX[i];
      OffsetPositionX(position);
      Serial.print("Die Spiegel stehen auf X: "); Serial.print(XNow);

      position = AdventY[i];
      OffsetPositionY(position);
      Serial.print(" und Y: "); Serial.println(YNow);
     
      delay(1000);
    }
  stepperX.off();
  stepperY.off();
  }


  else if (val == 7) // move stepwise in x
  {
    Serial.println("Bewegung in x-Richtung. Wieviele Schritte vorwärts?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    Serial.print("Moving by: "); Serial.println(position);
    stepperX.move(moveClockwise, position);
    delay(1000);
    stepperX.off();
  }

  else if (val == 8) // move stepwise in y
  {
    Serial.println("Bewegung in y-Richtung. Wieviele Schritte vorwärts?");
    while (Serial.available() == 0) {
        // wait for Input
      }
    position = Serial.parseInt();
    Serial.print("Moving by: "); Serial.println(position);
    stepperY.move(moveClockwise, position);
    delay(1000);
    stepperY.off();
  }



  delay(500);
}
