/*
    UltraSonicSensorProxy uses a Arduino Nano or Pro Mini to connect to ultrasonic sensors etc.
    Then it is connected back to the main Arduino Uno through I2C bus, as a slave.

    There are six ultrasonic sensors: head, head-left, head-right, tail, tail-left, 
    tail-right. They are seperated 60 degrees from each other.

    Connections:
        For all ultrasonic sensors:
            VCC  <->  5V
            GND  <->  GND
        head sensor:  Trig  <->  PIN_D2,  Echo  <->  PIN_D3
        head-left:    Trig  <->  PIN_D4,  Echo  <->  PIN_D5
        head-right:   Trig  <->  PIN_D6,  Echo  <->  PIN_D7
        tail sensor:  Trig  <->  PIN_D8,  Echo  <->  PIN_D9
        tail-left:    Trig  <->  PIN_D10,  Echo  <->  PIN_D11
        tail-right:   Trig  <->  PIN_D12,  Echo  <->  PIN_D13

    I2C communication protocol:
        Address of this slave: 0x10;
        All distances are measured in centimeter;
        Initial state after boot-up: all thresholds of six sensors are set to maximum
        After boot-up:
            onReceive: two bytes used, the first is sensorIndex, the second is threshold to set
            onRequest: UltraSonicSensorNumber number of bytes returned, sorted by their index
*/

#include <Arduino.h>
#include <Wire.h>
#include "constants.h"

uint8_t thresholds[UltraSonicSensorNumber] = {255};
uint8_t distances[UltraSonicSensorNumber]  = {255};
uint8_t trigPins[UltraSonicSensorNumber];
uint8_t echoPins[UltraSonicSensorNumber];

void setup() {
    Wire.begin(UltraSonicSensorProxyAddress);
    Wire.onReceive(set_threshold);
    Wire.onRequest(get_distances);
    int i;
    for (i=0; i<UltraSonicSensorNumber; i++) {
        trigPins[i] = 2 * i + 2;
        pinMode(trigPins[i], OUTPUT);
        echoPins[i] = 2 * i + 3;
        pinMode(echoPins[i], INPUT);
    }
}

void loop() {
    int sensorIndex;
    unsigned long distance;
    for (sensorIndex=0; sensorIndex<UltraSonicSensorNumber; sensorIndex++) {
        digitalWrite(trigPins[sensorIndex], LOW);
        delayMicroseconds(2);
        digitalWrite(trigPins[sensorIndex], HIGH);
        delayMicroseconds(12);
        digitalWrite(trigPins[sensorIndex], LOW);
        distance = pulseIn(echoPins[sensorIndex], HIGH, UltraSonicSensorTimeOut) / 29 / 2;
        distances[sensorIndex] = distance > 255 ? 255 : (uint8_t)distance;
    }
    delay(60);  // do not trigger too frequently
}

void set_threshold(int numBytes) {
    // the first byte identifies which sensor to apply the threshold
    // the second byte tells how much to set as threshold
    uint8_t sensorIndex = Wire.read();
    uint8_t threshold = Wire.read();
    if (sensorIndex < UltraSonicSensorNumber) {
        thresholds[sensorIndex] = threshold;
    }
}

void get_distances() {
    Wire.write(distances, UltraSonicSensorNumber);
}

