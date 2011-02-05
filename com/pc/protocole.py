# -*- coding: utf-8 -*-


C_SEP_SEND = ':'	# séparation entre les données de la commande à envoyer aux cartes

C_SEP1	=	'|'		# séparation entre l'id de la commande et la commande
C_SEP2	=	'/'		# séparation entre les différentes données renvoyées par la 
C_SEP3	=	'_'		# séparation de niveau supérieur pour renvoyer des tableaux 2D

IDENTIFICATION	=	0
PING			=	1

# autre
SHARP			=	2

# asserv
GOAL_A			=	2
GOAL_R			=	3
ANGLE_A			=	4
ANGLE_R			=	5
POSITION		= 	6


CMD = {	'I': IDENTIFICATION,
		'P': PING,
		'S': SHARP,
		'G': GOAL_A,
		'g': GOAL_R,
		'A': ANGLE_A,
		'a': ANGLE_R,
		'P': POSITION
		}

