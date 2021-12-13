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
#include <avr\sleep.h>
#include "sensor.h"
#include "button.h"

#define INT_SIZE_OF(X) (X / sizeof(int))

/**
 * @brief HC-SR04 ultrasonic sensor
 * @wiring Trigger Pin: 4, Echo Pin: 5
 *
 */
SENSOR hcsr04 = {4, 5, 200, 0, 0};

/**
 * @brief
 * @wiring IN Pin: 6
 *
 */
OUTPUT_DEVICE relay = {6, LOW, {0, 0}};

/**
 * @brief
 * @wiring RED - pin: 7, YELLOW - pin: 8, GREEN - pin: 9
 */
BLINKING_LED ledRed = {{7, LOW}, 250};
OUTPUT_DEVICE ledYellow = {8, LOW};
OUTPUT_DEVICE ledGreen = {9, LOW};

OUTPUT_DEVICE *leds[3] = {&ledGreen, &ledYellow, &ledRed.device};

/**
 * @brief
 * @wiring plus pin: 9
 */
BUZZER buzzer = {11, 20};

/**
 * @brief
 * @wiring pull-down : 9
 */
BUTTON btn1 = {2, LOW, LOW, 60};

/**
 * @brief Arduino state handling structure
 *
 */
DELAYED_INTERRUPT intr = {LOW};
#define INTERRUPT_DELAY 3000

void setup() {
  Serial.begin(9600);

  setupSensor(&hcsr04);

  pinMode(relay.pin, OUTPUT);
  pinMode(ledRed.device.pin, OUTPUT);
  pinMode(ledYellow.pin, OUTPUT);
  pinMode(ledGreen.pin, OUTPUT);
  pinMode(buzzer.pin, OUTPUT);

  pinMode(btn1.pin, INPUT);

  interruptGoToSleep(&intr.intr, 2);
}

void loop() {
  // checkButtonChangeDevState(&btn1, &intr.intr.arduinoState);

  // operations under the active system
  if (intr.intr.arduinoState) {
    // opening the doors
    if (!relay.state) {
      switchDoor(&relay);
    }
    measureDistance(&hcsr04, true);
    checkDistance(&hcsr04, leds, &ledRed, &buzzer, &intr);
    tone(buzzer.pin, buzzer.freq);
  }

  // closing the door
  if (relay.state && !intr.intr.arduinoState) {
    Serial.println("Closing door");
    switchDoor(&relay);
    resetAllDevicesStates(leds, INT_SIZE_OF(sizeof(leds)), LOW);
    noTone(buzzer.pin);
    setStates();
    interruptGoToSleep(&intr.intr, 2);
  }

  setStates();
}

void delayInterrupt(DELAYED_INTERRUPT *intr, int interruptDelay = 1500) {
  if (millis() - intr->time > interruptDelay) {
    intr->intr.arduinoState = LOW;
    intr->time = millis();
  }
}

void interruptGoToSleep(INTERRUPT *intr, int interruptPin) {
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptWakeUp, HIGH);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  Serial.println("Going to sleep");
  delay(20);
  sleep_cpu();
}

void interruptWakeUp() {
  sleep_disable();
  intr.intr.arduinoState = HIGH;
  Serial.println("woken up");
}

void checkDistance(SENSOR *sensor, OUTPUT_DEVICE *devices[],
                   BLINKING_LED *blinkingLed, BUZZER *buzzer,
                   DELAYED_INTERRUPT *intr) {
  if (sensor->distance > 0 && sensor->distance <= 4) {
    blinkLed(blinkingLed);
    devices[1]->state = LOW;
    devices[0]->state = LOW;

    buzzer->freq = 5000;

    // get the millis to delayed interrupt
    if(intr->time + INTERRUPT_DELAY + 100 < millis()){
      intr->time = millis();
    }

    delayInterrupt(intr, INTERRUPT_DELAY);
  }

  if (sensor->distance > 4 && sensor->distance <= 10) {
    devices[2]->state = LOW;
    devices[1]->state = HIGH;
    devices[0]->state = LOW;

    buzzer->freq = 1000;
  }

  if (sensor->distance > 10) {
    devices[2]->state = LOW;
    devices[1]->state = LOW;
    devices[0]->state = HIGH;

    buzzer->freq = 500;
  }
}

void blinkLed(BLINKING_LED *led) {
  if (millis() - led->lastBlinkMs > led->blinkingSpeed) {
    led->device.state = !led->device.state;
    led->lastBlinkMs = millis();
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