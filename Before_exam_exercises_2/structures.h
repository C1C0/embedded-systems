#ifndef STRUCTURES
#define STRUCTURES

/**
 * @brief Defines 2 + 2 pin Ultrasonic Sensor
 * use alongside with sensor.cpp
 */
typedef struct SENSOR {
  /**
   * @brief Trigger pin
   */
  byte trigPin : 4;

  /**
   * @brief Echo pin
   */
  byte echoPin : 4;

  /**
   * @brief Specifies time how often should sensor read the value
   */
  unsigned int readingTime;

  /**
   * @brief Measured distance
   */
  int distance : 9;

  /**
   * @brief Duration of pulse in microseconds
   */
  unsigned long duration;

  /**
   * @brief Specifies, if new initialization (of measurement) should start
   */
  byte newInit : 1;

  /**
   * @brief last reading time of trigger pin- prefferably use micro() function
   */
  unsigned long trigLastChange;

  /**
   * @brief last reading of the sensor - prefferable use with millis() function
   */
  unsigned long lastReading;

} SENSOR;

#endif