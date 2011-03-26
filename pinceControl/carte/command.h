#ifndef COMMAND_H_
#define COMMAND_H_

#include "WProgram.h"
#include "message.h"
#include "pinceControl.h"
#include "sensor.h"
#include "tourelle.h"

/*
	erreur = requette non prise en compte
	
    COMMANDES : CARTE SM,
	0 (identification) 		--> 'SM'
	1 (Ping)				--> 'Pong'
	12 n (Q_SHARP : retour sharp n)	
							--> 'valeur du sharp n'
	13 n s (Q_TRIGGERSHARP : surveiller que le sharp n ne dépasse le seuil s)			
							--> '1' (recu) ou '0' (recu mais erreur) --> '1' (seuil dépassé + arret seuil)
	14 n (Q_MS : retourne la valeur du MS avant (0) ou arriere(1) )	
	 						--> 'valeur du MS
	15 n s (Q_TRIGGERMS : message quand le microswitch n à atteint la valeur s)	
							--> '1' (recu) ou '0' (recu mais erreur) --> '1' (seuil atteint + arret seuil)
	16 n p (pince n (0 : AV / 1 : AR) etat p (fermé : 0 / sérré : 1 / ouvert : 2))
							--> '1' (recu) '0' (recu mais erreur)
	17 n p (moteur numero n va a la position p
	 						--> '1' (recu) '0' (recu mais erreur)	--> '1'(atteint)
 */ 

#define Q_PING 				1
#define Q_IDENT 			0
#define Q_SHARP 			12
#define Q_TRIGGERSHARP 		13
#define Q_MS	 			14
#define Q_TRIGGERMS	 		15
#define Q_PINCE 			16
#define Q_POSITION 			17


void cmd(int,int,int*);

#endif /* COMMAND_H_ */
