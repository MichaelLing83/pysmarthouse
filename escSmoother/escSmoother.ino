#include <Servo.h>

/*
  My RC controller:
    1. connections:
      Throttle is on Channel 2
      The wheel is on Channel 1
      Switch button on Channel 3
    2. functionality
      a. Throttle: smoothed so that
        1) it can produce only motor speed in [NEUTRAL, FULL_FORWARD / THROTTLE_SMOOTH_FACTOR]
            to reach full speed, use the Wheel
        * work-around to that the Red Brick ESC + Motor does not spin until ~1550us pulse.
          There is a THROTTLE_SMOOTH_OFFSET about 30us as well.
      b. Wheel:
        1) When the wheel is at neutral, ignore input from throttle;
        2) otherwise the wheel input is transparently passed on to ESC.
      c. swith button on Channel 3: not used yet
*/

// Debug flags
#define ENABLE_DEBUG  // enable this line if get debug print

#ifdef ENABLE_DEBUG
#define DDD Serial
unsigned long last_debug_print_time;
unsigned long curr_time;
const unsigned long debug_print_interval = 500; // ms
#endif  // ENABLE_DEBUG

// motor speed/drive constants
const long NEUTRAL = 1530;
const long BIAS_WINDOW = THROTTLE_SMOOTH_OFFSET;
const long FULL_FORWARD = 2000;
const long FULL_REVERSE = 1000;

const long THROTTLE_SMOOTH_FACTOR = 4;
const long THROTTLE_SMOOTH_OFFSET = 30;

// channel 2 on pin D2
const int ch2Pin = 2;
volatile long pulseStartTimeCh2 = 0;
volatile long lastPulseWidthCh2 = 0;

// channel 1 on pin D3
const int ch1Pin = 3;
volatile long pulseStartTimeCh1 = 0;
volatile long lastPulseWidthCh1 = 0;

// channel 3 (on/off only) on pin D4
const int ch3Pin = 4;

// two ESC can be connected
const int esc0Pin = A0;
const int esc1Pin = A1;

Servo esc0, esc1;

void setup() {
  attachInterrupt(digitalPinToInterrupt(ch2Pin), pulseInCh2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ch1Pin), pulseInCh1, CHANGE);
  esc0.attach(esc0Pin);
  esc1.attach(esc1Pin);
  //pinMode(ch3Pin, INPUT_PULLUP);
#ifdef ENABLE_DEBUG
  DDD.begin(9600);
  last_debug_print_time = 0;
#endif
}

void loop() {
  long wheelPulseWidth = lastPulseWidthCh1;
  long throttlePulseWidth = lastPulseWidthCh2;
#ifdef ENABLE_DEBUG
  curr_time = millis();
  if (last_debug_print_time > curr_time || last_debug_print_time + debug_print_interval <= curr_time) {
    DDD.print("Ch1: ");
    DDD.print(wheelPulseWidth);
    DDD.print("; Ch2: ");
    DDD.print(throttlePulseWidth);
    last_debug_print_time = millis();
  }
#endif
  // if Wheel is not at neutral
  if (wheelPulseWidth < NEUTRAL - BIAS_WINDOW || wheelPulseWidth > NEUTRAL + BIAS_WINDOW) {
    // pass Wheel throttle transparently to both ESC's
    esc0.writeMicroseconds(wheelPulseWidth);
    esc1.writeMicroseconds(wheelPulseWidth);
  } else {
    // the wheel is at neutral, apply smoothed throttle input
    if (throttlePulseWidth > NEUTRAL + BIAS_WINDOW) {
      esc0.writeMicroseconds((throttlePulseWidth - NEUTRAL - BIAS_WINDOW) / THROTTLE_SMOOTH_FACTOR + NEUTRAL + THROTTLE_SMOOTH_OFFSET);
      esc1.writeMicroseconds((throttlePulseWidth - NEUTRAL - BIAS_WINDOW) / THROTTLE_SMOOTH_FACTOR + NEUTRAL + THROTTLE_SMOOTH_OFFSET);
    } else if (throttlePulseWidth >= FULL_REVERSE) {
      esc0.writeMicroseconds(throttlePulseWidth);
      esc1.writeMicroseconds(throttlePulseWidth);
    } else {
      // RC controller could be turned off. we want to arm ESC anyway.
      esc0.writeMicroseconds(FULL_REVERSE);
      esc1.writeMicroseconds(FULL_REVERSE);
    }
  }
}

void pulseInCh2() {
  if (digitalRead(ch2Pin)) {
    pulseStartTimeCh2 = micros();
  } else {
    lastPulseWidthCh2 = micros() - pulseStartTimeCh2;
  }
}

void pulseInCh1() {
  if (digitalRead(ch1Pin)) {
    pulseStartTimeCh1 = micros();
  } else {
    lastPulseWidthCh1 = micros() - pulseStartTimeCh1;
  }
}
