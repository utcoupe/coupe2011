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



scan = scanPorts()
if not scan:
	print "aucun port connecté..."
	exit()
	
	
ports = []
for s in scan:
	ports.append((s, 115200))
ports.append(("../exe",None))


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
	
	cmd_split = cmd.split(' ',1) # <carte><cmd>
	
	
	if not cmd:
		print 'P'
		cmd = 'P'
		cmd_split = ['P']
	if cmd_split[0] == 'test':
		server.testPing(port, cmd_split[1])
	elif cmd_split[0] == 'track':
		it = InteruptableThreadedLoop(None, 'tracking')
		it.start(loopTestTracking)
		c = raw_input()
		it.stop()
		print 'arret du thread tracking en cours...'
	elif cmd_split[0] == 'exit':
		server.stop()
		break
	elif cmd_split[0] == 'loop':
		cmd_split2 = raw_input().split(' ',1)
		server.makeLoop(cmd_split2[0], cmd_split2[1], cmd_split[1])
	elif cmd_split[0] == 'stop':
		server.stopScreen(int(cmd_split[1]))
	else:
		event = server.addCmd(cmd_split[1], cmd_split[0])
		event.wait(1)
		r = server.getReponse(cmd_split[1], cmd_split[0])
		print "Reponse :", r


print 'fin thread pcincipal'





