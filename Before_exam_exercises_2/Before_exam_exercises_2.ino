/**
 * @file Before_exam_exercises_2.ino
 * @author Kristian Palovic
 * @brief Garage door opener
 * Make a system that can control a garagedoor opener, and be used as a parking
 * sensor that is mounted inside the garage.
 *
 * When a button is pushed, a relay is turned on (the garagedoor opens), and
 * starts the distance measurement to an object.
 *
 * If the object is further away than 1m, LED 1 should be lit. If the object
 * moves closer than 80cm LED 1 should turn off,
 * and LED 2 should turn on. If the object moves within 50cm LED 2 should go
 * off, and LED 3 should start blinking. When the
 * object is within 30cm LED 3 should be constantly lit, and the relay should
 * turn off (garagedoor closes).
 *
 * The program has to be made so that after initialisation (void setup),
 * the system goes into sleep mode, untill an interrupt
 * (button press) wakes up the system. When the system is awoken, the loop code
 * should only consist of a single function
 * that does all distance measuring and control of LEDs, as well as the logic
 * necessary to turn the relay off and put the
 * system back to sleep. So the loop should be no more than 5-10 lines of code.
 * The distance measuring function you will
 * need to write yourself.
 * @version 0.1
 * @date 2021-12-08
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <stdbool.h>
#include "sensor.h"
#include "button.h"

#define INT_SIZE_OF(X) (X / sizeof(int))

/**
 * @brief HC-SR04 ultrasonic sensor
 * @wiring Trigger Pin: 2, Echo Pin: 3
 *
 */
SENSOR hcsr04 = {2, 3, 200, 0, 0};

/**
 * @brief
 * @wiring IN Pin: 4
 *
 */
OUTPUT_DEVICE relay = {4, LOW, {0, 0}};

/**
 * @brief
 * @wiring pin: 5
 */
BLINKING_LED ledRed = {{5, LOW}, 250};
OUTPUT_DEVICE ledYellow = {6, LOW};
OUTPUT_DEVICE ledGreen = {7, LOW};

OUTPUT_DEVICE *leds[3] = {&ledGreen, &ledYellow, &ledRed.device};

/**
 * @brief
 * @wiring plus pin: 9
 */
BUZZER buzzer = {9, 20};

/**
 * @brief
 * @wiring pull-down : 9
 */
BUTTON btn1 = {8, LOW, LOW, 60};

byte systemActive = false;

unsigned long time = 0;

void setup() {
  Serial.begin(9600);

  setupSensor(&hcsr04);

  pinMode(relay.pin, OUTPUT);
  pinMode(ledRed.device.pin, OUTPUT);
  pinMode(ledYellow.pin, OUTPUT);
  pinMode(ledGreen.pin, OUTPUT);
  pinMode(buzzer.pin, OUTPUT);

  pinMode(btn1.pin, INPUT);
}

void loop() {
  checkButtonChangeDevState(&btn1, &systemActive);

  // operations under the active system
  if (systemActive) {
    // opening the doors
    if (!relay.state) {
      switchDoor(&relay);
    }
    measureDistance(&hcsr04, true);
    checkDistance(&hcsr04, leds, &ledRed);
  }

  // closing the door
  if (relay.state && !systemActive) {
    switchDoor(&relay);
    resetAllDevicesStates(leds, INT_SIZE_OF(sizeof(leds)), LOW);
  }

  // if ((millis() - time) > 100) {
  //   time = millis();
  //   buzzer.freq += 20;
  // }

  // tone(buzzer.pin, buzzer.freq);

  // Serial.println(ledRed.state);

  setStates();
}

void checkDistance(SENSOR *sensor, OUTPUT_DEVICE *devices[],
                   BLINKING_LED *blinkingLed) {
  if (sensor->distance > 0 && sensor->distance <= 4) {
    blinkLed(blinkingLed);
    devices[1]->state = LOW;
    devices[0]->state = LOW;
  }

  if (sensor->distance > 4 && sensor->distance <= 10) {
    devices[2]->state = LOW;
    devices[1]->state = HIGH;
    devices[0]->state = LOW;
  }

  if (sensor->distance > 10) {
    devices[2]->state = LOW;
    devices[1]->state = LOW;
    devices[0]->state = HIGH;
  }
}

void blinkLed(BLINKING_LED *led) {
  if (millis() - led->lastBlinkMs > led->blinkingSpeed) {
    led->device.state = !led->device.state;
    led->lastBlinkMs = millis();
    Serial.print("fugujem");
  }
}

/**
 * @brief Sets the specified states in lopp
 *
 */
void setStates() {
  digitalWrite(ledRed.device.pin, ledRed.device.state);
  digitalWrite(ledYellow.pin, ledYellow.state);
  digitalWrite(ledGreen.pin, ledGreen.state);
  digitalWrite(relay.pin, relay.state);
}

void resetAllDevicesStates(OUTPUT_DEVICE *devices[], int numberOfDevices,
                           byte state) {
  for (int i = 0; i < numberOfDevices; i++) {
    devices[i]->state = state;
  }
}

/**
 * @brief Switches openning the door
 *
 * @param operatingDevice
 */
void switchDoor(OUTPUT_DEVICE *operatingDevice) {
  operatingDevice->state = !operatingDevice->state;
}