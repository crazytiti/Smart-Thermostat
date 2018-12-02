
/*
Horo3231.cpp - Bib gerant la lecture et l'écriture des modules horodateurs DS1307-3231
//Attention!!! les fonctions relatives aux alarmes ne concernent que le DS3231
*/
#include "Arduino.h"
#include "Horo3231.h"

// Fonction de lecture de l'heure et date sur composant RTC
void Horo3231::readDS3231time(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)

{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
// Fonction de transformation des éléments horo en chaine: strHoro
void Horo3231::horoStamp(String *strHoro){
   
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; // Variables Heure/date
readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 
   // Construction de la chaine de données myData (JJ/MM/AA;hh:mn:ss;)
  *strHoro = String(dayOfMonth, DEC);
 *strHoro += "/" ;
 *strHoro += String(month,DEC) ;
 *strHoro += "/" ;
 *strHoro +=String(year, DEC);//
 *strHoro += ";";
 *strHoro += String(hour, DEC); //
 *strHoro += ":" ;
   if (minute<10){
  *strHoro += "0";
  }
 *strHoro +=String(minute,DEC);
 *strHoro +=  ":";
    if (second<10){
  *strHoro += "0";
  } 
 *strHoro += String(second, DEC);
 *strHoro += ";";
}
// Fonction reduite de transformation des éléments horo en chaine: strShort (JJ/MM;HH/mm)
void Horo3231::horoShort(String *strShort){
   
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; // Variables Heure/date
readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 
   // Construction de la chaine de données myData (JJ/MM;hh:mn;)
  *strShort = String(dayOfMonth, DEC);
 *strShort += "/" ;
 *strShort += String(month,DEC) ;
 *strShort += ";";
 *strShort += String(hour, DEC); //
 *strShort += ":" ;
   if (minute<10){
  *strShort += "0";
  }
 *strShort +=String(minute,DEC);
 *strShort += ";";
}
// Fonction reduite de transformation des éléments horo en chaine: strDate (JJ/MM;)
void Horo3231::horoJJMM(String *strDate){
   
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; // Variables Heure/date
readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 
   // Construction de la chaine de données myData (JJ/MM;)
  *strDate = String(dayOfMonth, DEC);
 *strDate += "/" ;
 *strDate += String(month,DEC) ;
 *strDate += ";";
}
// Fonction reduite de transformation des éléments horo en chaine: strHeure (HH/mm;)
void Horo3231::horoHHmm(String *strHeure){
   
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; // Variables Heure/date
readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 
   // Construction de la chaine de données myData (JJ/MM/AA;hh:mn:ss;)
 *strHeure += String(hour, DEC); //
 *strHeure += ":" ;
   if (minute<10){
  *strHeure += "0";
  }
 *strHeure +=String(minute,DEC);
 *strHeure += ";";
}

// Fonction affichage de l'heure et date sur liaison serie
void Horo3231::displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  switch(dayOfWeek){
  case 1:
    Serial.print("Dimanche");
    break;
  case 2:
    Serial.print("Lundi");
    break;
  case 3:
    Serial.print("Mardi");
    break;
  case 4:
    Serial.print("Mercredi");
    break;
  case 5:
    Serial.print("Jeudi");
    break;
  case 6:
    Serial.print("Vendredi");
    break;
  case 7:
    Serial.print("Samedi");
    break;
  }
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
   Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" ");
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.println(second, DEC);
 
}

//Fonction de mise Date et Heure
//Auparavant ne pas omettre de caster en bytes
void Horo3231::setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
//Fonction de mise à l'heure de l'alarme 1
void Horo3231::setDS3231Alarme1(byte second, byte minute, byte hour, byte dayOfWorDate)
{
  	// sets Alarme1 time and Alarme1 date data to DS3231

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x7); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWorDate)); // set day of week (1=Sunday, 7=Saturday) or date
 
  Wire.endTransmission();
}

//Fonction de mise à l'heure de l'alarme 2
void Horo3231::setDS3231Alarme2(byte minute, byte hour, byte dayOfWorDate)
{
  	// sets Alarme1 time and Alarme1 date data to DS3231

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0b); // set next input to start at the minute register
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWorDate)); // set day of week (1=Sunday, 7=Saturday) or Date
  
  Wire.endTransmission();
}

