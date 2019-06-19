#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);


//Uzivatelske znaky
byte firstChar[] = {
  B11111,
  B10101,
  B00100,
  B00000,
  B00000,
  B10001,
  B11011,
  B11111
};

byte secondChar[] = {
  B11011,
  B10001,
  B10001,
  B00000,
  B10001,
  B11011,
  B10001,
  B11111
};

byte thirdChar[] = {
  B11011,
  B10001,
  B10001,
  B11011,
  B00000,
  B11011,
  B10001,
  B11111
};

byte fourthChar[] = {
  B11011,
  B10001,
  B10001,
  B00000,
  B10001,
  B10001,
  B11011,
  B11111
};

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

//Flags --> Interrupts
bool redBtnPressed = false;
bool blueBtnPressed = false;
bool greenBtnPressed = false;
bool blackBtnPressed = false;

//Flags --> Non-Interrupts / States
bool redLedState = false;  //Test red led
bool redLedSwitching = false;  //Test red led

int lastSwitch1State = false;
int lastSwitch2State = false;

int programState = 0;

//Timers
unsigned int redLedDelay = 200; //Test red led
unsigned long redLedWait = 0;

unsigned int btnWaitDelay = 200;
unsigned long redBtnWait = 0;
unsigned long blueBtnWait = 0;
unsigned long greenBtnWait = 0;
unsigned long blackBtnWait = 0;

//PROGRAM 0 VARS
unsigned long textPrintWait = 0;
unsigned int textPrintDelay = 200;
bool cursorEna = false;

//PROGRAM 1 VARS
unsigned long textAnimWait = 0;
unsigned int textAnimDelay = 200;
int animIndex = 0;
int animState = 0;
String firstMsg = "                    Soutez v programovani                    ";
String secondMsg = "                    okresni kolo                    ";

//PROGRAM 2 VARS
int presses = 0;
int totalPresses = 0;
unsigned int pressDelay = 5000;
unsigned long pressWait = 0;

//PROGRAM 3 VARS
bool refresh = false;
bool start = false;
int gameState = 0;
int randomVals[4] = {0,0,0,0};
int points = 0;
int sameChars = 0;
unsigned int decisionDelay = 500;
unsigned long decisionWait = 0;

int ledDelay = 100;

unsigned int gameDelay = 60000;
unsigned long gameWait = 0;
int gameTimeSec = 0;

int buttonPressed = 0;

//Functions declaration
void redBtnCallback();
void blueBtnCallback();
void greenBtnCallback();
void blackBtnCallback();
void checkInter();
int toDec(int, int);
void clearLeds();

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
//Interrupt check --> Buttons
void checkInter() {
  //RED BUTTON
  if(redBtnPressed && redBtnWait < millis()) {
    redBtnPressed = false;
    redBtnWait = millis() + btnWaitDelay;
    if(programState == 3) {
      if(buttonPressed == 0 && start) {
        buttonPressed = 1;
      }
      else {
        buttonPressed = -1;
      }
    }
    start = true;
  }

  //BLUE BUTTON
  if(blueBtnPressed && blueBtnWait < millis()) {
    blueBtnPressed = false;
    blueBtnWait = millis() + btnWaitDelay;
    if(programState == 3) {
      if(buttonPressed == 0 && start) {
        buttonPressed = 2;
      }
      else {
        buttonPressed = -1;
      }
    }
    start = true;
  }

  //GREEN BUTTON
  if(greenBtnPressed && greenBtnWait < millis()) {
    greenBtnPressed = false;
    greenBtnWait = millis() + btnWaitDelay;
    if(programState == 3) {
      if(buttonPressed == 0 && start) {
        buttonPressed = 3;
      }
      else {
        buttonPressed = -1;
      }
    }
    start = true;
  }

  //BLACK BUTTON
  if(blackBtnPressed && blackBtnWait < millis()) {
    blackBtnPressed = false;
    blackBtnWait = millis() + btnWaitDelay;
    if(programState == 2) {
      presses++;
    }
    if(programState == 3) {
      if(buttonPressed == 0 && start) {
        buttonPressed = 4;
      }
      else {
        buttonPressed = -1;
      }
    }
    start = true;
  }

  redBtnPressed = false;
  blueBtnPressed = false;
  greenBtnPressed = false;
  blackBtnPressed = false;
}

//Convert Bin to Dec
int toDec(int a, int b) {
  return a + 2*b;
}

// Turn all leds off
void clearLeds() {
  digitalWrite(yellowLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, LOW);
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
  for(int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    //digitalWrite(leds[i], HIGH); //TEST LEDEK
  }

  //Interrupts (Buttons) init
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, RISING);

  //Inicializace displaje a znaku
  lcd.init();
  lcd.cursor_on();
  lcd.createChar(0, firstChar);
  lcd.createChar(1, secondChar);
  lcd.createChar(2, thirdChar);
  lcd.createChar(3, fourthChar);


  lcd.backlight();
}

