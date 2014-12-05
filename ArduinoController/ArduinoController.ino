/*
    One Arduino with support of:
        1. one relay;
        2. one temperature sensor;
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

    Overall description:
        1. Use ESP8266 to connect to house's wifi and then to Raspberry Pi;
        2. Use bitlash to open a control "port":
            simply execute whatever string command is received
        3. Report back readings from sensors through ESP8266
*/
#include "bitlash.h"
#include <OneWire.h>
#include "Relay.h"

/**** Defines ****************************************************************/
// Relay related
#define RELAY_IN_PORT   3
#define RELAY_OFF   HIGH

// Temperature sensor (DS18B20) related
#define DS18B20_PIN 4
/**** End of Defines *********************************************************/

/**** Global Variables *******************************************************/
Relay relay(RELAY_IN_PORT, RELAY_OFF);
OneWire ds18b20(DS18B20_PIN);
/**** End of Global Variables ************************************************/

/**** Function Wrapping for bitlash ******************************************/
#include "bitlashRelay.h"
#include "bitlashDS18B20.h"
/**** Function Wrapping for bitlash ******************************************/

// the setup function runs once when you press reset or power the board
void setup() {
    relay.begin(RELAY_OFF);
    
    // register all bitlash functions
    register_bitlash_relay();
    register_bitlash_ds18b20();
}

// the loop function runs over and over again forever
void loop() {
}
