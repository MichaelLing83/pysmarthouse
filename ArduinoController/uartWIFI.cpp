/*
DONE: Find a good way to wait for ESP8266_TIMEOUT. In current solution, if we are unlucky and there is a wrap-around of timer, the we could
    wait forever.
    Answer: this is not a problem, since when overflow happens, the subtraction result will be a really big number (thanks to unsigned)
TODO: Find if head and tail pattern are universal, so we could handle them at one place.
*/

#include "uartWIFI.h"

#ifdef ESP8266_DEBUG
    #define DBGR(message)   Serial.print(message)
    #define DBG(message)    Serial.println(message) // debug output with newline
    #define DBGW(message)   Serial.write(message)
#else
    #define DBGR(message)
    #define DBG(message)
    #define DBGW(message)
#endif

#ifdef ESP8266_DEBUG_2
    #define DBG2R(message)  Serial.print(message)
    #define DBG2(message)   Serial.println(message)
#else
    #define DBG2R(message)
    #define DBG2(message)
#endif

#ifdef UNO
    #ifdef ESP8266_DEBUG
        SoftwareSerial softSerial(_DBG_RXPIN_, _DBG_TXPIN_);
    #endif
#endif

int chlID;  //client id(0-4)

void WIFI::begin(void)
{
    #ifdef ESP8266_DEBUG
        DebugSerial.begin(debugBaudRate);   //The default baud rate for debugging is 9600
    #endif
    DBG2("WIFI::begin() starts");
    _newline = "\r\n";
    _cell.begin(ESP8266_BAUDRATE);
    _cell.flush();
    _cell.setTimeout(ESP8266_TIMEOUT);
    Reset();    // TODO: at other places, Reset results are not checked, is it a problem that Reset fails?
    DBG2("WIFI::begin() ends");
}


/*************************************************************************
Initialize port

    mode:    setting operation mode
        STA:        Station
        AP:         Access Point
        AT_STA:     Access Point & Station
    ssid:   Wifi SSID (network name)
    pwd:    Wifi password
    chl:    channel number if AP or AP_STA
    ecn:    encryption method if AP or AP_STA
        OPEN          0
        WEP           1
        WAP_PSK       2
        WAP2_PSK      3
        WAP_WAP2_PSK  4

    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::Initialize(byte mode, String ssid, String pwd, byte chl, byte ecn)
{
    DBG2R("WIFI::Initialize starts with ");
    DBG2R("mode=");DBG2R(mode);DBG2R(" SSID=");DBG2R(ssid);DBG2R(" pwd=");DBG2R(pwd);DBG2R(" chl=");DBG2R(chl);DBG2R(" ecn=");DBG2(ecn);
    boolean result = false;
    if (mode == STA || mode == AP || mode == AP_STA) {
        result = confMode(mode);
        Reset();
        // TODO: if confMode fails, does it make sense to continue with confJAP and confSAP?
        switch (mode) {
            case STA:
                confJAP(ssid, pwd);
                break;
            case AP:
                confSAP(ssid, pwd, chl, ecn);
                break;
            case AP_STA:
                confJAP(ssid, pwd);
                confSAP(ssid, pwd, chl, ecn);
                break;
            default: break;
        }
    }
    return result;
    DBG2R("WIFI::Initialize ends with ");DBG2(result);
}

/*************************************************************************
Set up TCP or UDP connection

    type:    TCP or UDP

    addr:    ip address

    port:    port number

    useMultiConn:    single / multiple connection
        false to use single connection
        true to use multiple connection

    id:    id number(0-4)

    return:
        true    -    successfully
        false    -    unsuccessfully

***************************************************************************/
boolean WIFI::ipConfig(byte type, String addr, int port, boolean useMultiConn, byte id)
{
    DBG2R("WIFI::ipConfig starts with ");
    DBG2R("type=");DBG2R(type);DBG2R(" addr=");DBG2R(addr);DBG2R(" port=");DBG2R(port);DBG2R(" useMultiConn=");DBG2R(useMultiConn);
    DBG2R(" id=");DBG2(id);
    boolean result = false;
    confMux(useMultiConn);
    delay(ESP8266_TIMEOUT); // TODO: is this necessary?
    if (!useMultiConn) {    // use single connection mode
        result = newMux(type, addr, port);
    } else {
        result = newMux(id, type, addr, port);
    }
    DBG2R("WIFI::ipConfig ends with ");DBG2(result);
    return result;
}

