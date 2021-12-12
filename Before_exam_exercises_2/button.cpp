#include <Arduino.h>
#include "structures.h"

void _checkButtonSetDebounce(BUTTON *, byte *currentButtonState);
void _checkPressTimeOfButton(BUTTON *, byte *state, byte *currentButtonState);

/**
 * @brief Part of the code taken from:
 * https://www.arduino.cc/en/pmwiki.php?n=Tutorial/Debounce
 *
 */
void checkButtonChangeDevState(BUTTON *button, byte *state) {

  byte currentButtonState = digitalRead(button->pin);

  _checkButtonSetDebounce(button, &currentButtonState);
  _checkPressTimeOfButton(button, state, &currentButtonState);

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
void _checkButtonSetDebounce(BUTTON *button, byte *currentButtonState) {
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
void _checkPressTimeOfButton(BUTTON *button, byte *state,
                             byte *currentButtonState) {
  // checks if the press time of the button is higher than debounce time
  // psecified on the button
  if ((millis() - button->lastDebounceTimeMS) > button->debounceDelay) {
    // check if button state has changed
    if (*currentButtonState != button->state) {
      button->state = *currentButtonState;

      // only toggle the object state if the new button state is HIGH
      if (button->state == HIGH) {
        *state = !(*state);
      }
    }
  }
}