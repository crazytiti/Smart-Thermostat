# Smart-Thermostat
Smart home thermostat with graphics and fully 7-days programs 

Hardware :
- ESP8266
- RaspberryPi with Apache, PHP7, SQLITE
- DS1820 temperature sensor
- 4 digit LCD with TM1637
- battery saved clock DS3231
- Some arduino/220V relay
- Push buttons

Goal : 
Have an household smart thermostat :
- temperature regulation (banging on/off with hysteresis)
- schedule mode or manual mode
- 7 days schedule with 10 temperature points a day
- temperature graphics
- web access and config
- Wall mounted device that show actual temp, time and can change mode and temperature setpoint.

Tips for power : I use a DC-DC 220-5V to charge the li-ion. This converter is plug to the heater relay.
When the heater (i have an oil boiler) is off a small current goes throught it and power the dc-dc.
When the heater is on the thermostat run on li-ion (~100mA)

The serveur folder contain basic/broken web code.
The serveur/smart folder contain the full Open Element project

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/sur%20mur.jpg)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/capture-web.jpg)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/planning.JPG)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/config.JPG)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/proto.jpg)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/schema%20v2.png)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/en_boite.jpg)

![picture](https://raw.githubusercontent.com/crazytiti/Smart-Thermostat/master/assemblage.jpg)
