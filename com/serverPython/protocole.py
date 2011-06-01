# -*- coding: utf-8 -*-


C_SEP_SEND = '.'	# séparation entre les données de la commande à envoyer aux cartes

class KillException(Exception):
	def __init__(self, msg = ""):
		Exception.__init__(self, "KillException : %s"%msg)

class TimeoutException(Exception):
	def __init__(self, msg = ""):
		Exception.__init__(self, "Timeout : %s"%msg)



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
Q_PWM 				=	10
Q_STOP				=	13
Q_PAUSE				=	14
Q_RESUME			=	15
Q_GETSENS			=	16

# others
Q_SHARP				= 31
Q_TMS				= 34
Q_SETPOSITION		= 36
Q_JACK				= 38
Q_LED				= 39
Q_PION				= 40
Q_PRECAL			= 46
Q_COLOR				= 50
Q_ULTRAPING			= 51



# cam
Q_SCAN_AV		= 62
Q_SCAN_AR		= 63


# ax12
Q_OPEN_MAX		= 72
Q_CLOSE			= 73
Q_SERRE			= 74

# phone
Q_SCAN_DEPART	= 80 # smartphone
Q_SCAN_DEPART_S	= 81 # simulation
Q_SCAN_PIONS	= 82 # smartphone
Q_SCAN_PIONS_S	= 83 # simulation

# server python
ID_SERVER		= 0
ID_CAM			= 1
ID_AX12			= 2
ID_ASSERV		= 3 # carte asserv
ID_OTHERS		= 4 # carte avec les pinces et sharps
ID_PHONE		= 5 # la visio du smartphone
ID_IA			= 6 # l'IA




UNKNOWN	= -1
BLUE	= 0
RED		= 1
GREEN	= 2

AVANT	= 0
ARRIERE	= 1

HAUT	= 1
BAS		= 2
MIDLE	= 3

E_INVALID_CMD						= -1
E_INVALID_PARAMETERS_NUMBERS		= -2
E_INVALID_PARAMETERS_TYPE			= -3
E_INVALID_PARAMETERS_VALUE			= -4
Q_KILL								= -69
E_TIMEOUT							= -43
E_BLOCK								= -44
W_PING_AV							= -30
W_PING_AR							= -31
W_SWITCH_COLOR						= -21
W_JACK								= -22
E_SURCHAUFFE						= -55
E_LOW_VOLTAGE						= -56
E_MERDE_AX12						= -57
E_CM5_NOT_CONN						= -58
W_MS								= -60



cmd_name = {
		Q_IDENT: "IDENTIFICATION",
		PING: "PING",
		Q_GOAL_ABS: "Q_GOAL_ABS",
		Q_GOAL_REL: "Q_GOAL_REL",
		Q_ANGLE_ABS: "Q_ANGLE_ABS",
		Q_ANGLE_REL: "Q_ANGLE_REL",
		Q_POSITION: "Q_POSITION",
		Q_AUTO_CALIB: "Q_AUTO_CALIB",
		Q_MANUAL_CALIB: "Q_MANUAL_CALIB",
		Q_STOP: "Q_STOP",
		Q_PAUSE: "Q_PAUSE",
		Q_RESUME: "Q_RESUME",
		Q_GETSENS: "Q_GETSENS",
		Q_SHARP: "Q_SHARP",
		Q_TMS: "Q_TMS",
		Q_SETPOSITION: "Q_SETPOSITION",
		Q_JACK: "Q_JACK",
		Q_LED: "Q_LED",
		Q_PION: "Q_PION",
		Q_PRECAL: "Q_PRECAL",
		Q_COLOR: "Q_COLOR",
		Q_ULTRAPING: "Q_ULTRAPING",
		Q_SCAN_AV: "Q_SCAN_AV",
		Q_SCAN_AR: "Q_SCAN_AR",
		Q_OPEN_MAX: "Q_OPEN_MAX",
		Q_CLOSE: "Q_CLOSE",
		Q_SERRE: "Q_SERRE",
		Q_KILL: "Q_KILL",
	}


def get_cname(id_cmd):
	if id_cmd in cmd_name:
		return cmd_name[id_cmd]
	else:
		return id_cmd