//Fonction d'écriture d'un octet (en fait pour reécrire le control register) 
//peut etre utilisé pour modifier la freq de la sortie SQW
//garder le bit de poid 6 du Control register à 1 -> X1XXXXXX
void Horo3231::SetByte(byte adToWrite, byte byteToWrite)
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(adToWrite);
	Wire.write(byteToWrite);
	Wire.endTransmission();
	
}
byte Horo3231::ReadByte(byte adToRead)
{
  //se positionner à l'adresse en mode ecriture
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(adToRead); // si on fait un nouveau write on écrit ce registre
   Wire.endTransmission(); //End car on veut lire
 // Pour lire le registre    
   Wire.requestFrom(DS3231_I2C_ADDRESS, 1); //(1 byte attendu)
return Wire.read();
}
// Fonction qui  met le MSB à 1 . Utilisé pour parametrer les alarmes
// voir tableau 2 alarm mask bits (Page 12 de la doc du DS3231)
// on passe en parametre l'adresse de l'octet on lit l'octet et on fait
// un "ou" logique avec 1000000 ce qui passe le MSB à 1 les autres bits sont inchangés
// on écrit le nouveau byte à l'adresse de l'ancien
// on retourne le nouveau byte pour info
byte Horo3231::SetMSBto1(byte AdrAlMask)
{
	byte AlarmElem;
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(AdrAlMask);
	Wire.endTransmission(); //End car on veut lire	
// Pour lire le registre AdrAlMask    
	Wire.requestFrom(DS3231_I2C_ADDRESS, 1); //(1 byte attendu)
	AlarmElem = Wire.read();
// MSB set to 1
	AlarmElem |= 0x80;
//	On le reécrit modifié MSB à 1
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(AdrAlMask);
	Wire.write(AlarmElem);
	Wire.endTransmission();
	return AlarmElem;
}

 //Fonction de lecture de l'entrée RS232 (mise à l'heure) retourne le nbre de caractères et peuple str[]
int Horo3231::ReadLine(char str[])
{
char c;
int index = 0;
while (true)
{
  if (Serial.available() > 0)
  {
  c = Serial.read();
    if (c != '\n') {
    str[index++] = c;
    } 
    else {
    str[index] = '\0'; // null termination character
    break;
    }
  }
}
return index;
}
//Fonction récupérant les minutes sur la liaison RS232
byte Horo3231::myMinute(char myData[]) {
char myStr[3];
myStr[0] = myData[3];
myStr[1] = myData[4];
return atoi(myStr);
}
//Fonction récupérant les Heures sur la liaison RS232
byte Horo3231::myHour(char myData[]) {
char myStr[3];
myStr[0] = myData[0];
myStr[1] = myData[1];
return atoi(myStr);
}
//Fonction recuperant le jour de la semaine sur entrée série
byte Horo3231::myDayOfWeek(char myData[]) {
  char myStr[1];
  myStr[0] = '0';
  myStr[1] = myData[6];
  return atoi(myStr);
}
//Fonction recuperant le jour du mois sur entrée série
 byte Horo3231::myDay(char myData[]) {
 char myStr[3];
myStr[0] = myData[8];
myStr[1] = myData[9];
return atoi(myStr);  
 }
 //Fonction recuperant le mois sur entrée série
 byte Horo3231::myMonth(char myData[]) {
 char myStr[3];
myStr[0] = myData[11];
myStr[1] = myData[12];
return atoi(myStr);  
 } 
 //Fonction recuperant l'année (sur 2 chiffres) sur entrée série
 byte Horo3231::myYear(char myData[]) {
 char myStr[3];
myStr[0] = myData[14];
myStr[1] = myData[15];
return atoi(myStr);  
 }
 
// Fonction: Convert normal decimal numbers to binary coded decimal
byte Horo3231::decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

// Fonction: Convert binary coded decimal to normal decimal numbers
byte Horo3231::bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

