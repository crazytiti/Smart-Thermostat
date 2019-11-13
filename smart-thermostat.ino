//use the 2.5.0 version of board manager
// make sure crystal frequency is set to the right value otherwise com will be giberish
// Include the libraries we need
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <EEPROM.h>
#include "wifi.h"
#include "NTP_client.h"
#include "DallasTemperature.h"
#include "Horo3231.h"
#include "settings.h"
#include "TM1637Display.h"
#include "functions.h"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

unsigned long last_capture = 0;
unsigned long manual_mode_time = 0;
int TimeCorrection = 7200;
uint8_t TMconsigne[1] = {0b01011000};
uint8_t TMcel[1] = {0b01100011};
uint8_t TMcel1[1] = {0b00100011};
uint8_t TMcel2[1] = {0b01000011};
uint8_t TMcel3[1] = {0b01100010};
uint8_t TMcel4[1] = {0b01100001};
HTTPClient http;
Horo3231 myHoro3231;
TM1637Display tm1637(tm1637_CLK, tm1637_DIO);
int wifiOk;
int temp_heure;     // boolean d'affichage lcd
int reload_config;  // boolean de changement de config
//-----variables de gestion thermostat---------
int th_mode = 'M';          //mode : M : manuel, P : planning, O : off
int bt_interrupt = 0;   //flag appuis sur boutons
int N_planning = 0;     //N de planning actif
int th_planning[7][20];  //7 jours de la semaine contiennent 10 couples température(à diviser par 10) / horaire(en minute depuis minuit)
float th_calibration;  //calibration sonde de t°
float th_hysteresis;   //seuil d'hystérésis
float th_consigne;      //consigne
struct confort {
  float temp;
  byte heures;
  byte minutes;
};
struct Jour {
  struct confort horaire[10];
};
struct Jour Planning[7];
//--------------

void animlogo(TM1637Display tm1637, int tempo) {
  tm1637.setSegments(TMcel1, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel2, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel3, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel4, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel1, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel2, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel3, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel4, 1, 3);   //display 'c'
  delay(tempo);
  tm1637.setSegments(TMcel, 1, 3);   //display 'c'
}

static void btPlus() {  
  noInterrupts();
  bt_interrupt = 1;
}

static void btMoins() {
  noInterrupts();
  bt_interrupt = 1;
}

void btMode() {
  switch (th_mode) {
    case 'O': th_mode = 'P';
      break;
    case 'P': th_mode = 'M';
      break;
    case 'M': th_mode = 'O';
      break;
  }
  Serial.print("Changement de mode : ");
  Serial.println(th_mode);
//  delay(300);
}

int get_config_int(String champ, HTTPClient *http, int* value) {
  String tempstr;
  Serial.println("Get config DB---------------------------------------");
  tempstr =  "thermostat_get_config.php?champ=";
  tempstr += champ;
  tempstr = bdd_adr + tempstr;
  Serial.println(tempstr);
  http->begin(tempstr);
  http->addHeader("Content-Type", "text/plain");
  int httpCode = http->GET();
  Serial.print("code retour: ");
  Serial.print(httpCode);
  String payload = http->getString();
  http->end();
  Serial.print(" reponse: ");
  Serial.println(payload);
  *value = payload[0] - 48;
  return (httpCode);
}

int get_config_char(String champ, HTTPClient *http, int* value) {
  String tempstr;
  Serial.println("Get config DB---------------------------------------");
  tempstr =  "thermostat_get_config.php?champ=";
  tempstr += champ;
  tempstr = bdd_adr + tempstr;
  Serial.println(tempstr);
  http->begin(tempstr);
  http->addHeader("Content-Type", "text/plain");
  int httpCode = http->GET();
  Serial.print("code retour: ");
  Serial.print(httpCode);
  String payload = http->getString();
  http->end();
  Serial.print(" reponse: ");
  Serial.println(payload);
  *value = payload[0];
  return (httpCode);
}

// fonctionne uniquement de -9.9 à 9.9
int get_config_float(String champ, HTTPClient *http, float* retvalue) {
  String tempstr;
  float value;
  Serial.println("Get config DB---------------------------------------");
  tempstr =  "thermostat_get_config.php?champ=";
  tempstr += champ;
  tempstr = bdd_adr + tempstr;
  Serial.println(tempstr);
  http->begin(tempstr);
  http->addHeader("Content-Type", "text/plain");
  int httpCode = http->GET();
  Serial.print("code retour: ");
  Serial.print(httpCode);
  String payload = http->getString();
  http->end();
  Serial.print(" reponse: ");
  Serial.println(payload);
  if (payload[0] == '-') {
    Serial.println("neg");
    value = -payload[1] + 48;
    value = value * 10;
    value -= (payload[3] - 48);
    value = value / 10;
  }
  else {
    Serial.println("pos");
    value = payload[0] - 48;
    value = value * 10;
    value += (payload[2] - 48);
    value = value / 10;
  }
  *retvalue = value;
  return (httpCode);
}

