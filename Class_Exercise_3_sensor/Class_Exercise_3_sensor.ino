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
 * WHat is this mini-project about
 *
 * ON LCD display show position of something on the
 * line
 *
 * e.g. (display):
 * AXDL sensor: 0 - in the middle
 * -255 <= x <= 255 ... dogether 510 individual bits
 * 1 step = x -/+ 30
 * 17 steps overall
 *
 * if(x == 0)
 *  ___________________
 * | -------- -------- |
 * |         _         |
 * |___________________|
 *
 * if(x == 60)
 *  ___________________
 * | ------------ ---- |
 * |             _     |
 * |___________________|
 *
 * if(x == -60)
 *  ___________________
 * | ---- ------------ |
 * |     _             |
 * |___________________|
 * .....................................
 *
 * also adding servo ... just for fun
 */

/**
 * required by APDS and AXDL and display
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
int x, y, z;
int xBefore;

/**
 * Display CONFIG PART
 */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

/**
 * Servo config part
 */
#include <Servo.h>
#define SERVO_PIN 9
Servo myservo; // create servo object to control a servo
int pos = 0;   // variable to store the servo positio

void servoConfig() {
  myservo.attach(SERVO_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(0);
  delay(2000);
  myservo.write(90);
  delay(2000);
  myservo.write(180);
  delay(2000);
  myservo.write(90);
}

void displayConfig() {
  lcd.init();

  lcd.backlight();

  printToLCDString(lcd, 1, 1, "--Initialization--");
  delay(1000);
  lcd.clear();
}

void printToLCDString(LiquidCrystal_I2C lcd, int col, int row, String message) {
  lcd.setCursor(col, row);
  lcd.print(message);
}

void breakLine(int overAllSteps, int actualStep) {
  char steps[overAllSteps + 2] = {};
  char underSteps[overAllSteps + 2] = {};
  char str[20];

  // get all steps
  for (int i = 0; i < overAllSteps; i++) {
    // do not display last step
    if (i == actualStep) {
      steps[i] = ' ';
      underSteps[i] = '-';
      continue;
    }

    steps[i] = '-';
    underSteps[i] = ' ';
  }

  strcpy(str, steps);
  printToLCDString(lcd, 1, 1, str);

  strcpy(str, underSteps);
  printToLCDString(lcd, 1, 2, str);
}

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

  adxl.setInactivityX(0);
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
  // read the accelerometer values and store them in variables  x
  adxl.readXYZ(&x, &y, &z);
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

int calculateStep() {
  // check if we are in plus or minus numbers
  if (x > 0) {
    return x / 30 + 7;
  } else {
    int plusX = x * -1;
    return 7 - plusX / 30;
  }
}

int calculateServo() {
  if (abs(xBefore - x) > 10) {
    xBefore = x;
    return map(x, -255, 255, 5, 175);
  } else {
    return 300;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing");

  // apdsConfig();
  axdlConfig();
  displayConfig();
  servoConfig();
}

void loop() {
  // checkApds();
  checkAxdl();
  breakLine(17, calculateStep());

  char str[20];
  sprintf(str, "%d", x);

  printToLCDString(lcd, 1, 3, "      ");
  printToLCDString(lcd, 1, 3, str);

  if ((x > 0 && x < 250) || (x < 0 && x > -250)) {
    pos = calculateServo();
    if (pos != 300) {
      myservo.write(pos);
      Serial.println("Moving");
    }else{
      Serial.println("Not moving");
    }
  }

  // sprintf(str, "%d", calculateServo());
  // Serial.println(str);

  delay(220);
}
