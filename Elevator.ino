// include the LCD library & Setup
#include <LiquidCrystal.h>
#include "Functions.h"
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Initializing integer variables
int button, floor_num, au_floor_num, floor_desired, au_floor_desired, x;
int num = 1;
bool state = true;

//declare all the required list
_List req_List;
_List curr_Pass;
stop_List dropOff_List;
Elevator elev;


void setup() {
  //Setup LCD and Serial Monitor
  Serial.begin(9600);
  lcd.begin(16, 2);

  //Title Page
  title_Menu();
  req_List.p_head = NULL;
}

void loop() {
  //Save voltage output of the 0 analog pin
  button = analogRead(0);

  //Ask user if they would like to enter Authorized User Mode (AUM) or just enter a request_Menu
  lcd.home();                   //Place cursor in the upper left hand corner
  lcd.print("Are you an AU?");  //Abbreviated to fit

  //Give the user two reponses (Yes : No)
  lcd.setCursor(0, 1);
  lcd.print("Yes           No");  //Choose between Yes or No

  if (button < 60) {  //They picked no
    request_Menu();   //Begin the request_Menu function
    

    while (analogRead(0) < 60) {
    }
  }

  else if (button < 200) {
  }  //They pressed "UP"; do nothing
  else if (button < 400) {
  }  //They pressed "DOWN"; do nothing

  else if (button < 600) {  //They picked yes
    aum();                  //Begin the Authorised User Mode Function
    while (analogRead(0) < 600) {
    }
  }
}


//=============================================== FUNCTIONS ==========================================

//Funtion for title_Menu page
void title_Menu() {
  lcd.setCursor(3, 0);  //(Column, Row)
  lcd.print("Functional");
  delay(1000);

  lcd.setCursor(4, 1);
  lcd.print("Elevator");
  delay(1500);
  lcd.clear();
}

//Funtion for Authorized User Mode (AUM)
void aum() {
  //Custom Character for AU mode
  byte aum_char[8] = { 0x0E, 0x11, 0x1F, 0x11, 0x00, 0x11, 0x11, 0x0E };
  lcd.createChar(0, aum_char);  //Creates the AU custom character

  lcd.clear();  // Clear screen before AU mode begins

  //Write "AU Mode Initiated" on the LCD for 1.5 seconds
  lcd.setCursor(0, 0);
  lcd.print("AU Mode");
  lcd.setCursor(0, 1);
  lcd.print("Initiated");
  delay(1500);
  lcd.clear();

  //Display the AU custom character in the top right corner
  lcd.write((byte)0x00);

  //Complete all current request_Menus
  lcd.setCursor(2, 0);
  lcd.print("Completing");
  delay(1000);
  lcd.setCursor(2, 1);
  lcd.print("All Tasks");
  delay(2200);
  lcd.clear();

  //Ask the AU for the floor they're  on
  lcd.write((byte)0x00);  //Display the AU custom character in the top right corner
  lcd.setCursor(2, 0);
  lcd.print("Current");
  lcd.setCursor(2, 1);
  lcd.print("Floor");
  while (au_floor_num != num) {
    floor_on_au();
  }

  //Ask the AU for the floor they would like to go to
  lcd.clear();
  lcd.write((byte)0x00);
  lcd.setCursor(2, 0);
  lcd.print("Desired");
  lcd.setCursor(2, 1);
  lcd.print("Floor");
  while (au_floor_desired != num) {
    desired_floor_au();
  }
}



//Function for request_Menu
void request_Menu() {
  //Clear the screen and display the prompt "What floor are you on?"
  lcd.clear();
  lcd.home();
  lcd.print("What floor are");
  lcd.setCursor(0, 1);
  lcd.print("you on?");
  while (floor_num != num) {
    floor_on();
  }

  //Clear the screen and display the prompt "What floor are you on?"
  lcd.clear();
  lcd.home();
  lcd.print("Desired");
  lcd.setCursor(0, 1);
  lcd.print("Floor");
  while (floor_desired != num) {
    desired_floor();
  }

  delay(800);
}

