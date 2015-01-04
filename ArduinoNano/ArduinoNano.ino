#include <SoftwareSerial.h>

#define SSID "michael" // insert your SSID
#define PASS "waterpigs" // insert your password
#define DST_IP "192.168.31.107" //RaspberryPi

SoftwareSerial esp8266(10, 11); // RX, TX

void setup()
{
    Serial.begin(9600);
    esp8266.begin(9600);
    sendDebug("AT+RST");    // reset
    delay(2000);
    sendDebug("AT");
    sendDebug("ATE0");  // disable echo
    connectWiFi();
    sendDebug("AT+CIPSTATUS");
    
    sendDebug("AT+CIPSTART=\"UDP\",\"192.168.31.107\",9999");
}

void loop()
{
    //char cmd[30] = "Kitchen;Temperature;-1900";
    String data = "Kitchen;Temperature;-1900";
    String cmd = "AT+CIPSEND=";
    cmd += data.length();
    esp8266.println(cmd);
    Serial.println(cmd);
    if (esp8266.find(">")) {
        Serial.print(">");
        esp8266.print(data);
        Serial.println(data);
    } else {
        sendDebug("AT+CIPCLOSE");
    }
    delay(2000);
}

void sendDebug(String cmd){
    esp8266.println(cmd);
    Serial.println(cmd);
    delay(1000);
    char tmp;
    tmp = esp8266.read();
    Serial.print(">");
    while (tmp != -1) {
        Serial.print(tmp);
        tmp = esp8266.read();
    }
    Serial.print("\n");
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
