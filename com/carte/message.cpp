#include "WProgram.h"
#include "message.h"
#include "command.h"

void initSerialLink()
{
	Serial.begin(SERIAL_BAUD);
}

///
/// Envoie le header
///
void sendHeader(int id_from, int id_cmd)
{
	Serial.print(id_from);
	Serial.print(C_SEP_SEND);
	Serial.print(id_cmd);
	Serial.print(C_SEP_SEND);
}


///
/// Envoie un int
///
void sendMessage(int id_from, int id_cmd, int i)
{
	sendHeader(id_from, id_cmd);
	Serial.println(i);
}

///
/// Envoie un tableau d'int
///
void sendMessage(int id_from, int id_cmd, int *tabi, int size)
{
	sendHeader(id_from, id_cmd);
	for (int i=0; i<size-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP_SEND);
	}
	Serial.println(tabi[size-1]);
}

///
/// Envoie un string
///
void sendMessage(int id_from, int id_cmd, char* str)
{
	sendHeader(id_from, id_cmd);
	Serial.println(str);
}

///
/// Envoie des strings et des int
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(int id_from, int id_cmd, char** tabs, int nbStr, int *tabi, int nbInt)
{
	sendHeader(id_from, id_cmd);
	
	for (int i=0; i<nbStr; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(C_SEP_SEND);
	}
	
	for (int i=0; i<nbInt-1; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP_SEND);
	}
	Serial.println(tabi[nbInt-1]);
}

///
/// Envoie des int et des strings
/// aucune protection, il faut au moins envoyer une chaine et un int
///
void sendMessage(int id_from, int id_cmd, unsigned char cmd, int* tabi, int nbInt, char** tabs, int nbStr)
{
	sendHeader(id_from, id_cmd);
	
	for (int i=0; i<nbInt; ++i)
	{
		Serial.print(tabi[i]);
		Serial.print(C_SEP_SEND);
	}
	
	for (int i=0; i<nbStr-1; ++i)
	{
		Serial.print(tabs[i]);
		Serial.print(C_SEP_SEND);
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
			case C_SEP_SEND:
			{
               	currentArg[currentArgIndex] = '\0';
       			args[argsIndex] = atoi(currentArg);
                argsIndex++;
        		currentArgIndex = 0;
        		break;
			}
			// fin de trame
			case '\n':
			{
                currentArg[currentArgIndex] = '\0';
        		args[argsIndex] = atoi(currentArg);
				cmd(args[0],args[1],args+2,argsIndex-1); // from, id_cmd, *args, sizeArgs
  				argsIndex = 0;
				currentArgIndex = 0;
				break;
			}
			default:
			{
				currentArg[currentArgIndex] = data;    
				currentArgIndex++;
			    break;
			}
		}
	}
}


