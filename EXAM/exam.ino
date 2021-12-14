#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

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

typedef struct DHT_S {
  /**
   * @brief expected delay between measurements
   *
   */
  int delay;
  float value;
  unsigned long lastMeasurement;
  unsigned long lastReading;
} DHT_S;

TEMP_METER lm35 = {A0, 0, 500, 0};

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
DHT_S dhtS = {500, 0};

float tempDiff = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  initLCD(&lcd);

  delay(1000);
}

void loop() {
  delayedTempCheck(&lcd, &lm35);
  checkTempDigital(&lcd, &dht, &dhtS);
  checkTempDiff(&lcd, &dhtS, &lm35);
}

void checkTempDiff(LiquidCrystal_I2C *lcd, DHT_S *dht_s, TEMP_METER *lm) {
  float differenceNow = abs(dht_s->value - lm->value);

  if (differenceNow != tempDiff) {
    tempDiff = differenceNow;

    char tempS[11] = "DI:";

    // Convert float value into string
    dtostrf(tempDiff, -3, 1, tempS + 3);
    // Append C
    strcat(tempS, "C");

    printToLCD(lcd, 10, 0, tempS);
  }
}

/**
 * @brief Setup for specified LCD display
 *
 * @param lcd
 */
void initLCD(LiquidCrystal_I2C *lcd) {
  lcd->init();
  lcd->backlight();

  printToLCD(lcd, 4, 2, "temp. meter");
  printToLCD(lcd, 6, 3, "tester");
  delay(1000);
  lcd->clear();
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

    checkTempAnalog(temp);

    // Convert float value into string
    dtostrf(temp->value, -3, 1, tempS + 3);

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
void checkTempAnalog(TEMP_METER *temp) {
  // read the value from the PIN,
  // convert this value to voltage (5000 / 1024.0),
  // and then to temperature (/10)
  temp->value = analogRead(lm35.pin) * (5000 / 1024.0F) / 10;
}

/**
 * @brief Delays the printing and checs digital temp meter
 *
 * @param lcd
 * @param dht
 * @param dht_s
 */
void checkTempDigital(LiquidCrystal_I2C *lcd, DHT *dht, DHT_S *dht_s) {
  if (millis() - dht_s->lastMeasurement > dht_s->delay) {
    char tempS[11] = "D7:";

    dht_s->lastMeasurement = millis();

    if (millis() - dht_s->lastReading > 300) {
      dht_s->value = dht->readTemperature();
      dht_s->lastReading = millis();
    }

    Serial.println(dht_s->value);

    // Convert float value into string
    dtostrf(dht_s->value, -3, 1, tempS + 3);

    // Append C
    strcat(tempS, "C");

    printToLCD(lcd, 0, 1, tempS);
  }
}