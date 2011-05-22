#!/usr/bin/env python
# -*- coding: utf-8 -*-
import serial, time, sys
from protocole import *


def send(idMess, retourMess):
	print '-1.%s.%s'%(idMess,retourMess)
	sys.stdout.flush()

'''
	--- PARAMETRES internes ---
	
Positions des AX12 sur le robot
	Face_front 
	1 -- 2
	|	|
	3 -- 4
	Face_back 
'''

Face_front = 0
Face_back = 1
 
Pinces_ouvertureMax = 0 # pour rechercher un pion
Pinces_ouvertureMin = 1 # pour lancher un pion
Pinces_fermetureMax = 2 # au debut du match
Pinces_fermetureMin = 3 # saisir un pion

CM5_actionOK = 50	   # valeur de retour quand l'action est fini
''' --------------------- '''

# Initialisation de la liaison serie
ser = serial.Serial()
ser.baudrate = 57600
ser.port = '/dev/ttyUSB0' #3				  # A modifier suivant l'OS
ser.timeout = 0.5
try:
	ser.close()
	ser.open()
except Exception as ex:
	sys.stderr.write(str(ex)+"\n")
	send(E_CM5_NOT_CONN,E_CM5_NOT_CONN)
	cm5_connected = False
else:
	cm5_connected = True

# Fonction de demande d'ouverture
def action_pince(face, action):
	if cm5_connected:
		ser.write(str(face))
		ser.write(str(action))

# Fonction de lecture des informations veant du CM-5
def read_CM5():
	if cm5_connected:
		timeMax = 5
		timeOutPerso = timeMax
		while timeOutPerso > 0:
			text = ser.readline()
			if text:
				return text
				timeOutPerso=timeMax
			timeOutPerso-=1
	else:
		return E_CM5_NOT_CONN
	
print 'Debut programme : com CM-5'
sys.stdout.flush()
keyB = 0

while (keyB != 13):
	valRetour = 0
	commandThomas = raw_input()
	input_split = commandThomas.split('.')
	trucQuiSertPas, idArenvoyer, keyB = 0,0,0
	if len(input_split) == 0:
		continue
	if len(input_split) >= 1:
		trucQuiSertPas = input_split[0]
	else:
		continue
	if len(input_split) >= 2:
		idArenvoyer = input_split[1]
	else:
		continue
	if len(input_split) >= 3:
		keyB = int(input_split[2])
	else:
		send(idArenvoyer, E_INVALID_PARAMETERS_NUMBERS)
		continue
	
	if keyB == Q_IDENT:
		send(idArenvoyer, "ax12")
		continue
	elif keyB == PING:
		send(idArenvoyer, "Pong")
		continue
		
	elif (keyB==Q_OPEN_MAX_AV):
		action_pince(Face_front, Pinces_ouvertureMax)
	elif (keyB==Q_OPEN_MAX_AR):
		action_pince(Face_back, Pinces_ouvertureMax)  

	elif (keyB==Q_OPEN_MIN_AV):
		action_pince(Face_front, Pinces_ouvertureMin)
	elif (keyB==Q_OPEN_MIN_AR):
		action_pince(Face_back, Pinces_ouvertureMin)

	elif (keyB==Q_SERRE_AV):
		action_pince(Face_front, Pinces_fermetureMin)
	elif (keyB==Q_SERRE_AR):
		action_pince(Face_back, Pinces_fermetureMin)

	elif (keyB==Q_CLOSE_AV):
		action_pince(Face_front, Pinces_fermetureMax)
	elif (keyB==Q_CLOSE_AR):
		action_pince(Face_back, Pinces_fermetureMax)

	valRetour=read_CM5()
	try:
		valRetour = int(valRetour)
		if(valRetour==0):
			send(idArenvoyer, 1)
		elif(valRetour==1):
			send(idArenvoyer, 1)
		elif(valRetour==2):
			send(idArenvoyer, E_SURCHAUFFE)
		else:
			send(idArenvoyer, valRetour)
	except TypeError:
		send(idArenvoyer, E_MERDE_AX12)
	except ValueError:
		send(idArenvoyer, E_MERDE_AX12)

ser.close()
print 'Fin du programme : com CM-5'
