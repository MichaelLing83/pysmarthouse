#include "TimerOne.h"
#include "VoltageSensor.h"
#include "CurrentSensor.h"

// Debug flags
#define ENABLE_DEBUG  // enable this line if get debug print
//#define DEBUG_CONSOLE // enable this line if Yun shield is used
#ifdef ENABLE_DEBUG
  #ifdef DEBUG_CONSOLE
    #include <Console.h>
    #define DDD Console
  #else
    #define DDD Serial
  #endif  // DEBUG_CONSOLE
#endif  // ENABLE_DEBUG

/*
 * Timer based interrupt
 * with 32ms as period, we can achieve following timer period by:
 *    if ((++timerCount) & 0x1 == 0): every 32ms * 2  =  64ms
 *    if ((++timerCount) & 0x3 == 0): every 32ms * 4 = 128ms
 *    if ((++timerCount) & 0x7 == 0): every 32ms * 8 = 256ms
 *    if ((++timerCount) & 0x15 == 0): every 32ms * 16 = 512ms = 0.512s
 *    if ((++timerCount) & 0x31 == 0): every 32ms * 32 = 1024ms = 1.024s
 *    if ((++timerCount) & 0x63 == 0): every 32ms * 64 = 2048ms = 2.048s
 *    if ((++timerCount) & 0x127 == 0): every 32ms * 128 = 4096ms = 4.096s
 *    if (++timerCount == 0): every 32ms * 256 = 8192ms = 8.192s
 */
#define TIMER_PERIOD  32  // timer pops up every 32 ms
volatile byte timerCount = 0;
volatile boolean flagReadVoltage = false;
volatile boolean flagReadCurrent = false;

// voltage sensor on analog pin 0
VoltageSensor volt(0);
// current sensor on analog pin 1
CurrentSensor amp(1);

void setup() {
  Timer1.initialize(TIMER_PERIOD * 1000);
  Timer1.attachInterrupt(timerIsr);
  #ifdef ENABLE_DEBUG
    #ifdef DEBUG_CONSOLE
      DDD.begin();
    #else
      DDD.begin(9600);
    #endif
  #endif
}

void loop() {
  if (flagReadVoltage) {
    #ifdef ENABLE_DEBUG
      DDD.print(millis());
      DDD.print("\tVoltage=");
      DDD.println(volt.read());
    #endif
    flagReadVoltage = false;
  }

  if (flagReadCurrent) {
    #ifdef ENABLE_DEBUG
      DDD.print(millis());
      DDD.print("\tCurrent=");
      DDD.println(amp.read());
    #endif
    flagReadCurrent = false;
  }

}

void timerIsr() {
  if (++timerCount == 0) {
    flagReadVoltage = true;
    flagReadCurrent = true;
  }
}


