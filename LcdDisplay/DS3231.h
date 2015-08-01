
#ifndef __DS3131_H__
#define __DS3131_H__

#include <Arduino.h>
#include "Wire.h"

const byte DS3231_I2C_ADDRESS	= 0x57;

class DS3231
{
private:
    int i2c_addr;
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    byte decToBcd(byte val);
    byte bcdToDec(byte val);
    void _readTime(void);

public:
    DS3231(int _i2c_addr);
    void begin(void);
    void setTime(byte second, byte minute, byte hour, byte dayOfWeek,
                 byte dayOfMonth, byte month, byte year);
    String readTime(void);
};

#endif	// __DS3131_H__
