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
#include "structures.h"

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
void measureDistance(SENSOR *sensor) {
  // Reading time perfomed only after specified reading time
  if ((millis() - sensor->lastReading) > sensor->readingTime) {

    // SENSOR has two states: Initializing and Pulsing (reading)
    if (sensor->newInit) {
      _initReading(sensor);
    } else {
      _getValue(sensor);
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
  _switchTrigAndGetMicro(sensor, LOW);

  // Clear the trigPin by setting it LOW:
  if ((micros() - sensor->trigLastChange) > 5) {
    _switchTrigAndGetMicro(sensor, HIGH);

    // Trigger the sensor by setting the trigPin high for 10 microseconds:
    if ((micros() - sensor->trigLastChange) > 10) {
      digitalWrite(sensor->trigPin, LOW);
      sensor->newInit = 0;
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
void _switchTrigAndGetMicro(SENSOR *sensor, byte state) {
  if (digitalRead(sensor->trigPin) != state) {
    digitalWrite(sensor->trigPin, state);
    sensor->trigLastChange = micros();
  }
}

/**
 * @brief measures value (distance) with the sensor and states newInit state to 1
 * 
 * @doc
 * distance = [HIGH level time] * [cm/micros] / 2
 * 
 * @param sensor 
 */
void _getValue(SENSOR *sensor) {
  sensor->lastReading = millis();
  sensor->newInit = 1;

  // Read the echoPin, pulseIn() returns the duration (length of the pulse)
  // in microseconds
  sensor->duration = pulseIn(sensor->echoPin, HIGH);
  sensor->distance = sensor->duration * 0.034 / 2;
}