/*
Control ultra sound echo module to measure distance.

Connection:
    VCC -> 5V
    Trig -> Control input
    Echo -> Output
    GND -> GND
*/

#include "UltraSoundDistanceMeas.h"

UltraSoundDistanceMeas ultraSoundDistanceMeas(9, 8);

#define ULTRASOUND_TRIG 9
#define ULTRASOUND_ECHO 8

unsigned long distance; // in um
#define ULTRASOUND_SPEED 340    // 340 m/s = 340 micrometer/microsecond
/* lowest distance: 2cm = 2*1000 mm = 2000*1000 micrometer.
   longest distance: 450 cm = 450*1000*1000 micrometer
   so "unsigned long" should suffice.
*/

// the setup function runs once when you press reset or power the board
void setup() {
    ultraSoundDistanceMeas.init();
    Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
    distance = ultraSoundDistanceMeas.measure();
    if (distance !=0 )
    {
        Serial.print("distance = ");
        Serial.print(distance);
        Serial.print(" micrometer = ");
        Serial.print(distance/1000);
        Serial.println(" mm");
        delay(2000);
    } else {
        Serial.println("Timed out. It's either too far away, or it's too close!");
    }
}