//recupere un jour et le stock dans la structure
int get_planning(byte numero_jour, struct Jour& jour, HTTPClient *http) {
  String tempstr;
  Serial.println("Get planning DB---------------------------------------");
  tempstr =  "thermostat_get_planning.php?jour=";
  tempstr += numero_jour;
  tempstr = bdd_adr + tempstr;
  Serial.println(tempstr);
  http->begin(tempstr);
  http->addHeader("Content-Type", "text/plain");
  int httpCode = http->GET();
  Serial.print("code retour: ");
  Serial.print(httpCode);
  String payload = http->getString();
  http->end();
  Serial.print(" reponse: ");
  Serial.println(payload);
  if (httpCode == 200) {
    int j = 0;
    while (get_champ(payload, tempstr) != 0) {
      if (j > 2) {
        jour.horaire[j - 3].heures = (tempstr[1] - 48) * 10 + tempstr[2] - 48;
        jour.horaire[j - 3].minutes = (tempstr[4] - 48) * 10 + tempstr[5] - 48;
        Serial.print(jour.horaire[j - 3].heures);
        Serial.print('h');
        Serial.print(jour.horaire[j - 3].minutes);
        Serial.print('t');
        get_champ(payload, tempstr);
        switch (tempstr.length()) {   //conversion en float suivant le nombre de char (ne marche que de 0 à 99.9, 1 décimal max)
          case 2:
            jour.horaire[j - 3].temp = tempstr[1] - 48;
            break;
          case 3:
            jour.horaire[j - 3].temp = (tempstr[1] - 48) * 10 + tempstr[2] - 48;
            break;
          case 4:
            jour.horaire[j - 3].temp = (tempstr[1] - 48) * 10 + tempstr[3] - 48;
            jour.horaire[j - 3].temp = jour.horaire[j - 3].temp / 10;
            break;
          case 5:
            jour.horaire[j - 3].temp = (tempstr[1] - 48) * 100 + (tempstr[2] - 48) * 10 + tempstr[4] - 48;
            jour.horaire[j - 3].temp = jour.horaire[j - 3].temp / 10;
            break;
          default:
            jour.horaire[j - 3].temp = 18.0;
            break;
        }
        Serial.print(jour.horaire[j - 3].temp);
        Serial.print(" | ");
      }
      j++;
    }
  }
  return (httpCode);
}

//recupere un champ de la trame de planning
int get_champ(String& source, String& dest) {
  int i = 0;
  dest = "";
  while ((source[i] != ',') & (i < source.length())) {
    dest += source[i];
    i++;
  }
  source = source.substring(i + 1);
  return (i);
}

