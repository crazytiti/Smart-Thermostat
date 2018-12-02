// Include the libraries we need
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <Time.h>
#include "wifi.h"
#include "NTP_client.h"
#include "DallasTemperature.h"
#include "Horo3231.h"
#include "settings.h"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

unsigned long last_capture=0;
HTTPClient http;
Horo3231 myHoro3231;
int wifiOk;

void setup(void)
{
  char timenow[20];
  String time3231;
  int GetTimeTentatives = 5;
  byte DSsecond, DSminute, DShour, DSdayOfWeek, DSdayOfMonth, DSmonth, DSyear;
  // start serial port
  Serial.begin(115200);
  Serial.println("Smart Thermostat");
  pinMode(LED, OUTPUT);
  // Start up the library
  sensors.begin();
  // WIFI
  wifiOk = 1 - init_wifi();
  //serveur de temps
  init_NTP();
  //--recupere l'heure actuel
  timenow[0] = ' ';
  if(wifiOk){
    Serial.println("Getting time");
    while(timenow[0] == ' ' && GetTimeTentatives > 0){
      get_time(timenow);
      delay(100);
      GetTimeTentatives--;
    }  
  }
  Wire.begin(I2C_SDA, I2C_SCL);
  myHoro3231.SetByte(0x0F, 0x8);
  if(timenow[0] != ' '){
    Serial.println("Time OK");
    myHoro3231.setDS3231time(second(), minute(), hour(), weekday(), day(), month(), year()- 2000);    //setting time for DS3231
  }
  else{
    Serial.println("Time using DS3231");
    myHoro3231.readDS3231time(&DSsecond, &DSminute, &DShour, &DSdayOfWeek, &DSdayOfMonth, &DSmonth, &DSyear);
    setTime(DShour, DSminute, DSsecond, DSdayOfMonth, DSmonth, 2000 + DSyear);
  }
  Serial.println("Horo3132: ");
  myHoro3231.displayTime();
  
  last_capture = now() - interval;
}

void loop(void)
{ 
  char timenow[20];
  String dbdialog = " ";
  String tempstr = " ";
  unsigned long timestamp;
  float current_temp;
  //------------capture 1 temperature ----------------
  sprintf(timenow,"%d %02d %02d J:%02d %02d %02d %02d", year(), month() , day(), weekday(), hour(), minute(), second());
  Serial.println(timenow);
  timestamp = now();
  Serial.println(timestamp);
  Serial.println("Horo3132: ");
  myHoro3231.displayTime();
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  digitalWrite(LED, !digitalRead(LED));
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  current_temp = sensors.getTempCByIndex(0);
  Serial.print("Temperature for the device 1 is: ");
  Serial.println(current_temp); 
 //----------------envoi sur SQLITE--------------------
 if(last_capture < (now() - interval)){
    last_capture = now();
    Serial.println("Sending to DB---------------------------------------");
    tempstr =  "thermostat_temp.php?temp=";
    tempstr += current_temp;
    tempstr += "&timestamp=";
    tempstr += timestamp;
    tempstr += "&consigne=19.1";
    dbdialog = bdd_adr + tempstr;
    Serial.println(dbdialog);
    http.begin(dbdialog);
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.GET();
    String payload = http.getString();
    http.end();
    Serial.println(payload);
    Serial.println(httpCode);
    Serial.println("Sending finish");
    
 }
 //----------------fonction thermostat-----------------
  
}
