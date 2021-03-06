
/*
ESP8266 library

Created by Stan Lee(Lizq@iteadstudio.com)
2014/10/8

Modified version
V1.0	released the first version of ESP8266 library



*/

#ifndef __UARTWIFI_H__
#define __UARTWIFI_H__
#include <Arduino.h>

#define UNO //uncomment this line when you use it with UNO board
//#define MEGA  //uncomment this line when you use it with MEGA board
#define ESP8266_DEBUG
//#define ESP8266_DEBUG_2

#ifdef ESP8266_DEBUG
    #include <SoftwareSerial.h>
    #define _DBG_RXPIN_ 10
    #define _DBG_TXPIN_ 11
    #define debugBaudRate 9600
    #define ESP8266_BAUDRATE 9600   // due to SoftwareSerial, try to run at a lower rate
#else
    #define ESP8266_BAUDRATE 115200 //The default baud rate of ESP8266 is 115200
#endif

#define ESP8266_TIMEOUT 4000    // 4 seconds

#ifdef UNO
    #ifdef ESP8266_DEBUG
        // when debugging, connect to ESP8266 using SoftSerial
        #define _cell softSerial
        #define DebugSerial Serial
    #else
        #define _cell Serial
    #endif
#endif

#ifdef MEGA
    #define _cell Serial1
    #ifdef ESP8266_DEBUG
        #define DebugSerial Serial
    #endif
#endif

//The way of encrypstion
#define    OPEN          0
#define    WEP           1
#define    WAP_PSK       2
#define    WAP2_PSK      3
#define    WAP_WAP2_PSK  4

//Communication mode
#define    TCP     1
#define    tcp     1
#define    UDP     0
#define    udp     0

#define    OPEN    1
#define    CLOSE   0

//The type of initialized WIFI
#define    STA     1
#define    AP      2
#define    AP_STA  3

#define SERIAL_TX_BUFFER_SIZE 128
#define SERIAL_RX_BUFFER_SIZE 128

class WIFI
{
private:
    String _newline;
public:

    void begin(void);

    //Initialize port
    boolean Initialize(byte mode, String ssid, String pwd, byte chl = 1, byte ecn = 2);
    boolean ipConfig(byte type, String addr, int port, boolean useMultiConn = false, byte id = 0);

    boolean Send(String str);  //send data in single connection mode
    boolean Send(byte id, String str);  //send data int multiple connection mode

    int ReceiveMessage(char *buf);

    //String begin(void);
    /*=================WIFI Function Command=================*/
    boolean Reset(void);    //reset the module
    boolean confMode(byte mode);   //set the working mode of module
    boolean confJAP(String ssid , String pwd);    //set the name and password of wifi
    boolean confSAP(String ssid , String pwd , byte chl , byte ecn);       //set the parametter of SSID, password, channel, encryption in AP mode.

    String showMode(void);   //inquire the current mode of wifi module
    String showAP(void);   //show the list of wifi hotspot
    String showJAP(void);  //show the name of current wifi access port
    boolean quitAP(void);    //quit the connection of current wifi
    String showSAP(void);     //show the parameter of ssid, password, channel, encryption in AP mode

    /*================TCP/IP commands================*/
    String showStatus(void);    //inquire the connection status
    String showMux(void);       //show the current connection mode(single or multiple)
    boolean confMux(boolean useMultiConn);    //set connection mode
    boolean newMux(byte type, String addr, int port);   //create new tcp or udp connection (single connection mode)
    boolean newMux(byte id, byte type, String addr, int port);   //create new tcp or udp connection (multiple connection mode)(id:0-4)
    void closeMux(void);   //close tcp or udp (single connection mode)
    void closeMux(byte id); //close tcp or udp (multiple connection mode)
    String showIP(void);    //show the current ip address
    boolean confServer(byte mode, int port);  //set the parameter of server
};

#endif  // __UARTWIFI_H__
