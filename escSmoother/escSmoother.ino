#include <Servo.h>

// Debug flags
#define ENABLE_DEBUG  // enable this line if get debug print

#ifdef ENABLE_DEBUG
    #define DDD Serial
#endif  // ENABLE_DEBUG

volatile long pulseStartTimeD2 = 0;
volatile long lastPulseWidthD2 = 0;
volatile long pulseStartTimeD3 = 0;
volatile long lastPulseWidthD3 = 0;

const long NEUTRAL = 1530;
const long FULL_FORWARD = 2000;
const long FULL_REVERSE = 1000;

Servo esc;

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), pulseInD2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), pulseInD3, CHANGE);
  esc.attach(A0); // A0
  #ifdef ENABLE_DEBUG
    DDD.begin(9600);
  #endif
}

void loop() {
  long pulseWidth = lastPulseWidthD2;
  DDD.print("D2: ");
  DDD.println(pulseWidth);
  if (pulseWidth > NEUTRAL) {
    pulseWidth = NEUTRAL + (pulseWidth - NEUTRAL) / 8;
  }
  if (pulseWidth >= FULL_REVERSE) {
    esc.writeMicroseconds(pulseWidth);
  } else {
    esc.writeMicroseconds(FULL_REVERSE);
  }
  pulseWidth = lastPulseWidthD3;
  DDD.print("D3: ");
  DDD.println(pulseWidth);
}

void pulseInD2() {
    if (digitalRead(2)) {
      pulseStartTimeD2 = micros();
    } else {
      lastPulseWidthD2 = micros() - pulseStartTimeD2;
    }
}

void pulseInD3() {
    if (digitalRead(3)) {
      pulseStartTimeD3 = micros();
    } else {
      lastPulseWidthD3 = micros() - pulseStartTimeD3;
    }
}


