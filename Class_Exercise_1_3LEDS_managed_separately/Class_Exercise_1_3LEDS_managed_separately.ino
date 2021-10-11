/*

   Created by Kristian Palovic
*/

#define R1 2
#define LED1 3
#define LED2 5
#define LED3 6

int state1Active = 0;
int state2Active = 0;
int state3Active = 0;

long long unsigned previousMillis [3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  pinMode(R1, OUTPUT);

  pinMode(LED1, INPUT);
  pinMode(LED2, INPUT);
  pinMode(LED3, INPUT);

  // check if realy works
  activateRelay(R1);
  delay(1000);
  deactivateRelay(R1);

  // check if LEDs work
  manageLEDState(LED1, HIGH);
  manageLEDState(LED2, HIGH);
  manageLEDState(LED3, HIGH);
  delay(1000);
  manageLEDState(LED1, LOW);
  manageLEDState(LED2, LOW);
  manageLEDState(LED3, LOW);

}

void loop() {
  /*
     Goal:

     make state on and off affected by milis()
  */

  state1Active = checkIfIntervalReached(1000, state1Active, 0);
  state2Active = checkIfIntervalReached(1200, state2Active, 1);
  state3Active = checkIfIntervalReached(1400, state3Active, 2);

  Serial.println("active1");
  Serial.println(state1Active);

  Serial.println("active2");
  Serial.println(state2Active);

  if (state1Active) {
    manageLEDState(LED1, HIGH);
  } else {
    manageLEDState(LED1, LOW);
  }

  if (state2Active) {
    manageLEDState(LED2, HIGH);
  } else {
    manageLEDState(LED2, LOW);
  }

  if (state3Active) {
    manageLEDState(LED3, HIGH);
  } else {
    manageLEDState(LED3, LOW);
  }

  if (!state1Active && !state2Active && !state3Active) {
    activateRelay(R1);
  } else {
    deactivateRelay(R1);
  }

}

int checkIfIntervalReached(int interval, int stateAffeted, int previousMillisIndex) {
  unsigned long millisProvided = millis();
  if (millisProvided - previousMillis[previousMillisIndex] > interval) {
    previousMillis[previousMillisIndex] = millisProvided;

    if (stateAffeted) {
      return 0;
    }
    return 1;
  }

  return stateAffeted;
}

/*
   Activates the LOW-init logic Relay
*/
void activateRelay(int relayPin) {
  digitalWrite(relayPin, LOW);
}

/*
   Deactivates LOW-init logic RELAY
*/
void deactivateRelay(int relayPin) {
  digitalWrite(relayPin, HIGH);
}

void manageLEDState(int LED, int state) {
  digitalWrite(LED, state);
}
