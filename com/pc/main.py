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
import math


from server import *
from timer import *



scan = scanPorts()
if not scan:
	print "aucun port connecté..."
	exit()
	
	
ports = []
for s in scan:
	ports.append((s, 115200))
#ports.append(("../exe",None))
ports.append((["./ObjectCamera","-b","-m","n"], None))


server = Server(ports)


#
# début d'IA
#

def searchTarget():
	print "demande à la camera..."
	event = server.addCmd("1", "camb")
	event.wait(1)
	r = server.getReponse("1", "camb")
	print "Reponse :", r
	print "demande de position à l'asserv"
	event = server.addCmd("k","asserv")
	event.wait(1)
	r = server.getReponse("k", "asserv")
	print "Reponse :", r
	xr,yr,ar = [ float(f) for f in r.split() ]
	targets = traiterReponseCamera(r)
	if not targets:
		print "rien trouvé"
	else:
		i = 0
		target = targets[i]
		while True:
			type,xt,yt = target
			Xt,Yt = absolute(xt,yt,xr,yr,ar)
			couleur = getColor(Xt,Yt)
			print "target on %i %i"%(Xt,Yt)
			print "color : %s"%couleur
			if inMap(xt,yt) and couleur == "rouge":
				print "on la prend !"
				return target
			i += 1
			if i < len(targets):
				target = targets[i]
			else:
				print "pas de cible potable"
				return None
		
	

def approchTarget(t):
	pass
	
def rotate(x,y,a):
	a = math.radians(a)
	cosa = math.cos(a)
	sina = math.sin(a)
	X = cosa*x - sina*y
	Y = sina*x + cosa*y
	return X,Y

def absolute(x,y,dx,dy,a):
	X,Y = rotate(x,y,a)
	return X+dx,Y+dy

def getColor(x,y):
	if x < 450:
		return 'vert'
	if x > 2550:
		return 'vert'
	if y%700 > 350:
		if (x-450)%700  > 350:
			return 'rouge'
		else:
			return 'bleu'
	else:
		if (x-450)%700  > 350:
			return 'bleu'
		else:
			return 'rouge'
		
def inMap(x,y):
	if x < 0 or y < 0 or x > 3000 or y > 2100:
		return False
	else:
		return True

print "recalage et avance..."
# se recaler
event = server.addCmd("x", "asserv")
event.wait(1)
print "Reponse :", server.getReponse("x", "asserv")
# avancer d'1m
event = server.addCmd("G 1500 350", "asserv")
event.wait(1)
print "Reponse :", server.getReponse("G 1500 350", "asserv")
print "on attend ~10 secondes"
for i in range(10):
	event = server.addCmd("k","asserv")
	event.wait(1)
	print "Reponse :", server.getReponse("k", "asserv")
	time.sleep(1)

# scan de la carte
target = searchTarget()
while not target:
	event = server.addCmd("a 60", "asserv")
	event.wait(1)
	print "Reponse :", server.getReponse("x", "asserv")
	time.sleep(1)
	target = searchTarget()

approchTarget(target)
	

	
server.stop()



"""
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

"""
print 'fin thread principal'





