#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Display constants
#define LCDWIDTH 20
#define LCDHEIGHT 4

//-------------------------------
//---------- Constants ----------
//-------------------------------

//Buttons
const int redBtn = 3; //INT 5
const int blueBtn = 2; //INT 4
const int greenBtn = 18; //INT 3
const int blackBtn = 19; //INT 2

//Two-pos switches
const int switch1 = 53; //SWITCH1
const int switch2 = 49; //SWITCH2
const int powerPin = 51; //PP

//Leds
const int redLed = 13;
const int greenLed  = 12;
const int blueLed  = 11;
const int yellowLed = 10;
const int leds[] = {13,12,11,10};

//PIR
const int pir = 29;

//Rotary encoder
const int dt = 27;
const int clk = 26;
const int reBtn = 28;
bool cw = false;

//Pos meter
const int pos = 35;

//RGB Led
const int rgbRed = 32; 
const int rgbGreen = 33;
const int rgbBlue = 34;
const int rgbLeds[] = {32,33,34};

//--------------------------------------
//---------- END OF CONSTANTS ----------
//--------------------------------------







//--------------------------------------
//---------- VARS / OBJECTS ------------
//--------------------------------------

//LCD Type
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystalScroller lcd = LiquidCrystalScroller(0x27,20,4);

//Flag for encoder
bool reBtnPressed = false;

//Toggle Button Flags
bool redBtnToggle = false;
bool blueBtnToggle = false;
bool greenBtnToggle = false;
bool blackBtnToggle = false;

bool reBtnToggle = false;

//Buttons laststates of toggles
bool redBtnLaststate = false;
bool blueBtnLaststate = false;
bool greenBtnLaststate = false;
bool blackBtnLaststate = false;

bool reBtnLaststate = false;

//Flags --> Non-Interrupts / States
int lastSwitch1State = false;
int lastSwitch2State = false;

//Flags change indicators
bool encoderChange = false;
bool pirChange = false;
bool posChange = false;

bool redBtnChanged = false;
bool blueBtnChanged = false;
bool greenBtnChanged = false;
bool blackBtnChanged = false;
bool reBtnChanged = false;

//Timers
//Debounce delay --> encoder
int btnWaitDelay = 50;
unsigned long reBtnWait = 0;

//Rotary encoder vars
int reBtnPressedlast = 0;
int dtLaststate = 0;
int clkLaststate = 0;
int encoderPos = 0;

int dtNow = 0;
int clkNow = 0;

//PIR vars
int pirLaststate = 0;
int pirState = 0;

//Pos vars
int posLaststate = 0;
int posState = 0;

//----------- Program state -----------
int programState = 0;



//----------- Program 1 vars -----------




//----------- Program 2 vars -----------
unsigned long animWait = 0;
int animDelay = 400;

int animShiftRow1 = 0;
int animShiftRow2 = 0;
int animShiftRow3 = 0;
int animShiftRow4 = 0;
int animState = 0;

String row1Text = "";
String row2Text = "";
String row3Text = "";
String row4Text = "";




//----------- Program 3 vars -----------
//LCD CustomChar
byte arrow[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000
};

bool ledsStates[] = {false,false,false,false};
bool rgbLedsStates[] = {false,false,false};
int menuState = 0;
int menuPage = 0;




//----------- Program 4 vars -----------
unsigned long timerTime = 0;
int timerDelay = 5000;
int counter = 0;






//---------------------------------
//---------- ACTIVE CODE ----------
//---------------------------------

//Functions declaration
void redBtnCallback();
void blueBtnCallback();
void greenBtnCallback();
void blackBtnCallback();
void clearLeds();
void clearRgbLeds();
void checkInter();
int toDec(int, int);

//CallBacks for interrupts --> flags
void redBtnCallback() {
  redBtnToggle = !redBtnToggle;
}

void blueBtnCallback() {
  blueBtnToggle = !blueBtnToggle;
}

void greenBtnCallback() {
  greenBtnToggle = !greenBtnToggle;
}

