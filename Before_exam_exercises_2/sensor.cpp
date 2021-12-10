/**
 * @file sensor.c
 * @author Kristian Palovic
 * @brief Sensor library - use only with Arduino
 * @version 0.1
 * @date 2021-12-08
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <Arduino.h>
#include <stdbool.h>
#include "structures.h"

void _initReading(SENSOR *);
void _switchTrigAndGetMicro(SENSOR *, int state, int switchTo);
void _getValue(SENSOR *, bool enablePrinting);
void _switchInitLevel(SENSOR *, int switchTo);

/**
 * @brief Sets the pins of sensor to correct states
 *
 * @param sensor
 */
void setupSensor(SENSOR *sensor) {
  pinMode(sensor->trigPin, OUTPUT);
  pinMode(sensor->echoPin, INPUT);
}

/**
 * @brief Measures distance on selected (supported) sensor of type SENSOR
 *
 * @param sensor
 */
void measureDistance(SENSOR *sensor, bool enablePrinting) {
  // Reading time perfomed only after specified reading time
  if ((millis() - sensor->lastReading) > sensor->readingTime) {

    // SENSOR has two states: Initializing and Pulsing (reading)
    if (sensor->newInit) {
      _initReading(sensor);
    } else {
      _getValue(sensor, enablePrinting);
    }
  }
}

/**
 * @brief Initializes reading of SENSOR
 *
 * @doc
 * TrigPin = LOW
 * wait 5 micros
 * TrigPin = HIGH
 * wait 10 micros
 * TrigPin = LOW
 * ... reading can start
 *
 * @param sensor
 */
void _initReading(SENSOR *sensor) {
  if (sensor->initLevel == 0) {
    _switchTrigAndGetMicro(sensor, LOW, 1);
  }

  // Clear the trigPin by setting it LOW:
  if ((micros() - sensor->trigLastChange) > 5) {
    if (sensor->initLevel == 1) {
      _switchTrigAndGetMicro(sensor, HIGH, 2);
    }

    // Trigger the sensor by setting the trigPin high for 10 microseconds:;
    if ((micros() - sensor->trigLastChange) > 10 && sensor->initLevel == 2) {
      digitalWrite(sensor->trigPin, LOW);
      sensor->initLevel = 0;
      sensor->newInit = 0;
      sensor->trigLastChange = micros();
    }
  }
}

/**
 * @brief Switchis state of trigPin to specified state value, only if previous
 * state is not the same
 * after switching sets micros() on trigLastChange
 *
 * @param sensor
 * @param state
 */
void _switchTrigAndGetMicro(SENSOR *sensor, int state, int switchTo) {
  if (digitalRead(sensor->trigPin) != state || sensor->initLevel != switchTo) {
    digitalWrite(sensor->trigPin, state);
    _switchInitLevel(sensor, switchTo);
    sensor->trigLastChange = micros();
  }
}

/**
 * @brief measures value (distance) with the sensor and states newInit state to
 * 1
 *
 * @doc
 * distance = [HIGH level time] * [cm/micros] / 2
 *
 * @param sensor
 */
void _getValue(SENSOR *sensor, bool enablePrinting = false) {
  // Read the echoPin, pulseIn() returns the duration (length of the pulse)
  // in microseconds
  sensor->duration = pulseIn(sensor->echoPin, HIGH);
  sensor->distance = sensor->duration * 0.034 / 2;

  if (enablePrinting) {
    Serial.print(sensor->distance);
    Serial.println(" cm");
  }

  sensor->lastReading = millis();
  sensor->newInit = 1;
}

void _switchInitLevel(SENSOR *sensor, int switchTo) {
  sensor->initLevel = switchTo;
}