#include "WProgram.h"
#include "message.h"
#include "command.h"

void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}


///
/// Envoie un int //TODO elle sert a rien celle ci, si ?
///
void sendMessage(int id_msg, char cmd, int i)
{
	Serial.print(id_msg);
	Serial.print(C_SEP1);
	Serial.print(cmd);
	Serial.print(C_SEP2);
	Serial.println(i);
}

///
/// Envoie un int
///
void sendMessage(int id_msg, int i)
{
	Serial.print(id);
	Serial.print('|');
	Serial.println(i);
}


///
/// Envoie un tableau d'int
///
void sendMessage(int id_msg, int *tabi, int size)
{
	Serial.print(id_msg);
	Serial.print(C_SEP1);
	for (int i=0; i<size-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP2);
	}
	Serial.println(tabi[size-1]);
}

///
/// Envoie un string
///
void sendMessage(int id_msg, char* str)
{
	Serial.print(id_msg);
	Serial.print(C_SEP1);
	Serial.println(str);
}

///
/// Envoie des strings et des int
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(int id_msg, char** tabs, int nbStr, int *tabi, int nbInt)
{
	Serial.print(id_msg);
	Serial.print(C_SEP1);
	
	for (int i=0; i<nbStr; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(C_SEP2);
	}
	
	for (int i=0; i<nbInt-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP2);
	}
	Serial.println(tabi[nbInt-1]);
}

///
/// Envoie des int et des strings
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(int id_msg, unsigned char cmd, int* tabi, int nbInt, char** tabs, int nbStr)
{
	Serial.print(id_msg);
	Serial.print(C_SEP1);
	
	for (int i=0; i<nbInt; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP2);
	}
	
	for (int i=0; i<nbStr-1; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(C_SEP2);
	}
	Serial.println(tabs[nbStr-1]);
}

///
/// Parse les donnees recues sur le port serie et appel la fonction cmd pour effectuer les traitements
///
void readIncomingData()
{
	static char currentArg[10]; //<= cedric : je reduis ici pour menager la memoire des arduino (vu qu'on les cast en entier avec atoi, le max c'est 6 normalement (-32000 -> +32000)
	static int args[10];
	static int argsIndex = 0;
	static int currentArgIndex = 0;

	/*
	A propos du protocole :
	- un message se termine par \n
	*/

	// s'il y a des donnees a lire
	int available = Serial.available();
	for(int i = 0; i < available; i++){
		// recuperer l'octet courant
		int data = Serial.read();
		switch(data){
			// separateur
			case C_SEND: {
               	currentArg[currentArgIndex] = '\0';
       			args[argsIndex] = atoi(currentArg);
                argsIndex++;
        		currentArgIndex = 0;
        		break;
			}
			// fin de trame
			case '\n': {
                currentArg[currentArgIndex] = '\0';
        		args[argsIndex] = atoi(currentArg);
  				argsIndex = 0;
				currentArgIndex = 0;
				cmd(args[0],args[1],args+2); // appel de la commande
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