//Function that keeps track of the floor number
void floor_on() {
  x = analogRead(0);  //Read button inputs
  lcd.setCursor(14, 1);
  delay(150);  //Delay between button presses


  if (x < 60) {
  }  //They pressed "RIGHT"; do nothing

  //If the UP Button is pressed, add 1 to the integer 'num'
  else if (x < 200) {
    num++;
    while (analogRead(0) < 200) {  //While the sensor is reading UP, do not increase the number
    }
  }

  //If the DOWN Button is pressed, subtract 1 from the integer 'num'
  else if (x < 400) {
    num--;
    while (analogRead(0) < 400) {
    }
  }

  else if (x < 600) {
  }  //They pressed "LEFT"; do nothing

  //Use the select button to pick a floor
  else if (x < 800) {
    Serial.print("You picked floor ");
    Serial.println(num);
    floor_num = num;
    while (analogRead(0) < 800) {
    }
  }

  // Ensure 'num' stays within the range of 1 and 4
  if (num > 4) {
    num = 1;
  } else if (num < 1) {
    num = 4;
  }

  lcd.print(num);  //Prints out the number
}

//Function that keeps track of the floor the user wants to go to
void desired_floor() {
  x = analogRead(0);  //Read button inputs
  lcd.setCursor(14, 1);
  delay(150);  //Delay between button presses

  if (x < 60) {
  }  //They pressed "RIGHT"; do nothing

  //If the UP Button is pressed, add 1 to the integer 'num'
  else if (x < 200) {
    num++;
    while (analogRead(0) < 200) {  //While the sensor is reading UP, do not increase the number
    }
  }

  //If the DOWN Button is pressed, subtract 1 from the integer 'num'
  else if (x < 400) {
    num--;
    while (analogRead(0) < 400) {
    }
  }

  else if (x < 600) {
  }  //They pressed "LEFT"; do nothing

  //Use the select button to pick a floor
  else if (x < 800) {
    Serial.print("You're going to floor ");
    Serial.println(num);
    floor_desired = num;
    while (analogRead(0) < 800) {
    }
  }

  // Ensure 'num' stays within the range of 1 and 4
  if (num > 4) {
    num = 1;
  } else if (num < 1) {
    num = 4;
  }

  lcd.print(num);  //Prints out the number
}

void floor_on_au() {
  x = analogRead(0);  //Read button inputs
  lcd.setCursor(14, 1);
  delay(150);  //Delay between button presses

  if (x < 60) {
  }  //They pressed "RIGHT"; do nothing

  //If the UP Button is pressed, add 1 to the integer 'num'
  else if (x < 200) {
    num++;
    while (analogRead(0) < 200) {  //While the sensor is reading UP, do not increase the number
    }
  }

  //If the DOWN Button is pressed, subtract 1 from the integer 'num'
  else if (x < 400) {
    num--;
    while (analogRead(0) < 400) {
    }
  }

  else if (x < 600) {
  }  //They pressed "LEFT"; do nothing

  //Use the select button to pick a floor
  else if (x < 800) {
    Serial.print("The AU is on floor ");
    Serial.println(num);
    au_floor_num = num;
    while (analogRead(0) < 800) {
    }
  }

  // Ensure 'num' stays within the range of 1 and 4
  if (num > 4) {
    num = 1;
  } else if (num < 1) {
    num = 4;
  }

  lcd.print(num);  //Prints out the number
}

void desired_floor_au() {
  x = analogRead(0);  //Read button inputs
  lcd.setCursor(14, 1);
  delay(150);  //Delay between button presses

  if (x < 60) {
  }  //They pressed "RIGHT"; do nothing

  //If the UP Button is pressed, add 1 to the integer 'num'
  else if (x < 200) {
    num++;
    while (analogRead(0) < 200) {  //While the sensor is reading UP, do not increase the number
    }
  }

  //If the DOWN Button is pressed, subtract 1 from the integer 'num'
  else if (x < 400) {
    num--;
    while (analogRead(0) < 400) {
    }
  }

  //Use the select button to pick a floor
  else if (x < 800) {
    Serial.print("The AU is going to floor ");
    Serial.println(num);
    au_floor_desired = num;
    while (analogRead(0) < 800) {
    }
  }

  else if (x < 600) {
  }  //They pressed "LEFT"; do nothing

  // Ensure 'num' stays within the range of 1 and 4
  if (num > 4) {
    num = 1;
  } else if (num < 1) {
    num = 4;
  }

  lcd.print(num);  //Prints out the number
}

//------Button Reference------//
/*
Right: button < 60 = 0
Up: button < 200 = 132
Down: button < 400 = 308
Left: button < 600 = 481
Select: button < 800 = 721

     if (x < 60) {} RIGHT
else if (x < 200){} UP
else if (x < 400){} DOWN
else if (x < 600){} LEFT
else if (x < 800){} SELECT
*/