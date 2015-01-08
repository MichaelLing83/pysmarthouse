#include "uartWIFI.h"

#ifdef ESP8266_DEBUG
    #define DBG(message)    Serial.println(message)
    #define DBGW(message)   Serial.write(message)
#else
    #define DBG(message)
    #define DBGW(message)
#endif

#ifdef UNO
    #ifdef ESP8266_DEBUG
        SoftwareSerial softSerial(_DBG_RXPIN_, _DBG_TXPIN_);
    #endif
#endif

int chlID;  //client id(0-4)

void WIFI::begin(void)
{
    _cell.begin(ESP8266_BAUDRATE);
    #ifdef ESP8266_DEBUG
        DebugSerial.begin(debugBaudRate);   //The default baud rate for debugging is 9600
    #endif
    _cell.flush();
    _cell.setTimeout(ESP8266_TIMEOUT);
    Reset();    // TODO: at other places, Reset results are not checked, is it a problem that Reset fails?
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
    boolean result = false;
    if (mode == STA || mode == AP || mode == AP_STA) {
        result = confMode(mode);
        Reset();
        // TODO: if confMode fails, does it make sense to continue with confJAP and confSAP?
        switch (mode) {
            case STA:
                /* result = confMode(mode);
                Reset(); */
                confJAP(ssid, pwd);
                break;
            case AP:
                /* result = confMode(mode);
                Reset(); */
                confSAP(ssid, pwd, chl, ecn);
                break;
            case AP_STA:
                /* result = confMode(mode);
                Reset(); */
                confJAP(ssid, pwd);
                confSAP(ssid, pwd, chl, ecn);
                break;
            default: break;
        }
    }
    return result;
}

/*************************************************************************
Set up TCP or UDP connection

    type:    TCP or UDP

    addr:    ip address

    port:    port number

    a:    set multiple connection
        0 for sigle connection
        1 for multiple connection

    id:    id number(0-4)

    return:
        true    -    successfully
        false    -    unsuccessfully

***************************************************************************/
boolean WIFI::ipConfig(byte type, String addr, int port, boolean a, byte id)
{
    boolean result = false;
    if (a == 0 )
    {
        confMux(a);

        long timeStart = millis();
        while (1)
        {
            long time0 = millis();
            if (time0 - timeStart > 5000)
            {
                break;
            }
        }
        result = newMux(type, addr, port);
    }
    else if (a == 1)
    {
        confMux(a);
        long timeStart = millis();
        while (1)
        {
            long time0 = millis();
            if (time0 - timeStart > 5000)
            {
                break;
            }
        }
        result = newMux(id, type, addr, port);
    }
    return result;
}

/*************************************************************************
Receive message from wifi

    buf:    buffer for receiving data

    chlID:    <id>(0-4)

    return:    size of the buffer


***************************************************************************/
int WIFI::ReceiveMessage(char *buf)
{
    //+IPD,<len>:<data>
    //+IPD,<id>,<len>:<data>
    String data = "";
    if (_cell.available()>0)
    {

        unsigned long start;
        start = millis();
        char c0 = _cell.read();
        if (c0 == '+')
        {

            while (millis()-start<5000)
            {
                if (_cell.available()>0)
                {
                    char c = _cell.read();
                    data += c;
                }
                if (data.indexOf("\nOK")!=-1)
                {
                    break;
                }
            }
            //Serial.println(data);
            int sLen = strlen(data.c_str());
            int i,j;
            for (i = 0; i <= sLen; i++)
            {
                if (data[i] == ':')
                {
                    break;
                }

            }
            boolean found = false;
            for (j = 4; j <= i; j++)
            {
                if (data[j] == ',')
                {
                    found = true;
                    break;
                }

            }
            int iSize;
            //DBG(data);
            //DBG("\r\n");
            if(found ==true)
            {
            String _id = data.substring(4, j);
            chlID = _id.toInt();
            String _size = data.substring(j+1, i);
            iSize = _size.toInt();
            //DBG(_size);
            String str = data.substring(i+1, i+1+iSize);
            strcpy(buf, str.c_str());
            //DBG(str);

            }
            else
            {
            String _size = data.substring(4, i);
            iSize = _size.toInt();
            //DBG(iSize);
            //DBG("\r\n");
            String str = data.substring(i+1, i+1+iSize);
            strcpy(buf, str.c_str());
            //DBG(str);
            }
            return iSize;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
Reboot the wifi module



***************************************************************************/
boolean WIFI::Reset(void)
{
    boolean reset_ok = false;
    unsigned long start;
    _cell.println("AT+RST");
    DBG("AT+RST");
    start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if(_cell.find("ready")==true)
        {
            reset_ok = true;
            break;
        }
    }
    DBG(String("Reset done with ") + reset_ok);
    return reset_ok;
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
    _cell.println("AT+CWMODE?");
    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1)
     {
         break;
     }
  }
    if(data.indexOf("1")!=-1)
    {
        return "Station";
    }else if(data.indexOf("2")!=-1)
    {
            return "AP";
    }else if(data.indexOf("3")!=-1)
    {
         return "AP+Station";
    }
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
    String data;
    _cell.print("AT+CWMODE=");
    _cell.println(mode);
    DBG(String("AT+CWMODE=") + mode);
    unsigned long start = millis();
    while (millis()-start < ESP8266_TIMEOUT) {
        if (_cell.available()) { // same logic as "if (_cell.available() > 0)", but saves two bytes
            data += _cell.read(); // saves 76 bytes compared to "data = data + _cell.read()"
        }
        if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1) {
            DBG("OK or no change");
            return true;
        }
        if (data.indexOf("ERROR") != -1 || data.indexOf("busy") != -1) {
            DBG("ERROR or busy");
            return false;
        }
    }
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
    delay(1000);
    while(1);
    unsigned long start;
    start = millis();
    while (millis()-start<8000) {
   if(_cell.available()>0)
   {
     char a =_cell.read();
     data=data+a;
   }
     if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
     {
         break;
     }
  }
    if(data.indexOf("ERROR")!=-1)
    {
        return "ERROR";
    }
    else{
       char head[4] = {0x0D,0x0A};
       char tail[7] = {0x0D,0x0A,0x0D,0x0A};
       data.replace("AT+CWLAP","");
       data.replace("OK","");
       data.replace("+CWLAP","WIFI");
       data.replace(tail,"");
       data.replace(head,"");

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
      String data;
      unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};
      data.replace("AT+CWJAP?","");
      data.replace("+CWJAP","AP");
      data.replace("OK","");
      data.replace(tail,"");
      data.replace(head,"");

          return data;
}


