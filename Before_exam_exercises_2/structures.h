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
   * @brief Specifies, if new initialization (of measurement) should start
   */
  byte newInit : 1;

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
  byte state : 1;

  /**
   * @brief Displayed position on display
   */
  byte uiPos[2];
} OUTPUT_DEVICE;

#endif