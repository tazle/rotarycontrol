
#ifndef LED_BLINKER_H
#define LED_BLINKER_H 1
#include <Timing.h>

class LedBlinker {
private:
  unsigned long lastChangeUs;
  int state;
  unsigned long delayUs;
  int pin;

public:
  LedBlinker(const int &_pin, const int &initialDelayMs):
    lastChangeUs(0),
    state(LOW),
    delayUs(initialDelayMs*1000),
    pin(_pin) {
  }

  void tick() {
    unsigned long nowUs = micros();
    unsigned long deltaUs = usDelta(lastChangeUs, nowUs);
    if (deltaUs > delayUs) {
      lastChangeUs = nowUs;
      state = !state;
      digitalWrite(pin, state);
    }
  }

  void setDelay(int delayMs) {
    delayUs = delayMs * 1000;
  }
};

#endif /* LED_BLINKER_H */
