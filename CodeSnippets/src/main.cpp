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

//----------- Program 3 vars -----------

//----------- Program 4 vars -----------







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

}

void programState2() {

}

void programState3() {

}

void programState4() {

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
  lcd.backlight();
  
  //lcd.createChar(0, arrow);
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


    //POZOR MUZE DELAT BORDEL
    encoderChange = true;
    pirChange = true;
    posChange = true;

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