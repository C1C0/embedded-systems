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
 * ledPin (3) => led
 *
 * button 1: 1  2 - Legs taken: 1 (c: VCC), 2 (c: buttonPin (8) + pull down
 * resistor)
 *         X  X
 *         .  .
 *         3  4
 *
 *
 * button 2: 1  2 - Legs taken: 1 (c: VCC), 2 (c: buttonPin (9) + pull down
 * resistor)
 *         X  X
 *         .  .
 *         3  4
 *
 * OLED Display SH1106 - display1: RES - 5, DC - 6, CS - 7
 *
 * Medium Font:
 *  ________________
 * |T:XX.XC P2:H 3:L|
 * |                |
 * |                |
 * |                |
 * |                |
 * |                |
 * |                |
 * |                |
 *  ________________
 */

#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#define PRINT_DELAY 1000
#define UI_PRINT_DELAY 500

/**
 * @brief Relay structure and state
 *
 */
struct OUTPUT_DEVICE {
  char pin;
  char state;
  char uiPos[2];
};

struct OUTPUT_DEVICE relay = {2, HIGH, {8, 0}};
struct OUTPUT_DEVICE led = {3, HIGH, {13, 0}};

/**
 * @brief Button structure and states
 *
 */
struct BUTTON {
  char pin;
  char state;
  char previousState;
  unsigned long debounceDelay;
};

struct BUTTON button1 = {8, LOW, LOW, 75};
struct BUTTON button2 = {9, LOW, LOW, 50};

/**
 * @brief Times variables
 *
 */
struct TIMES {
  unsigned long lastDebounceTime;
  unsigned long lastReadingTime;
} times = {0, 0};

/**
 * @brief Temperature meter
 *
 */
struct TEMP_METER {
  char pin;
  float value;
  short int checkTimeMs;
  unsigned long lastCheckTimeMS;
} lm35 = {A0, 0, 2000, 0};

/**
 * @brief SPI Display object
 *
 */
U8X8_SH1106_128X64_NONAME_4W_HW_SPI display1(/* cs=*/7, /* dc=*/6,
                                             /* reset=*/5);

void setup() {
  Serial.begin(9600);

  pinMode(relay.pin, OUTPUT);
  pinMode(led.pin, OUTPUT);
  pinMode(button1.pin, INPUT);
  pinMode(button2.pin, INPUT);
  pinMode(lm35.pin, INPUT);

  // Set initial state of OUTPUT_DEVICE
  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);

  setupDisplay(&display1, "Hello Stranger");

  // Initial print of pin state
  UIDisplayPrintOutputDeviceState(&display1, &relay, 1);
  UIDisplayPrintOutputDeviceState(&display1, &led, 0);
}

void loop() {
  checkButtonChangeDevState(&button1, &relay);
  checkButtonChangeDevState(&button2, &led);

  delayedPrint();

  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);

  UIUpdateOutputDev(&display1, &relay, &button1, 1);
  UIUpdateOutputDev(&display1, &led, &button2, 0);

  UIUpdateOutputTemp(&display1, &lm35);
}

/**
 * @brief Part of the code taken from:
 * https://www.arduino.cc/en/pmwiki.php?n=Tutorial/Debounce
 *
 */
void checkButtonChangeDevState(BUTTON *button, OUTPUT_DEVICE *device) {
  char currentButtonState = digitalRead(button->pin);

  checkButtonSetDebounce(button, &currentButtonState);
  checkPressTimeOfButton(button, device, &currentButtonState);

  // Save the button state
  button->previousState = currentButtonState;
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
    times.lastDebounceTime = millis();
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
  if ((millis() - times.lastDebounceTime) > button->debounceDelay) {
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

/**
 * @brief decides, when update UI
 *
 * @param display
 * @param device
 * @param button
 */
void UIUpdateOutputDev(U8X8_SH1106_128X64_NONAME_4W_HW_SPI *display,
                       OUTPUT_DEVICE *device, BUTTON *button, char printP) {
  if (button->previousState != button->state) {
    UIDisplayPrintOutputDeviceState(display, device, printP);
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
  if ((millis() - temp->lastCheckTimeMS) > temp->checkTimeMs) {
    char tempS[8] = "T:";

    temp->lastCheckTimeMS = millis();

    // read the value from the PIN,
    // convert this value to voltage (5000 / 1024.0),
    // and then to temperature (/10)
    temp->value = analogRead(lm35.pin) * (5000 / 1024.0F) / 10;

    // Convert float value into string    
    dtostrf(temp->value, -3, 1, tempS+2);

    // Append C
    strcat(tempS, "C");

    // Draw UI
    display->drawString(0, 0, tempS);
  }
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