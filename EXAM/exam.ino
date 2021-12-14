#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C
    lcd(0x27, 20,
        4); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);

  initLCD(&lcd);
  printToLCD(&lcd, 3, 2, "Hello world 1");
}

void loop() { Serial.println("TEst works"); }

/**
 * @brief Setup for specified LCD display
 *
 * @param lcd
 */
void initLCD(LiquidCrystal_I2C *lcd) {
  lcd->init();
  lcd->backlight();
}

/**
 * @brief print to LCD
 * 
 * @param lcd 
 * @param col 
 * @param row 
 * @param message 
 */
void printToLCD(LiquidCrystal_I2C *lcd, int col, int row, char *message) {
  lcd->setCursor(col, row);
  lcd->print(message);
}