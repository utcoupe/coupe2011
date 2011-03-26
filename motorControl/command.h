#ifndef COMMAND_H_
#define COMMAND_H_

/* extrait de protocole.txt

    0 (identification)      --> 'asserv'
    1 (Ping)                --> 'Pong'
    2 x y (Goal en Absolu)  --> '1'      --> '1' ou '0' (1: but atteind, 0: annulation) suivi de x y a
    3 x y (Goal en relatif) -->  '1'     --> '1' ou '0' (1: but atteind, 0: annulation) suivi de x y a
    4 a (Angle en Absolu)   --> '1'      --> '1' ou '0' (1: but atteind, 0: annulation) suivi de x y a
    5 a (Angle en relatif)  --> '1'      --> '1' ou '0' (1: but atteind, 0: annulation) suivi de x y a
    6 (position)            --> 'x y a'
    7 (recalage)            --> '1'      --> '1' ou '0' (1: but atteind, 0: annulation) suivi de x y a

 */



#define Q_IDENT 			0
#define Q_PING 				1
#define Q_GOAL_ABS 			2
#define Q_GOAL_REL 			3
#define Q_ANGLE_ABS 		4
#define Q_ANGLE_REL 		5
#define Q_POSITION 			6
#define Q_AUTO_CALIB		7
#define Q_MANUAL_CALIB		8
#define Q_DELAY 			9
#define Q_PWM 				10
#define Q_MODIF_GOAL_ABS 	11
#define Q_MODIF_GOAL_REL 	12
#define Q_STOP 				13
#define Q_PAUSE				14
#define Q_RESUME			15


#define Q_DEBUG				99


void cmd(int,int,int*);

#endif /* COMMAND_H_ */
