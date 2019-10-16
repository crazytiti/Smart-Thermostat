#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <Time.h>
#include <Time2.h> //(conflit avec la lib Time.h du wifi shield, il faut également modifier les fichiers de la lib)

//#define TimeCorrection  7200 //fuseau horaire ou heure été/hiver en sec

void init_NTP();
void get_time(char* timenow, int TimeCorrection);
unsigned long get_timestamp(int TimeCorrection);
unsigned long sendNTPpacket(IPAddress& address);
