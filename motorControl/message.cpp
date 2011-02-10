#include "WProgram.h"
#include "message.h"
#include "command.h"

void initSerialLink(){
	Serial.begin(SERIAL_BAUD);
}


///
/// Envoie un int
///
void sendMessage(int id, int i)
{
	Serial.print(id);
	Serial.print('|');
	Serial.println(i);
}

///
/// Envoie un tableau de int
///
void sendMessage(int id, int size, int* tab )
{
	Serial.print(id);
	Serial.print('|');
	for(int i=0;i<size-1;i++){
		Serial.print(tab[i]);
		Serial.print(':');
	}
	Serial.println(tab[size-1]);
}

///
/// Envoie une chaine
///
void sendMessage(int id, char* str)
{
	Serial.print(id);
	Serial.print("|");
	Serial.println(str);
}


///
/// Parse les donnees recues sur le port serie et appel la fonction cmd pour effectuer les traitements
///
void readIncomingData()
{
	static char currentArg[10];
	static int args[10];
	static int argsIndex = 0;
	static int currentArgIndex = 0;

	/*
	A propos du protocole :
	- un message se termine par '\n'
	*/

	/*s'il y a des donnees a lire*/
	int available = Serial.available();
	for(int i = 0; i < available; i++){
		/*recuperer l'octet courant*/
		int data = Serial.read();
		switch(data){
			/*separateur*/
			case ':': {
               	currentArg[currentArgIndex] = '\0';
       			args[argsIndex] = atoi(currentArg);
                argsIndex++;
        		currentArgIndex = 0;
        		break;
			}
			/*fin de trame*/
			case '\n': {
                currentArg[currentArgIndex] = '\0';
        		args[argsIndex] = atoi(currentArg);
  				argsIndex = 0;
				currentArgIndex = 0;
				cmd(args[0],args[1],args+2);
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

