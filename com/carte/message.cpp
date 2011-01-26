#include "WProgram.h"
#include "message.h"
#include "command.h"


using namespace std;


void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}


///
/// Envoie un int
///
void sendMessage(char cmd, int i)
{
	Serial.print(cmd);
	Serial.print(",");
	Serial.println(i);
}
///
/// Envoie un tableau d'int
///
void sendMessage(char cmd, int *tabi, int size)
{
	Serial.print(cmd);
	Serial.print(",");
	for (int i=0; i<size-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	Serial.println(tabi[size-1]);
}
///
/// Envoie un string
///
void sendMessage(unsigned char cmd, char* str)
{
	Serial.print(cmd);
	Serial.print(",");
	Serial.println(str);
}
///
/// Envoie des strings et des int
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(unsigned char cmd, char** tabs, int nbStr, int *tabi, int nbInt)
{
	Serial.print(cmd);
	Serial.print(",");
	
	for (int i=0; i<nbStr; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(' ');
	}
	
	for (int i=0; i<nbInt-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	Serial.println(tabi[nbInt-1]);
}
///
/// Envoie des int et des strings
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(unsigned char cmd, int* tabi, int nbInt, char** tabs, int nbStr)
{
	Serial.print(cmd);
	Serial.print(",");
	
	for (int i=0; i<nbInt; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(' ');
	}
	
	for (int i=0; i<nbStr-1; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(' ');
	}
	Serial.println(tabs[nbStr-1]);
}

///
/// lit les données entrant puis appelle analyseMessage quand le message est complet
/// pour savoir un début et fin du message, le message doit être encadré de chevrons
/// <msg>
///
void readIncomingData()
{
	static unsigned char buffer[100];
	static int bufferIndex = 0;
	/*
	A propos du protcole :
	- un message commence par < et se termine par >
	- le format des trames est dans Spec_protocole.pdf
	*/
	// S'il y a des données à lire
	int available = Serial.available();
	for(int i = 0; i < available; i ++){
		int data = Serial.read();
		
		switch(data){
			case '<': bufferIndex = 0; break;
			case '>': analyzeMessage(bufferIndex, buffer); break;
			default:
				buffer[bufferIndex] = data;
			        bufferIndex++;
			break;
		}
	}
}

///
/// analyse le message puis lance la fonction 'cmd' de comùande.cpp
/// 
/// @author Matthieu Thomas
/// @param
/// 	size	: la taille du buffer (calculée dans readIncomingData())
///		buffer	: le message
///
void analyzeMessage(int size, unsigned char* buffer)
{
	int message[10];
	int indexMessage = 0;
	char valeur[20];
	int indexValeur = 0;
	char c;
	
	message[0] = 2;
	for (int i=2; i<size; ++i)
	{
		c = buffer[i]; // on lit
		valeur[indexValeur] = c;
		if (c == ' ' || i == size-1)
		{
			if (i == size-1)
				++indexValeur;
			valeur[indexValeur] = '\0'; // on rajoute la fin de chaine
			message[indexMessage] = atoi(valeur); // on transforme en int
			++indexMessage; // on incrémente l'index des messages
			indexValeur = 0; // on réinitialise l'index de la chaine
		}
		++indexValeur; // on incremente l'index de la chaine
	}
	
	// CALL command.cpp (Don't forget this file is generic for all our arduino board)
	cmd(buffer[0], message);

}


