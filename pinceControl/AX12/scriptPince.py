# -*- coding: utf-8 -*-
import serial, time
from protocole import *

'''
    --- PROTOCOLE ---
'''

MERDE = 0
Retour_OK = 1
Surchauffe = 2
LowVoltage = 3

Demande_ouverture_max_avant = 0
Demande_ouverture_max_arrie = 1

Demande_ouverture_min_avant = 6
Demande_ouverture_min_arrie = 7

Demande_fermeture_max_avant = 2
Demande_fermeture_max_arrie = 3

Demande_serrage_avant = 4
Demande_serrage_arrie = 5

def send(idMess, retourMess):
    print '%s.%s'%(idMess,retourMess)

'''
    --- PARAMETRES internes ---
    
Positions des AX12 sur le robot
    Face_front 
    1 -- 2
    |    |
    3 -- 4
    Face_back 
'''

Face_front = 0
Face_back = 1
 
Pinces_ouvertureMax = 0 # pour rechercher un piont
Pinces_ouvertureMin = 1 # pour lancher un piont
Pinces_fermetureMax = 2 # au debut du match
Pinces_fermetureMin = 3 # saisir un piont

CM5_actionOK = 50       # valeur de retour quand l'action est fini
''' --------------------- '''

# Initialisation de la liaison serie
ser = serial.Serial()
ser.baudrate = 57600
ser.port = '/dev/ttyUSB0' #3                  # A modifier suivant l'OS
ser.timeout = 0.5
ser.close()

# Fonction de demande d'ouverture
def action_pince(face, action):
    ser.write(face)
    ser.write(action)

# Fonction de lecture des informations veant du CM-5
def read_CM5():
    timeMax = 5
    timeOutPerso = timeMax
    while timeOutPerso > 0:
        text = ser.readline()
        if text:
            return text
            timeOutPerso=timeMax
        timeOutPerso-=1
    
print 'Debut programme : com CM-5'
keyB = 0
ser.open()

while (keyB != 13):
    valRetour = 0

    commandThomas = raw_input()
    trucQuiSertPas, idArenvoyer, keyB = commandThomas.split('.',3) 

    keyB = int(keyB)
    
    if (keyB==Demande_ouverture_max_avant):
        action_pince(Face_front, Pinces_ouvertureMax)
    elif (keyB==Demande_ouverture_max_arrie):
        action_pince(Face_back, Pinces_ouvertureMax)  

    elif (keyB==Demande_ouverture_min_avant):
        action_pince(Face_front, Pinces_ouvertureMin)
    elif (keyB==Demande_ouverture_min_arrie):
        action_pince(Face_back, Pinces_ouvertureMin)

    elif (keyB==Demande_serrage_avant):
        action_pince(Face_front, Pinces_fermetureMin)
    elif (keyB==Demande_serrage_arrie):
        action_pince(Face_back, Pinces_fermetureMin)

    elif (keyB==Demande_fermeture_max_avant):
        action_pince(Face_front, Pinces_fermetureMax)
    elif (keyB==Demande_fermeture_max_arrie):
        action_pince(Face_back, Pinces_fermetureMax)

    valRetour=read_CM5()
    try:
        valRetour = int(valRetour)
        if(valRetour==0):
            send(idArenvoyer, Retour_OK)
        if(valRetour==1):
            send(idArenvoyer, Retour_OK)
        if(valRetour==2):
            send(idArenvoyer, Surchauffe)
    except TypeError:
        send(idArenvoyer, MERDE)
    except ValueError:
        send(idArenvoyer, MERDE)

ser.close()
print 'Fin du programme : com CM-5'
