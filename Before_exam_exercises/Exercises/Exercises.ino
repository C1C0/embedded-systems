/**
 * @file Exercises.ino
 * @author Kristian
 * @brief Make a program that can control a relay from the push of a button, and
 * prints the state of the relay every second on the serial monitor. Actually,
 * second button controls LED
 * @version 0.1
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 */

/**
 * @wiring
 * relayPin (2) => relay: pin "IN"
 * ledPin (4) => led
 * breathingLED (3) => led
 * lm35 (A0) => LM35 temp.
 *
 * button 1: 1  2 - Legs taken: 1 (c: VCC),
 *           X  X               2 (c: buttonPin (8) + pull down resistor)
 *           .  .
 *           3  4
 *
 *
 * button 2: 1  2 - Legs taken: 1 (c: VCC),
 *           X  X               2 (c: buttonPin (9) + pull down resistor)
 *           .  .
 *           3  4
 *
 * OLED Display SH1106 - display1: RES - 5, DC - 6, CS - 7
 *
 * Medium Font:
 *  ________________
 * |T:XX.XC PX:H X:L|
 * |                |
 * |                |
 * |                |
 * |                |
 * |                |
 * |                |
 * |PX:FAST         |
 *  ________________
 *
 */

#include <stdbool.h>
#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

/**
 * @brief Delay for function delayedPrint()
 */
#define PRINT_DELAY 1000

/**
 * @brief Max upper temperature for systems operations
 */
#define MAX_UP_TEMP 30.0

// Other constants
#define SLOW_BREATH "SLOW"
#define FAST_BREATH "FAST"

/**
 * @brief Relay structure and state
 */
typedef struct OUTPUT_DEVICE {
  byte pin : 4;
  byte state : 1;
  byte uiPos[2];
} OUTPUT_DEVICE;

OUTPUT_DEVICE relay = {2, HIGH, {8, 0}};
OUTPUT_DEVICE led = {4, HIGH, {13, 0}};

/**
 * @brief LED which should has breathing function
 *
 */
typedef struct BREATHING_LED {
  OUTPUT_DEVICE device;

  /**
   * @brief Expected value range: <0; 255>
   *        Higher for faster blinking
   */
  short int smoothnessPts;

  /**
   * @brief Actual point of smoothness
   *        Represents actual brightness of LED at single moment
   */
  short int actualSmoothPt;

  /**
   * @brief delay between each state change
   *        does not change only speed but also smoothness
   *        of transitions
   *        Value range: <0;255>, recommended: 5
   */
  byte delay;

  /**
   * @brief Preferebly used to assigned millis() function value
   *        for further comparison
   */
  unsigned long lastCheckTimeMS;
} BREATHING_LED;

BREATHING_LED breathingLed = {{3, HIGH, {0, 8}}, 255, 0, 5, 0};

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

BUTTON button1 = {8, LOW, LOW, 75, 0};
BUTTON button2 = {9, LOW, LOW, 50, 0};

/**
 * @brief Times variables
 */
typedef struct TIMES {
  /**
   * @brief Preferebly used to assigned millis() function value
   *        for further comparison
   */
  unsigned long lastReadingTime;
} TIMES;

TIMES times = {0};

/**
 * @brief Temperature meter
 */
typedef struct TEMP_METER {
  byte pin : 4;

  /**
   * @brief last measured value should be stored here
   */
  float value;

  /**
   * @brief How often to check the temperature of specified meter
   */
  unsigned int checkTimeMs;

  /**
   * @brief Preferebly used to assigned millis() function value
   *        for further comparison
   */
  unsigned long lastCheckTimeMS;
} TEMP_METER;

TEMP_METER lm35 = {A0, 0, 500, 0};

/**
 * @brief SPI Display object
 */
U8X8_SH1106_128X64_NONAME_4W_HW_SPI display1(/* cs=*/7, /* dc=*/6,
                                             /* reset=*/5);

void setup() {
  Serial.begin(9600);

  setupDisplay(&display1, "Hello Stranger");

  pinMode(relay.pin, OUTPUT);
  pinMode(led.pin, OUTPUT);
  pinMode(button1.pin, INPUT);
  pinMode(button2.pin, INPUT);
  pinMode(lm35.pin, INPUT);

  // Set initial state of OUTPUT_DEVICE
  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);

  // Initial print of pin state
  UIDisplayPrintOutputDeviceState(&display1, &relay, 1);
  UIDisplayPrintOutputDeviceState(&display1, &led, 0);
}

void loop() {
  checkButtonChangeDevState(&display1, &button1, &relay, 1);
  checkButtonChangeDevState(&display1, &button2, &led, 0);

  delayedTempCheck(&display1, &lm35, MAX_UP_TEMP, &relay, &breathingLed, 1);

  PWMLEDBreathe(&breathingLed);

  // delayedPrint();

  assignStates();
}

/**
 * @brief Assing states to of specified devices
 * 
 */
void assignStates() {
  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);
}

/**
 * @brief Part of the code taken from:
 * https://www.arduino.cc/en/pmwiki.php?n=Tutorial/Debounce
 *
 */
void checkButtonChangeDevState(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                               BUTTON *button, OUTPUT_DEVICE *device,
                               char printP) {
  char currentButtonState = digitalRead(button->pin);

  checkButtonSetDebounce(button, &currentButtonState);
  checkPressTimeOfButton(button, device, &currentButtonState);

  // Save the button state
  button->previousState = currentButtonState;

  if (button->previousState != button->state) {
    UIDisplayPrintOutputDeviceState(display, device, printP);
  }
}

/**
 * @brief If the switch changed, due to noise or pressing, reset the debouncing
 * timer
 *
 * @param button
 * @param currentButtonState
 */
