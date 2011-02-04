#include "WProgram.h"
#include "message.h"
#include "command.h"

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
/// Parse les donnees recues sur le port serie et appel la fonction cmd pour effectuer les traitements
/// Exemple : <G 1000 1000 150>
///
void readIncomingData()
{
	static char currentArg[20];
	static int args[10];
	static int argsIndex = 0;
	static int currentArgIndex = 0;

	/*
	A propos du protocole :
	- un message commence par < et se termine par >
	*/

	/*s'il y a des donnees a lire*/
	int available = Serial.available();
	for(int i = 0; i < available; i++){
		/*recuperer l'octet courant*/
		int data = Serial.read();
		switch(data){
			/*separateur*/
			case ' ': {
                if(argsIndex>0){
                	currentArg[currentArgIndex] = '\0';
        			args[argsIndex] = atoi(currentArg);
                }
                else
                    args[argsIndex] = currentArg[0];
                argsIndex++;
        		currentArgIndex = 0;
        		break;
			}
			/*fin de trame*/
			case '\n': {
                if(argsIndex>0){
                	currentArg[currentArgIndex] = '\0';
        			args[argsIndex] = atoi(currentArg);
                }
                else
                    args[argsIndex] = currentArg[0];
				argsIndex = 0;
				currentArgIndex = 0;
				cmd(args[0], args+1);
				break;
			}
			default: {
				currentArg[currentArgIndex] = data;    
				currentArgIndex++;
			    break;
			}
		}
	}
}



/*
///
/// analyse le message puis lance la fonction 'cmd' de commande.cpp
/// 
/// @author Matthieu Thomas
/// @param
/// 	size	: la taille du buffer (calculee dans readIncomingData())
///		buffer	: le message
///
void analyzeMessage(int size, unsigned char* buffer)
{
	int message[10];
	int indexMessage = 0;
	char valeur[20];
	int indexValeur = 0;
	char c;
	
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
			++indexMessage; // on incremente l'index des messages
			indexValeur = -1; // on reinitialise l'index de la chaine
		}
		++indexValeur; // on incremente l'index de la chaine
	}
	
	// CALL command.cpp (Don't forget this file is generic for all our arduino board)
	cmd(buffer[0], message);

}

*/
