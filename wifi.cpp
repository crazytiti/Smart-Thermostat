#include "wifi.h"
#include "wifi_key.h"

/*
char ssid[] = "SSID";  //  your network SSID (name)
char pass[] = "PASS";       // your network password
=> put in the wifi_key.h 
*/

void init_wifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int i=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    i++;
    if (i > 50){
      ESP.restart();
    }
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}