void blackBtnCallback() {
  blackBtnToggle = !blackBtnToggle;
}

/* ----------------------------------
 * ----------- Functions ------------
 * ---------------------------------- */
//Turn off all leds
void clearLeds() {
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
}

//Turn off RGB Led 
void clearRgbLeds() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(rgbLeds[i], LOW);
  }
}
 
//Interrupt check --> Buttons
void checkInter() {
  //ENCODER BUTTON
  reBtnPressed = !digitalRead(reBtn);
  if(reBtnPressed && !reBtnPressedlast && reBtnWait < millis()) {
    reBtnPressed = false;
    reBtnWait = millis() + btnWaitDelay;

		//Toggle logic
		reBtnToggle = !reBtnToggle;
  
		//CODE
	}
	reBtnPressedlast = !digitalRead(reBtn);

  reBtnPressed = false;
}

//Convert to bin
int toDec(int a, int b) {
  return a + 2*b;
}


/* -------------------------------------
 * ---------- PROGRAM METHODS ----------
 * ------------------------------------- */
void programState1() {
  //Rotary encoder logic
  if(encoderChange) {
    //Encoder clamp pos
    if(encoderPos > 3) {
      encoderPos = 0;
    }
    else if(encoderPos < 0) {
      encoderPos = 3;
    }

    digitalWrite(rgbRed, cw);
    lcd.setCursor(12,0);
    lcd.print("        ");
    lcd.setCursor(0,0);
    lcd.print("Encoder pos: "+String(encoderPos));
  }

  //Encoder click
  if(reBtnChanged) { 
    clearLeds();
    digitalWrite(leds[encoderPos], HIGH);
  }

  //PIR Logic
  if(pirChange) {
    lcd.setCursor(0,1);
    lcd.print("          ");
    lcd.setCursor(0,1);
    lcd.print("PIR: ");
    lcd.print(pirState);
    digitalWrite(rgbBlue, pirState);
  }

  //Shock sensor
  int posState = digitalRead(pos);
  if(posChange) {
    lcd.setCursor(0,2);
    lcd.print("          ");
    lcd.setCursor(0,2);
    lcd.print("Pos: ");
    lcd.print(posState);
    digitalWrite(rgbGreen, posState);
  }

  if(redBtnChanged || blueBtnChanged || greenBtnChanged || blackBtnChanged) {
    lcd.setCursor(5,3);
    lcd.print("   ");
    lcd.setCursor(0,3);
    lcd.print(String(redBtnToggle)+" "+String(blueBtnToggle)+" "+String(greenBtnToggle)+" "+String(blackBtnToggle));
  }
}

void programState2() {
  if(animWait < millis()) {
    animWait = millis() + animDelay;

    //Rotace textu 1
    String orig1 = "PIR: "+String(pirState)+"  |  Encoder pos: "+String(encoderPos)+"  |  Shock detect: "+String(posState);
    String new1Text = "                   " + orig1 + " "; 
    row1Text = new1Text.substring(0+animShiftRow1,19+animShiftRow1);

    //Rotace textu 2
    String orig2 = "redBtn: "+String(redBtnToggle)+"  |  blueBtn: "+String(blueBtnToggle)+"  |  greenBtn: "+String(greenBtnToggle)+"  |  blackBtn: "+String(blackBtnToggle);
    String new2Text = "                   " + orig2 + " "; 
    row2Text = new2Text.substring(0+animShiftRow1,19+animShiftRow1);

    //Samotny vypis
    lcd.setCursor(0,0);
    lcd.print(row1Text);

    lcd.setCursor(0,1);
    lcd.print(row2Text);

    //Pričtení do rotace
    animShiftRow1++;

    //Pokud již text "zajel" tak resetuj rotaci
    if(animShiftRow1 > new2Text.length()) {
      animShiftRow1 = 0;
    }
  }
  else if(encoderChange || posChange || pirChange) {
    //Pokud nastala zmena v hodnotach, tak vypis znovu BEZ rotace.
    String orig = "PIR: "+String(pirState)+"  |  Encoder pos: "+String(encoderPos)+"  |  Shock detect: "+String(posState);
    String newText = "                   " + orig + " "; 
    row1Text = newText.substring(0+animShiftRow1,19+animShiftRow1);

    lcd.setCursor(0,0);
    lcd.print(row1Text);
  }
}

