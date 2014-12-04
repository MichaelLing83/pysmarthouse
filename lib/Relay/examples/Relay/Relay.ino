/*
Control a relay

Connection:

*/
#include "Relay.h"

Relay relay(9, HIGH);

// the setup function runs once when you press reset or power the board
void setup() {
    relay.begin(LOW);
}

// the loop function runs over and over again forever
void loop() {
    relay.on();
    delay(1);
    relay.off();
    delay(1);
}
