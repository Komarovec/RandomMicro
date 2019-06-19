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

//Flags --> Interrupts
bool redBtnPressed = false;
bool blueBtnPressed = false;
bool greenBtnPressed = false;
bool blackBtnPressed = false;

bool reBtnPressed = false;

//Toggle Button Flags
bool redBtnToggle = false;
bool blueBtnToggle = false;
bool greenBtnToggle = false;
bool blackBtnToggle = false;

bool reBtnToggle = false;

//Buttons laststates
bool redBtnLaststate = false;
bool blueBtnLaststate = false;
bool greenBtnLaststate = false;
bool blackBtnLaststate = false;

bool reBtnLaststate = false;

//Flags --> Non-Interrupts / States
bool displayChanged = false; //Indicates change of the display
bool redLedState = false;
bool redLedSwitching = false;

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
int btnWaitDelay = 100;

unsigned long redBtnWait = 0;
bool redBtnWaiting = false;

unsigned long blueBtnWait = 0;
bool blueBtnWaiting = false;

unsigned long greenBtnWait = 0;
bool greenBtnWaiting = false;

unsigned long blackBtnWait = 0;
bool blackBtnWaiting = false;

unsigned long reBtnWait = 0;
bool reBtnPressedlast = false;

unsigned long animWait = 0;
int animDelay = 400;

//Rotary encoder vars
int dtLaststate = 0;
int clkLaststate = 0;
int encoderPos = 0;

//PIR vars
int pirLaststate = 0;

//Pos vars
int posLaststate = 0;

//LOOP COUNTING
int loops = 0;
unsigned long loopWait = 0;
int loopDelay = 1000;

//----------- Program state -----------
int programState = 0;

//----------- Program 2 vars -----------
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
  redBtnPressed = true;
}

void blueBtnCallback() {
  blueBtnPressed = true;
}

void greenBtnCallback() {
  greenBtnPressed = true;
}

void blackBtnCallback() {
  blackBtnPressed = true;
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
  //RED BUTTON
  if(redBtnPressed && !redBtnWaiting) {
    redBtnWait = millis() + btnWaitDelay;
    redBtnWaiting = true;
  }
  else if(redBtnWait < millis() && redBtnWaiting) {
    if(digitalRead(redBtn) == LOW) {
      redBtnToggle = !redBtnToggle;
      redBtnWaiting = false;
    }
  }
  
  //BLUE BUTTON
  if(blueBtnPressed && !blueBtnWaiting) {
    blueBtnWait = millis() + btnWaitDelay;
    blueBtnWaiting = true;
  }
  else if(blueBtnWait < millis() && blueBtnWaiting) {
    if(digitalRead(blueBtn) == LOW) {
      blueBtnToggle = !blueBtnToggle;
      blueBtnWaiting = false;
    }
  }

  //GREEN BUTTON
  if(greenBtnPressed && !greenBtnWaiting) {
    greenBtnWait = millis() + btnWaitDelay;
    greenBtnWaiting = true;
  }
  else if(greenBtnWait < millis() && greenBtnWaiting) {
    if(digitalRead(greenBtn) == LOW) {
      greenBtnToggle = !greenBtnToggle;
      greenBtnWaiting = false;
    }
  }

  //BLACK BUTTON
  if(blackBtnPressed && !blackBtnWaiting) {
    blackBtnWait = millis() + btnWaitDelay;
    blackBtnWaiting = true;
  }
  else if(blackBtnWait < millis() && blackBtnWaiting) {
    if(digitalRead(blackBtn) == LOW) {
      blackBtnToggle = !blackBtnToggle;
      blackBtnWaiting = false;
    }
  }

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


  //Deactivate any non-actual presses
  redBtnPressed = false;
  blueBtnPressed = false;
  greenBtnPressed = false;
  blackBtnPressed = false;

  reBtnPressed = false;
}

//Convert to bin
int toDec(int a, int b) {
  return a + 2*b;
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
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, CHANGE);

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
   //Check buttons and interrupts
  checkInter();

  //Changed vars
  encoderChange = false;
  pirChange = false;
  posChange = false;

	redBtnChanged = false;
	blueBtnChanged = false;
	greenBtnChanged = false;
	blackBtnChanged = false;
	reBtnChanged = false;

  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    displayChanged = true;
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
  int dtNow = digitalRead(dt);
  int clkNow = digitalRead(clk);
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
  int pirState = digitalRead(pir);
  if(pirState != pirLaststate) {
    pirChange = true;

    pirLaststate = pirState;
  }

  //Shock sensor
  int posState = digitalRead(pos);
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
  
  //CODE
}