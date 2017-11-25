/*
 * Project MyProject
 * Description:
 * Author:
 * Date:
 */

 int LED = D7;
 int SWITCH = A3;
 int ROTARY_LINE_1 = D1;
 int ROTARY_LINE_2 = D4;

// setup() runs once, when the device is first turned on.
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(ROTARY_LINE_1, INPUT_PULLUP);
  pinMode(ROTARY_LINE_2, INPUT_PULLUP);
  attachInterrupt(SWITCH, debounceSwitch, CHANGE);
  attachInterrupt(ROTARY_LINE_1, debounceRotary, CHANGE);
  attachInterrupt(ROTARY_LINE_2, debounceRotary, CHANGE);
  USBSerial1.begin();
}

static const int MIN_DELAY = 250;
static const int MAX_DELAY = 1000;

static int counter = MIN_DELAY;

#define MICROS_ROLLOVER ((unsigned long)59652323)
static unsigned long usDelta(unsigned long old_us, unsigned long new_us) {
    new_us -= old_us;
    if((long)new_us < 0)
        new_us += MICROS_ROLLOVER;
    return new_us;
}

bool checkDebounce(unsigned long *lastCallTime, unsigned long debounceUs) {
  unsigned long now = micros();
  unsigned long delta = usDelta(*lastCallTime, now);
  if (delta >= debounceUs) {
    *lastCallTime = now;
    return true;
  } else {
    return false;
  }
}

static unsigned long switchDebounceUs = 0;
static bool switchState = false;

void debounceSwitch() {
  if (checkDebounce(&switchDebounceUs, 1000)) {
    switchState = !switchState;
    if (switchState) {
      incCounter(50);
//      USBSerial1.printf("counter: %d\n", counter);
    }
  } else {
//    USBSerial1.printf("debounce switch\n");
  }
}

static unsigned long rotaryDebounceUs = 0;

void debounceRotary() {
  if (checkDebounce(&rotaryDebounceUs, 1000)) {
    triggerRotaryRead();
  } else {
    //USBSerial1.printf("debounce rotary\n");
  }
}

void incCounter(int increment) {
  counter += increment;
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  int delay_ms = counter;
  if (counter < MIN_DELAY) {
    delay_ms = MIN_DELAY;
    counter = MAX_DELAY;
  }
  if (counter > MAX_DELAY) {
    delay_ms = MAX_DELAY;
    counter = MIN_DELAY;
  }

  digitalWrite(LED, HIGH);
  delay(delay_ms);
  digitalWrite(LED, LOW);
  delay(delay_ms);
}