/*************************************************************************
Receive message from wifi

    buf:    buffer for receiving data

    chlID:    <id>(0-4)

    return:    size of the buffer

TODO: this method should take parameter like (unsigned char* buf, unsigned int max_length) so we could check the received data length does
    not exceeds the buffer length.
TOCO: define a new struct as return type, which consists of iSize and chlID (which could be -1 to mark chlID doesn't exist).
***************************************************************************/
int WIFI::ReceiveMessage(char *buf)
{
    //+IPD,<len>:<data>
    //+IPD,<id>,<len>:<data>
    String data;
    int iSize = 0;
    unsigned long start = millis();

    // read in all data, which ends with "\nOK".
    while (millis() - start < ESP8266_TIMEOUT * 2) {
        while (_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("\nOK") != -1) {
            break;
        }
    }
    DBGR("data=");DBG(data);

    // post processing
    // it can be in form "+IPD,len:data" or "+IPD,id,len:data"
    int head_start, head_end, id_start, len_start, data_start;
    String id, len;
    String head;
    head_start = data.indexOf('+');
    head_end = data.indexOf(':');
    if (head_start == -1 || head_end == -1 || head_start >= head_end) {
        DBG("No head");
        return 0;
    }
    head = data.substring(head_start, head_end);  // gives either "+IPD,len" or "+IPD,id,len"
    id_start = head.indexOf(',') + 1;
    len_start = head.lastIndexOf(',') + 1;
    if (id_start == -1 || len_start == -1) {
        DBG("No id or len");
        return 0;
    } else if (id_start != len_start) {
        // it's "+IPD,id,len"
        id = head.substring(id_start, len_start-1);
        DBG(id);
        chlID = id.toInt();
    }
    len = head.substring(len_start);
    iSize = len.toInt();
    data = data.substring(head_end+1);
    if (iSize > data.length()) {
        DBG("Not fully received");
        return 0;
    }
    // copy received bytes into given buffer
    data.toCharArray(buf, iSize);
    return iSize;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
Reboot the wifi module



***************************************************************************/
boolean WIFI::Reset(void)
{
    DBG("WIFI::Reset starts");
    boolean result = false;
    unsigned long start;
    _cell.println("AT+RST");
    DBG("AT+RST");
    start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if (_cell.find("ready")) {
            result = true;
            break;
        }
    }
    DBG("WIFI::Reset ends with ");
    DBG(result);
    return result;
}

/*********************************************
 *********************************************
 *********************************************
             WIFI Function Commands
 *********************************************
 *********************************************
 *********************************************
 */

/*************************************************************************
Inquire the current mode of wifi module

    return:    string of current mode
        Station
        AP
        AP+Station

***************************************************************************/
String WIFI::showMode()
{
    String data;
    String result = "Unknown";
    _cell.println("AT+CWMODE?");
    DBG("AT+CWMODE?");
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1) {
            break;
        }
    }
    DBG(data);
    if (data.indexOf("1") != -1) {
        result = "Station";
    } else if (data.indexOf("2") != -1) {
        result = "AP";
    } else if (data.indexOf("3") != -1) {
        result = "AP+Station";
    }
    DBG(result);
    return result;
}



/*************************************************************************
Configure the operation mode

    mode:
        1    -    Station
        2    -    AP
        3    -    AP+Station

    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::confMode(byte mode)
{
    DBG("WIFI::confMode starts");
    String data;
    boolean result;
    String cmd = "AT+CWMODE=";
    cmd += mode;
    _cell.println(cmd);
    DBG(cmd);
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if (_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1) {
            DBG("OK or no change");
            result = true;
            break;
        }
        if (data.indexOf("ERROR") != -1 || data.indexOf("busy") != -1) {
            DBG("ERROR or busy");
            break;
        }
    }
    DBG(data);
    DBG("WIFI::confMode ends");
    DBG(result);
    return result;
}


/*************************************************************************
Show the list of wifi hotspot

    return: string of wifi information
        encryption,SSID,RSSI


***************************************************************************/
String WIFI::showAP(void)
{
    String data;
    _cell.flush();
    _cell.print("AT+CWLAP\r\n");
    DBG("AT+CWLAP\r\n");
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1 || data.indexOf("ERROR") != -1 ) {
            break;
        }
    }
    DBG(data);
    if(data.indexOf("ERROR") != -1) {
        return "ERROR";
    } else {
        data.replace("AT+CWLAP","");
        data.replace("OK","");
        data.replace("+CWLAP","WIFI");
        while (data.indexOf(_newline) != -1) {
            data.replace(_newline, "");
        }
        DBG(data);
        return data;
    }
 }


/*************************************************************************
Show the name of current wifi access port

    return: string of access port name
        AP: <SSID>

***************************************************************************/
String WIFI::showJAP(void)
{
    _cell.flush();
    _cell.println("AT+CWJAP?");
    DBG("AT+CWJAP?");
    String data;
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1 || data.indexOf("ERROR") != -1 ) {
            break;
        }
    }
    DBG(data);
    data.replace("AT+CWJAP?","");
    data.replace("+CWJAP","AP");
    data.replace("OK","");
    while (data.indexOf(_newline) != -1) {
        data.replace(_newline, "");
    }
    DBG(data);
    return data;
}


