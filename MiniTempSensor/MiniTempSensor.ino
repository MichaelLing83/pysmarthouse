/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

    Connection:
        # DS18B20   <-> Pro Mini
        GND   ->  GND
        VCC   ->  5V
        DATA  ->  4.7 kOhm    ->  5V
        DATA  ->  PIN_D2

        # 4-digit 7-segment display <-> Pro Mini
        digit_1 <-  PIN_12  <->  PIN_D0
        digit_2 <-  PIN_1  <->  PIN_D1
        digit_3 <-  PIN_5  <->  PIN_D12
        digit_4 <-  PIN_9  <->  PIN_D13
        segment_A   <-  PIN_14  <->  PIN_D4
        segment_B   <-  PIN_8  <->  PIN_D5
        segment_C   <-  PIN_6  <->  PIN_D6
        segment_D   <-  PIN_2,7  <->  PIN_D7
        segment_E   <-  PIN_4  <->  PIN_D8
        segment_F   <-  PIN_13  <->  PIN_D9
        segment_G   <-  PIN_10  <->  PIN_D10
        decimal_point   <-  PIN_3  <->  PIN_D11

Observations:
  1. the reading from DS18B20 seems to be drifting quite much ~2 degrees after boot up;
  2. the reading from DS18B20 seems to drift slightly after the initial big drift stops;

TODO:
  1. Attach a F2481AH 4-digits 7-segment display;
*/

#include <Arduino.h>
#include "TemperatureSensor.h"
#include "FourDigitDisplay.h"

const unsigned long UPDATE_INTERVAL_MS  = 60000; // update every 60 seconds

//#define ENABLE_DEBUG

OneWire ds(2);
FourDigitDisplay fdd(0, 1, 12, 13, 4, 5, 6, 7, 8, 9, 10, 11);

void setup() {
    Serial.end();
    fdd.init();
}

#ifdef ENABLE_DEBUG
    int test_values[9] = {-999, -87, -6, 0, 12, 345, 6789, 9999, 0};
#endif

void loop() {
    int i;
    float temperature = get_temperature(&ds);
    int int_temp = int(temperature);
    #ifdef ENABLE_DEBUG
        //Serial.println(temperature);
        int j;
        /*
        delay(500);
        for (i=1; i<5; i++) {
          fdd.select_digit(i);
          for (j=0; j<10; j++) {
            fdd.digit(j);
            delay(100);
          }
        }*/
        delay(500);
        fdd.display(1.23, 2000);/*
        for (i=0; i<1000; i++) {
            fdd.display(45);
        }*/
    #else
        fdd.display(temperature, UPDATE_INTERVAL_MS);
    #endif
}
