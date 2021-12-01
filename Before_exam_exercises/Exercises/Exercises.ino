/**
 * @file Exercises.ino
 * @author Kristian
 * @brief Make a program that can control a relay from the push of a button, and
 * prints the state of the relay every second on the serial monitor.
 * @version 0.1
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 */

/**
 * @wiring
 * relayPin (2) => relay: pin "IN"
 *
 * button: 1  2 - Legs taken: 1 (c: VCC), 2 (c: buttonPin (8) + pull down
 * resistor)
 *         X  X
 *         .  .
 *         3  4
 *
 */

#define PRINT_DELAY 1000

/**
 * @brief Relay structure and state
 *
 */
struct RELAY {
  char pin;
  char state;
} relay = {2, LOW};

/**
 * @brief Button structure and states
 *
 */
struct BUTTON {
  char pin;
  char state;
  char previousState;
  unsigned long debounceDelay;
} button = {8, LOW, LOW, 75};

/**
 * @brief Times variables
 *
 */
struct TIMES {
  unsigned long lastDebounceTime;
  unsigned long lastReadingTime;
} times = {0, 0};

void setup() {
  Serial.begin(9600);

  pinMode(relay.pin, OUTPUT);
  pinMode(button.pin, INPUT);

  digitalWrite(relay.pin, relay.state);
}

void loop() {
  checkButton();
  delayedPrint();
  digitalWrite(relay.pin, relay.state);
}

/**
 * @brief Part of the code taken from: https://www.arduino.cc/en/pmwiki.php?n=Tutorial/Debounce
 * 
 */
void checkButton() {
  char currentButtonState = digitalRead(button.pin);

  // If the switch changed, due to noise or pressing:
  if (currentButtonState != button.previousState) {
    // reset the debouncing timer
    times.lastDebounceTime = millis();
  }

  if ((millis() - times.lastDebounceTime) > button.debounceDelay) {
    // check if button state has changed
    if (currentButtonState != button.state) {
      button.state = currentButtonState;

      // only toggle the LED if the new button state is HIGH
      if (button.state == HIGH) {
        relay.state = !relay.state;
      }
    }
  }

  // Save the button state
  button.previousState = currentButtonState;
}

/**
 * @brief Print relay state to Serial
 * 
 */
void delayedPrint() {
  if ((millis() - times.lastReadingTime) > PRINT_DELAY) {
    times.lastReadingTime = millis();
    Serial.println(relay.state);
  }
}