void setup(void)
{
  char timenow[20];
  String time3231;
  String dbdialog = " ";
  String tempstr = " ";
  int GetTimeTentatives = 8;
  int retourhttp = 0;
  int adr_eeprom = 0;
  byte DSsecond, DSminute, DShour, DSdayOfWeek, DSdayOfMonth, DSmonth, DSyear;

  attachInterrupt(digitalPinToInterrupt(ButtonPlus), btPlus, FALLING);
  attachInterrupt(digitalPinToInterrupt(ButtonMoins), btMoins, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(ButtonMode), btMode, FALLING);
  temp_heure = 0;
  // start serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("Smart Thermostat");
  pinMode(LED, OUTPUT);
  pinMode(Heater, OUTPUT);
  pinMode(ButtonPlus, INPUT);
  pinMode(ButtonMoins, INPUT);
  // LCD
  tm1637.setBrightness(0x07);
  tm1637.clear();
  tm1637.showNumberDecEx(888, 0b01000000, false, 3, 0);
  tm1637.setSegments(TMcel, 1, 3);   //display 'c'
  // Start up the library DS1820
  sensors.begin();
  // WIFI
  wifiOk = 1 - init_wifi();
  //-----------------  récupération config en BDD ---------------------------
  EEPROM.begin(1024);            //alloue en ram l'espace nécésaire aux actions d'eeprom
  retourhttp = get_config_int("fuseau", &http, &TimeCorrection);
  if (retourhttp == 200) { //sauvegarde en eeprom
    EEPROM.put(adr_eeprom, TimeCorrection);
    Serial.print("adr: ");
    Serial.println(adr_eeprom);
  }
  else { //restaure eeprom
    EEPROM.get(adr_eeprom, TimeCorrection);
  }
  adr_eeprom += sizeof(TimeCorrection);
  TimeCorrection = TimeCorrection * 3600;
  retourhttp = get_config_char("mode", &http, &th_mode);
  if (retourhttp == 200) { //sauvegarde en eeprom
    EEPROM.put(adr_eeprom, th_mode);
    Serial.print("adr: ");
    Serial.println(adr_eeprom);
  }
  else { //restaure eeprom
    EEPROM.get(adr_eeprom, th_mode);
  }
  adr_eeprom += sizeof(th_mode);
  retourhttp = get_config_int("N_planning", &http, &N_planning);
  if (retourhttp == 200) { //sauvegarde en eeprom
    EEPROM.put(adr_eeprom, N_planning);
    Serial.print("adr: ");
    Serial.println(adr_eeprom);
  }
  else { //restaure eeprom
    EEPROM.get(adr_eeprom, N_planning);
  }
  adr_eeprom += sizeof(N_planning);
  retourhttp = get_config_float("calibration", &http, &th_calibration);
  if (retourhttp == 200) { //sauvegarde en eeprom
    EEPROM.put(adr_eeprom, th_calibration);
    Serial.print("adr: ");
    Serial.println(adr_eeprom);
  }
  else { //restaure eeprom
    EEPROM.get(adr_eeprom, th_calibration);
  }
  adr_eeprom += sizeof(float);
  retourhttp = get_config_float("hysteresis", &http, &th_hysteresis);
  if (retourhttp == 200) { //sauvegarde en eeprom
    EEPROM.put(adr_eeprom, th_hysteresis);
    Serial.print("adr: ");
    Serial.println(adr_eeprom);
    EEPROM.commit();
  }
  else { //restaure eeprom
    EEPROM.get(adr_eeprom, th_hysteresis);
  }
  adr_eeprom += sizeof(float);

  for (int i = 1; i < 8; i++) {
    retourhttp = get_planning(i, Planning[i - 1], &http);
    if (retourhttp == 200) { //sauvegarde en eeprom
      EEPROM.put(adr_eeprom, Planning[i - 1]);
      Serial.print("adr: ");
      Serial.println(adr_eeprom);
      EEPROM.commit();
      delay(20);
    }
    else { //restaure eeprom
      EEPROM.get(adr_eeprom, Planning[i - 1]);
    }
    adr_eeprom += sizeof(Planning[i - 1]);
  }

  //----------------- affichage des settings ---------------------------
  Serial.println("Planning: ");
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 10; j++) {
      Serial.print(Planning[i].horaire[j].temp);
      Serial.print("°c ");
      Serial.print(Planning[i].horaire[j].heures);
      Serial.print("h");
      Serial.print(Planning[i].horaire[j].minutes);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.print("fuseau: ");
  Serial.println(TimeCorrection);
  Serial.print("th_mode: ");
  Serial.println(th_mode);
  Serial.print("N_planning: ");
  Serial.println(N_planning);
  Serial.print("calib: ");
  Serial.println(th_calibration);
  Serial.print("hyst: ");
  Serial.println(th_hysteresis);
  th_consigne = 22.5;
  //serveur de temps
  init_NTP();
  //--recupere l'heure actuel
  timenow[0] = ' ';
  if (wifiOk) {
    Serial.println("Getting time");
    while (timenow[0] == ' ' && GetTimeTentatives > 0) {
      get_time(timenow, TimeCorrection);
      delay(100);
      Serial.print("-");
      GetTimeTentatives--;
    }
  }
  Wire.begin(I2C_SDA, I2C_SCL);
  myHoro3231.SetByte(0x0F, 0x8);
  if (timenow[0] != ' ') {
    Serial.println("Time OK");
    myHoro3231.setDS3231time(second(), minute(), hour(), weekday(), day(), month(), year() - 2000);   //setting time for DS3231
  }
  else {
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
  int retourhttp = 0;
  //------------capture 1 temperature ----------------
  sprintf(timenow, "%d %02d %02d J:%02d %02d %02d %02d", year(), month() , day(), weekday(), hour(), minute(), second());
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
  if (bt_interrupt){
    bt_interrupt=0;
    while(digitalRead(ButtonPlus) == LOW){
      th_consigne += 0.1;
      Serial.print("Passage en mode manuel, consigne : ");
      Serial.println(th_consigne);
      tm1637.showNumberDecEx(th_consigne * 10, 0b01000000, false, 3, 0);
      tm1637.setSegments(TMcel4, 1, 3);   //display 'c'
      delay(450);
    }
    while(digitalRead(ButtonMoins) == LOW){
      th_consigne -= 0.1;
      Serial.print("Passage en mode manuel, consigne : ");
      Serial.println(th_consigne);
      tm1637.showNumberDecEx(th_consigne * 10, 0b01000000, false, 3, 0);
      tm1637.setSegments(TMcel4, 1, 3);   //display 'c'
      delay(450);
    }    
    manual_mode_time = now();
    interrupts();
  }
  th_consigne = get_consigne();
  Serial.print("Consigne: ");
  Serial.println(th_consigne);
  if (temp_heure) { //affiche la température ou l'heure
    tm1637.showNumberDecEx(current_temp * 10, 0b01000000, false, 3, 0);
    tm1637.setSegments(TMcel, 1, 3);   //display '°'
    if (set_heater(current_temp)) {
      animlogo(tm1637, 100);
    }
    else {
      delay(1200);
    }
    temp_heure = 1 - temp_heure;
  }
  else {
    tm1637.showNumberDecEx(hour(), 0b00000000, true, 2, 0);
    tm1637.showNumberDecEx(minute(), 0b00000000, true, 2, 2);
    delay(400);
    tm1637.showNumberDecEx(hour(), 0b01000000, true, 2, 0);
    delay(400);
    tm1637.showNumberDecEx(hour(), 0b00000000, true, 2, 0);
    delay(400);
    tm1637.showNumberDecEx(hour(), 0b01000000, true, 2, 0);
    delay(200);
    temp_heure = 1 - temp_heure;
  }
  //----------------envoi sur SQLITE--------------------
  if (last_capture < (now() - interval)) {
    last_capture = now();
    if (current_temp > -31 & current_temp < 81) {
      Serial.println("Sending to DB---------------------------------------");
      tempstr =  "thermostat_temp.php?temp=";
      tempstr += current_temp;
      tempstr += "&timestamp=";
      tempstr += timestamp;
      tempstr += "&consigne=";
      tempstr += th_consigne;
      dbdialog = bdd_adr + tempstr;
      Serial.println(dbdialog);
      http.begin(dbdialog);
      http.addHeader("Content-Type", "text/plain");
      int httpCode = http.GET();
      String payload = http.getString();
      http.end();
      Serial.println(payload);
      Serial.print(" reponse: ");
      Serial.println(httpCode);
      Serial.println("Sending finish");
    }
    else {
      Serial.println("--------------Temp out of range--------------");
    }

    //------------------- si changement de config => reset ------------
    retourhttp = get_config_int("rechargement", &http, &reload_config);
    if ((retourhttp == 200) & (reload_config == 1)) {
      tempstr =  "thermostat_reset.php";
      dbdialog = bdd_adr + tempstr;
      Serial.println(dbdialog);
      http.begin(dbdialog);
      http.addHeader("Content-Type", "text/plain");
      int httpCode = http.GET();
      String payload = http.getString();
      http.end();
      Serial.println(payload);
      Serial.print(" reponse: ");
      Serial.println(httpCode);
      Serial.println("Sending finish");
      Serial.println("Reset to reload config");
      ESP.restart();  //reset
    }
  }
}
//----------------fonction thermostat-----------------
int set_heater(float current_temp) {
  if (th_mode == 'O') {
    digitalWrite(Heater, 1);
    Serial.println("OFF");
    return 0;
  }
  if (digitalRead(Heater)) {
    if (th_consigne > (current_temp - th_hysteresis)) {
      digitalWrite(Heater, 0);
      Serial.println("Heat");
      return 1;
    }
    else {
      digitalWrite(Heater, 1);
      Serial.println("Cool");
      return 0;
    }
  }
  else {
    if (th_consigne > (current_temp + th_hysteresis)) {
      digitalWrite(Heater, 0);
      Serial.println("Heat");
      return 1;
    }
    else {
      digitalWrite(Heater, 1);
      Serial.println("Cool");
      return 0;
    }
  }
  return 0;
}

float get_consigne(void) {
  float consigne = 18;
  int today = dayofweek(now());
  Serial.print("Day of week: ");
  Serial.println(today);
  if (manual_mode_time > (now() - 3600)) {
    consigne = th_consigne;
    Serial.println("manual mode");
  }
  else {
    Serial.println("planning mode");
    // parcours décrémental des horaires de planning du jour pour trouver la plage horaire actuellement valide
    for (int i = 9; i > -1; i--) {
      if (hour() < Planning[today].horaire[i].heures) {
        consigne = Planning[today].horaire[i].temp;
      }
      if (hour() == Planning[today].horaire[i].heures) {
        if (minute() <= Planning[today].horaire[i].minutes) {
          consigne = Planning[today].horaire[i].temp;
        }
      }
    }
  }
  if (consigne > 25) {
    consigne = 25.0;
  }
  return consigne;
}
