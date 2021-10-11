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
 * required by APDS and AXDL
 */
#include <Wire.h>

/**
 * APDS CONFIG PART
 */
#include <SparkFun_APDS9960.h>
#define APDS9960INT 2 // Interrupt pin
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isrFlag = 0;

/**
 * AXDL CONFIG PART
 */
#include <ADXL345.h>
ADXL345 adxl; // variable adxl is an instance of the ADXL345 library

// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd1(0x25, 20, 4);

/**
 * Configures APDS sensor
 *
 * Part copied from axdl library demo
 */
void axdlConfig() {
  adxl.powerOn();

  // set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75);   // 62.5mg per increment
  adxl.setInactivityThreshold(75); // 62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

  // look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
}

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

/**
 * Loop functionality for APDS
 * Part copied from axdl library demo
 */
void checkAxdl() {
  int x, y, z;
  // read the accelerometer values and store them in variables  x,y,z
  adxl.readXYZ(&x, &y, &z);

  // Output x,y,z values
  // Serial.print("Value of X:");
  // Serial.println(x);
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
  Serial.println("Initializing");

  apdsConfig();
  axdlConfig();
}

void loop() {
  checkApds();
  checkAxdl();
}
