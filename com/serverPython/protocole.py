# -*- coding: utf-8 -*-


C_SEP_SEND = ':'	# séparation entre les données de la commande à envoyer aux cartes

C_SEP1	=	'|'		# séparation entre l'id de la commande et la commande
C_SEP2	=	'/'		# séparation entre les différentes données renvoyées par la 
C_SEP3	=	'_'		# séparation de niveau supérieur pour renvoyer des tableaux 2D

Q_IDENT			=	0
PING			=	1

# autre
SHARP			=	2

# asserv
GOAL_A			=	2
GOAL_R			=	3
ANGLE_A			=	4
ANGLE_R			=	5
Q_POSITION		= 	6
Q_AUTO_RECAL	=	7

# server python
ID_SERVER		= 0
ID_ASSERV		= 1 # carte asserv
ID_OTHERS       = 2 # carte avec les pinces et sharps
ID_IA           = 3 # l'IA
