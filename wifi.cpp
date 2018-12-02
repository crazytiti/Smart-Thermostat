#include "wifi.h"
#include "wifi_key.h"

#define maxtry  50
/*
char ssid[] = "SSID";  //  your network SSID (name)
char pass[] = "PASS";       // your network password
=> put in the wifi_key.h 
*/

int init_wifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int i=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    i++;
    if (i > maxtry){
      break;
      //ESP.restart();
    }
  }
  if(i > maxtry){
    Serial.println("No wifi connection");
    return (1);
  }
  else{
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  
    return (0);
  } 
}
