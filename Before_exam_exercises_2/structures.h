#ifndef STRUCTURES
#define STRUCTURES

/**
 * @brief Defines 2 + 2 pin Ultrasonic Sensor
 * use alongside with sensor.cpp
 */
typedef struct SENSOR {
  /**
   * @brief Trigger pin
   * ! INIT VALUE !
   */
  byte trigPin : 4;

  /**
   * @brief Echo pin
   * ! INIT VALUE !
   */
  byte echoPin : 4;

  /**
   * @brief Specifies time how often should sensor read the value
   * ! INIT VALUE !
   */
  unsigned int readingTime;

  /**
   * @brief Measured distance
   */
  int distance : 9;

  /**
 * @brief Defines level of initialization
 *
 */
  byte initLevel : 4;

  /**
   * @brief Duration of pulse in microseconds
   */
  unsigned long duration;

  /**
   * @brief last reading time of trigger pin- prefferably use micro() function
   */
  unsigned long trigLastChange;

  /**
   * @brief last reading of the sensor - prefferable use with millis() function
   */
  unsigned long lastReading;

} SENSOR;

/**
 * @brief Any device with OUTPUT PIN and STATE
 */
typedef struct OUTPUT_DEVICE {
  byte pin : 4;
  byte state;

  /**
   * @brief Displayed position on display
   */
  byte uiPos[2];
} OUTPUT_DEVICE;

/**
 * @brief Device producing sound
 */
typedef struct BUZZER{
  byte pin: 4;

  /**
   * @brief specifies actual played frequency
   */
  int freq: 15;

} BUZZER;

/**
 * @brief Button structure and states
 */
typedef struct BUTTON {
  byte pin : 4;
  byte state : 1;

  /**
   * @brief Used for comparison to "state"
   */
  byte previousState : 1;

  /**
   * @brief Used for delaying swithing of button
   *        if flickers - used higher number
   *        range: (0; 128)
   */
  byte debounceDelay : 7;

  /**
   * @brief Preferebly used to assigned millis() function value
   *        for further comparison
   */
  unsigned long lastDebounceTimeMS;
} BUTTON;

#endif