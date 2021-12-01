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
struct OUTPUT_DEVICE {
  char pin;
  char state;
};

struct OUTPUT_DEVICE relay = {2, HIGH};
struct OUTPUT_DEVICE led = {3, HIGH};

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

void setup() {
  Serial.begin(9600);

  pinMode(relay.pin, OUTPUT);
  pinMode(led.pin, OUTPUT);
  pinMode(button1.pin, INPUT);
  pinMode(button2.pin, INPUT);

  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);
}

void loop() {
  checkButton(&button1, &relay);
  checkButton(&button2, &led);

  delayedPrint();

  digitalWrite(relay.pin, relay.state);
  digitalWrite(led.pin, led.state);
}

/**
 * @brief Part of the code taken from:
 * https://www.arduino.cc/en/pmwiki.php?n=Tutorial/Debounce
 *
 */
void checkButton(BUTTON *button, OUTPUT_DEVICE *device) {
  char currentButtonState = digitalRead(button->pin);

  // If the switch changed, due to noise or pressing:
  if (currentButtonState != button->previousState) {
    // reset the debouncing timer
    times.lastDebounceTime = millis();
  }

  if ((millis() - times.lastDebounceTime) > button->debounceDelay) {
    // check if button state has changed
    if (currentButtonState != button->state) {
      button->state = currentButtonState;

      // only toggle the object state if the new button state is HIGH
      if (button->state == HIGH) {
        device->state = !device->state;
      }
    }
  }

  // Save the button state
  button->previousState = currentButtonState;
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