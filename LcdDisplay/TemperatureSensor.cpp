/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
#include "TemperatureSensor.h"

float get_temperature(OneWire* ds_p){
    //returns the temperature from one DS18S20 in DEG Celsius

    byte data[12];
    byte addr[8];

    if (!ds_p->search(addr))
    {
        //no more sensors on chain, reset search
        ds_p->reset_search();
        return -1000;
    }

    if (OneWire::crc8( addr, 7) != addr[7])
    {
        //Serial.println("CRC is not valid!");
        return -1000;
    }

    if ( addr[0] != 0x10 && addr[0] != 0x28)
    {
        //Serial.print("Device is not recognized");
        return -1000;
    }

    ds_p->reset();
    ds_p->select(addr);
    ds_p->write(0x44,1); // start conversion, with parasite power on at the end

    byte present = ds_p->reset();
    ds_p->select(addr);  
    ds_p->write(0xBE); // Read Scratchpad

    for (int i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = ds_p->read();
    }

    ds_p->reset_search();

    byte MSB = data[1];
    byte LSB = data[0];

    float tempRead = ((MSB << 8) | LSB); //using two's compliment
    float TemperatureSum = tempRead / 16;

    return TemperatureSum;
}