/*************************************************************************
Configure the SSID and password of the access port

    return:
    true    -    successfully
    false   -    unsuccessfully


***************************************************************************/
boolean WIFI::confJAP(String ssid , String pwd)
{

    _cell.print("AT+CWJAP=");
    _cell.print("\"");     //"ssid"
    _cell.print(ssid);
    _cell.print("\"");

    _cell.print(",");

    _cell.print("\"");      //"pwd"
    _cell.print(pwd);
    _cell.println("\"");


    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
        if(_cell.find("OK")==true)
        {
           return true;

        }
    }
    return false;
}
/*************************************************************************
//quite the access port

        return:
            true    -    successfully
            false    -    unsuccessfully


***************************************************************************/

boolean WIFI::quitAP(void)
{
    _cell.println("AT+CWQAP");
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
        if(_cell.find("OK")==true)
        {
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
      String data;
      unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};
      data.replace("AT+CWSAP?","");
      data.replace("+CWSAP","mySAP");
      data.replace("OK","");
      data.replace(tail,"");
      data.replace(head,"");

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
    _cell.print("AT+CWSAP=");
    _cell.print("\"");     //"SSID"
    _cell.print(ssid);
    _cell.print("\"");

    _cell.print(",");

    _cell.print("\"");      //"pwd"
    _cell.print(pwd);
    _cell.print("\"");

    _cell.print(",");
    _cell.print(String(chl));

    _cell.print(",");
    _cell.println(String(ecn));
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
        if(_cell.find("OK")==true )
        {
           return true;
        }
     }

     return false;

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
      String data;
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }

          char head[4] = {0x0D,0x0A};
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};
          data.replace("AT+CIPSTATUS","");
          data.replace("OK","");
          data.replace(tail,"");
          data.replace(head,"");

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

      unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }
          char head[4] = {0x0D,0x0A};
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};
          data.replace("AT+CIPMUX?","");
          data.replace("+CIPMUX","showMux");
          data.replace("OK","");
          data.replace(tail,"");
          data.replace(head,"");

          return data;
}

/*************************************************************************
Configure connection mode (single or multiple)

    a:        connection mode
        0    -    single
        1    -    multiple

    return:
        true    -    successfully
        false    -    unsuccessfully
***************************************************************************/
boolean WIFI::confMux(boolean a)
{
    _cell.print("AT+CIPMUX=");
    _cell.println(a);
    unsigned long start;
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
        if(_cell.find("OK")==true )
        {
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
     char a =_cell.read();
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
boolean WIFI::newMux( byte id, byte type, String addr, int port)
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
     char a =_cell.read();
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
     char a =_cell.read();
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
     char a =_cell.read();
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
     char a =_cell.read();
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
     char a =_cell.read();
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
    //DBG("AT+CIFSR\r\n");
    for(int a=0; a<3;a++)
    {
    _cell.println("AT+CIFSR");
    start = millis();
    while (millis()-start<ESP8266_TIMEOUT) {
     while(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("AT+CIFSR")!=-1)
     {
         break;
     }
    }
    if(data.indexOf(".") != -1)
    {
        break;
    }
    data = "";
  }
    //DBG(data);
    //DBG("\r\n");
    char head[4] = {0x0D,0x0A};
    char tail[7] = {0x0D,0x0D,0x0A};
    data.replace("AT+CIFSR","");
    data.replace(tail,"");
    data.replace(head,"");

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
    _cell.print("AT+CIPSERVER=");
    _cell.print(String(mode));
    _cell.print(",");
    _cell.println(String(port));

    String data;
    unsigned long start;
    start = millis();
    boolean found = false;
    while (millis()-start<ESP8266_TIMEOUT) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("no charge")!=-1)
     {
        found = true;
         break;
     }
  }
  return found;
}
