#ifndef __VOLTAGE_SENSOR_CPP__
#define __VOLTAGE_SENSOR_CPP__

#include "VoltageSensor.h"

VoltageSensor::VoltageSensor(int _pin):
  pin(_pin){
}

float VoltageSensor::read() {
  return analogRead(pin) * VOLTAGE_RESOLUTION;
}

#endif  // __VOLTAGE_SENSOR_CPP__