/*************************************************************************
Configure the SSID and password of the access port

    return:
        true    -    successfully
        false   -    unsuccessfully


***************************************************************************/
boolean WIFI::confJAP(String ssid, String pwd)
{
    DBG("WIFI::confJAP starts");
    boolean result = false;
    String cmd = "AT+CWJAP=\"";
    cmd += ssid;
    cmd += "\",\"";
    cmd += pwd;
    cmd += "\"";
    _cell.println(cmd);
    DBG(cmd);
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.find("OK")) {
            result = true;
            break;
        }
    }
    DBG("WIFI::confJAP ends");
    DBG(result);
    return result;
}
/*************************************************************************
Quite the access port

    return:
        true    -    successfully
        false    -    unsuccessfully

***************************************************************************/
boolean WIFI::quitAP(void)
{
    _cell.println("AT+CWQAP");
    DBG("AT+CWQAP");
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.find("OK")) {
            return true;
        }
    }
    return false;
}

/*************************************************************************
Show the parameter of SSID, password, channel, encryption in AP mode

    return:
        mySAP:<SSID>,<password>,<channel>,<encryption>

***************************************************************************/
String WIFI::showSAP()
{
    _cell.println("AT+CWSAP?");
    DBG("AT+CWSAP?");
    String data;
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1 || data.indexOf("ERROR") != -1 ) {
            break;
        }
    }
    DBG(data);
    data.replace("AT+CWSAP?","");
    data.replace("+CWSAP","mySAP");
    data.replace("OK","");
    while (data.indexOf(_newline) != -1) {
        data.replace(_newline, "");
    }
    DBG(data);
    return data;
}

/*************************************************************************
Configure the parameter of SSID, password, channel, encryption in AP mode

    return:
        true    -    successfully
        false    -    unsuccessfully

***************************************************************************/

boolean WIFI::confSAP(String ssid , String pwd , byte chl , byte ecn)
{
    boolean result = false;
    unsigned long start;
    String cmd = "AT+CWSAP=\"";
    cmd += ssid;
    cmd += "\",\"";
    cmd += pwd;
    cmd += "\",";
    cmd += chl;
    cmd += ",";
    cmd += ecn;
    _cell.println(cmd);
    DBG(cmd);
    start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if (_cell.find("OK")) {
            result = true;
            break;
        }
    }
    return result;
}


/*********************************************
 *********************************************
 *********************************************
             TPC/IP Function Command
 *********************************************
 *********************************************
 *********************************************
 */

/*************************************************************************
Inquire connection status

    return: string of connection status
        <ID>  0-4
        <type>  TCP or UDP
        <addr>  IP
        <port>  port number

***************************************************************************/

String WIFI::showStatus(void)
{
    _cell.println("AT+CIPSTATUS");
    DBG("AT+CIPSTATUS");
    String data;
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1) {
            break;
        }
    }
    DBG(data);
    data.replace("AT+CIPSTATUS","");
    data.replace("OK","");
    while (data.indexOf(_newline) != -1) {
        data.replace(_newline, "");
    }
    DBG(data);
    return data;
}

/*************************************************************************
Show the current connection mode (single or multiple)

    return: string of connection mode
        0    -    single
        1    -    multiple

***************************************************************************/
String WIFI::showMux(void)
{
    String data;
    _cell.println("AT+CIPMUX?");
    DBG("AT+CIPMUX?");
    unsigned long start  = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1) {
            break;
        }
    }
    DBG(data);
    data.replace("AT+CIPMUX?","");
    data.replace("+CIPMUX","showMux");
    data.replace("OK","");
    while (data.indexOf(_newline) != -1) {
        data.replace(_newline, "");
    }
    DBG(data);
    return data;
}

/*************************************************************************
Configure connection mode (single or multiple)

    useMultiConn:   if multiple connection mode should be used
        false   -    use single connection mode
        true    -    use multiple connection mode

    return:
        true    -    successfully
        false    -    unsuccessfully
***************************************************************************/
boolean WIFI::confMux(boolean useMultiConn)
{
    String cmd = "AT+CIPMUX=";
    cmd += useMultiConn;
    _cell.println(cmd);
    DBG(cmd);
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if (_cell.find("OK")) {
           return true;
        }
    }
    return false;
}


