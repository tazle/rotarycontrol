#include <LedBlinker.h>
#include <Timing.h>

// Rotery stuff

const int8_t KNOBDIR[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
 -1,  0,  0,  1,
  0,  1, -1,  0
};

enum Direction {ROT_LEFT, ROT_RIGHT};

struct Rotary {
  int8_t state = 3;
  Direction lastDir = ROT_LEFT;
  int32_t position = 0;
};

bool rotaryStateChanged(Rotary &rotary, int sig1, int sig2) {
  int8_t newState = sig1 | (sig2 << 1);
  int8_t change = KNOBDIR[newState | (rotary.state<<2)];
  return change != 0;
}

void rotaryUpdate(Rotary &rotary, int sig1, int sig2) {
  int8_t newState = sig1 | (sig2 << 1);
  int8_t change = KNOBDIR[newState | (rotary.state<<2)];
  rotary.position += change;
  rotary.state = newState;
  if (change < 0) {
    rotary.lastDir = ROT_LEFT;
  } else if (change > 0) {
    rotary.lastDir = ROT_RIGHT;
  }
}

// The rest

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
static Rotary rotary;

void debounceRotary() {
  int l1 = digitalRead(ROTARY_LINE_1);
  int l2 = digitalRead(ROTARY_LINE_2);

  if (rotaryStateChanged(rotary, l1, l2) && checkDebounce(&rotaryDebounceUs, 1000)) {
    rotaryUpdate(rotary, l1, l2);
    USBSerial1.printf("rot: %d, %d, %d\n", rotary.state, rotary.lastDir, rotary.position);
  } else {
    //USBSerial1.printf("debounce rotary\n");
  }
}

static bool needRotaryRead = false;

void incCounter(int increment) {
  //counter += increment;
}

static const int MIN_DELAY = 50;
static const int MAX_DELAY = 500;
static const int DEFAULT_DELAY = 250;

static LedBlinker led(LED, DEFAULT_DELAY);

int iter = 0;
// loop() runs over and over again, as quickly as it can execute.
void loop() {
  int delay_ms = rotary.position + DEFAULT_DELAY;
  if (delay_ms < MIN_DELAY) {
    delay_ms = MIN_DELAY;
  }
  if (delay_ms > MAX_DELAY) {
    delay_ms = MAX_DELAY;
  }

  led.setDelay(delay_ms);
  led.tick();

  delay(10);
  iter++;
}
