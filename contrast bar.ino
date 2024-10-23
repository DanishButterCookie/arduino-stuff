#include <LiquidCrystal.h>


// these are the pins the screen is connected with
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// these are custom characters (5x8)
byte middle[8] = {
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111
};

byte middlef[8] = {
  0b11111,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b11111
};

byte right[8] = {
  0b11111,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b11111
};

byte rightf[8] = {
  0b11111,
  0b00001,
  0b11101,
  0b11101,
  0b11101,
  0b11101,
  0b00001,
  0b11111
};

byte left[8] = {
  0b11111,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b11111
};

byte leftf[8] = {
  0b11111,
  0b10000,
  0b10111,
  0b10111,
  0b10111,
  0b10111,
  0b10000,
  0b11111
};

void setup() {
  // initialize LCD and set up the number of columns and rows:
  lcd.begin(16, 2);

  lcd.createChar(1, middle);
  lcd.createChar(2, middlef);
  lcd.createChar(3, right);
  lcd.createChar(4, rightf);
  lcd.createChar(5, left);
  lcd.createChar(6, leftf);
  lcd.setCursor(0, 0);
  lcd.print("Contrast help");
  lcd.print(" v1");
  lcd.setCursor(0,1)
  ;lcd.print("contra: ")

;}

void loop() {
  // read the potentiometer on A0:
  int r = analogRead(A0);
  // map the result to 200 - 1000:
  int delayTime = r
  // set the cursor to the bottom row, 5th position:
  ;lcd.setCursor(8,1)
  ;if (r<86) {
    ;lcd.write(5);
  } else {
    ;lcd.write(6);
  }
  ;lcd.setCursor(9,1)
  ;if (r<86*2) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(10,1)
  ;if (r<86*3) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(11,1)
  ;if (r<86*4) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(12,1)
  ;if (r<86*5) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(13,1)
  ;if (r<86*6) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(14,1)
  ;if (r<86*7) {
    ;lcd.write(1);
  } else {
    ;lcd.write(2);
  }
  ;lcd.setCursor(15,1)
  ;if (r<86*8-10) {
    ;lcd.write(3);
  } else {
    ;lcd.write(4);
  }
  lcd.write(3);
;}
