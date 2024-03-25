#include "Functions.h"

Elevator elevator;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  elevator.current_Floor = 1;
  set_Elev(&elevator, 3, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