void programState3() {
  //Clamp spodni hranice a horni rotace
  if(encoderChange) {  
    //Main menu
    if(menuState == 0) {
      //Spodni hranice
      if(encoderPos < 0)
        encoderPos = 1;
      //Horni hranice
      else if(encoderPos > 1)
        encoderPos = 0; //Prehoupavani menu na zacatek nebo konec
    }

    //Leds menu
    else if(menuState == 1) {
      //Prvni stranka
      if(menuPage == 0) {
        if(encoderPos < 0) {
          menuPage = 1;
          encoderPos = 0; //NORMALNE BY BYL 3 ALE DRUHE MENU JE ZKRACENE POUZE NA JEDNU HODNOTU !!!!
          lcd.clear();
        }
        else if(encoderPos > 3) {
          menuPage = 1;
          encoderPos = 0;
          lcd.clear();
        }
      }

      //Druha stranka
      else if(menuPage == 1) {
        if(encoderPos < 0) {
          menuPage = 0;
          encoderPos = 3;
          lcd.clear();
        }
        else if(encoderPos > 0) {
          menuPage = 0;
          encoderPos = 0;
          lcd.clear();
        }
      }
    }

    //RGB Menu
    else if(menuState == 2) {
      if(encoderPos < 0)
        encoderPos = 3; 
      if(encoderPos > 3)
        encoderPos = 0;
    }
  }
  if(encoderPos < 0) {
    encoderPos = 0;
  }

  //Akce
  if(reBtnChanged) {
    //Main menu
    if(menuState == 0) {
      menuState = encoderPos+1;
      lcd.clear();
    }

    //Leds menu
    else if(menuState == 1) {
      if(menuPage == 0) {
        ledsStates[encoderPos] = !ledsStates[encoderPos]; 
        digitalWrite(leds[encoderPos], ledsStates[encoderPos]);
      }
      else if(menuPage == 1) {
        if(encoderPos == 0) {
          menuState = 0;
          menuPage = 0;
          encoderPos = 0;
          lcd.clear();
        }
      }
    }

    //RGB menu
    else if(menuState == 2) {
      if(encoderPos == 3) {
        menuState = 0;
        encoderPos = 0;
        lcd.clear();
      }
      else {
        rgbLedsStates[encoderPos] = !rgbLedsStates[encoderPos];
        digitalWrite(rgbLeds[encoderPos], rgbLedsStates[encoderPos]);
      }
    }
  }

  if(encoderChange || reBtnChanged) { 
    //Cursor things
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(0,3);
    lcd.print(" ");

    lcd.setCursor(0,encoderPos);
    lcd.write(0);

    //Main menu
    if(menuState == 0) {
      lcd.setCursor(2,0);
      lcd.print("Leds settings");

      lcd.setCursor(2,1);
      lcd.print("RGB Led settings");
    }

    //Leds menu
    else if(menuState == 1) {
      if(menuPage == 0) {
        lcd.setCursor(2,0);
        lcd.print("RED LED: "+String(ledsStates[0] ? "ON " : "OFF"));
      
        lcd.setCursor(2,1);
        lcd.print("GREEN LED: "+String(ledsStates[1] ? "ON " : "OFF"));

        lcd.setCursor(2,2);
        lcd.print("BLUE LED: "+String(ledsStates[2] ? "ON " : "OFF"));

        lcd.setCursor(2,3);
        lcd.print("YELLOW LED: "+String(ledsStates[3] ? "ON " : "OFF"));
      }
      else if(menuPage == 1) {
        lcd.setCursor(2,0);
        lcd.print("BACK");
      }
    }

    //RGB Menu
    else if(menuState == 2) {
      lcd.setCursor(2,0);
      lcd.print("RED LED: "+String(rgbLedsStates[0] ? "ON " : "OFF"));
    
      lcd.setCursor(2,1);
      lcd.print("GREEN LED: "+String(rgbLedsStates[1] ? "ON " : "OFF"));

      lcd.setCursor(2,2);
      lcd.print("BLUE LED: "+String(rgbLedsStates[2] ? "ON " : "OFF"));

      lcd.setCursor(2,3);
      lcd.print("BACK");
    }
  }
}

