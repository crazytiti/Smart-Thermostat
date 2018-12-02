/*
Horo.h - Bib gerant la lecture et l'écriture des modules horodateurs DS1307 (simple) et 3231 (plus precis et plus riche)
Attention toutes les fonctions ne sont pas valides sur DS1307 (uniquement heure et date)
Cette biblio fonctionne pour Arduino et également pour ESP8266
*/
#ifndef Horo3231_h // If we haven't read this file before...
#define Horo3231_h // ...read it now. This prevents double-including
#include "Arduino.h" // Not needed for our code, but often included
// si utilisation sur UNO ou NANO utiliser A4 (SDA) et A5 (SCL)
#include "Wire.h"   //utilisation ESP12E pin GPIO_4 (D2) pour SDA et GPIO_5 (D1) pour SCL
#define DS3231_I2C_ADDRESS 0x68
class Horo3231
{
public:
// constructor
//	Horo3231();
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; // Variables Heure/date	
// fonction de lecture du composant 3231-1307 les données sont passées par pointeurs
//Chaque valeur est dispo séparément
	void readDS3231time(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year);

	// fonction lecture de horodate le résultat est dans la chaine "strHoro" 
// sous la forme d'une chaine "jj/mm/aa;hh:mn:ss;"
	void horoStamp(String *strHoro);

	// fonction réduite lecture de horodate le résultat est dans la chaine "strShort" 
// sous la forme d'une chaine "jj/mm;hh:mn;"
	void horoShort(String *strShort);

// Fonction reduite de transformation des éléments horo en chaine: strDate (JJ/MM;)
void horoJJMM(String *strDate);

// Fonction reduite de transformation des éléments horo en chaine: strHeure (HH/mm;)
void horoHHmm(String *strHeure);

	//	//Fonction de mise Date et Heure
	void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
// 	//Fonction de mise Date Heure minutes et secondes Alarme 1
void setDS3231Alarme1(byte second, byte minute, byte hour, byte dayOfWorDate);
 	//Fonction de mise Date Heure et minutes Alarme 2 (pas de seconde dans cette fonction)
void setDS3231Alarme2(byte minute, byte hour, byte dayOfWorDate);
//Fonction de controle de l'alarme 1 ou 2 (en fait reécrit le control register) 
//peut etre utilisé pour modifier la freq de la sortie SQW
//garder les 2 bits de poids fort du Control register à 1 -> 11XXXXXX
void SetByte(byte adToWrite, byte byteToWrite);
// fonction qui lit un byte
byte ReadByte(byte adToRead);
// Fonction qui  met le MSB à 1 . Utilisé pour parametrer les alarmes
// voir tableau 2 alarm mask bits (Page 12 de la doc du DS3231)
// on passe en parametre l'adresse de l'octet à modifier
byte SetMSBto1(byte AdrAlMask);
// Fonction affichage de l'heure et date sur liaison serie "JdeSemaine JJ/MM/AA hh;mn;ss"	
	void displayTime();
//Fonction de lecture de l'entrée RS232 (mise à l'heure) retourne le nbre de caractères et peuple str[]
//Utilisé egalement pour toute saisie par entrée série (retourne le nbre de caract.)
	int ReadLine(char str[]);
//Fonction récupérant les minutes sur la liaison RS232	
	byte myMinute(char myData[]);
	//Fonction récupérant les Heures sur la liaison RS232
	byte myHour(char myData[]);
//Fonction recuperant le jour de la semaine sur entrée série	
	byte myDayOfWeek(char myData[]);
//Fonction recuperant le jour du mois sur entrée série	
	byte myDay(char myData[]);
//Fonction recuperant le mois sur entrée série	
	byte myMonth(char myData[]);
 //Fonction recuperant l'année (sur 2 chiffres) sur entrée série	
	byte myYear(char myData[]);
// Fonction: Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val);
// Fonction: Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val);
private:
};

#endif