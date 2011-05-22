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



"""
##############################
##		DEBUG/RELEASE		##
##############################
RELEASE		= 0
DEBUG		= 1
MOD			= DEBUG

##############################
##			IMPORTS			##
##############################
from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *
import time
import math
import threading
import sys
from debugClient import *
from pathfinding import *
from geometry.vec import *
from geometry.circle import *


MAX_MSG		= 10000
if MOD == RELEASE:
	VITESSE		= 200
	CONN_MOD	= MOD_CIN
	DEBUG_LVL	= 0
	AGE_MAX		= 10 # si un pion n'a pas été vu par un scan au bout de AGE_MAX, c'est qu'il ne doit plus être sur la carte
else:
	VITESSE 	= 130
	CONN_MOD	= MOD_TCP
	DEBUG_LVL	= 1
	AGE_MAX		= 10


# constantes de recalage
RECAL_X		= 3
RECAL_Y		= 4
RECAL_A		= 5

# constantes pour prendre les pions dans le vert
X_PRISE = 300
X_DEPLACEMENT = 500

class Robot:
	def __init__(self):
		"""
		Lorsque le robot envoie un message, il envoi aussi un id_msg compris entre 0 et MAX_MSG
		Lorsqu'il reçoit un message, il sait de qui il provient grâce au server,
		il retrouve ensuite à quelle commande il correspond grâce à l'id_msg. (opération effectuée dans RobotClient)
		"""
		self._lock_write = threading.Lock()
		self.client = RobotClient(self,CONN_MOD) # client pour communiquer avec le serveur
		self._e_stop = threading.Event() # pour arreter le robot
		self.debug = Debug(DEBUG_LVL) # pour debugger
		threading.Thread(None, self._loopReset, "Robot._loopReset()").start()
		
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
		self.write("CONN_MOD : %s"%CONN_MOD, colorConsol.OKBLUE)
		self.write("DEBUG_LVL : %s"%DEBUG_LVL, colorConsol.OKBLUE)
		self.write("AGE_MAX : %s"%AGE_MAX, colorConsol.OKBLUE)
		
		self.pinces = (Pince(), Pince())
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self.pos = (0,0,0)
		self.color = RED
		
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0

		self.client.start() # demarrage du client
		
	def _loopReset(self):
		fifo = self.client.addFifo( MsgFifo( W_SWITCH_COLOR ) )
		while True:
			fifo.getMsg()
			self.addCmd(ID_OTHERS, Q_LED, -1)
			self.reset()
			self.client.addFifo(fifo)
	
	def addCmd(self, id_device, id_cmd, *args):
		"""
		(non blockant)
		Envoyer une commande
		
		@param id_device id du device (asserv, cam,...)
		@param id_cmd id de la commande
		@param args les éventuels arguments
		
		@return id_msg
		"""
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
		return 0 if self.id_msg==0 else (self.id_msg-1)

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
				reponse.append(fifo.getMsg(timeout[i]))
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
	
	def start(self):
		"""
		Démarage du robot
		"""
		while True:
			while self._e_stop.isSet():
				time.sleep(0.5)
			if MOD == DEBUG:
				"""self.color = BLUE
				self.addBlockingCmd(2, (0.5,None), ID_ASSERV, Q_AUTO_CALIB, self.color)
				self.do_path(((self.symX(1850),300),))
				while True:
					self.do_path(((self.symX(1850),700),(self.symX(800),700),(self.symX(800),1400),(self.symX(1850),1400)))
				continue
				self.preparation()"""
				if self.preparation() >= 0:
					self.write("* CALIBRATION MANUELLE *", colorConsol.HEADER)
					self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 1150, 700, 0)
					self.write("")
					self.construireTourVerte()
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
			"""else:
				r = self.preparation()
			self.write("preparation %s"%r)
			if r >= 0:
				if MOD == RELEASE: threading.Timer(88, self.stop, ("90s !",)).start()
				if self.color == BLUE:
					self.do_path(((1000,300),))
				else:
					self.do_path(((2000,300),))
				self.addBlockingCmd(1, 10, ID_ASSERV, Q_ANGLE_ABS, 90, VITESSE-30)

				
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
					self.addBlockingCmd(2, (0.5,5), ID_ASSERV, Q_ANGLE_REL, 30, VITESSE - 30)
					self.write("")"""

	def preparation(self):
		"""
		(blockant)
		Ce qui doit etre fait pendant les 3 minutes
		Choix de la couleur
		Recalage

		@return 0 si ok, <0 sinon
		"""
		self.write('preparation')
		retour = 0
		# pour recevoir les messages
		fifo = self.client.addFifo( MsgFifo(W_JACK, Q_KILL, Q_COLOR, Q_AUTO_CALIB, Q_PRECAL) )
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
					

			"""self.write("* JACK POUR RECALAGE *")
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
					break"""
			
			loop1.stop()
			loop2.stop()
			loop1.join()
			loop2.join()
			self.addCmd(ID_OTHERS, Q_LED, self.color)
			self.update_pos()
		
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
			self.write(ex, colorConsol.FAIL)
			retour = -101
		finally:
			self.client.removeFifo(fifo)
			loop1.stop()
			loop2.stop()
			loop1.join()
			loop2.join()
			return retour
		
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
		
	def stop(self, msg=None):
		""" arret du robot """
		if msg: self.write(msg, colorConsol.FAIL)
		self.addCmd(-1, Q_KILL)
		self.addCmd(ID_ASSERV,Q_STOP)
		self.addCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -1)
		self.addCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -2)
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
		self.pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
		self.write("NEW POS : "+str(self.pos))
		self.client.removeFifo(fifo)
		self.debug.log(D_UPDATE_POS, self.pos)
		#self.write("time update pos %s"%(time.time() - start))
		
	def do_path(self, path):
		"""
		(blockant)
		@todo interuption si erreur parceque la position n'est pas la bonne
		Suit un chemin
		La fonction peut tout couper si elle reçoit un message de danger de la tourelle
		ou si après avoir comparé la position actuelle du robot avec celle éstimée 
		il y a une anomalie
		
		@param path<(x,y)> la liste des points
		"""
		retour = 0
		if self._e_stop.isSet():
			retour = -42
		else:
			self.update_pos()
			
			self.debug.log(D_DELETE_PATH)
			self.debug.log(D_SHOW_PATH,path)
			
			fifo = self.client.addFifo( MsgFifo(Q_GOAL_ABS, Q_ANGLE_ABS, Q_GETSENS, Q_POSITION, Q_KILL, W_PING_AV, W_PING_AR) )

			goals = []
			for p in path:
				self.addCmd(ID_ASSERV, Q_GOAL_ABS, p[0],p[1],VITESSE)
				goals.append(p)

			nb_accuse_recep = 0
			timeLastPing = 0
			inPause = False
			loopPosition = None
			try:
				while not self._e_stop.isSet() and nb_accuse_recep<len(path):
					if inPause and time.time() - timeLastPing > 0.5:
						self.addCmd(ID_ASSERV, Q_RESUME)
						inPause = False
					m = fifo.getMsg(0.5,"accusé de reception") # timeout de 0.5 seconde pour les accusés de receptions
					if m.id_cmd == Q_GOAL_ABS:
						nb_accuse_recep += 1
					elif m.id_cmd == Q_KILL: # arret
						raise KillException("Q_KILL")
					elif m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR:
						if not inPause :
							self.addCmd(ID_ASSERV, Q_PAUSE)
							self.write("WARNING : Robot.do_path : detection adversaire", colorConsol.WARNING)
							inPause = True
						timeLastPing = time.time()
				
				self.write("Tous les accusés de receptions reçus")

				self.addCmd(ID_ASSERV, Q_GETSENS)
				loopPosition = LoopCmd(self, 1, 1, ID_ASSERV, Q_POSITION)
				loopPosition.start()
				
				nb_point_reach = 0
				while not self._e_stop.isSet() and nb_point_reach<len(path):
					m = fifo.getMsg(2,"do_path 2ème partie")
					if inPause and time.time() - timeLastPing > 0.5:
						self.addCmd(ID_ASSERV, Q_RESUME)
						inPause = False
					if m.id_cmd == Q_GOAL_ABS:
						self.addCmd(ID_ASSERV, Q_GETSENS)
						nb_point_reach += 1
					elif m.id_cmd == Q_GETSENS:
						self.addCmd(ID_OTHERS, Q_ULTRAPING, -1)
						self.addCmd(ID_OTHERS, Q_ULTRAPING, -2)
						self.addCmd(ID_OTHERS, Q_ULTRAPING, m.content)
					elif m.id_cmd == Q_KILL: # arret
						raise KillException("Q_KILL")
					elif m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR:
						if not inPause:
							self.addCmd(ID_ASSERV, Q_PAUSE)
							self.write("WARNING : Robot.do_path : detection adversaire", colorConsol.WARNING)
							inPause = True
						timeLastPing = time.time()
					elif m.id_cmd == Q_POSITION:
						new_pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
						self.debug.log(D_UPDATE_POS, new_pos)
						if 	abs(new_pos[0] - path[nb_point_reach][0]) > 50 \
							and abs(new_pos[1] - path[nb_point_reach][1]) > 50 \
							and abs(new_pos[0] - self.pos[0]) < 30 \
							and abs(new_pos[1] - self.pos[1]) < 30 \
							and abs(new_pos[2] - self.pos[2]) < 5:
							self.write("WARNING : Robot.do_path : detection anomalie deplacement", colorConsol.WARNING)
							#self.addCmd(ID_ASSERV, Q_STOP)
							#retour = E_BLOCK
							#break
						self.pos = new_pos
			except KillException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = Q_KILL
			except TimeoutException as ex:
				self.write(ex, colorConsol.FAIL)
				retour = E_TIMEOUT
			except Exception as ex:
				self.write(ex, colorConsol.FAIL)
			finally:
				# arret de la récupération en boucle de la position
				if loopPosition: loopPosition.stop()
				# destruction de la fifo
				self.client.removeFifo(fifo)
				# arret de l'écoute des pings
				self.addCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -1)
				self.addCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -2)

		return retour

	def go_point(self, x,y):
		return self.do_path(((x,y),))
			
	
	def scan(self, fast=False):
		"""
		(blockant)
		@todo gestion si la réponse n'arrive pas assez vite
		demande un scan à la camera, fait toujours deux tentatives
		Met à jourla liste des pions actuelle

		"""
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
		
	def dumpObj(self, id_pince):
		"""
		(blockant)
		ouvrir la pince
		@param id_pince (int) AVANT ou ARRIERE
		"""
		if id_pince == AVANT:
			return self.addBlockingCmd(1, 2, ID_AX12, Q_OPEN_MAX_AV)
		else:
			return self.addBlockingCmd(1, 2, ID_AX12, Q_OPEN_MAX_AR)

	
	def takeObj(self, id_pince):
		"""
		(blockant)
		ramasser un objet (serre et remonte)
		@param target (Pion)
		@return True si l'objet a été pris, False sinon
		"""
		self.addBlockingCmd(2, (1,10), ID_OTHERS, Q_PRECAL, id_pince)
		if id_pince == AVANT:
			self.addBlockingCmd(1, 2, ID_AX12, Q_SERRE_AV)
		else:
			self.addBlockingCmd(1, 2, ID_AX12, Q_SERRE_AR)

	def construireTourVerte(self):
		"""
		@todo reculer en même temps qu'on lève les pinces à certains moment
		Script pour construire une tour à partir de ce qu'il y a dans la zone verte
		"""
		self.write("* CONSTRUCTION TOUR VERTE *", colorConsol.HEADER)
		listeVerte = (TOUR,PION_1,TOUR,PION_1,TOUR)
		listeYVerte = (690,970,1250,1530,1810)

		nbTours = 0
		for p in listeVerte[:3]:
			if p == TOUR: nbTours += 1
		if nbTours < 2:
			debut = 0
		else:
			debut = 1
		
		# prise du premier pion avec pince AVANT
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[debut]) 	# devant le premier pion
		self._takePionVert(debut, AVANT)
		self.write("1 ére tour prise", colorConsol.OKGREEN)

		# prise du deuxième pion avec pince ARRIERE
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[debut+1]) 	# devant le deuxième
		self._takePionVert(debut+1, ARRIERE)
		self.write("2 ème tour prise", colorConsol.OKGREEN)

		# prise du troisième
		if listeVerte[debut] == PION_1 and listeVerte[debut+1] == PION_1:
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(90), VITESSE-30)
			self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[debut+2]) # devant le troisième
			self.dumpObj(ARRIERE)
			self.addBlockingCmd(2, (1, 10), ID_OTHERS, Q_SETPOSITION, ARRIERE, 9500)
			self._takePionVert(debut+2, ARRIERE) # prise troisième pion
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(90), VITESSE-30)
			self.dumpObj(ARRIERE) # lache le pion à l'arrière (tour) sur le pion
			self.takeObj(ARRIERE) # reprend (pion + tour)
			self.addBlockingCmd(2, (1, 10), ID_OTHERS, Q_SETPOSITION, ARRIERE, 9500)
			self.dumpObj(AVANT) # lache le pion à l'avant
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_REL, 180, VITESSE-30) # demi tour
			self.dumpObj(ARRIERE) # lache la tour et le pion
			self.takeObj(ARRIERE) # 2 pions + 1 tour AVANT
		else:
			if listeVerte[debut] == TOUR:
				angle = 90
				id1 = ARRIERE
				id2 = AVANT
			else:
				angle = -90
				id1 = AVANT
				id2 = ARRIERE
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(angle), VITESSE-30)
			self.dumpObj(id1) # pose le pion
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_REL, 180, VITESSE-30) # tourne
			self.dumpObj(id2) # pose la tour sur le pion
			self.takeObj(id2) # un pion + une tour dans la pince id2
			self.addBlockingCmd(2, (1, 10), ID_OTHERS, Q_SETPOSITION, id2, 9500)
			self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[debut+2]) # devant le troisième
			self._takePionVert(debut+2, id1) # prise troisième pion pince id1
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(angle), VITESSE-30)
			self.dumpObj(id1)
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_REL, 180, VITESSE-30) # demi tour
			self.dumpObj(id2)
			self.takeObj(id2) # 2 pions + 1 tour AVANT
		self.write("3 ème tour prise", colorConsol.OKGREEN)
			
				
	def _takePionVert(self, index, id_pince):
		listeYVerte = (690,970,1250,1530,1810)
		retour = 0
		if id_pince == ARRIERE:
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(0), VITESSE-30)
			self.addBlockingCmd(1, 2, ID_AX12, Q_OPEN_MAX_AR)
		else:
			self.addBlockingCmd(2, (1,5), ID_ASSERV, Q_ANGLE_ABS, self.symA(180), VITESSE-30)
			self.addBlockingCmd(1, 2, ID_AX12, Q_OPEN_MAX_AV)
		self.addBlockingCmd(2, (1, 10), ID_OTHERS, Q_PRECAL, id_pince)
		
		self.go_point(self.symX(X_PRISE),listeYVerte[index]) # avance
		self.takeObj(id_pince)
		fifo = self.client.addFifo( MsgFifo(Q_SETPOSITION) )
		self.addCmd(ID_OTHERS, Q_SETPOSITION, id_pince, 9500) # lève la pince
		self.go_point(self.symX(X_DEPLACEMENT),listeYVerte[index]) # recul en même temps
		nb_reception = 0
		try:
			while True:
				m = fifo.getMsg(10)
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
			
		return retour
		

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


		
if __name__ == '__main__':
	robot = Robot()
	robot.start()
	
