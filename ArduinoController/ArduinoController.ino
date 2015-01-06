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
            DQ  ->  4.7 kOhm    ->  5V
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
        1. Use ESP8266 to connect to house's wifi and then to Raspberry Pi;
        2. Use bitlash to open a control "port":
            simply execute whatever string command is received
        3. Report back readings from sensors through ESP8266
    
    Binary sketch size:         28458 Bytes
    Binary sketch size limit:
    Arduino Uno:                32256 Bytes
    Arduino Nano w/ ATmega328:  30720 Bytes
*/
#include "bitlash.h"
//#include <OneWire.h>
#include "Relay.h"
//#include "TemperatureSensor.h"
//#include "LightSensor.h"
//#include "InfraredSensor.h"
#include "uartWIFI.h"
#include <SoftwareSerial.h>

/**** Defines ****************************************************************/
// Relay related
#define RELAY_IN_PORT   3
#define RELAY_OFF   HIGH
// Temperature sensor (DS18B20) related
//#define DS18B20_PIN 4
// Light sensor
//#define LIGHTSENSOR_PORT 5
// Infrared sensor
//#define INFRAREDSENSOR_PORT 6
// WIFI config for ESP8266
#define SSID       "michael"
#define PASSWORD   "waterpigs"
#define SERVER_IP "192.168.31.107" //RaspberryPi
#define SERVER_PORT 9999
// Reporting configuration
#define REPORT_INTERVAL 10  // in seconds, between two reports
#define ARDUINO_ID "FrontDoorStep"  // ID
/**** End of Defines *********************************************************/

/**** Global Variables *******************************************************/
Relay relay(RELAY_IN_PORT, RELAY_OFF);
//OneWire ds18b20(DS18B20_PIN);
//LightSensor lightSensor(LIGHTSENSOR_PORT);
//InfraredSensor infraredSensor(INFRAREDSENSOR_PORT);
WIFI wifi;  // ESP8266 serial over Wifi
char wifiBuffer[128];   // buffer for receiving data

unsigned long last_report_time;
/**** End of Global Variables ************************************************/

/**** Function Wrapping for bitlash ******************************************/
#include "bitlashRelay.h"
//#include "bitlashDS18B20.h"
//#include "bitlashLightSensor.h"
//#include "bitlashInfraredSensor.h"
/**** Function Wrapping for bitlash ******************************************/

// the setup function runs once when you press reset or power the board
void setup() {
    relay.begin(RELAY_OFF);
    //lightSensor.begin();
    //infraredSensor.begin();

    // register all bitlash functions
    register_bitlash_relay();
    //register_bitlash_ds18b20();
    //register_bitlash_infraredSensor();

    last_report_time = 0;

    // initialize ESP8266
    wifi.begin();
    if(!wifi.Initialize(STA, SSID, PASSWORD)) {
        wifi.Initialize(STA, SSID, PASSWORD);
    }
    delay(8000);  //make sure the module have enough time to get an IP address
    //DebugSerial.println(wifi.showIP()); //show ip address of module

    //delay(5000);
    wifi.ipConfig(UDP, SERVER_IP, SERVER_PORT); // configure server info

    //DebugSerial.println("setup done..");
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
        report += "Relay;";
        report += String(int(100 * relay.cur_lvl()));
        report += ";";
        // temperature sensor
        //report += "Temperature;";
        //report += String(int(100 * get_temperature(&ds18b20)));
        //report += ";";
        // light sensor
        //report += "Light;";
        //report += String(int(100 * lightSensor.get()));
        //report += ";";
        // infra-red sensor
        //report += String("Infrared;");
        //report += String(int(100 * infraredSensor.get()));
        //report += ";";
        wifi.Send(report);
        delay(2000);    // do we need to be sure sending is finished?

        // receive response (cmd) from server
        int data_len = wifi.ReceiveMessage(wifiBuffer);
        if (data_len >= 4) {    // minimum length of CMD datagram, i.e. "CMD;"
            String datagram = String(wifiBuffer);
            if (datagram.startsWith("CMD;", 0)) {
                // this is a valid CMD datagram
                datagram = datagram.substring(4);
                while (datagram.length() > 0) {
                    // there is at least one cmd to be executed
                    int index = datagram.indexOf(';');
                    String cmd = datagram.substring(0, index);
                    cmd.toCharArray(wifiBuffer, cmd.length());
                    doCommand(wifiBuffer);
                    datagram = datagram.substring(index+1);
                }
            }
        }
    }
}
