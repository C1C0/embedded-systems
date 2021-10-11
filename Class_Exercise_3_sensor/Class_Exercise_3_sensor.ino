/**
 * @file Class_Exercise_3_sensor.ino
 * @author Kristian Palovic
 * @brief
 *    Multiple I2C DEVICES
   - APDS 9960 Multisensor
   - AXDL 345 accellerometer
   - Display
   - LED
 * @version 0.1
 * @date 2021-10-11
 *
 * @copyright Copyright (c) 2021
 *
 */

/**
 * APDS CONFIG PART
 */
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#define APDS9960INT 2 // Interrupt pin
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isrFlag = 0;

// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd1(0x25, 20, 4);

/**
 * Configures APDS sensor
 *
 * Part copied from GestureTest of APDS 9960 lib.
 */
void apdsConfig() {
  // Set interrupt pin as input
  pinMode(APDS9960INT, INPUT);

  // Initialize Serial port
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - Configuration"));
  Serial.println(F("--------------------------------"));

  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if (apds.init()) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start running the APDS-9960 gesture sensor engine
  if (apds.enableGestureSensor(true)) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

/**
 * Loop functionality for APDS
 * Part copied from GestureTest of APDS 9960 lib.
 */
void checkApds() {
  if (isrFlag == 1) {
    detachInterrupt(0);
    handleGesture();
    isrFlag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }
}

void interruptRoutine() { isrFlag = 1; }

/**
 * Provides certain functionality on
 * recorded gesture
 */
void handleGesture() {
  if (apds.isGestureAvailable()) {
    switch (apds.readGesture()) {
    case DIR_UP:
      Serial.println("UP");
      break;
    case DIR_DOWN:
      Serial.println("DOWN");
      break;
    case DIR_LEFT:
      Serial.println("LEFT");
      break;
    case DIR_RIGHT:
      Serial.println("RIGHT");
      break;
    case DIR_NEAR:
      Serial.println("NEAR");
      break;
    case DIR_FAR:
      Serial.println("FAR");
      break;
    default:
      Serial.println("NONE");
    }
  }
}

void setup() {
  Serial.begin(9600);

  apdsConfig();
}

void loop() { checkApds(); }
