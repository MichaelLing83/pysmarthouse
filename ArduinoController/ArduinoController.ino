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
        1 relay
            VCC ->
            GND -> GND
            IN  -> PIN_3
        2 DS18B20
            GND ->  GND
            VDD ->  5V
            DAT ->  4.7 kOhm    ->  5V
            DAT ->  PIN_4
        3 light sensor
            VCC ->  5V
            GND ->  GND
            DO  ->  PIN_5
        4 Infrared sensor
            VCC ->  5V
            GND ->  GND
            OUT ->  PIN_6

        5 ESP8266
            If you hold ESP8266 with antenna end upwards and all chips facing you, then from upper left corner to upper right corner, then
            from lower left corner to lower right corner, you have pins:
                GND,    GPIO2,  GPIO0,  URXD
                UTXD,   CH_PD,  RST,    VCC
            connection:
                GND     ->  GND
                URXD    ->  TXD 1
                UTXD    ->  RXD 0
                CH_PD   ->  3.3V
                VCC     ->  3.3V
            connection for debugging:
                URXD    ->  PIN_11
                UTXD    ->  PIN_10

    Overall description:
        1. Use ESP8266 to connect to wifi;
        2. Send periodically report using ASCII and UDP datagram to predefined IP address and port;
        3. For each UDP report sent, one command datagram can be received and executed;

    Binary sketch size:         16278 Bytes (if all chips are used)
    Binary sketch size limit:
    Arduino Uno:                32256 Bytes
    Arduino Nano w/ ATmega328:  30720 Bytes
*/

/**** Debug Settings *********************************************************/
/**** End of Debug Settings **************************************************/

/**** Chip Usage **************************************************************
    Comment out the define if the chip is not used.
*/
#define USE_RELAY
#define USE_TEMPERATURE_SENSOR
//#define USE_LIGHT_SENSOR
//#define USE_INFRARED_SENSOR
// ESP8266 is always used
/**** End of Chip Usage ******************************************************/

/**** Chip Connection *********************************************************
    Change port connection if otherwise
*/
#ifdef USE_RELAY
    #include "Relay.h"
    #define RELAY_IN_PORT   3
    #define RELAY_OFF   HIGH
    Relay relay(RELAY_IN_PORT, RELAY_OFF);
#endif

#ifdef USE_TEMPERATURE_SENSOR
    #include <OneWire.h>
    #include "TemperatureSensor.h"
    #define DS18B20_PIN 4
    OneWire ds18b20(DS18B20_PIN);
#endif

#ifdef USE_LIGHT_SENSOR
    #include "LightSensor.h"
    #define LIGHTSENSOR_PORT 5
    LightSensor lightSensor(LIGHTSENSOR_PORT);
#endif

#ifdef USE_INFRARED_SENSOR
    #include "InfraredSensor.h"
    #define INFRAREDSENSOR_PORT 6
    InfraredSensor infraredSensor(INFRAREDSENSOR_PORT);
#endif
/**** End of Chip Connection *************************************************/

/**** ESP8266 Configurations *************************************************/
#include "uartWIFI.h"
#ifdef ESP8266_DEBUG
    #include <SoftwareSerial.h>
#endif
#include "Cmd.h"
#define SSID       "michael"
#define PASSWORD   "waterpigs"
#define SERVER_IP "192.168.31.107" //RaspberryPi
#define SERVER_PORT 9999
// Reporting configuration
#define REPORT_INTERVAL 10  // in seconds, between two reports
#define ARDUINO_ID "FrontDoorStep"  // ID
WIFI wifi;  // ESP8266 serial over Wifi
char wifiBuffer[128];   // buffer for receiving data
/**** End of ESP8266 Configurations ******************************************/

/**** Defines ****************************************************************/
/**** End of Defines *********************************************************/

/**** Global Variables *******************************************************/
unsigned long last_report_time;
/**** End of Global Variables ************************************************/

// the setup function runs once when you press reset or power the board
void setup() {
    #ifdef USE_RELAY
        relay.begin(RELAY_OFF);
    #endif
    //lightSensor.begin();
    //infraredSensor.begin();

    last_report_time = 0;

    // initialize ESP8266
    wifi.begin();
    wifi.Initialize(STA, SSID, PASSWORD);
    delay(8000);  //make sure the module have enough time to get an IP address
    wifi.ipConfig(UDP, SERVER_IP, SERVER_PORT); // configure server info
}

// the loop function runs over and over again forever
void loop() {
    // we send a report every 60 seconds
    unsigned long cur_time = millis();
    bool do_report = false;
    if (cur_time < last_report_time)
    {
        // time counter has wrapped around.
        do_report = true;
    }
    else if (cur_time - last_report_time >= REPORT_INTERVAL*1000)
    {
        do_report = true;
    }

    if (do_report)
    {
        last_report_time = cur_time;
        String report = String("REPORT;"); // + String(ARDUINO_ID) + ";";
        report += ARDUINO_ID;
        report += ";";
        // relay status
        #ifdef USE_RELAY
            report += "Relay;";
            report += String(int(100 * relay.cur_lvl()));
            report += ";";
        #endif
        // temperature sensor
        #ifdef USE_TEMPERATURE_SENSOR
            report += "Temperature;";
            report += String(int(100 * get_temperature(&ds18b20)));
            report += ";";
        #endif
        // light sensor
        #ifdef USE_LIGHT_SENSOR
            report += "Light;";
            report += String(int(100 * lightSensor.get()));
            report += ";";
        #endif
        // infra-red sensor
        #ifdef USE_INFRARED_SENSOR
            report += String("Infrared;");
            report += String(int(100 * infraredSensor.get()));
            report += ";";
        #endif
        wifi.Send(report);

        // receive response (cmd) from server
        int data_len = wifi.ReceiveMessage(wifiBuffer);
        if (data_len >= 4) {    // minimum length of CMD datagram, i.e. "CMD;"
            String datagram = String(wifiBuffer);
            if (datagram.startsWith("CMD;", 0)) {
                // this is a valid CMD datagram
                datagram = datagram.substring(4);
                Cmd(datagram).execute();
            }
        }
    }
}
