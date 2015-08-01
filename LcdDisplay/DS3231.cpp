
#include "DS3231.h"


DS3231::DS3231(int _i2c_addr):
    i2c_addr(_i2c_addr)
{
}

void DS3231::begin() {
    Wire.begin();
}

// Convert normal decimal numbers to binary coded decimal
byte DS3231::decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte DS3231::bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void DS3231::setTime(byte second, byte minute, byte hour, byte dayOfWeek,
                     byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(i2c_addr);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void DS3231::_readTime()
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(i2c_addr, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
}

String DS3231::readTime()
{
    _readTime();
    String s_time = String("") + hour + ":";
    //Serial.print(hour, DEC); Serial.print(":");
    if (minute < 10) {
        s_time += "0";
    }
    s_time += minute;
    //Serial.print(minute, DEC); Serial.print(":");
    s_time += ":";
    if (second < 10) {
        s_time += "0";
    }
    s_time += second;
    //Serial.print(second, DEC); Serial.print(" ");
    s_time += " ";
    s_time += dayOfMonth;
    //Serial.print(dayOfMonth, DEC); Serial.print("/");
    s_time += "/";
    s_time += month;
    //Serial.print(month, DEC); Serial.print("/");
    s_time += "/";
    s_time += year;
    //Serial.print(month, DEC); Serial.print(" ");
    s_time += " ";
    switch (dayOfWeek) {
        case 1: s_time += "Sun"; break;
        case 2: s_time += "Mon"; break;
        case 3: s_time += "Tue"; break;
        case 4: s_time += "Wed"; break;
        case 5: s_time += "Thu"; break;
        case 6: s_time += "Fri"; break;
        case 7: s_time += "Sat"; break;
        default: break;
    }
    //Serial.println(dayOfWeek);
    return s_time;
}
