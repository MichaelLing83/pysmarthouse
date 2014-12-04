/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file Relay.h
 *
 * Class providing handling of one relay
    VCC ->  5V
    GND ->  GND
    IN  ->  PIN_x
 */

#ifndef __RELAY_H__
#define __RELAY_H__

#include <Arduino.h>

class Relay
{
private:
    uint8_t _on_lvl;
    uint8_t _cur_lvl;
    uint8_t _pin;

public:

    Relay(uint8_t __pin, uint8_t __on_lvl);

    void begin(uint8_t default_lvl);
    void on(void);
    void off(void);
    uint8_t cur_lvl(void);
    void on_lvl(uint8_t __on_lvl);
    uint8_t on_lvl(void);
    uint8_t pin(void);
};

#endif // __RELAY_H__


