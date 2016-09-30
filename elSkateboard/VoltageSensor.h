#ifndef __VOLTAGE_SENSOR_H__
#define __VOLTAGE_SENSOR_H__

#include <Arduino.h>
#define VOLTAGE_RESOLUTION  0.0244  // = 5V * 5 / 1024

class VoltageSensor
{
  private:
    int pin;

  public:
    VoltageSensor(int _pin);
    float read();
};
#endif  // __VOLTAGE_SENSOR_H__
