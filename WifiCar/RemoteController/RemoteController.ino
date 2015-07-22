#include "joystick.h"

XindaJoy joy(26,25,24);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  joy.poll();
  Serial.print(joy.isClicked() ? '!' : '_');
  Serial.println(joy.getDirection());
}