void programState4() {
  if(timerTime < millis()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Counter: "+String(counter));
    counter = 0;
    timerTime = millis() + timerDelay;
  }
  if(blackBtnChanged) {
    counter++;
  }
}



/* ----------------------------------
 * ---------- Main methods ----------
 * ---------------------------------- */
void setup() {
  //Setup serial
  Serial.begin(9600);

  //Button init
  pinMode(redBtn, INPUT);
  pinMode(blueBtn, INPUT);
  pinMode(greenBtn, INPUT);
  pinMode(blackBtn, INPUT);

  //Switches init
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  //LEDs init
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  //Interrupts (Buttons) init
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, FALLING);

  //Rotary encoder
  pinMode(dt, INPUT);
  pinMode(clk, INPUT);
  pinMode(reBtn, INPUT_PULLUP);

  //PIR
  pinMode(pir, INPUT);

  //RGB LED
  pinMode(rgbRed, OUTPUT);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);

  //POS
  pinMode(pos, INPUT_PULLUP);

  //LCD init
  lcd.init();
  lcd.createChar(0, arrow);
  lcd.backlight();
}

void loop() {
  //Changed vars
  encoderChange = false;
  pirChange = false;
  posChange = false;

	redBtnChanged = false;
	blueBtnChanged = false;
	greenBtnChanged = false;
	blackBtnChanged = false;
	reBtnChanged = false;

  //Check buttons and interrupts
  checkInter();

  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;
    programState = toDec(switch1State, switch2State);

    encoderChange = true;
    pirChange = true;
    posChange = true;

    animShiftRow1 = 0;
    lcd.clear();
    clearLeds();
    clearRgbLeds();
  } 

  //Rotary encoder logic
  dtNow = digitalRead(dt);
  clkNow = digitalRead(clk);
  if(dtNow != dtLaststate) {
    encoderChange = true;

    dtLaststate = dtNow;
    if(clkNow == dtNow) {
      encoderPos++;
      cw = true;
    }
    else {
      encoderPos--;
      cw = false;
    }
  }

  //PIR Logic
  pirState = digitalRead(pir);
  if(pirState != pirLaststate) {
    pirChange = true;

    pirLaststate = pirState;
  }

  //Shock sensor
  posState = digitalRead(pos);
  if(posState != posLaststate) {
    posChange = true;

    posLaststate = posState;
  }

	//Buttons check
	if(redBtnToggle != redBtnLaststate) {
		redBtnLaststate = redBtnToggle;
		redBtnChanged = true;
	}
	if(blueBtnToggle != blueBtnLaststate) {
		blueBtnLaststate = blueBtnToggle;
		blueBtnChanged = true;
	}
	if(greenBtnToggle != greenBtnLaststate) {
		greenBtnLaststate = greenBtnToggle;
		greenBtnChanged = true;
	}
	if(blackBtnToggle != blackBtnLaststate) {
		blackBtnLaststate = blackBtnToggle;
		blackBtnChanged = true;
	}
	if(reBtnToggle != reBtnLaststate) {
		reBtnLaststate = reBtnToggle;
		reBtnChanged = true;
	}


  /* ----------------------------------
  * ---------- MAIN LOGIC ------------
  * ---------------------------------- */
  if(programState == 0) {
    programState1();
  }
  else if(programState == 1) {
    programState2();
  }
  else if(programState == 2) {
    programState3();
  }
  else if(programState == 3) {
    programState4();
  }
}