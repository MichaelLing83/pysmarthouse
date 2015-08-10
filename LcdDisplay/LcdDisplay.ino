/*  Connections:
        20-char 4-line LCD display QC2004A:
            PIN_1 to PIN_16  <->  I2C adapter PIN_1 to PIN_16
        I2C adapter: hole it with all chips facing you, and 16 pins on the top, then these pins
        from left to right are PIN_16 to PIN_1
            GND  <->  GND
            VCC  <->  5V
            SDA  <->  A4 (on Mini);  SDA  <->  1kOhm  <->  5V
            SCL  <->  A5 (on Mini);  SCL  <->  1kOhm  <->  5V
            its I2C addr: 0x27
        Clock chip SZ-042:
            GND  <->  GND
            VCC  <->  5V
            SDA  <->  A4 (on Mini);  SDA  <->  1kOhm  <->  5V
            SCL  <->  A5 (on Mini);  SCL  <->  1kOhm  <->  5V
            its I2C addr: 0x68
        DS18B20 temperature sensor:
            GND  <->  GND;
            VCC  <->  5V;
            DAT  <->  D4 (on Mini);  DAT  <->  1kOhm  <->  5V
        PIR sensor:
            GND  <->  GND
            VCC  <->  5V
            OUT  <->  D2 (on Mini)
*/


#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include "DS3231.h"
#include "TemperatureSensor.h"

//#define ENABLE_DEBUG

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

DS3231 clock(0x68);
boolean set_time = false;

OneWire ds(4);  // temperature sensor

const int PIR_PIN = 2;
const byte ON_PERIOD = 20;   // 60 * 1024 milliseconds
volatile byte onTime;

void motion() {
    onTime = ON_PERIOD;
}

void setup()
{
    // initialize the LCD
    lcd.begin();
    
    clock.begin();
    
    #ifdef ENABLE_DEBUG
        Serial.begin(19200);
    #endif

    // Turn on the blacklight and print a message.
    lcd.backlight();
    if (set_time) {
        clock.setTime(0, 43, 19, 6, 31, 7, 15);
    }
    
    onTime = ON_PERIOD;
    //pinMode(2, INPUT);
    attachInterrupt(0, motion, CHANGE);
}

void loop()
{
    unsigned long start_time = millis();
    byte consumedSec = 0;
    if (onTime !=0 ) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
    String curTime = clock.readTime();
    #ifdef ENABLE_DEBUG
        Serial.println(curTime);
        Serial.print("onTime="); Serial.println(onTime);
    #endif
    lcd.setCursor(0, 0);  // column, row
    lcd.print(curTime);
    
    float temperature = get_temperature(&ds);
    #ifdef ENABLE_DEBUG
        Serial.print("Temperature: "); Serial.println(temperature);
    #endif
    lcd.setCursor(0, 1);
    lcd.print("Temp: "); lcd.setCursor(6, 1);
    lcd.print(temperature); lcd.setCursor(11, 1);
    lcd.print(" Celsius");
    //String temperature = String("Temp: ") + temperature + " Celsius";
    //lcd.print(String("Temp: ") + temperature + " Celsius");
    delay(1000);
    consumedSec = (millis() - start_time) >> 10;
    if (consumedSec < onTime) {
        onTime -= consumedSec;
    } else {
        onTime = 0;
    }
}

