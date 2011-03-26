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
import socket


from server import *
from timer import *



scan = scanPorts()
	
ports = []
for s in scan:
	ports.append((s, 115200))
#ports.append(("../exe",None))
#ports.append((["./ObjectCamera","-b","-m","n"], None))
ports.append((["python","../../emulation/emulation.py","asserv"],None))

if not ports:
	print "rien à connecter..."
	exit()
	

server = Server(ports)


HOST = ''		# Symbolic name meaning all available interfaces
PORT = 50000	# Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(1)
while 1:
	conn, addr = s.accept()
	print 'Connected by', addr
	while 1:
		cmd = conn.recv(1024) # en octets/chars
		if not cmd:
			print "not cmd"
			break
		else:
			cmd_split = cmd.split(' ',1) # <carte><cmd>
			try:
				socket_rep = []
				if cmd_split[0] == 'wait':
					r = reponse.read(int(cmd_split[1]), 1)
					print "Reponse :", r
					socket_rep.append("Reponse : %s"%r)
				elif cmd_split[0] == 'test':
					id_client,cmd = cmd_split[1].split(' ',1)
					server.testPing(id_client, cmd)
				elif cmd_split[0] == 'track':
					searchTarget()
				elif cmd_split[0] == 'shutdown':
					server.stop()
					socket_rep.append('close')
				elif cmd_split[0] == 'close':
					socket_rep.append('close')
				elif cmd_split[0] == 'loop':
					cmd_split2 = raw_input().split(' ',1)
					server.makeLoop(cmd_split2[0], cmd_split2[1], cmd_split[1])
				elif cmd_split[0] == 'stop':
					server.stopScreen(int(cmd_split[1]))
				else:
					reponse = server.addCmd(cmd_split[1], cmd_split[0])
					r = reponse.read(0,1)
					socket_rep.append("Reponse : %s"%r)
					print "Reponse :", r
			except IndexError as ex:
				print "mauvaise commande, IndexError : %s"%ex
				socket_rep.append("mauvaise commande, IndexError : %s"%ex)
			except KeyError as ex:
				print "mauvaise commande, KeyError : %s"%ex
			
			for r in socket_rep:
				conn.send(r)
			if cmd_split[0] == 'shutdown': break
	conn.close()
	if cmd_split[0] == 'shutdown': break


#
# début d'IA
#
"""
def searchTarget():
	print "demande à la camera..."
	event = server.addCmd("1", "camb")
	event.wait(1)
	cmd = raw_input()
	
	if cmd:
		cmd_split = cmd.split(' ',1) # <carte><cmd>

		try:
			if cmd_split[0] == 'wait':
				r = reponse.read(int(cmd_split[1]), 1)
				print "Reponse :", r
			elif cmd_split[0] == 'test':
				id_client,cmd = cmd_split[1].split(' ',1)
				server.testPing(id_client, cmd)
			elif cmd_split[0] == 'track':
				searchTarget()
			elif cmd_split[0] == 'exit':
				server.stop()
				break
			elif cmd_split[0] == 'loop':
				cmd_split2 = raw_input().split(' ',1)
				server.makeLoop(cmd_split2[0], cmd_split2[1], cmd_split[1])
			elif cmd_split[0] == 'stop':
				server.stopScreen(int(cmd_split[1]))
			else:
				reponse = server.addCmd(cmd_split[1], cmd_split[0])
				r = reponse.read(0,1)
				print "Reponse :", r
		except IndexError as ex:
			print "mauvaise commande, IndexError : %s"%ex
		except KeyError as ex:
			print "mauvaise commande, KeyError : %s"%ex
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
			if inMap(xt,yt):
				if couleur == "rouge":
					print "on la prend !"
					return target
				else:
					print "pas la bonne couleur..."
			else:
				print "hors map..."
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
		return True"""
"""
print "recalage et avance..."
# se recaler
reponse = server.addCmd("x", "asserv")
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
	print "Reponse :", server.getReponse("a", "asserv")
	time.sleep(1)
	target = searchTarget()

approchTarget(target)
	

	
server.stop()
"""
"""


# read, send and get output of a command
while True:
	cmd = raw_input()
	
	if cmd:
		cmd_split = cmd.split(' ',1) # <carte><cmd>

		try:
			if cmd_split[0] == 'wait':
				r = reponse.read(int(cmd_split[1]), 1)
				print "Reponse :", r
			elif cmd_split[0] == 'test':
				id_client,cmd = cmd_split[1].split(' ',1)
				server.testPing(id_client, cmd)
			elif cmd_split[0] == 'track':
				searchTarget()
			elif cmd_split[0] == 'exit':
				server.stop()
				break
			elif cmd_split[0] == 'loop':
				cmd_split2 = raw_input().split(' ',1)
				server.makeLoop(cmd_split2[0], cmd_split2[1], cmd_split[1])
			elif cmd_split[0] == 'stop':
				server.stopScreen(int(cmd_split[1]))
			else:
				reponse = server.addCmd(cmd_split[1], cmd_split[0])
				r = reponse.read(0,1)
				print "Reponse :", r
		except IndexError as ex:
			print "mauvaise commande, IndexError : %s"%ex
		except KeyError as ex:
			print "mauvaise commande, KeyError : %s"%ex
"""

print 'fin thread principal'





