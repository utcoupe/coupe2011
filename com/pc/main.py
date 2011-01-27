#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
	commandes funky:
		loop <time> puis <cmd>: aficher une nouvelle fenetre avec en boucle un envoi/reception de la commande spécifiée
		stop <n>: killer le nième écran
		test <cmd>: lance 300 fois la commande et affiche le temps d'execution
		exit: quitter
"""


import threading
import sys

from message import *
from timer import *


port = 'ACM0'
ports = []
ports.append((port,115200))
#ports.append(('ACM1',115200))

server = Server(ports)
'''
class Ordre(self):
	def __init__(self, cmd, fonction, isAliveCmd):
		self.cmd = cmd
		self.fonction = fonction
		self.timeSinceLastReponse = 0
	
	""" regarde si il y a un retour """
	def check(self):
		server.getRcv, (cmd, 'ACM0'
		

class IA():
	def __init__(self):
		self.cmdRunning = dict() # les commandes en cours
		self.errors		= dict() # les erreurs reçues
	
	def allerA(self):
	
'''


arreter = False
def stop():
	global arreter
	print 'timeout'
	arreter = True
	
	
"""
# read, send and get output of a command
def loopCmd(stopCmd):
	global arreter
	print 'mainLoop'
	cmd = raw_input()
	
	cperso = cmd.split()
	try:
		if not cmd:
			print 'P'
			cmd = 'P'
			cperso = ['P']
		if cperso[0] == 'test':
			server.testPing(port, cperso[1])
		elif cperso[0] == 'cam':
			print 'ok'
			server.sendToCam(cperso[1])
			r = server.listenCam()
			if r > 0:
				list = traiterReponseCamera(r)
				print list
		elif cperso[0] == 'exit':
			stopCmd()
			server.stop()
		elif cperso[0] == 'bin': # poubelle
			server.bin(port)
		elif cperso[0] == 'loop':
			cmd = raw_input()
			server.makeLoop(port, cmd, cperso[1])
		elif cperso[0] == 'stop':
			server.stopScreen(int(cperso[1]))
		else:
			server.addCmd(cmd, port)
			t = threading.Timer(1,stop)
			t.start()
			r = server.getRcv(cmd, port)
			while not r and not arreter:
				r = server.getRcv(cmd, port)
			t.cancel()
			arreter = False
			print r
	except Exception as ex:
		print ex
		print sys.exc_info()
"""
def loopCmd(stopCmd):
	global arreter
	# demander la liste à la camera
	print "envoie à la cam..."
	server.sendToCam(1)
	print "récupération des valeures..."
	reponseCamera = server.listenCam()
	listeObjets = traiterReponseCamera(reponseCamera)
	# on récupère le premier pion de la liste
	type, x, y = listeObjets[0]
	# on va au premier pour le pousser
	print "on va vers %s %s"%(x,y)
	cmd = "g %s %s"%(x,y)
	#server.addCmd("g %s %s"%(x,y), 'ACM0')
	
	server.addCmd(cmd, port)
	t = threading.Timer(1,stop)
	t.start()
	r = server.getRcv(cmd, port)
	while not r and not arreter:
		r = server.getRcv(cmd, port)
	t.cancel()
	arreter = False
	print r
	
	time.sleep(10)


def makeLoop(target, args= [], kwargs={}):
	while True:
		target(*args, **kwargs)


loop = InteruptableThreadedLoop(None, "loop principale")
loop.start(loopCmd, (loop.stop,))

while loop.isAlive():
	time.sleep(3)

print 'fin thread pcincipal'