void checkButtonSetDebounce(BUTTON *button, char *currentButtonState) {
  if (*currentButtonState != button->previousState) {
    button->lastDebounceTimeMS = millis();
  }
}

/**
 * @brief check press tiem of the button and change state of specified output
 * device
 *
 * @param button
 * @param device
 * @param currentButtonState
 */
void checkPressTimeOfButton(BUTTON *button, OUTPUT_DEVICE *device,
                            char *currentButtonState) {
  // checks if the press time of the button is higher than debounce time
  // psecified on the button
  if ((millis() - button->lastDebounceTimeMS) > button->debounceDelay) {
    // check if button state has changed
    if (*currentButtonState != button->state) {
      button->state = *currentButtonState;

      // only toggle the object state if the new button state is HIGH
      if (button->state == HIGH) {
        device->state = !device->state;
      }
    }
  }
}

/**
 * @brief Print relay state to Serial
 *
 */
void delayedPrint() {
  if ((millis() - times.lastReadingTime) > PRINT_DELAY) {
    times.lastReadingTime = millis();
    Serial.println(relay.state);
    Serial.println(led.state);
  }
}

void delayedTempCheck(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                      TEMP_METER *temp, float max_up_temp,
                      OUTPUT_DEVICE *device, BREATHING_LED *PWMLed,
                      char printP) {
  if ((millis() - temp->lastCheckTimeMS) > temp->checkTimeMs) {
    checkIfTurnOnFan(display, temp, max_up_temp, device, PWMLed, printP);
    UIUpdateOutputTemp(display, temp);
  }
}

/**
 * @brief Check temperature every X seconds - specified in TEMP_METER structure
 *
 * @param display
 * @param temp
 */
void UIUpdateOutputTemp(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                        TEMP_METER *temp) {
  char tempS[8] = "T:";

  temp->lastCheckTimeMS = millis();

  checkTemp(temp);

  // Convert float value into string
  dtostrf(temp->value, -3, 1, tempS + 2);

  // Append C
  strcat(tempS, "C");

  // Draw UI
  display->drawString(0, 0, tempS);
}

/**
 * @brief Checks if value of temperature is higher than specified value
 * If so, changes state of device
 *
 * @param temp
 * @param device
 */
void checkIfTurnOnFan(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                      TEMP_METER *temp, float max_top_temp,
                      OUTPUT_DEVICE *device, BREATHING_LED *PWMLed,
                      char printP) {
  char prevState = device->state;

  if (temp->value > max_top_temp) {
    device->state = HIGH;
    setPWMLedBreathingSpeed(FAST_BREATH, PWMLed);
  } else {
    device->state = LOW;
    setPWMLedBreathingSpeed(SLOW_BREATH, PWMLed);
  }

  if (prevState != device->state) {
    UIDisplayPrintOutputDeviceState(display, device, printP);
  }
}

/**
 * @brief Performs breathing on output device
 *
 * @param PWMLed
 */
void PWMLEDBreathe(BREATHING_LED *PWMLed) {
  if ((millis() - PWMLed->lastCheckTimeMS) > PWMLed->delay) {
    // Sets the actual chekcing time for delay
    PWMLed->lastCheckTimeMS = millis();
    PWMLed->actualSmoothPt++;

    if (PWMLed->actualSmoothPt > PWMLed->smoothnessPts) {
      PWMLed->actualSmoothPt = 0;
    }

    // Performs the actual brightness of LED
    float pwm_val = 255.0 * (1.0 - abs((2.0 * ((float)PWMLed->actualSmoothPt /
                                               (float)PWMLed->smoothnessPts)) -
                                       1.0));

    analogWrite(PWMLed->device.pin, int(pwm_val));
  }
}

/**
 * @brief On selected BREATHING_LED sets the speed of breathing
 *
 * @param speed
 * @param PWMLed
 */
void setPWMLedBreathingSpeed(char *speed, BREATHING_LED *PWMLed) {
  if (strcmp(speed, "FAST")) {
    PWMLed->smoothnessPts = 100;
  }

  if (strcmp(speed, "SLOW")) {
    PWMLed->smoothnessPts = 25;
  }
}

/**
 * @brief Get the Temperature of temp. meter
 *
 * @param temp
 */
void checkTemp(TEMP_METER *temp) {
  // read the value from the PIN,
  // convert this value to voltage (5000 / 1024.0),
  // and then to temperature (/10)
  temp->value = analogRead(lm35.pin) * (5000 / 1024.0F) / 10;
}

/**
 * @brief print state of SELECTED device
 *
 * @param display
 * @param device
 */
void UIDisplayPrintOutputDeviceState(
    U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display, OUTPUT_DEVICE *device,
    char printP) {

  char s[5];

  if (printP == 1) {
    sprintf(s, "P%d:%c", device->pin, device->state == 0 ? 'L' : 'H');
  } else {
    sprintf(s, "%d:%c", device->pin, device->state == 0 ? 'L' : 'H');
  }

  display->drawString(device->uiPos[0], device->uiPos[1], s);
}

/**
 * @brief Sets up chosen display
 *
 */
void setupDisplay(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                  char *setupMessage) {
  display->begin();
  display->setPowerSave(0);
  display->setFont(u8x8_font_chroma48medium8_r);

  displayTextToMiddle(display, setupMessage);
  delay(1000);
  display->clear();
}

/**
 * @brief Prints string to the middle of the screen
 *
 * @param s
 */
void displayTextToMiddle(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                         char *s) {
  char sLen = strlen(s);

  char row = display->getRows() / 2;
  char col = display->getCols() / 2 - (sLen / 2);

  display->drawString(col, row, s);
}