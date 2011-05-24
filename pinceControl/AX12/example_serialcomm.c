#include "../../include/libCM-5.h"

/* Disposition des AX12
 
 1 -- 2
 |    |
 |    |
 3 -- 4

*/

#define LIMIT_MAX_TEMP  60
#define PAS_SERRAGE		50

// Messages et alarme
#define TACHE_OK		1
#define SURCHAUFFE     	2
#define LOW_VOLTAGE 	3

// Entiérement ouvert
#define OPEN_MAX_GAUCHE 624
#define OPEN_MAX_DROITE 424

// Replié
#define CLOSE_MAX_GAUCHE 276
#define CLOSE_MAX_DROITE 741

// Serré
#define CLOSE_MIN_GAUCHE 1024-524	
#define CLOSE_MIN_DROITE 1024-524	

// Temps d'attente en milli
#define WAIT_AFTER_ORDER 10
#define WAIT_PAS		 400
#define WAIT_FOR_CLOSE 	 100
#define CHARGE_MAX		 1400

// -----------
#define AX_1 1 
#define AX_2 2 
#define AX_3 3
#define AX_4 4

#define FRONT 0
#define BACK  1

#define CHOIX_FACE 0

#define ACTION_OUV_MAX 0
#define ACTION_OUV_MIN 1
#define ACTION_FER_MAX 2
#define ACTION_FER_MIN 3
#define ACTION_CHECK_AX12 4

// ------------
void reboot()
{
	char face = FRONT;
	char state = CHOIX_FACE;
	byte ax_gauche = AX_1;
	byte ax_droit = AX_2;
}

// ------------

int main(void)
{
	PortInitialize();								// Port Initialize
	TimerInitialize(TIMER0, 128, 1);				// Timer 0 Initialize
	SerialInitialize(SERIAL_PORT0,1,RX_INTERRUPT);					//485
	SerialInitialize(SERIAL_PORT1,DEFAULT_BAUD_RATE,RX_INTERRUPT); 	//232
	sei();											// Enable Interrupt
	
	byte bKeyin;
	byte action;
	char face = FRONT;
	char state = CHOIX_FACE;
	byte ax_gauche = AX_1;
	byte ax_droit = AX_2;
	
	word posG = 512;
	word posD = 512;
	
	//TxDString("\r\n UTCoupe CM-5 Control \r\n");
	
	while(1)
	{
		// Recuperation de la commande
		bKeyin = RxD8Interrupt();
		bKeyin = (int)bKeyin-48;
		
			if(state==CHOIX_FACE)
			{
					if(bKeyin==FRONT || bKeyin==BACK)
					{
						face=bKeyin;
						state = (state+1)%2;
					}
					else
					{
						reboot();
					}
			}
			else
			{
				// Selection des AX12 en fonction de la face
				action = bKeyin;
				if(face == FRONT){ 	ax_gauche = AX_1; 	ax_droit = AX_2;
				}else{				ax_gauche = AX_4;	ax_droit = AX_3; 	}
				
				switch (action){
					
					// Ouverture maximale
					case ACTION_OUV_MAX:{
						WriteWord( ax_gauche, P_GOAL_POSITION_L, OPEN_MAX_GAUCHE);
						MiliSec(WAIT_FOR_CLOSE);
						WriteWord( ax_droit , P_GOAL_POSITION_L, OPEN_MAX_DROITE);
						MiliSec(WAIT_AFTER_ORDER);
						while( (ReadWord(ax_gauche, P_MOVING) && (ReadWord(ax_droit, P_MOVING) )))
						{
							MiliSec(WAIT_AFTER_ORDER);
						}
						TxDWord10(TACHE_OK);
					break;}
					
					// Fermeture maximale ou repliement
					case ACTION_FER_MAX:{
						WriteWord( ax_gauche, P_GOAL_POSITION_L, CLOSE_MAX_GAUCHE);
						MiliSec(WAIT_FOR_CLOSE);
						WriteWord( ax_droit , P_GOAL_POSITION_L, CLOSE_MAX_DROITE);
						MiliSec(WAIT_AFTER_ORDER);
						while
						( 
							( ReadWord(ax_gauche, P_MOVING) && (ReadWord(ax_droit, P_MOVING)) )
						)
						{
							MiliSec(WAIT_AFTER_ORDER);
						}
						TxDWord10(TACHE_OK);
					break;}
					
					// SERRAGE
					case ACTION_FER_MIN:{
						WriteWord( ax_gauche, P_CW_COMPLIANCE_SLOPE, 2);
						WriteWord( ax_droit , P_CW_COMPLIANCE_SLOPE, 2);
						MiliSec(WAIT_AFTER_ORDER);
						WriteWord( ax_gauche, P_CCW_COMPLIANCE_SLOPE, 2);
						WriteWord( ax_droit , P_CCW_COMPLIANCE_SLOPE, 2);
						MiliSec(WAIT_AFTER_ORDER+200);
						WriteWord( ax_gauche, P_GOAL_POSITION_L, 482); //486
						WriteWord( ax_droit , P_GOAL_POSITION_L, 542); //538
						MiliSec(200);
						TxDWord10(TACHE_OK);
					break;}
					
					//
					default:{
						reboot();
					break;}					
				}
				state = (state+1)%2;
			}
			
			/*
			// TEST DE FIN ET VERIFICATION ALARM
			MiliSec(WAIT_AFTER_ORDER);
			if( ReadWord( AX_1, P_PRESENT_TEMPERATURE ) >= LIMIT_MAX_TEMP ){
				TxDWord10(SURCHAUFFE);
				ALARMA = 1;
			}
			else if( ReadWord( AX_2, P_PRESENT_TEMPERATURE ) >= LIMIT_MAX_TEMP ){
				TxDWord10(SURCHAUFFE);
				ALARMA = 1;
			}
			else if( ReadWord( AX_3, P_PRESENT_TEMPERATURE ) >= LIMIT_MAX_TEMP ){
				TxDWord10(SURCHAUFFE);
				ALARMA = 1;
			}
			else if( ReadWord( AX_4, P_PRESENT_TEMPERATURE ) >= LIMIT_MAX_TEMP ){
				TxDWord10(SURCHAUFFE);
				ALARMA = 1;
			}
			* */
			/*
			else if( ReadWord( AX_4, P_PRESENT_VOLTAGE ) > LIMIT_MAX_TEMP ){
				TxDWord10(SURCHAUFFE);
				ALARMA = 1;
			}*/
			
	}
	
	return 0;
}
