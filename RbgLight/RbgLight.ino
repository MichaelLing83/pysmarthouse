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
#define R_PIN 8
#define B_PIN 9
#define G_PIN 10
#define LIGHT_ON HIGH
#define LIGHT_OFF LOW

// color map: R, B, G (bit 0, bit 1, bit 2)
const unsigned int RED = 1;
const unsigned int BLUE = 2;
const unsigned int GREEN = 4;

// the setup function runs once when you press reset or power the board
void setup() {
    rbgLight.begin();
    randomSeed(0);
}

// the loop function runs over and over again forever
void loop() {
    rbgLight.on(random(0, 8));
    //light_on(random(0, 8));
    delay(500);
}

void light_on(unsigned int color)
{
    if (color & RED)
    {
        digitalWrite(R_PIN, LIGHT_ON);
    } else {
        digitalWrite(R_PIN, LIGHT_OFF);
    }

    if (color & BLUE)
    {
        digitalWrite(B_PIN, LIGHT_ON);
    } else {
        digitalWrite(B_PIN, LIGHT_OFF);
    }

    if (color & GREEN)
    {
        digitalWrite(G_PIN, LIGHT_ON);
    } else {
        digitalWrite(G_PIN, LIGHT_OFF);
    }
}

void light_off() {
    digitalWrite(R_PIN, LIGHT_OFF);
    digitalWrite(B_PIN, LIGHT_OFF);
    digitalWrite(G_PIN, LIGHT_OFF);
}
