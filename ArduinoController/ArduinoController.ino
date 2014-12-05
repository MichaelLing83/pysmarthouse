/*
    One Arduino with support of:
        1. one relay;
        2. one temperature sensor;
        3. one light sensor;
        4. one infrared detector;
        5. one ESP8266 (Serial over Wifi);

    Connection:
        # relay
            VCC ->  
            GND -> GND
            IN  -> PIN_3
    Overall description:
        1. Use ESP8266 to connect to house's wifi and then to Raspberry Pi;
        2. Use bitlash to open a control "port":
            simply execute whatever string command is received
        3. Report back readings from sensors through ESP8266
*/
#include "bitlash.h"
#include "Relay.h"

// Relay related
#define RELAY_IN_PORT   3
#define RELAY_OFF   HIGH

// Global variables
Relay relay(RELAY_IN_PORT, RELAY_OFF);

// Function for bitlash
#include "bitlashRelay.h"

// the setup function runs once when you press reset or power the board
void setup() {
    relay.begin(RELAY_OFF);
}

// the loop function runs over and over again forever
void loop() {
}
