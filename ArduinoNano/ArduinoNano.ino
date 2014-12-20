#include <SoftwareSerial.h>

#define SSID "michael" // insert your SSID
#define PASS "waterpigs" // insert your password
#define DST_IP "192.168.31.107" //RaspberryPi

SoftwareSerial esp8266(10, 11); // RX, TX

void setup()
{
    Serial.begin(9600);
    esp8266.begin(9600);
    sendDebug("AT");
}

void loop()
{
}

void sendDebug(String cmd){
    esp8266.println(cmd);
    Serial.println(cmd);
} 

boolean connectWiFi()
{
  sendDebug("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendDebug(cmd);
  delay(5000);
  if(esp8266.find("OK")){
    Serial.println("RECEIVED: OK");
    return true;
  }else{
    Serial.println("RECEIVED: Error");
    return false;
  }
}
