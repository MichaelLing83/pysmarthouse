/*
Control RGB LED light.

Connection:
    V -> GND
    R -> as defined below
    B -> as defined below
    G -> as defined below
*/
#include "RbgLight.h"

RbgLight rbgLight(9, 10, 11, HIGH);

// the setup function runs once when you press reset or power the board
void setup() {
    rbgLight.begin();
    randomSeed(0);
}

// the loop function runs over and over again forever
void loop() {
    //rbgLight.on(random(0, 8));
    //rbgLight.flash_pattern(random(0, 8));
    //rbgLight.on(random(0, 8));
    //digitalWrite(1, HIGH);
    //delay(1000);
    // rbgLight.off();
    rbgLight.flash(random(1, 8));
    delay(500);
}
