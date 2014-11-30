/*
Control RGB LED light.

Connection:
    V -> GND
    R -> as defined below
    B -> as defined below
    G -> as defined below
*/
#include "RbgLight.h"

RbgLight rbgLight(8, 9, 10, HIGH);

// the setup function runs once when you press reset or power the board
void setup() {
    rbgLight.begin();
    randomSeed(0);
}

// the loop function runs over and over again forever
void loop() {
    rbgLight.on(random(0, 8));
    delay(500);
}
