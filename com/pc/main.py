#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
	commandes funky:
		loop <time> puis <cmd>: aficher une nouvelle fenetre avec en boucle un envoi/reception de la commande spécifiée
		stop <n>: killer le nième écran
		test <cmd>: lance 300 fois la commande et affiche le temps d'execution
		cam <num>: Recuperer les données de la nieme cam
		exit: quitter
"""


import threading
import sys

from server import *
from timer import *



scan = scanPorts("ttyACM") # présent aussi dans gestionnaireerreur.py
if not scan:
	print "aucun port connecté..."
	exit()
	
	
ports = []
for s in scan:
	ports.append((s, 115200))

server = Server(ports)
port = "autre"

# exemple interaction automatique camera
# le robot fait une demande à la camera
# puis il demande à l'asserv d'aller sur la position de ce pion
def loopTestTracking():
	global arreter
	# demander la liste à la camera
	listeObjets = []
	while not listeObjets:
		print "envoie à la cam..."
		server.sendToCam(1)
		print "récupération des valeures..."
		reponseCamera = server.listenCam()
		# on format la réponse pour avoir une liste
		listeObjets = traiterReponseCamera(reponseCamera)
		if not listeObjets:
			print "rien trouvé, on retente dans 1 seconde"
			cmd = "a 60"
        		server.addCmd(cmd, port)
			time.sleep(0.3)
			server.addCmd("r", port)
			
	# on récupère le premier pion de la liste
	type, x, y = listeObjets[0]
	# on va au premier pour le pousser
	print "on va vers %s %s"%(x,y)
	cmd = "m %s %s 100"%(x,y)
	#server.addCmd("g %s %s 100"%(x,y), 'ACM0')
	
	server.addCmd(cmd, port)
	t = threading.Timer(1,stop)
	t.start()
	r = server.getRcv(cmd, port)
	while not r and not arreter:
		r = server.getRcv(cmd, port)
	t.cancel()
	arreter = False
	print r
	
	#time.sleep(1)



# read, send and get output of a command
while True:
	cmd = raw_input()
	
	cperso = cmd.split()
	
	if not cmd:
		print 'P'
		cmd = 'P'
		cperso = ['P']
	if cperso[0] == 'test':
		server.testPing(port, cperso[1])
	elif cperso[0] == 'cam':
		server.sendToCam(cperso[1])
		r = server.listenCam()
		if r > 0:
			list = traiterReponseCamera(r)
			print list
	elif cperso[0] == 'track':
		it = InteruptableThreadedLoop(None, 'tracking')
		it.start(loopTestTracking)
		c = raw_input()
		it.stop()
		print 'arret du thread tracking en cours...'
	elif cperso[0] == 'exit':
		server.stop()
		break
	elif cperso[0] == 'bin': # poubelle
		server.bin(port)
	elif cperso[0] == 'loop':
		cmd = raw_input()
		server.makeLoop(port, cmd, cperso[1])
	elif cperso[0] == 'stop':
		server.stopScreen(int(cperso[1]))
	else:
		event = server.addCmd(cmd, port)
		event.wait(1)
		r = server.getReponse(cmd,port)
		print "Reponse :", r


print 'fin thread pcincipal'





