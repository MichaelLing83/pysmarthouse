/*
Control RGB LED light.

Connection:
    V -> GND
    R -> as defined below
    B -> as defined below
    G -> as defined below
*/
#include "RbgLight.h"
#include "bitlash.h"

#define SERIAL_BAUD_RATE 57600

RbgLight rbgLight(9, 10, 11, HIGH);

numvar timer1(void) { 
	return TCNT1; 	// return the value of Timer 1
}

numvar light_on(void)
{
    if (getarg(0) == 0)
    {
        // use default color
        rbgLight.on(rbgLight.default_color());
    } else {
        rbgLight.on(getarg(1));
    }
    return 0;
}

numvar light_off(void)
{
    rbgLight.off();
    return 0;
}

// the setup function runs once when you press reset or power the board
void setup() {
    rbgLight.begin();
    initBitlash(SERIAL_BAUD_RATE);
    randomSeed(0);
    addBitlashFunction("timer1", (bitlash_function) timer1);
    addBitlashFunction("light_on", (bitlash_function) light_on);
    addBitlashFunction("light_off", (bitlash_function) light_off);
}

// the loop function runs over and over again forever
void loop() {
    runBitlash();
    //rbgLight.on(random(0, 8));
    //rbgLight.flash_pattern(random(0, 8));
    //rbgLight.on(random(0, 8));
    //digitalWrite(1, HIGH);
    //delay(1000);
    // rbgLight.off();
    //rbgLight.flash(random(1, 8));
    //delay(500);
}
