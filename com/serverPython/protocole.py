# -*- coding: utf-8 -*-


C_SEP_SEND = '.'	# séparation entre les données de la commande à envoyer aux cartes

class KillException(Exception):
	pass

class TimeoutException(Exception):
	pass


Q_IDENT			=	0
PING			=	1

# asserv
Q_GOAL_ABS			=	2
Q_GOAL_REL			=	3
Q_ANGLE_ABS			=	4
Q_ANGLE_REL			=   5
Q_POSITION			= 	6
Q_AUTO_CALIB		=	7
Q_MANUAL_CALIB		=	8
Q_STOP				=	13
Q_PAUSE				=	14
Q_RESUME			=	15
Q_GETSENS			=	16

# others
Q_SHARP				= 31
Q_PINCE				= 35
Q_SETPOSITION		= 36
Q_JACK				= 38
Q_LED				= 39
Q_PION				= 40
Q_GETPOSITION		= 45
Q_PRECAL			= 46
Q_COLOR				= 50
Q_ULTRAPING			= 51

# cam
Q_SCAN_AV		= 62
Q_SCAN_AR		= 63


# server python
ID_SERVER		= 0
ID_CAM			= 1
ID_ASSERV		= 3 # carte asserv
ID_OTHERS		= 2 # carte avec les pinces et sharps
ID_IA			= 4 # l'IA




UNKNOWN	= -1
BLUE	= 0
RED		= 1
GREEN	= 2

AVANT	= 0
ARRIERE	= 1

Q_KILL								= -42
E_INVALID_CMD						= -1
E_INVALID_PARAMETERS_NUMBERS		= -2
W_PING_AV							= -30
W_PING_AR							= -31
W_SWITCH_COLOR						= -21
W_JACK								= -22
