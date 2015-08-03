
#ifndef __ULTRASONICSENSORPROXY_H__
#define __ULTRASONICSENSORPROXY_H__

const uint8_t UltraSonicSensorProxyAddress = 0x10;
const uint8_t UltraSonicSensorNumber = 0x6;
const unsigned long UltraSonicSensorTimeOut = 300 * 2 * 29;


const uint8_t UltraSonicSensorProxyCmdSetThreshold    = 0x0;
const uint8_t UltraSonicSensorProxyCmdRequestDistance = 0x1;

#endif
