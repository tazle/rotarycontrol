
#ifndef TIMING_H
#define TIMING_H 1
#define MICROS_ROLLOVER ((unsigned long)59652323)
static unsigned long usDelta(unsigned long old_us, unsigned long new_us) {
    new_us -= old_us;
    if((long)new_us < 0)
        new_us += MICROS_ROLLOVER;
    return new_us;
}

#endif /* TIMING_H */
