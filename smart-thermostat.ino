// Include the libraries we need
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include "wifi.h"
#include "NTP_client.h"
#include "DallasTemperature.h"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  char timenow[20];
  // start serial port
  Serial.begin(115200);
  Serial.println("Smart Thermostat");
  // Start up the library
  sensors.begin();
  init_wifi();
  init_NTP();
  timenow[0] = ' ';
  while(timenow[0] == ' '){
    get_time(timenow);
  }
}

void loop(void)
{ 
  char timenow[20];
  unsigned long timestamp;
  sprintf(timenow,"%d %02d %02d %02d %02d %02d", year(), month() , day(), hour(), minute(), second());
  Serial.println(timenow);
  timestamp = now();
  Serial.println(timestamp);
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 is: ");
  Serial.println(sensors.getTempCByIndex(0));  
}
