/*
  Petit test !
 */

#include "message.h"




void setup()
{
 	initSerialLink(); // Initialisation de la lisaison serie (voir message.cpp)
	pinMode(12, OUTPUT); // Rouge   
	pinMode(13, OUTPUT); // Vert
}

void loop()
{
	readIncomingData();
}




