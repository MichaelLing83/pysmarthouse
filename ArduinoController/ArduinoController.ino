/*
    One Arduino with support of:
        1. one relay;
        2. one temperature sensor;
            http://bildr.org/2011/07/ds18b20-arduino/
            http://www.hobbytronics.co.uk/ds18b20-arduino
            http://www.pjrc.com/teensy/td_libs_OneWire.html
            https://github.com/milesburton/Arduino-Temperature-Control-Library
            http://www.milesburton.com/?title=Dallas_Temperature_Control_Library
            http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
        3. one light sensor;
        4. one infrared detector;
        5. one ESP8266 (Serial over Wifi);

    Connection:
        # relay
            VCC ->  
            GND -> GND
            IN  -> PIN_3
        # DS18B20
            GND ->  GND
            VDD ->  5V
            DQ  ->  4.7 kOhm    ->  5V
        # light sensor
            VCC ->  5V
            GND ->  GND
            DO  ->  PIN_5
        # Infrared sensor
            VCC ->  5V
            GND ->  GND
            OUT ->  PIN_6

    Overall description:
        1. Use ESP8266 to connect to house's wifi and then to Raspberry Pi;
        2. Use bitlash to open a control "port":
            simply execute whatever string command is received
        3. Report back readings from sensors through ESP8266
*/
#include "bitlash.h"
#include <OneWire.h>
#include "Relay.h"
#include "LightSensor.h"
#include "InfraredSensor.h"

/**** Defines ****************************************************************/
// Relay related
#define RELAY_IN_PORT   3
#define RELAY_OFF   HIGH
// Temperature sensor (DS18B20) related
#define DS18B20_PIN 4
// Light sensor
#define LIGHTSENSOR_PORT 5
// Infrared sensor
#define INFRAREDSENSOR_PORT 6
/**** End of Defines *********************************************************/

/**** Global Variables *******************************************************/
Relay relay(RELAY_IN_PORT, RELAY_OFF);
OneWire ds18b20(DS18B20_PIN);
LightSensor lightSensor(LIGHTSENSOR_PORT);
InfraredSensor infraredSensor(INFRAREDSENSOR_PORT);
/**** End of Global Variables ************************************************/

/**** Function Wrapping for bitlash ******************************************/
#include "bitlashRelay.h"
#include "bitlashDS18B20.h"
#include "bitlashLightSensor.h"
#include "bitlashInfraredSensor.h"
/**** Function Wrapping for bitlash ******************************************/

// the setup function runs once when you press reset or power the board
void setup() {
    relay.begin(RELAY_OFF);
    lightSensor.begin();
    infraredSensor.begin();
    
    // register all bitlash functions
    register_bitlash_relay();
    register_bitlash_ds18b20();
    register_bitlash_infraredSensor();
}

// the loop function runs over and over again forever
void loop() {
}
