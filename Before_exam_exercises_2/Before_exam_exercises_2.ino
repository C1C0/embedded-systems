/**
 * @file Before_exam_exercises_2.ino
 * @author Kristian Palovic
 * @brief Garage door opener
 * Make a system that can control a garagedoor opener, and be used as a parking sensor that is mounted inside the garage. 
 * When a button is pushed, a relay is turned on (the garagedoor opens), and starts the distance measurement to an object. 
 * If the object is further away than 1m, LED 1 should be lit. If the object moves closer than 80cm LED 1 should turn off, 
 * and LED 2 should turn on. If the object moves within 50cm LED 2 should go off, and LED 3 should start blinking. When the
 * object is within 30cm LED 3 should be constantly lit, and the relay should turn off (garagedoor closes). The program 
 * has to be made so that after initialisation (void setup), the system goes into sleep mode, untill an interrupt
 * (button press) wakes up the system. When the system is awoken, the loop code should only consist of a single function 
 * that does all distance measuring and control of LEDs, as well as the logic necessary to turn the relay off and put the
 * system back to sleep. So the loop should be no more than 5-10 lines of code. The distance measuring function you will
 * need to write yourself.
 * @version 0.1
 * @date 2021-12-08
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "sensor.h"

/**
 * @brief HC-SR04 ultrasonic sensor
 * @wiring Trigger Pin: 2, Echo Pin: 3
 *
 */
SENSOR hcsr04 = {2, 3, 1600, 0, 0};

/**
 * @brief 
 * @wiring IN Pin: 4
 * 
 */
OUTPUT_DEVICE relay = {4, HIGH, {0, 0}};

int distance = 0;
unsigned int time = 0;

void setup() {
  Serial.begin(9600);

  setupSensor(&hcsr04);
  pinMode(relay.pin, OUTPUT);
}

void loop() {
  measureDistance(&hcsr04, true);

  // if(hcsr04.distance < 5){
  //   relay.state = LOW;
  // }else{
  //   relay.state = HIGH;
  // }

  // digitalWrite(relay.pin, relay.state);
}