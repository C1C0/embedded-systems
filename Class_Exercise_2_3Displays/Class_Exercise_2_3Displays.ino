/*
   Created by Kristian Palovic and Radek Strouf

   LCD code:
   by YWROBOT
   - Compatible with the Arduino IDE 1.0
   - Library version:1.1
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd1(0x25, 20, 4);
LiquidCrystal_I2C lcd2(0x26, 20, 4);
LiquidCrystal_I2C lcd3(0x27, 20, 4);

void setup() {
  Serial.begin(9600);

  lcd1.init();
  lcd2.init();
  lcd3.init();

  lcd1.backlight();
  lcd2.backlight();
  lcd3.backlight();

  printToLCD(lcd1, 1, 1, "Ahoj Radko");

  printToLCD(lcd2, 2, 1, "Hello Radek");

  printToLCD(lcd3, 3, 1, "Čus Radku");
}

void loop() {
  /*
     Goal:

     display on different displays :)
  */
}

void printToLCD(LiquidCrystal_I2C lcd, int col, int row, String message) {
  lcd.setCursor(col, row);
  lcd.print(message);
}