/*************************************************************************
Set up TCP or UDP connection for single connection mode.

    type:    TCP or UDP

    addr:    IP address

    port:    port number


    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::newMux(byte type, String addr, int port)
{
    String data;
    _cell.print("AT+CIPSTART=");
    if(type>0)
    {
        _cell.print("\"TCP\"");
    }else
    {
        _cell.print("\"UDP\"");
    }
    _cell.print(",");
    _cell.print("\"");
    _cell.print(addr);
    _cell.print("\"");
    _cell.print(",");
//    _cell.print("\"");
    _cell.println(String(port));
//    _cell.println("\"");
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 || data.indexOf("ERROR")!=-1)
     {
         return true;
     }
  }
  return false;
}
/*************************************************************************
Set up TCP or UDP connection    (multiple connection mode)

    type:    TCP or UDP

    addr:    IP address

    port:    port number

    id:    id number(0-4)

    return:
        true    -    successfully
        false    -    unsuccessfully

***************************************************************************/
boolean WIFI::newMux(byte id, byte type, String addr, int port)
{

    _cell.print("AT+CIPSTART=");
    _cell.print("\"");
    _cell.print(String(id));
    _cell.print("\"");
    if(type>0)
    {
        _cell.print("\"TCP\"");
    }
    else
    {
        _cell.print("\"UDP\"");
    }
    _cell.print(",");
    _cell.print("\"");
    _cell.print(addr);
    _cell.print("\"");
    _cell.print(",");
//    _cell.print("\"");
    _cell.println(String(port));
//    _cell.println("\"");
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 )
     {
         return true;
     }
  }
  return false;


}
/*************************************************************************
Send data in single connection mode

    str:    string of message

    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::Send(String str)
{
    _cell.print("AT+CIPSEND=");
//    _cell.print("\"");
    _cell.println(str.length());
//    _cell.println("\"");
    unsigned long start;
    start = millis();
    bool found;
    while (millis()-start<5000) {
        if(_cell.find(">")==true )
        {
            found = true;
           break;
        }
     }
     if(found)
        _cell.print(str);
    else
    {
        closeMux();
        return false;
    }


    String data;
    start = millis();
    while (millis()-start<5000) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("SEND OK")!=-1)
     {
         return true;
     }
  }
  return false;
}

/*************************************************************************
Send data in multiple connection mode

    id:     <id>(0-4)

    str:    string of message

    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::Send(byte id, String str)
{
    _cell.print("AT+CIPSEND=");

    _cell.print(String(id));
    _cell.print(",");
    _cell.println(str.length());
    unsigned long start;
    start = millis();
    bool found;
    while (millis()-start<5000) {
        if(_cell.find(">")==true )
        {
            found = true;
           break;
        }
     }
     if(found)
        _cell.print(str);
    else
    {
        closeMux(id);
        return false;
    }


    String data;
    start = millis();
    while (millis()-start<5000) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("SEND OK")!=-1)
     {
         return true;
     }
  }
  return false;
}

/*************************************************************************
Close up TCP or UDP connection for single connection mode.
***************************************************************************/
void WIFI::closeMux(void)
{
    _cell.println("AT+CIPCLOSE");

    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("Linked")!=-1 || data.indexOf("ERROR")!=-1 || data.indexOf("we must restart")!=-1)
     {
         break;
     }
  }
}


/*************************************************************************
Set up TCP or UDP connection for multiple connection mode.

    id: connection ID number(0-4)
***************************************************************************/
void WIFI::closeMux(byte id)
{
    _cell.print("AT+CIPCLOSE=");
    _cell.println(String(id));
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
     if(_cell.available()>0)
     {
     char a = (char) _cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("Link is not")!=-1 || data.indexOf("Cant close")!=-1)
     {
         break;
     }
  }

}

/*************************************************************************
Show current IP address

    return: IP address as a String

***************************************************************************/
String WIFI::showIP(void)
{
    String data;
    unsigned long start;
    _cell.println("AT+CIFSR");
    start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        while(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("AT+CIFSR") != -1) {
            break;
        }
    }
    DBG(data);
    data.replace("AT+CIFSR","");
    while (data.indexOf(_newline) != -1) {
        data.replace(_newline, "");
    }
    DBG(data);
    return data;
}

/*************************************************************************
Set server parameters

    mode:
        0    -    close server mode
        1    -    open server mode

    port:    <port>

    return:
        true    -    successfully
        false   -    unsuccessfully

***************************************************************************/
boolean WIFI::confServer(byte mode, int port)
{
    String data;
    boolean result = false;
    String cmd = "AT+CIPSERVER=";
    cmd += mode;
    cmd += ",";
    cmd += port;
    _cell.println(cmd);
    DBG(cmd);

    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.available()) {
            data += (char) _cell.read();
        }
        if (data.indexOf("OK") != -1 || data.indexOf("no charge") != -1) {
            result = true;
            break;
        }
    }
    return result;
}
