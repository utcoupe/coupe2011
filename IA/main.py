#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Il y a trois type de fonctions:
- calcul un calcul de l'IA, aucun échange avec le serveur n'est necessaire
- blockant une fonction communiquant avec le serveur et attendant des réponses
- non-blockant une fonction communiquant avec le serveur sans attendre de réponse

CheckList
- Vérifier timer
- Vérifier la vitesse
- Enlever tous les raw_input()
- passer en mode release
- relire préparation


"""
import os
import sys
ROOT_DIR  = os.path.split(os.path.split(os.path.abspath(__file__))[0])[0]
print ROOT_DIR

##############################
##		DEBUG/RELEASE		##
##############################
RELEASE		= 0
DEBUG		= 1
MOD			= DEBUG

##############################
##			IMPORTS			##
##############################
import time
import math
import threading
import copy
import traceback
sys.path.append(os.path.join(ROOT_DIR, "IA"))
from debugClient import *
from pathfinding import *
from geometry.vec import *
from geometry.circle import *
from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *
from case import *


MAX_MSG		= 10000
if MOD == RELEASE:
	VITESSE		= 200
	VITESSE_ROT	= 200
	CONN_MOD	= MOD_TCP
	DEBUG_LVL	= 0
	AGE_MAX		= 10 # si un pion n'a pas été vu par un scan au bout de AGE_MAX(s), c'est qu'il ne doit plus être sur la carte
else:
	VITESSE 	= 200
	VITESSE_ROT	= 200
	CONN_MOD	= MOD_TCP
	DEBUG_LVL	= 1
	AGE_MAX		= 10


# constantes de recalage
RECAL_X		= 3
RECAL_Y		= 4
RECAL_A		= 5

# constantes pour prendre les pions dans le vert
X_PRISE = 300
X_DEPLACEMENT = 700

# distance entre le centre du robot et le fond des pinces (mm)
D_CENTER_2_PINCE = 60

# rayon d'un pion (mm)
R_PION = 100

# delay avant d'activer les pings (s)
DELAY_PINGS		= 3

CASES = []
# initialisation des cases
for x in xrange(625,2500,350):
	CASES.append([Case(x,250)])
	for y in xrange(525, 1600,350):
		CASES[len(CASES)-1].append(Case(x,y))

				
class Robot:
	def __init__(self):
		"""
		Lorsque le robot envoie un message, il envoi aussi un id_msg compris entre 0 et MAX_MSG
		Lorsqu'il reçoit un message, il sait de qui il provient grâce au server,
		il retrouve ensuite à quelle commande il correspond grâce à l'id_msg. (opération effectuée dans RobotClient)
		"""

		
		self._lock_write = threading.Lock()
		self.e_validate_ident = threading.Event()
		self._e_stop = threading.Event() # pour arreter le robot
		self.client = RobotClient(self,CONN_MOD) # client pour communiquer avec le serveur
		self.debug = Debug(os.path.join(ROOT_DIR,"IA","debug","main.py"),DEBUG_LVL) # pour debugger
		
		if MOD == DEBUG:
			self.write("##############################", colorConsol.FAIL)
			self.write("##        MODE DEBUG        ##", colorConsol.FAIL)
			self.write("##############################", colorConsol.FAIL)
		elif MOD == RELEASE:
			self.write("##############################", colorConsol.FAIL)
			self.write("##       MODE RELEASE       ##", colorConsol.FAIL)
			self.write("##############################", colorConsol.FAIL)
		else:
			self.write("##############################", colorConsol.FAIL)
			self.write("##       UNKNOWN MODE       ##", colorConsol.FAIL)
			self.write("##############################", colorConsol.FAIL)
		self.write("VITESSE : %s"%VITESSE, colorConsol.OKBLUE)
		self.write("VITESSE ROT : %s"%VITESSE_ROT, colorConsol.OKBLUE)
		self.write("CONN_MOD : %s"%CONN_MOD, colorConsol.OKBLUE)
		self.write("DEBUG_LVL : %s"%DEBUG_LVL, colorConsol.OKBLUE)
		self.write("AGE_MAX : %s"%AGE_MAX, colorConsol.OKBLUE)
		
		self.pinces = (Pince(), Pince())
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self.pos = (0,0,0)
		self.color = RED
		self.time_begin = -1
		
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0
		self._lock_id_msg = threading.Lock()

		self.client.start() # demarrage du client
		self.activeReset = True
		
		
		threading.Thread(None, self._loopReset, "Robot._loopReset()").start()
		threading.Thread(None, self._loopUpdatePos, "Robot._loopUpdatePos").start()
		

		
	def _loopReset(self):
		self.e_validate_ident.wait()
		fifo = self.client.addFifo( MsgFifo( W_SWITCH_COLOR ) )
		while True:
			m = fifo.getMsg()
			if m.id_cmd == W_SWITCH_COLOR:
				self.addCmd(ID_OTHERS, Q_LED, -1)
				if self.activeReset:
					self.reset()
					self.client.addFifo(fifo)
	
	def _loopUpdatePos(self):
		self.e_validate_ident.wait()
		fifo = self.client.addFifo( MsgFifo(Q_POSITION) )
		while True:
			start = time.time()
			self.addCmd(ID_ASSERV, Q_POSITION)
			try:
				m = fifo.getMsg(1)
			except TimeoutException:
				pass
			else:
				self.pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
				if len(self.pos) != 3:
					self.write("ERROR : reception pos : %s"%self.pos, colorConsol.FAIL)
					self.pos = (0,0,0)
				self.debug.log(D_UPDATE_POS, self.pos)
			delay = 0.5 - (time.time() - start)
			if delay > 0: time.sleep(delay)
		self.client.removeFifo(fifo)

	def test(self):
		"""
		Check list
		"""
		self.activeReset = False
		self.write("* TEST LEDS *", colorConsol.HEADER)
		self.addCmd(ID_OTHERS, Q_LED, RED)
		time.sleep(0.5)
		self.addCmd(ID_OTHERS, Q_LED, BLUE)
		time.sleep(0.5)
		self.addCmd(ID_OTHERS, Q_LED, -1)
		
		self.write("* TEST ASCENSEURS *", colorConsol.HEADER)
		self.write("test ms recalage avant")
		raw_input("appuyez sur une touche pour lancer le test")
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_PRECAL, AVANT)
		raw_input("appuyez sur une touche pour continuer les tests")
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_PRECAL, AVANT)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, AVANT, HAUT)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, AVANT, BAS)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, AVANT, HAUT)
		self.write("test ms recalage arrière")
		raw_input("appuyez sur une touche pour lancer le test")
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_PRECAL, ARRIERE)
		raw_input("appuyez sur une touche pour continuer les tests")
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_PRECAL, ARRIERE)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, ARRIERE, HAUT)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, ARRIERE, BAS)
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_SETPOSITION, ARRIERE, HAUT)
		
		self.write("* TEST AX12 *", colorConsol.HEADER)
		self.addBlockingCmd(1, 3, ID_AX12, Q_CLOSE, AVANT)
		self.addBlockingCmd(1, 3, ID_AX12, Q_OPEN_MAX, AVANT)
		self.addBlockingCmd(1, 3, ID_AX12, Q_SERRE, AVANT)
		self.addBlockingCmd(1, 3, ID_AX12, Q_CLOSE, AVANT)
		self.addBlockingCmd(1, 3, ID_AX12, Q_CLOSE, ARRIERE)
		self.addBlockingCmd(1, 3, ID_AX12, Q_OPEN_MAX, ARRIERE)
		self.addBlockingCmd(1, 3, ID_AX12, Q_SERRE, ARRIERE)
		self.addBlockingCmd(1, 3, ID_AX12, Q_CLOSE, ARRIERE)
		
		self.write("* TEST JACK *", colorConsol.HEADER)
		self.write("attente d'activation du jack...")
		self.waitJackSignal()
		
		self.write("* TEST SWITCH COLOR *", colorConsol.HEADER)
		for i in xrange(2):
			self.write("attente switch color %s/2..."%(i+1))
			fifo = self.client.addFifo( MsgFifo(W_SWITCH_COLOR, Q_COLOR) )
			m = fifo.getMsg()
			self.addCmd(ID_OTHERS, Q_COLOR)
			m = fifo.getMsg()
			color = int(m.content)
			self.addCmd(ID_OTHERS, Q_LED, color)
			
		self.write("* TEST MS *", colorConsol.HEADER)
		self.write("face avant...")
		self.write("appui...")
		self.waitMSSignal(AVANT, 1)
		self.write("relachement...")
		self.waitMSSignal(AVANT, 0)
		self.write("face arriere...")
		self.write("appui...")
		self.waitMSSignal(ARRIERE, 1)
		self.write("relachement...")
		self.waitMSSignal(ARRIERE, 0)
		
		self.write("* TEST TOURELLE *", colorConsol.HEADER)
		fifo = self.client.addFifo( MsgFifo(W_PING_AV, W_PING_AR) )
		self.addCmd(ID_OTHERS, Q_ULTRAPING, AVANT)
		raw_input("appuyez sur une touche et passer la main devant...")
		while True:
			m = fifo.getMsg()
			if m.id_cmd == W_PING_AV:
				break
		self.addCmd(ID_OTHERS, Q_ULTRAPING, ARRIERE)
		raw_input("appuyez sur une touche et passer la main derriere...")
		while True:
			m = fifo.getMsg()
			self.write(m)
			if m.id_cmd == W_PING_AR:
				break
		self.addCmd(ID_OTHERS, Q_ULTRAPING, -1)
		raw_input("fini...")
		self.activeReset = True
		
			
	def start(self):
		"""
		Démarage du robot
		"""
		self.e_validate_ident.wait()
		while True:
			while self._e_stop.isSet():
				time.sleep(0.5)
			if MOD == DEBUG:
				#time.sleep(2)
				self.test()
				exit()
				#self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 400, 400, 0)
				"""while True:
					self.do_path(((800,400),(400,400)))
				exit()"""
				"""while True:
					for i in xrange(0,361, 45):
						self.tourne(i)"""
				"""while True:
					self.go_point(500,0)
					raw_input()
					self.go_point(0,0)
					#self.do_path(((400,0),(400,400),(0,400),(0,0)))"""
				"""while True:
					self.go_point(0,400)
					self.tourne(180)
					self.go_point(400,400)
					self.tourne(180)
					self.go_point(400,0)
					self.tourne(180)
					self.go_point(0,0)
					self.tourne(180)"""
				"""self.tourne(5)
				self.tourne(0)
				self.tourne(1)
				self.tourne(0)
				self.do_path(((0,400),(400,0),(0,0)))
				self.go_point(400,0)
				self.go_point(0,400)
				self.go_point(0,0)
				self.go_point(100,100)
				self.go_point(0,100)
				self.go_point(0,0)
				self.go_point(50,10)"""
				#raw_input()
				#self.color = RED
				#self.pos = (1500,1050,0)
				#print self._volerPion(AVANT)
				#exit()
				#self.script_ratisserMap()
				#listeVerte = self.scanListeVerte()
				#print listeVerte
				#self.script_construireTourVerte()
				#self.test()
				#continue
				"""self.color = BLUE
				self.addBlockingCmd(2, (0.5,None), ID_ASSERV, Q_AUTO_CALIB, self.color)
				self.do_path(((self.symX(1850),300),))
				while True:
					self.do_path(((self.symX(1850),700),(self.symX(800),700),(self.symX(800),1400),(self.symX(1850),1400)))
				continue
				self.preparation()"""
				"""self.write("* CALIBRATION MANUELLE *", colorConsol.HEADER)
				self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 200, 200, 0)
				self.write("")
				self.addCmd(ID_OTHERS, Q_ULTRAPING, AVANT)
				fifo = self.client.addFifo( MsgFifo(W_PING_AV, W_PING_AR) )
				while True:
					m = fifo.getMsg()
					if m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR:
						pos_adv = self.get_pos_adv(m)
						self.write(m.content)
						self.write(pos_adv)"""
				if self.preparation() >= 0:
					threading.Timer(88, self.stop, ("90s !",)).start()
					self.time_start = time.time()
					self.write(" * START * ", colorConsol.HEADER)
					listeVerte = (PION_1,TOUR,PION_1,PION_1,PION_1)
					#listeVerte = self.scanListeVerte()
					#continue
					"""self.write("* CALIBRATION MANUELLE *", colorConsol.HEADER)
					self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 975, 1225, 0)
					self.write("")"""
					#self.script_homologation()
					#return
					self.go_point(self.symX(800), 300)
					id_pince = self.script_construireTourVerte(listeVerte)
					self.script_allerPoserTourVerte(id_pince)
					self.script_ratisserMap()
					exit()
					"""while 1:
						if self.do_path(((400,0),(0,0))) < 0:
							raw_input("bouh tu t'es coincé")"""
					continue
				else:
					continue
				self.color = BLUE
				self.pos = (1500,1000,90)
				self.debug.log(D_UPDATE_POS, self.pos)
				pions = [Pion(2300,600),Pion(2300,2000)]
				for p in pions:
					p.calculCase()
					p.calculColor(self.color)
				self.debug.log(D_PIONS, map(lambda p: tuple(p), pions))
				#self.findTakeTarget(pions)
				self.findPoussTarget(pions)
				raw_input("press")
				return
				#"""
				"""
				self.color = RED
				self.write("* CALIBRATION MANUELLE *")
				self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 1850, 700, 180)
				self.write("")
				r  = 0
				#self.calibCam()
				#self.testCam()
				#"""
			else:
				if self.preparation() >= 0:
					threading.Timer(88, self.stop, ("90s !",)).start()
					self.time_start = time.time()
					self.write(" * START * ", colorConsol.HEADER)
					listeVerte = (PION_1,TOUR,PION_1,PION_1,PION_1)
					listeVerte = self.scanListeVerte()
					#self.script_homologation()
					#return
					self.go_point(self.symX(800), 300)
					id_pince = self.script_construireTourVerte(listeVerte)
					self.script_allerPoserTourVerte(id_pince)
					self.script_ratisserMap()
					return
			"""else:
				r = self.preparation()
			self.write("preparation %s"%r)
			if r >= 0:
				if MOD == RELEASE: threading.Timer(88, self.stop, ("90s !",)).start()
				if self.color == BLUE:
					self.do_path(((1000,300),))
				else:
					self.do_path(((2000,300),))
				self.addBlockingCmd(1, 10, ID_ASSERV, Q_ANGLE_ABS, 90, VITESSE_ROT)

				
				while not self._e_stop.isSet():
					self.update_pos()
					self.debug.log(D_UPDATE_POS,self.pos)
					self.write("* SCAN *")
					time.sleep(0.2)
					l = self.scan(True)
					self.write("")

					self.updatePions(l)
					
					self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))

					if self.pions:
						target, objectif, path = self.findPoussTarget(self.pions)
						
						self.write("cible : %s"%target)
						self.write("objectif : %s"%objectif)
						self.write("path : %s"%path)

						if target:
							self.write("* MOVE *")
							if self.do_path(path):
								# on update la pos actuelle de la target
								target.pos = objectif
								self.write("objectif : %s"%objectif)
								self.write("target.pos : %s"%target.pos)
								target.calculCase()
								target.calculColor(self.color)
								self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))
							self.write("")
							#raw_input("press")
							continue
							
						
					self.write("* TOURNE *")
					self.addBlockingCmd(2, (0.5,5), ID_ASSERV, Q_ANGLE_REL, 30, VITESSE_ROT)
					self.write("")"""

	def preparation(self):
		"""
		(blockant)
		Ce qui doit etre fait pendant les 3 minutes
		Choix de la couleur
		Recalage

		@return 0 si ok, <0 sinon
		"""
		self.write('" * PREPARATION BEGIN * ', colorConsol.HEADER)
		retour = 0
		# pour recevoir les messages
		fifo = self.client.addFifo( MsgFifo(W_JACK, Q_COLOR, Q_AUTO_CALIB, Q_PRECAL) )
		# pour faire clignoter les leds
		loop1 = LoopCmd(self, 0, 1, ID_OTHERS, Q_LED, RED)
		loop2 = LoopCmd(self, 0.5, 1, ID_OTHERS, Q_LED, BLUE)
		try:
			self.addCmd(ID_OTHERS, Q_COLOR)
			self.write("* RÉCUPÉRATION COULEUR *", colorConsol.HEADER)
			m = fifo.getMsg(1)
			if m.id_cmd == Q_COLOR:
				self.color = int(m.content)
			elif m.id_cmd == Q_KILL:
				raise KillException("Q_KILL")
			
			if self.color == RED:
				self.write("COULEUR ROUGE !")
			else:
				self.write("COULEUR BLEU !")
			self.addCmd(ID_OTHERS, Q_LED, self.color)
			self.write("")

			
			self.write("* RECALAGE PINCES *", colorConsol.HEADER)
			self.addCmd(ID_AX12, Q_CLOSE, AVANT)
			self.addCmd(ID_AX12, Q_CLOSE, ARRIERE)
			self.addCmd(ID_OTHERS, Q_PRECAL, AVANT)
			self.addCmd(ID_OTHERS, Q_PRECAL, ARRIERE)
			nb_rep = 0
			while True:
				m = fifo.getMsg()
				if m.id_cmd == Q_KILL:
					raise KillException("Q_KILL")
				elif m.id_cmd == Q_PRECAL:
					nb_rep+=1
					if nb_rep >= 4:
						break
			self.write("")
					
			
			self.write("* JACK POUR RECALAGE *")
			while True:
				m = fifo.getMsg()
				if m.id_cmd == Q_KILL:
					raise KillException("Q_KILL")
				elif m.id_cmd == W_JACK and int(m.content) == 0:
					# on fait clignoter les leds
					loop1.start()
					loop2.start()
					self.write("* RECALAGE *")
					self.addBlockingCmd(2, (0.5,None), ID_ASSERV, Q_AUTO_CALIB, self.color)
					self.write("")
					break
			
			loop1.stop()
			loop2.stop()
			loop1.join()
			loop2.join()
			self.addCmd(ID_OTHERS, Q_LED, self.color)
			#self.update_pos()
			
			self.write("* ATTENTE DU JACK *")
			while True:
				m = fifo.getMsg()
				if m.id_cmd == Q_KILL:
					raise KillException("Q_KILL")
				elif m.id_cmd == W_JACK and int(m.content) == 0:
					self.write("ON Y VAS !")
					self.write("")
					break
		except KillException as ex:
			self.write(ex, colorConsol.FAIL)
			retour = Q_KILL
		except TimeoutException as ex:
			self.write(ex, colorConsol.FAIL)
			retour = E_TIMEOUT
		except Exception as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
			retour = -101
		finally:
			self.client.removeFifo(fifo)
			loop1.stop()
			loop2.stop()
			loop1.join()
			loop2.join()
		self.write('" * PREPARATION END * ', colorConsol.HEADER)
		return retour


	def waitJackSignal(self):
		"""
		Attend qu'on enlève le jack
		"""
		fifo = self.client.addFifo( MsgFifo(W_JACK) )
		while True:
			m = fifo.getMsg()
			if m.id_cmd == W_JACK and int(m.content) == 0:
				break
		self.client.removeFifo(fifo)

	def waitMSSignal(self, id_face, state):
		fifo = self.client.addFifo( MsgFifo(W_MS) )
		try:
			while True:
				m = fifo.getMsg()
				if m.id_cmd == W_MS and int(m.content.split(C_SEP_SEND)[1]) == state:
					break
		except Exception as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
		finally:
			self.client.removeFifo(fifo)
		
	def dumpObj(self, id_pince):
		"""
		(blockant)
		ouvrir la pince
		@param id_pince (int) AVANT ou ARRIERE
		"""
		return self.addBlockingCmd(1, 3, ID_AX12, Q_OPEN_MAX, id_pince)


	def ascenseurPinces(self, id_pince, pos):
		try:
			self.addBlockingCmd(2, (1,5), ID_OTHERS, Q_SETPOSITION, id_pince, pos)
		except TimeoutException as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
	
	def takeObj(self, id_pince):
		"""
		(blockant)
		ramasser un objet (serre et remonte)
		@param target (Pion)
		@return True si l'objet a été pris, False sinon
		"""
		self.ascenseurPinces(id_pince, BAS)
		self.addBlockingCmd(1, 3, ID_AX12, Q_SERRE, id_pince)

	
	def stop(self, msg=None):
		""" arret du robot """
		if msg: self.write("stop : %s"%msg, colorConsol.FAIL)
		self.addCmd(ID_ASSERV,Q_STOP)
		self.addCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -1)
		self.update_pos()
		pos_pince_av = self.pos_rel(self.pos, radians(self.pos[2]), D_CENTER_2_PINCE + R_PION)
		self.addCmd(ID_AX12, Q_KILL)
		time.sleep(0.5)
		self.addCmd(-1, Q_KILL)
		self._e_stop.set()
		self.client.stop()

	
	def reset(self):
		"""
		reset du robot
		"""
		self.write("* * * * * * *", colorConsol.FAIL)
		self.write("*   RESET   *", colorConsol.FAIL)
		self.write("* * * * * * *", colorConsol.FAIL)
		self.stop()
		self.client.reset()
		time.sleep(0.5)
		
		self.pinces = (Pince(), Pince())
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self.pos = (0,0,0)
		self.color = RED
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0
		
		self.write("* ROBOT IS READY *", colorConsol.OKGREEN)
		self._e_stop.clear()
		
	def isEmpty(self):
		return (self.pinces[0].isEmpty() and self.pinces[1].isEmpty())
	
	def isFull(self):
		return (self.pinces[0].isFull() and self.pinces[1].isFull())

	def testCam(self, fast=True):
		""" boucle sur un scan """
		while True:
			start = time.time()
			# scan
			self.write("* SCAN *")
			l = self.scan(fast)
			self.write("")

			self.updatePions(l)
			
			if self.pions:
				target, objectif, path = self.findPoussTarget(self.pions)
				
				self.write("cible : %s"%target)
				self.write("objectif : %s"%objectif)
				self.write("path : %s"%path)

			ellapse = time.time() - start
			self.write("time : %s"%ellapse)
	
	def calibCam(self):
		while 1:
			self.pos = (1850,700,180)
			self.debug.log(D_UPDATE_POS, self.pos)
			# pour écouter la réponse de la cam
			fifo = self.client.addFifo( MsgFifo(64, 65) )
			
			# première tentative
			self.addCmd(ID_CAM, 64)
			l = eval(str(fifo.getMsg().content))

			# arreter d'écouter
			self.client.removeFifo(fifo)
			
			# transformation en objet
			l = map(lambda p: Pion(p[1], p[2], p[0]), l)

			if l:
				x = l[0].pos.x
				y = l[0].pos.y
				
				Cx,Cy = 1150 - x - self.pos[0], y
				print "calib",Cx,Cy
				l = self._changeRepere(Cx,Cy,l)
				self.write("Résultat change repere scan : %s"%l)
				l = self._filtreInMap(l)
				self.write("Résultat filtre map scan : %s"%l)
				# recalculer les couleurs et la case
				for p in l:
					p.calculColor(self.color)
					p.calculCase()
				self.debug.log(D_PIONS, tuple(map(lambda p: tuple(p), l)))

			if 'q' == raw_input():
				break
		
	def update_pos(self):
		"""
		(bloquant)
		Récupérer la position actuelle
		"""
		#start = time.time()
		fifo = self.client.addFifo( MsgFifo(Q_POSITION) )
		self.addCmd(ID_ASSERV, Q_POSITION)
		m = fifo.getMsg()
		try:
			new_pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
		except Exception as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
			new_pos = self.pos
		self.pos = new_pos
		self.write("NEW POS : "+str(self.pos))
		self.client.removeFifo(fifo)
		self.debug.log(D_UPDATE_POS, self.pos)
		#self.write("time update pos %s"%(time.time() - start))
		
	def do_path(self, path, timeout=None):
		"""
		(blockant)
		@todo interuption si erreur parceque la position n'est pas la bonne
		Suit un chemin
		La fonction peut tout couper si elle reçoit un message de danger de la tourelle
		ou si après avoir comparé la position actuelle du robot avec celle éstimée 
		il y a une anomalie
		
		@param path<(x,y)> la liste des points
		"""
		if timeout:
			time_stop = time.time() + timeout
		else:
			time_stop = time.time() + 3600
		retour = 0
		if self._e_stop.isSet():
			retour = -42
		else:
			self.write("* DO_PATH %s BEGIN *"%(path,), colorConsol.HEADER)

			for x,y in path:
				if time.time() < time_stop:
					if self.go_point(x,y, timeout) == E_TIMEOUT:
						retour = E_TIMEOUT
						break
				else:
					retour = E_TIMEOUT
					break
				
			self.write("* DO_PATH END *", colorConsol.HEADER)
		return retour

	def go_point(self, x,y, timeout=None):
		"""
		(bloquant|nbloquant)
		Va a un point unique
		
		@param x,y coords du point en mm

		@return (int)
		"""
		if timeout:
			timeout = time.time() + timeout
		else:
			timeout = time.time() + 3600
		x,y = int(round(x)),int(round(y))
		self.write("* GO_POINT %s %s BEGIN *"%(x,y), colorConsol.HEADER)
		if self._e_stop.isSet():
			retour = -42
		else:
			retour = 0
			self.update_pos()
			# test si petit déplacement ou pas
			if abs(self.pos[0] - x) < 100 and abs(self.pos[1] - y) < 100:
				self.write("petit déplacement")
				# tourne en premier
				l = Line(Vec2(self.pos[0],self.pos[1]), Vec2(x,y))
				if self.angle_diff(radians(self.pos[2]), l.teta) > pi/2:
					self.tourne(degrees(l.teta)+180.0)
				else:
					self.tourne(degrees(l.teta))
				
			self.update_pos()
			if abs(self.pos[0] - x) < 40 and abs(self.pos[1] - y) < 40:
				self.write("déplacement trop petit %s %s %s"%(self.pos, abs(self.pos[0] - x), abs(self.pos[1] - y)))
				self.write("* GO_POINT END *", colorConsol.HEADER)
				return retour
			
			self.addCmd(ID_ASSERV, Q_STOP)
			self.addCmd(ID_ASSERV, Q_RESUME)
			fifo = self.client.addFifo( MsgFifo(Q_GOAL_ABS, Q_POSITION, W_PING_AR, W_PING_AV) )
			sens_ping = self.sens_next_cible((x,y))
			timeLastPing = 0
			inPause = False
			self.addCmd(ID_ASSERV, Q_GOAL_ABS, x, y, VITESSE)
			self.addCmd(ID_OTHERS, Q_ULTRAPING, sens_ping)
			nb_recv = 0
			nb_anomalies = 0
			time_to_check_anomalie = time.time() + 1.5
			try:
				while True:
					if time.time() > timeout:
						raise TimeoutException("Robot.go_point : temps pour aller au point trop long")
					if inPause and time.time() - timeLastPing > 0.5:
						self.addCmd(ID_ASSERV, Q_RESUME)
						inPause = False
					m = fifo.getMsg(2)
					if m.id_cmd!=Q_POSITION: self.write("Robot.go_point : %s"%m)
					if m.id_cmd == Q_GOAL_ABS:
						nb_recv += 1
						if nb_recv >= 2: break
					elif time.time() > self.time_begin + DELAY_PINGS and (m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR):
						pos_adv = self.get_pos_adv(m)
						
						if (300 < pos_adv[0] < 2700) and (300 < pos_adv[1] < 1800):
							if not inPause:
								self.addCmd(ID_ASSERV, Q_PAUSE)
								self.write("WARNING : Robot.go_point : detection adversaire", colorConsol.WARNING)
								inPause = True
							timeLastPing = time.time()
					elif not inPause and m.id_cmd == Q_POSITION and time.time() > time_to_check_anomalie:
						new_pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
						if self._anomalie_deplacement_delta((x,y), self.pos, new_pos):
							self.write("WARNING : Robot.go_point : detection anomalie deplacement", colorConsol.WARNING)
							nb_anomalies += 1
							if nb_anomalies > 2:
								self.write("ERROR : Robot.do_path : detection anomalie deplacement critique", colorConsol.FAIL)
								self.addCmd(ID_ASSERV, Q_STOP)
								if sens_ping == AVANT:
									self.addBlockingCmd(2, (1,2), ID_ASSERV, Q_PWM, -70, 1000)
								else:
									self.addBlockingCmd(2, (1,2), ID_ASSERV, Q_PWM, 70, 1000)
								retour = E_BLOCK
								break
						else:
							nb_anomalies = 0
						self.pos = new_pos
						self.debug.log(D_UPDATE_POS,self.pos)
					elif m.id_cmd == Q_KILL:
						raise KillException("Q_KILL")
			except KillException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = Q_KILL
			except TimeoutException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = E_TIMEOUT
			except Exception as ex:
				self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
				retour = -101
			finally:
				self.addCmd(ID_ASSERV, Q_STOP)
				self.client.removeFifo(fifo)
				self.addCmd(ID_OTHERS, Q_ULTRAPING, -1)

			time.sleep(0.2)
		
		self.write("* GO_POINT END *", colorConsol.HEADER)
		return retour


	def tourne(self, a, timeout=None):
		"""
		(bloquant|nbloquant)
		Fait tourner le robot
		
		@param a angle en degrès

		@return (int)
		"""
		if timeout:
			timeout = time.time() + timeout
		else:
			timeout = time.time() + 3600
		a = int(round(a))
		self.write("* TOURNE %s *"%(a), colorConsol.HEADER)
		if self._e_stop.isSet():
			retour = -42
		else:
			retour = 0
			self.update_pos()
			if self.angle_diff(radians(self.pos[2]),radians(a)) < 1.0 * pi / 180.0:
				self.write("angle trop petit")
				self.write("* TOURNE END *", colorConsol.HEADER)
				return retour
			self.addCmd(ID_ASSERV, Q_STOP)
			self.addCmd(ID_ASSERV, Q_RESUME)
			fifo = self.client.addFifo( MsgFifo(Q_POSITION, Q_ANGLE_ABS) )

			nb_anomalies = 0
			
			self.addCmd(ID_ASSERV, Q_ANGLE_ABS, a, VITESSE_ROT)

			nb_recv = 0
			try:
				while True:
					if time.time() > timeout:
						raise TimeoutException("Robot.go_point : temps pour aller au point trop long")
					m = fifo.getMsg(2)
					#if m.id_cmd!=Q_POSITION: self.write("tourne : %s"%m)
					self.write("tourne : %s"%m)
					if m.id_cmd == Q_ANGLE_ABS:
						nb_recv += 1
						if nb_recv >= 2: break
					elif m.id_cmd == Q_POSITION:
						new_pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
						self.debug.log(D_UPDATE_POS, new_pos)
						if self._anomalie_deplacement_alpha(a, self.pos, new_pos):
							self.write("WARNING : Robot.tourne : detection anomalie rotation", colorConsol.WARNING)
							nb_anomalies += 1
							if nb_anomalies > 2:
								self.write("WARNING : Robot.tourne : detection anomalie deplacement critique", colorConsol.FAIL)
								retour = E_BLOCK
								break
						else:
							nb_anomalies = 0
						self.pos = new_pos
						self.debug.log(D_UPDATE_POS,self.pos)
					elif m.id_cmd == Q_KILL:
						raise KillException("Q_KILL")
			except KillException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = Q_KILL
			except TimeoutException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = E_TIMEOUT
			except Exception as ex:
				self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
				retour = -101
			finally:
				self.addCmd(ID_ASSERV, Q_STOP)
				# destruction de la fifo
				self.client.removeFifo(fifo)
			
			#time.sleep(0.5)	
			self.write("* TOURNE END *", colorConsol.HEADER)

		return retour

	def _anomalie_deplacement_delta(self, objectif, last_pos, new_pos):
		"""
		(calcul)
		Detecte si il y a une anomalie de déplacement, puis remplace last_pos par new_pos

		@return (True/False)
		"""
		self.write(str(objectif) + str(last_pos) + str(new_pos))
		retour = (	(abs(objectif[0] - new_pos[0]) > 40 or abs(objectif[1] - new_pos[1]) > 40) \
					and (abs(new_pos[0] - last_pos[0]) < 10 or abs(new_pos[1] - last_pos[1]) < 10) \
				 )
		
		return retour

	def _anomalie_deplacement_alpha(self, objectif, last_pos, new_pos):
		retour = (	self.angle_diff(radians(objectif), radians(new_pos[2])) > 2.0 * pi / 180.0 \
					and self.angle_diff(radians(last_pos[2]), radians(new_pos[2])) < 1.0 * pi / 180.0 \
				 )
		
		return retour 

	def _tryLoop(self, f, *args, **kwargs):
		"""
		Execute la fonction f à l'interieu d'un try catch,
		la boucle s'arrete si f renvoie False, ou qu'un erreur
		survient
		
		@param f la fonction a executer

		@return (int)
		"""
		retour = 0
		try:
			while True:
				if not f(*args, **kwargs):
					break
		except KillException as ex:
			self.write(ex, colorConsol.FAIL)
			retour = Q_KILL
		except TimeoutException as ex:
			self.write(ex, colorConsol.FAIL)
			retour = E_TIMEOUT
		except Exception as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
			retour = -101
		return retour
		
	####################################################################
	#					UTILISATION DE LA VISIO						   #
	####################################################################
	def scanListeVerte(self):
		"""
		(bloquant)
		"""
		self.write("* SCAN LISTE VERTE *", colorConsol.HEADER)
		r = self.addBlockingCmd(1, 4, ID_PHONE, Q_SCAN_DEPART)
		self.write(" result : %s"%r)
		listeVerte = eval(r.content)
		if self.color == RED:
			listeVerte.reverse()
		listeVerte = map(lambda x: TOUR if x else PION_1, listeVerte)
		self.write(listeVerte)
		self.write("* SCAN LISTE VERTE END *", colorConsol.HEADER)
		return listeVerte
	
	def scan(self, fast=False):
		"""
		(blockant)
		@todo gestion si la réponse n'arrive pas assez vite
		demande un scan à la camera, fait toujours deux tentatives
		Met à jourla liste des pions actuelle

		"""
		self.write("* SCAN *", colorConsol.HEADER)
		l = []

		# pour écouter la réponse de la cam
		fifo = self.client.addFifo( MsgFifo(Q_SCAN_AV, Q_SCAN_AR, 64, 65) )
		
		# première tentative
		if not fast:
			self.addCmd(ID_CAM, Q_SCAN_AV)
		else:
			self.addCmd(ID_CAM, 64)
		l = eval(str(fifo.getMsg().content))
		
		self.write("Résultat brut scan : %s"%l)
		
		# arreter d'écouter
		self.client.removeFifo(fifo)
		
		# transformation en objet
		l = map(lambda p: Pion(p[1], p[2], p[0]), l)
		
		# récupération de la position
		self.update_pos()
		# transformation des valeurs
		Cx = 130
		Cy = -150
		l = self._changeRepere(Cx,Cy,l)
		self.write("Résultat change repere scan : %s"%l)
		# recalculer les couleurs et la case
		for p in l:
			p.calculColor(self.color)
			p.calculCase()
		l = self._filtreInMap(l)
		self.write("Résultat filtre map scan : %s"%l)

		return l
		
	def _changeRepere(self, Cx, Cy, l):
		"""
		Effectue le changement de repère pour la réponse de la caméra

		@param Cx, Cy décalage de la caméra dans le repère robot
		@param l liste<Pion> liste des pions trouvés par la caméra

		@return (liste<Pion>) liste des pions dans le nouveau repère
		"""
		cosa = math.cos(math.radians(float(self.pos[2])))
		sina = math.sin(math.radians(float(self.pos[2])))
		def f(pion):
			pion.pos.x = int(cosa * float(pion.pos.x + Cx) - sina * float(pion.pos.y + Cy)) + self.pos[0]
			pion.pos.y = int(sina * float(pion.pos.x + Cx) + cosa * float(pion.pos.y + Cy)) + self.pos[1]
			return pion
		return map(lambda p: f(p), l)
			
	def _filtreInMap(self, l):
		"""
		Filtre les pions hors carte

		@param l liste<Pion>

		@return (liste<Pion>) liste filtrée
		"""
		return filter(lambda p: (0 < p.pos.x < 3000) and (0 < p.pos.y < 2100), l)



	####################################################################
	#						IA DYNAMIQUE							   #
	####################################################################
	
	def updatePions(self, l):
		"""
		(calcul)
		@todo cette focntion est à chier pour l'instant (surtout le filtre avec l'age),
		il faudrait savoir quels pions ont bougés, par exemple à partir de
		quel angle du robot a été fait le scan on peut savoir quels pions de
		l'ancien scan sont affectés, on peut donc savoir que si un pion n'est
		pas retrouvé dans ce champs c'est qu'il a été déplacé.
		
		A partir du résultat du nouveau scan et de l'ancien, l'IA doit
		faire un "merge", le résultat est stocké dans self.pions
		
		@param l (list<Pion>) le résultat du scan

		@return None (le résultat est mis dans self.pions
		"""
		# update des pions
		"""pions_to_add = []
		for new_p in l:
			for p in self.pions:
				if new_p == p:
					new_p.update(p,self.color)
					break
			else:
				pions_to_add.append(new_p)
		self.pions += pions_to_add

		self.pions = filter(lambda p: p.age() < AGE_MAX, self.pions)
		
		self.debug.log(D_DELETE_PATH)
		self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))
		"""
		self.pions = l
		
		
	def sortPionsByImportance(self, pions):
		"""
		(calcul)
		Tri les pions par ordre d'importance (les pions que l'IA va essayer
		de récupérer en premier, pour l'instant plus un pion est proche
		plus il est important.

		@param pions (list< <x,y> >)

		@return None (la liste est passée en pointeur)
		"""
		def plusProche(p1,p2):
			x1,y1 = p1.pos.x,p1.pos.y
			x2,y2 = p2.pos.x,p2.pos.y
			
			v1 = abs(x1-self.pos[0])**2+abs(y1-self.pos[1])**2
			v2 = abs(x2-self.pos[0])**2+abs(y2-self.pos[1])**2
			
			return v1 - v2

		# tri pour avoir les pions par proximité
		pions.sort(cmp=lambda p1,p2: plusProche(p1,p2))
	
	def findTakeTarget(self,pions):
		"""
		(calcul)
		Trouve un pion à prendre,
		renvoie le pion cible et la path

		@param pions (list<Pion>)

		@return target (Pion), path (list< <x,y> >)
		"""
		self.debug.log(D_DELETE_PATH)
		if not pions:
			return None, []
			
		circles = self._findObstacles(pions)
		
		# on cherche une cible
		for p in pions:
			if p.isOtherColor(self.color):
				path = find_path(Vec2(self.pos[0],self.pos[1]), p.pos, circles)
				if path and self._pathValid(path):
					target = p
					break
		else:
			return None,[]

		# on va maintenant faire en sorte de s'arreter avant
		pos_devant = Line(Vec2(path[-1][0],path[-1][1]), Vec2(path[-2][0],path[-2][1])).pointFrom(250)
		path = path[:-1] + [tuple(pos_devant)]

		self.debug.log(D_SHOW_PATH,path)
		
		return target, path
		

	def findPoussTarget(self,pions):
		"""
		(calcul)
		Trouve un pion à pousser,
		renvoie le pion cible, l'objectif(la position de la case sur laquelle
		on veut pousser le pion) et le path

		@param pions liste<Pion>
		
		@return target(Pion),objectif(Vec2), path (list< <x,y> >)
		"""
		self.debug.log(D_DELETE_PATH)
		if not pions:
			return None, None, []
			

		circles = self._findObstacles(pions)
		self.write("obstacles : %s"%circles)
		# les position des cases aec un pion dessus
		cases_with_pion = map(lambda p: p.case, pions)
		# le périmètre de jeu : là où l'on peut poser les pions
		game_rectangle = Rectangle((450,0),(2550,1750))
		
		# on cherche une cible
		for p in pions:
			if p.isOtherColor(self.color):
				target = p
				# on va essayer de pousser cette cible selon l'axe 0x
				case_objectif = target.case + (350,0) if self.pos[0] < target.pos.x else target.case + (-350,0)
				if case_objectif in game_rectangle:
					# position devant le pion
					pos_pousse = self._position_pousse(target,case_objectif)
					# vérification qu'il n'y a pas un pion à nous sur la case devant ou derrière
					case_p_pousse = target.case + (-350,0) if self.pos[0] < target.pos.x else target.case + (350,0)
					if case_p_pousse not in cases_with_pion and case_objectif not in cases_with_pion:
						# appelle de la fonction
						path = find_path(Vec2(self.pos[0],self.pos[1]), pos_pousse, circles)
						if self._pathValid(path):
							break
				# tentative selon l'axe Oy
				case_objectif = target.case + (0,350) if self.pos[1] < target.pos.y else target.case + (0,-350)
				if case_objectif in game_rectangle:
					# position devant le pion
					pos_pousse = self._position_pousse(target,case_objectif)
					# vérification qu'il n'y a pas un pion à nous sur la case devant ou derrière
					case_p_pousse = target.case + (0,-350) if self.pos[1] < target.pos.y else target.case + (0,350)
					if  case_p_pousse not in cases_with_pion and case_objectif not in cases_with_pion:
						# appelle de la fonction
						path = find_path(Vec2(self.pos[0],self.pos[1]), pos_pousse, circles)
						if self._pathValid(path):
							break
		else: # aucune cible ne convient
			return None, None, []


		case_stop = Line(case_objectif, target.pos).pointFrom(160)
		path += [tuple(target.pos), tuple(case_stop), (target.pos.x,target.pos.y)]

		
		"""# décalage du point d'arrivée (à 120mm du pion)
		Ax,Ay, Bx,By = path[-4], path[-3], path[-2], path[-1]
		dAB = math.sqrt((Bx-Ax)**2 + (By-Ay)**2)
		path[-2] -= int(120 * (Bx-Ax) / dAB)
		path[-1] -= int(120 * (By-Ay) / dAB)"""
		
		self.debug.log(D_SHOW_PATH,path)
		
		return target, case_objectif, path

	def _findObstacles(self, pions):
		"""
		(calcul)

		@param pions (list< <x,y> >)

		@return (list<Circle>) une liste d'obstacles exploitables par findPath
		"""
		# comme obstacles on ne prend que nos pions
		circles = filter(lambda p: p.color == self.color, pions)
		# on remplace ces Pion par des cercles pour la fonction find_path
		circles = map(lambda p: Circle(p.pos, 300), circles)
		return circles
		
	def _pathValid(self, path):
		"""
		(calcul)
		Détermine si un path est umprintable par le robot ou non

		@param path (list< <x,y> >)
		@return True/False
		"""
		for x,y in path[::2]:
			if not (400 < x < 2600) or not (200 < y < 1500):
				return False
		return True

		
	def _position_pousse(self, target, objectif):
		"""
		(calcul)
		Renvoie la position qui permettra de pousser la target sur la case en avançant droit

		@param target (Pion) le pion à pousser
		@param objectif (Vec2) case où l'on veut pousser le pion

		@return (Vec2) la position de pousse
		"""
		l = Line(target.pos, objectif)
		return l.pointFrom(-300)


	####################################################################
	#							IA SCRIPT							   #
	####################################################################
	def script_rush(self):
		"""
		(script IA)
		Rusher pour aller chercher un pion vert adverse et le faire chier
		"""
		pass


	def script_homologation(self):
		"""
		(script IA)
		Homologation : prend un pion et le pose
		"""
		self.do_path(((self.symX(800), 300), (self.symX(X_DEPLACEMENT),690)))
		self._takePionVert(0,AVANT)
		time.sleep(1)
		# avec pinces
		self.tourne(90)
		case = CASES[self.symC(0)][3]
		l = Line(Vec2(case.x,case.y),Vec2(self.pos[0],self.pos[1]))
		pos_2_dump = l.pointFrom(R_PION + D_CENTER_2_PINCE)
		self.go_point(pos_2_dump.x, pos_2_dump.y)
		self.dumpObj(AVANT)#"""
		self.addCmd(ID_AX12, Q_CLOSE, AVANT)
		while not self._e_stop.isSet():
			r = self.do_path(((800,700),(2200,700)))
			if r == Q_KILL:
				break
		
		
	
	def script_allerPoserTourVerte(self, id_pince):
		"""
		Script pour aller poser la tour verte

		@param id_pince la pince dans laquelle se trouve la tour
		"""
		r = self.do_path(((self.symX(625),1550),(self.symX(1500),1550),(self.symX(1500),1750)), 20) # aller devant la case bonus
		if r == E_TIMEOUT:
			r = self.go_point(self.symX(975),1400, 8)
			if r!=E_TIMEOUT:
				if id_pince == AVANT:
					self.tourne(90)
				else:
					self.tourne(-90)
			else:
				self.go_point(self.symX(975),700)
				if id_pince == AVANT:
					self.tourne(90)
				else:
					self.tourne(-90)
			self.dumpObj(id_pince)
			while True:
				time.sleep(1)
		else:
			if id_pince == AVANT:
				angle = 135
				pwm = 80
			else:
				angle = -45
				pwm = -80
			self.tourne(self.symA(angle))
			self.dumpObj(id_pince) # lacher l'objet

			self.ascenseurPinces(id_pince, HAUT)
			self.addBlockingCmd(2, (1,3), ID_ASSERV, Q_PWM, pwm, 2000)
			if id_pince == AVANT:
				self.ascenseurPinces(ARRIERE, BAS)
				self.addBlockingCmd(1, 4, ID_AX12, Q_OPEN_MAX, ARRIERE)
			else:
				self.ascenseurPinces(AVANT, BAS)
				self.addBlockingCmd(1, 4, ID_AX12, Q_OPEN_MAX, AVANT)
			self.addBlockingCmd(2, (1,2), ID_ASSERV, Q_PWM, -pwm, 1500)
		
	def script_construireOtherTourVerte(self, listeVerte):
		self.color = self.otherColor(self.color)
		try:
			retour = self.script_allerPoserTourVerte(listeVerte)
		except Exception as ex:
			self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
			retour = -101
		finally:
			self.color = self.otherColor(self.color)
		return retour
		
	def script_construireTourVerte(self, listeVerte):
		"""
		(script IA)
		Script pour construire une tour à partir de ce qu'il y a dans la zone verte

		@return (int) id_pince avec la tour
		"""
		self.write("* CONSTRUCTION TOUR VERTE *", colorConsol.HEADER)
		listeYVerte = (690,970,1250,1530,1810)

		p1 = 0
		p2 = 1
		if (listeVerte[p1] == TOUR and listeVerte[p2] == TOUR):
			p2 += 1
		p3 = p2 + 1
		if (listeVerte[p1] == PION_1 and listeVerte[p2] == PION_1 and listeVerte[p3] == PION_1) \
			or (listeVerte[p1] == TOUR and listeVerte[p3] == TOUR) \
			or (listeVerte[p2] == TOUR and listeVerte[p3] == TOUR):
			p3 += 1
			
		#print p1,p2,p3
		#raw_input()

		
		# prise du premier pion avec pince AVANT
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p1]) 	# devant le premier pion
		self._takePionVert(p1, AVANT)
		#return AVANT
		self.write("1 ére tour prise", colorConsol.OKGREEN)

		# prise du deuxième pion avec pince ARRIERE
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p2]) 	# devant le deuxième
		self._takePionVert(p2, ARRIERE)
		self.write("2 ème tour prise", colorConsol.OKGREEN)

		# prise du troisième
		if listeVerte[p1] == PION_1 and listeVerte[p2] == PION_1:
			self.tourne(self.symA(90)) # tourne
			self.dumpObj(ARRIERE) # lache pion à l'arriere
			self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p3]) # devant le troisième
			self._takePionVert(p3,ARRIERE) # prend le pion dans la pince arrière
			self.tourne(-90)
			self._combinerFaces(ARRIERE) # construit dans la pince arrière
			self.ascenseurPinces(ARRIERE, HAUT)
			self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p2]-100) # devant le deuxième
			self.dumpObj(ARRIERE)
			self.takeObj(ARRIERE)
			self.write("3 ème tour prise", colorConsol.OKGREEN)
			return ARRIERE
		else:
			if listeVerte[p1] == TOUR:
				angle = 90
				p_with_pion = ARRIERE
				p_with_tour = AVANT
			else:
				angle = -90
				p_with_pion = AVANT
				p_with_tour = ARRIERE
			self.tourne(angle)
			self._combinerFaces(p_with_tour)
			self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p3]) # devant le troisième
			self._takePionVert(p3, p_with_pion) # prise troisième pion avec la pince maintenant vide
			self.tourne(angle) # tourne
			if p3 > 2:
				self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[p3]-200) # on recul un peu pour etre sur de ne pas se taper la bordure
			self._combinerFaces(p_with_tour)
			self.write("3 ème tour prise", colorConsol.OKGREEN)
			return p_with_tour
				
	def _takePionVert(self, index, id_pince):
		self.write("* _takePionVert begin *", colorConsol.HEADER)
		listeYVerte = (690,970,1250,1530,1810)
		retour = 0
		if id_pince == AVANT:
			self.tourne(self.symA(180))
		else:
			self.tourne(self.symA(0))
		#raw_input("ouvrir...")
		self.addBlockingCmd(1, 3, ID_AX12, Q_OPEN_MAX, id_pince)
		#raw_input("descendre...")
		self.ascenseurPinces(id_pince, BAS)
		
		#raw_input("avancer...")
		pwm = 100
		if id_pince == ARRIERE:
			pwm = -pwm
		self.write("pwm : %s"%pwm)
		self.addBlockingCmd(2, (1,3), ID_ASSERV, Q_PWM, pwm, 2000)
		self.takeObj(id_pince)
		self.addBlockingCmd(2, (1,1), ID_ASSERV, Q_PWM, -pwm, 500)
		fifo = self.client.addFifo( MsgFifo(Q_SETPOSITION) )
		self.addCmd(ID_OTHERS, Q_SETPOSITION, id_pince, HAUT) # lève la pince
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[index]) # recul en même temps
		nb_reception = 0
		try:
			while True:
				m = fifo.getMsg(5)
				if m.id_cmd == Q_KILL:
					raise KillException
				elif m.id_cmd == Q_SETPOSITION:
					nb_reception += 1
					if nb_reception >= 2:
						break
		except TimeoutException as ex:
			self.write(ex, colorConsol.FAIL)
			retour = E_TIMEOUT
		finally:
			self.client.removeFifo(fifo)
			
		self.write("* _takePionVert end *", colorConsol.HEADER)
		return retour


		
	def _combinerFaces(self, id_face):
		"""
		@param id_pince la pince qui contient le pion à mettre sur
		l'autre, au final cette pince contiendra les deux pions
		"""
		self.update_pos()
		if id_face == AVANT:
			id1 = ARRIERE
			id2 = AVANT
			d_avance = 100
			d_recul = -105
			a = self.pos[2] + 180
		else:
			id1 = AVANT
			id2 = ARRIERE
			d_avance = -100
			d_recul = 105
			a = self.pos[2] + 180
		pos_recul = self.pos_rel(self.pos, radians(self.pos[2]), d_avance)
		pos_avance = self.pos_rel(self.pos, radians(self.pos[2]), d_recul)
		self.ascenseurPinces(AVANT, HAUT)
		self.ascenseurPinces(ARRIERE, HAUT)
		#raw_input("dump")
		self.dumpObj(id1) # lache
		#raw_input("recul")
		self.go_point(*pos_recul)
		#raw_input("tourne")
		self.tourne(a)
		#raw_input("avance")
		self.go_point(*pos_avance)
		#raw_input("lache & prend")
		self.dumpObj(id2) # lache
		self.takeObj(id2) # reprend



	def script_ratisserMap(self):
		"""
		(script IA)
		Tourner sur la carte pour tout shooter et prendre les pions si ils tombent dans nos pinces
		"""
		checksPoints = (CASES[self.symC(2)][4], CASES[self.symC(0)][2], CASES[self.symC(2)][0], CASES[self.symC(4)][2])
			
		self.addCmd(ID_AX12, Q_OPEN_MAX, AVANT)
		self.addCmd(ID_AX12, Q_OPEN_MAX, ARRIERE)
		self.ascenseurPinces(AVANT, BAS)
		self.ascenseurPinces(ARRIERE, BAS)
		time.sleep(2)
		
		fifo = None
		
		while not self._e_stop.isSet():
			for c in checksPoints:
				if fifo: self.client.removeFifo(fifo)
				if self._e_stop.isSet():
					break
				fifo = self.client.addFifo( MsgFifo(Q_GOAL_ABS, Q_POSITION, W_MS, W_PING_AV, W_PING_AR) )
				sens_ping = self.sens_next_cible((c.x,c.y))
				self.addCmd(ID_ASSERV, Q_GOAL_ABS, c.x, c.y, VITESSE)
				self.addCmd(ID_OTHERS, Q_ULTRAPING, sens_ping)
				nb_recep = 0
				nb_anomalies = 0
				last_pos = copy.copy(self.pos)
				try:
					while not self._e_stop.isSet():
						m = fifo.getMsg(2)
						if m.id_cmd == Q_GOAL_ABS:
							nb_recep+=1
							if nb_recep > 1: break
						elif m.id_cmd == W_MS:
							id_pince, id_state = [ int(_) for _ in m.content.split(C_SEP_SEND) ]
							self.write("retisser_map : %s"%m.content, colorConsol.WARNING)
							if id_state == 1:
								self.write("WARNING : Robot.go_point : detection objet", colorConsol.WARNING)
								self.addCmd(ID_ASSERV, Q_STOP)
								self._volerPion(id_pince)
								fifo.clear()
								self.addCmd(ID_ASSERV, Q_GOAL_ABS, c.x, c.y, VITESSE)
						elif m.id_cmd == Q_POSITION:
							new_pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
							if self._anomalie_deplacement_delta((x,y), last_pos, new_pos):
								self.write("WARNING : Robot.go_point : detection anomalie deplacement", colorConsol.WARNING)
								nb_anomalies += 1
								if nb_anomalies > 2:
									self.write("WARNING : Robot.do_path : detection anomalie deplacement critique", colorConsol.FAIL)
									self.addCmd(ID_ASSERV, Q_STOP)
									retour = E_BLOCK
									break
							self.pos = new_pos
							self.debug.log(D_UPDATE_POS,self.pos)
							nb_anomalies = 0
						elif time.time() > self.time_begin + DELAY_PINGS and (m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR):
							pos_adv = self.get_pos_adv(m)
							if (300 < pos_adv[0] < 2700) and (300 < pos_adv[1] < 1800):
								self.write("WARNING : Robot.go_point : detection adversaire", colorConsol.WARNING)
								self.addBlockingCmd(1, 5, ID_ASSERV, Q_STOP)
								self.tourne(self.pos[2] + 90)
								self.client.removeFifo(fifo)
								break
						elif m.id_cmd == Q_KILL:
							raise KillException("Q_KILL")
				except KillException as ex:
					self.write(ex, colorConsol.FAIL)
					retour = Q_KILL
					break
				except TimeoutException as ex:
					self.write(ex, colorConsol.FAIL)
					retour = E_TIMEOUT
				except Exception as ex:
					self.write(str(traceback.print_tb(sys.exc_info()[2])) + "\n" + str(ex), colorConsol.FAIL)
				finally:
					self.client.removeFifo(fifo)

		self.addCmd(ID_OTHERS, Q_ULTRAPING, -1)
		if fifo: self.client.removeFifo(fifo)



	def _volerPion(self, id_pince):
		"""
		@todo décommenter
		(script IA)
		Un pion est tombé par hasard dans nos pinces ? on le prend !
		"""
		self.addBlockingCmd(1, 3, ID_AX12, Q_OPEN_MAX, id_pince)
		self.takeObj(id_pince) # prend le pion
		case = self._findNearCaseToDump()
		if not case:
			self.write("no case", colorConsol.WARNING)
			return
		self.update_pos()
		l = Line(Vec2(case.x,case.y), Vec2(self.pos[0],self.pos[1]))
		pos_to_dump = l.pointFrom(130) # position pour poser le pion
		pos_recul = l.pointFrom(400)
		self.debug.log(D_DELETE_PATH)
		self.debug.log(D_SHOW_PATH,((self.pos[0],self.pos[1]),(pos_to_dump[0],pos_to_dump[1])))
		teta = l.teta + pi # angle de nous vers la case
		#raw_input("tourne")
		if id_pince == ARRIERE:
			self.tourne(degrees(teta) + 180)
		else:
			self.tourne(degrees(teta))
		#raw_input("go pos to dump")
		self.go_point(pos_to_dump[0],pos_to_dump[1], 2)
		#raw_input("dump")
		self.dumpObj(id_pince)
		#raw_input("reculer")
		self.go_point(pos_recul[0],pos_recul[1], 2)

		
	def _findNearCaseToDump(self):
		"""
		@todo décommenter le update_pos
		Trouver une case proche de nous où l'on peut poser un pion
		"""
		self.update_pos()
		limite = 351
		nearCases = []
		nearCase = None
		d_near = 1E10
		
		for x,column in enumerate(CASES):
			if abs(column[0].x-self.pos[0]) < limite:
				for y,case in enumerate(column):
					if abs(case.y-self.pos[1]) < limite:
						if case.color == self.color:
							nearCases.append(case)

		# recherche d'une case vide
		for case in nearCases:
			if case.empty():
				l = Line(Vec2(case.x,case.y), Vec2(self.pos[0],self.pos[1]))
				if len(l) < d_near:
					nearCase = case
					d_near = len(l)

		# si aucune n'est vide tant pis on bourine
		if not nearCase:
			for case in nearCases:
				l = Line(Vec2(case.x,case.y), Vec2(self.pos[0],self.pos[1]))
				if len(l) < d_near:
					nearCase = case
					d_near = len(l)
					
		return nearCase


	def script_i_scan(self):
		"""
		(script IA)
		Script utilisant les caméras smartphone
		"""
		pass
		
	####################################################################
	#							TOOLS								   #
	####################################################################
					
	def addCmd(self, id_device, id_cmd, *args):
		"""
		(non blockant)
		Envoyer une commande
		
		@param id_device id du device (asserv, cam,...)
		@param id_cmd id de la commande
		@param args les éventuels arguments
		
		@return id_msg
		"""
		self._lock_id_msg.acquire()
		# création du message
		msg = str(id_device)+C_SEP_SEND+str(self.id_msg)+C_SEP_SEND+str(id_cmd)
		for a in args: msg += C_SEP_SEND+str(a)
		
		# sauvegarde de la commande
		self.cmd[self.id_msg] = id_cmd
		
		# envoie
		self.client.send(msg)
		
		# increment de id_msg
		self.id_msg += 1
		if self.id_msg >= MAX_MSG:
			self.id_msg = 0
		
		# return de id_msg
		if self.id_msg==0:
			self._lock_id_msg.release()
			return 0
		else:
			self._lock_id_msg.release()
			return (self.id_msg-1)


	def addBlockingCmd(self, nb_msg, timeout, id_device, id_cmd, *args):
		"""
		(blockant)
		Envoyer une commande et attendre la réponse
		
		@raise TimeoutException
		
		@param timeout le timeout, None si complétement bloquant, une liste si plusieurs messages attendus
		@param nb_msg le nombre de messages attendus
		@param id_device id du device (asserv, cam,...)
		@param id_cmd id de la commande
		@param args les éventuels arguments

		@return la réponse ou None si timeout
		"""
		fifo = self.client.addFifo( MsgFifo(id_cmd) )
		self.addCmd(id_device, id_cmd, *args)
		if nb_msg > 1:
			reponse = []
			for i in xrange(nb_msg):
				r = fifo.getMsg(timeout[i])
				self.write("addBlockingCmd %s %s"%(nb_msg,r))
				reponse.append(r)
		elif nb_msg == 1:
			reponse = fifo.getMsg(timeout)
		else: reponse = None
		self.client.removeFifo(fifo)
		return reponse
		
	
	def write(self, msg, color=None):
		""" pour écrire sans se marcher sur les doigts """
		self._lock_write.acquire()
		try:
			if color: print color+str(msg).strip()+colorConsol.ENDC
			else: sys.stderr.write(str(msg).strip()+"\n")
		finally:
			self._lock_write.release()

			
	def symX(self,x):
		"""
		(calcul)
		Fonction ne s'appliquant que si la couleur est ROUGE
		Symétrique par rapport au milieu du terrain
		Fonction à appliquer à tous les points en dur
		@param x (mm) abscisse
		@return symétrique si couleur ROUGE
		"""
		if self.color == RED:
			return 3000 - x
		else:
			return x

	def symA(self,a):
		"""
		(calcul)
		Fonction ne s'appliquant que si la couleur est ROUGE
		Symétrique par rapport au milieu du terrain
		Fonction à appliquer à tous les points en dur
		@param a (degrès) l'angle
		@return symétrique si couleur ROUGE
		"""
		if self.color == RED:
			return 180 - a
		else:
			return a

	def symC(self,Cx):
		"""
		(calcul)
		retourne l'abscisse de la CASES opposée
		"""
		if self.color == RED:
			return len(CASES) - 1 - Cx
		else:
			return Cx
	
	def otherColor(self, c):
		"""
		Renvoie la couleur adverse de (c)

		@param c (int) la couleur
		@return (int) la color opposée
		"""
		if c == BLUE:
			return RED
		else:
			return BLUE

	def pos_rel(self, pos, t, d):
		return (int(pos[0] + (cos(t) * float(d))), int(pos[1] + (sin(t) * float(d))))

	def angle_diff(self, a,b):
		return abs(atan2(sin(a-b), cos(a-b)))
	
	def sens_next_cible(self, cible):
		"""
		Le sens dans lequel va avancer le robot pour la prochaine cible
		"""
		self.write("* sens_next_cible begin *", colorConsol.HEADER)
		self.update_pos()
		l = Line(Vec2(self.pos[0],self.pos[1]), Vec2(cible[0],cible[1]))
		if self.angle_diff(l.teta, radians(self.pos[2])) > pi/2.0:
			retour = ARRIERE
		else:
			retour =  AVANT
		self.write("* sens_next_cible end *", colorConsol.HEADER)
		return retour

	def get_pos_adv(self, message):
		content_split = [ int(_) for _ in message.content.split(C_SEP_SEND) ]
		if GAUCHE == content_split[0]:
			teta = 40
		elif DROITE == content_split[0]:
			teta = -40
		else:
			teta = 0
		if message.id_cmd == W_PING_AR:
			teta += 180
		self.update_pos()
		pos_adv = self.pos_rel(self.pos, radians(teta), (content_split[0]+10)*10)
		return pos_adv
	
if __name__ == '__main__':
	robot = Robot()
	robot.start()
	
