# -*- coding: utf-8 -*-


C_SEP_SEND = '.'	# séparation entre les données de la commande à envoyer aux cartes


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

# others
Q_WARNING			= 3
Q_PINCE				= 35
Q_SETPOSITION		= 36
Q_PION				= 40
Q_GETPOSITION		= 45
Q_PRECAL			= 46

#cam
Q_SCAN_AV		= 62
Q_SCAN_AR		= 63


# server python
ID_SERVER		= 0
ID_CAM			= 1
ID_ASSERV		= 3 # carte asserv
ID_OTHERS		= 2 # carte avec les pinces et sharps
ID_IA			= 4 # l'IA




BLUE	= 0
RED		= 1
