#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/**
 * @brief from I2C hello world example
 *
 */
LiquidCrystal_I2C
    lcd(0x27, 20,
        4); // set the LCD address to 0x27 for a 16 chars and 2 line display

/**
 * @brief Temperature meter
 */
typedef struct TEMP_METER {
  byte pin : 4;

  /**
   * @brief last measured value should be stored here
   */
  float value;

  /**
   * @brief How often to check the temperature of specified meter
   */
  unsigned int checkTimeMs;

  /**
   * @brief Preferebly used to assigned millis() function value
   *        for further comparison
   */
  unsigned long lastCheckTimeMS;
} TEMP_METER;

TEMP_METER lm35 = {A0, 0, 500, 0};

void setup() {
  Serial.begin(9600);

  initLCD(&lcd);
  printToLCD(&lcd, 3, 2, "Hello world 1");
}

void loop() { delayedTempCheck(&lcd, &lm35); }

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

/**
 * @brief Delays the checking and re-printing of message
 *
 * @param lcd
 * @param temp
 */
void delayedTempCheck(LiquidCrystal_I2C *lcd, TEMP_METER *temp) {
  if ((millis() - temp->lastCheckTimeMS) > temp->checkTimeMs) {

    char tempS[11] = "A0:";

    temp->lastCheckTimeMS = millis();

    checkTemp(temp);

    // Convert float value into string
    dtostrf(temp->value, -3, 1, tempS + 2);

    // Append C
    strcat(tempS, "C");

    printToLCD(lcd, 0, 0, tempS);
  }
}

/**
 * @brief Get the Temperature of temp. meter
 *
 * @param temp
 */
void checkTemp(TEMP_METER *temp) {
  // read the value from the PIN,
  // convert this value to voltage (5000 / 1024.0),
  // and then to temperature (/10)
  temp->value = analogRead(lm35.pin) * (5000 / 1024.0F) / 10;
}