#include <Servo.h>

/*
   My RC controller:
    Throttle is on Channel 2
    The wheel is on Channel 1
    Switch button on Channel 3

    Throttle is smoothed in the forward direction, by THROTTLE_SMOOTH_FACTOR, as the Red Brick ESC + Motor does not spin until ~1550us pulse.
    There is a THROTTLE_SMOOTH_OFFSET about 30us as well.

    When the wheel is at neutral, input from throttle is used; otherwise the wheel input is transparently passed on to ESC.

    Channel 3 is connected to a relay.
*/
// Debug flags
#define ENABLE_DEBUG  // enable this line if get debug print

#ifdef ENABLE_DEBUG
#define DDD Serial
#endif  // ENABLE_DEBUG

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
const int ch3Pin = A1;

const long NEUTRAL = 1530;
const long BIAS_WINDOW = THROTTLE_SMOOTH_OFFSET;
const long FULL_FORWARD = 2000;
const long FULL_REVERSE = 1000;

Servo esc;

void setup() {
  attachInterrupt(digitalPinToInterrupt(ch2Pin), pulseInCh2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ch1Pin), pulseInCh1, CHANGE);
  esc.attach(A0); // A0
  //pinMode(ch3Pin, INPUT_PULLUP);
#ifdef ENABLE_DEBUG
  DDD.begin(9600);
#endif
}

void loop() {
  long wheelPulseWidth = lastPulseWidthCh1;
  long throttlePulseWidth = lastPulseWidthCh2;
  int ch3 = analogRead(ch3Pin);
#ifdef ENABLE_DEBUG
  DDD.print("Ch1: ");
  DDD.print(wheelPulseWidth);
  DDD.print("; Ch2: ");
  DDD.print(throttlePulseWidth);
  DDD.print("; Ch3: ");
  DDD.println(ch3);
#endif
  if (wheelPulseWidth < NEUTRAL - BIAS_WINDOW || wheelPulseWidth > NEUTRAL + BIAS_WINDOW) {
    esc.writeMicroseconds(wheelPulseWidth);
  } else {
    // the wheel is at neutral, apply smoothed throttle input
    if (throttlePulseWidth > NEUTRAL + BIAS_WINDOW) {
      esc.writeMicroseconds((throttlePulseWidth - NEUTRAL - BIAS_WINDOW) / THROTTLE_SMOOTH_FACTOR + NEUTRAL + THROTTLE_SMOOTH_OFFSET);
    } else if (throttlePulseWidth >= FULL_REVERSE) {
      esc.writeMicroseconds(throttlePulseWidth);
    } else {
      // RC controller could be turned off. we want to arm ESC anyway.
      esc.writeMicroseconds(FULL_REVERSE);
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