void loop() {
  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;

    lcd.clear();
    clearLeds();
    totalPresses = 0;
    gameState = 0;
    gameWait = 0;
    lcd.cursor_off();
  }
  programState = toDec(switch1State, switch2State);

  //Test interruptů
  checkInter();

  //Zobrazeni znaku na displaji
  if(programState == 0) {
      lcd.cursor_on();
      lcd.setCursor(0,0);
      lcd.print("Start");
      lcd.setCursor(0,1);
      lcd.write(0);
      lcd.setCursor(1,1);
      lcd.write(1);
      lcd.setCursor(2,1);
      lcd.write(2);
      lcd.setCursor(3,1);
      lcd.write(3);
  
  }

  //Animace textu
  else if(programState == 1) {
    if(!(textAnimWait < millis())) {
        return;
    }
    textAnimWait = millis() + textAnimDelay;

    if((animIndex+20 <= firstMsg.length()) && animState == 0) {
      digitalWrite(greenLed, HIGH);
      lcd.setCursor(0,0);
      lcd.print(firstMsg.substring(animIndex,20+animIndex));
    }
    else if((animIndex+20 <= secondMsg.length()) && animState == 1) {
      digitalWrite(yellowLed, HIGH);
      lcd.setCursor(0,1);
      lcd.print(secondMsg.substring(animIndex,20+animIndex));
    }
    else {
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, LOW);
      animState = (animState == 0) ? 1 : 0;
      animIndex = 0;
    }

    animIndex++;  
  }

  //Pocitani stisku tlacitka
  else if(programState == 2) {
    lcd.setCursor(0,3);
    if(pressWait-millis() < 1000) {
      lcd.print("Zbyva 1 s");
    } 
    else if(pressWait-millis() < 2000) {
      lcd.print("Zbyva 2 s");
    } 
    else if(pressWait-millis() < 3000) {
      lcd.print("Zbyva 3 s");
    } 
    else if(pressWait-millis() < 4000) {
      lcd.print("Zbyva 4 s");
    } 
    else {
      lcd.print("Zbyva 5 s");
    }
    //Cekani na ubehnuti 5 sekund
    if(pressWait < millis()) {
      pressWait = millis() + pressDelay; //PressDelay = 5000 ms --> spusti se kazdych 5 s
      
      //Smazani nejstarsiho stisku
      if(presses > 0) {
        presses--;
      }
      totalPresses += presses;

      clearLeds();

      if(totalPresses == 0) {
        digitalWrite(yellowLed, HIGH);
      }
      else if(totalPresses == 1) {
        digitalWrite(greenLed, HIGH);
      }
      else if(totalPresses == 2) {
        digitalWrite(blueLed, HIGH);
      }
      else {
        digitalWrite(redLed, HIGH);
      }

      lcd.setCursor(0,0);
      String data = "Pocet platnych";
      lcd.print(data);
      lcd.setCursor(0,1);
      data = "stisku ";
      data += totalPresses;
      lcd.print(data);

      presses = 0;
    }
  }

  //Videostop
  else if(programState == 3) {
    if(start) { 
      if(gameWait < millis()) {
        gameWait = millis() + gameDelay; 
        points = 0;
        start = false;
      }
      else { 
        lcd.setCursor(0,1);
        String data = "Body:";
        data += points;
        data += " Cas: ";
        int timeRem = abs((gameWait-millis())/1000);
        data += String(timeRem);
        if(timeRem >= 10) {
          data += "s   ";
        }
        else {
          data += "s   ";
        }
        lcd.print(data);

        if(gameState == 0) {
          int fi = 0, se = 0, th = 0, fo = 0; 
          for(int i = 0; i < 4; i++) {
            lcd.setCursor(i*2,0);
            int randNum = random(0,3);
            if(randNum == 0) fi++;
            else if(randNum == 1) se++;
            else if(randNum == 2) th++;
            else if(randNum == 3) fo++;
            lcd.write(randNum);
          }
          if(fi >= se && fi >= th && fi >= fo) {
            sameChars = fi;
          }
          else if(se >= fi && se >= th && se >= fo) {
            sameChars = se;
          }
          else if(th >= fi && th >= se && th >= fo) {
            sameChars = th;
          }
          else if(fo >= fi && fo >= se && fo >= th) {
            sameChars = fo;
          }
          buttonPressed = 0;
          //DEBUG
          //lcd.setCursor(0,3);
          //lcd.print(sameChars);

          gameState = 1;
          decisionWait = millis() + decisionDelay;
        }
        else if(gameState == 1) {
          if(decisionWait < millis()) {
           if(buttonPressed == 0 || buttonPressed == -1) {
             digitalWrite(yellowLed, HIGH);
           }
           else if(sameChars == buttonPressed) {
             points += sameChars;
             digitalWrite(greenLed, HIGH);
           }
           else {
             digitalWrite(redLed, HIGH);
           }
          /* DEBUG
          lcd.setCursor(0,4);
          lcd.print(buttonPressed);
          */
           delay(100);
           clearLeds();
           gameState = 0;
           buttonPressed = 0;
          }
        }
      }
    }
  }

  //DEFAULT
  else {

  }
}