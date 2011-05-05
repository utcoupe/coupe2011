# -*- coding: utf-8 -*-
"""
@todo interupts danger, 90s pour la majorité des fonctions


"""

from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *
from time import sleep
import math
from debugClient import *
from pathfinding import *
from geometry.vec import *
from geometry.circle import *



MAX_MSG		= 50
VITESSE 	= 130

# constantes de recalage
RECAL_X		= 3
RECAL_Y		= 4
RECAL_A		= 5

class Robot:
	def __init__(self):
		"""
		Lorsque le robot envoie un message, il envoi aussi un id_msg compris entre 0 et MAX_MSG
		Lorsqu'il reçoit un message, il sait de qui il provient grâce au server,
		il retrouve ensuite à quelle commande il correspond grâce à l'id_msg,
		il active alors deux events : celui self.events[id_device][id_cmd] et celui
		self.msg_events[id_msg]
		"""
		self.pinces = (Pince, Pince)
		self.client = RobotClient(self)
		
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self.pos = (0,0,0)
		self.target_pos = (-1,-1) # la position du pion visé
		
		self._e_stop = threading.Event()
		
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0
		
		self.debug = Debug()
	
	def addCmd(self, id_device, id_cmd, *args):
		"""
		Envoyer une commande
		(blockant)
		
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
		Envoyer une commande et attendre la réponse
		(blockant)
		
		@param timeout le timeout, None si complétement bloquant, une liste si plusieurs messages attendus
		@param nb_msg le nombre de messages attendus
		@param id_device id du device (asserv, cam,...)
		@param id_cmd id de la commande
		@param args les éventuels arguments

		@return la réponse ou None si timeout
		"""
		fifo = self.client.addFifo( MsgFifo(id_cmd) )
		self.addCmd(id_device, id_cmd, args)
		if nb_msg == 1:
			reponse = []
			for i in xrange(nb_msg):
				reponse.append(fifo.getMsg(timeout[i]))
		elif nb_msg > 1:
			reponse = fifo.getMsg(timeout)
		else: reponse = None
		self.client.removeFifo(fifo)
		return reponse
		
	
	def write(self, msg):
		self.client.write(msg)
	
	def start(self):
		""" démarage du robot """
		self.client.start()

		self.write("* LE ROBOT DÉMARRE *")
		self.write("* CALIBRATION MANUELLE *")
		self.addCmd(ID_ASSERV, Q_MANUAL_CALIB, (1500,1050,0))
		self.write("* UPDATE POS *")
		self.update_pos()
		
		"""
		# les pinces en haut
		self.addCmd(ID_OTHERS, Q_PPOSITION, (0,1000))
		
		# demande de position
		self.update_pos()
		
		# calibrage
		fifo = self.client.addFifo( MsgFifo(Q_AUTO_CALIB) )
		self.addCmd(ID_ASSERV, Q_AUTO_CALIB, (RED,))
		r = fifo.getMsg() # accusé de reception
		r = fifo.getMsg()
		self.write(r)
		self.client.removeFifo(fifo)
		
		# sortie
		self.write("c'est parti")
		
		"""
		
		self.write("scan know !")
		sleep(0.5)
		
		# scan
		pions = self.scan()
		
		
		if pions:
			target = self.treatScan(pions)
			
			self.write("cible : %s"%target)
			
	
	def stop(self, msg=None):
		""" arret du robot """
		if msg: self.write(msg)
	
	def reset(self):
		""" reset du robot
		self.client, self.pos, self.pinces ne sont pas remis à zéro
		"""
		self.stop()
		self.target_pos = (-1,-1) # la position du pion visé
		# clear des events
		for events in self.events:
			for e in events:
				e.clear()
		self.id_msg = 0
		# clear des pions
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self._e_stop.clear()
		
	def isEmpty(self):
		return (self.pinces[0].isEmpty() and self.pinces[1].isEmpty())
	
	def isFull(self):
		return (self.pinces[0].isFull() and self.pinces[1].isFull())
	
	def update_pos(self):
		"""
		Récupérer la position actuelle
		(bloquant)
		"""
		fifo = self.client.addFifo( MsgFifo(Q_POSITION) )
		self.addCmd(ID_ASSERV, Q_POSITION)
		m = fifo.getMsg()
		self.pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
		self.write(self.pos)
		self.client.removeFifo(fifo)
		self.debug.log(D_UPDATE_POS, self.pos)
		
	def do_path(self, *path):
		"""
		@todo interuption danger, interuption 90s, interuption si erreur parceque la position n'est pas la bonne
		suit un chemin
		La fonction peut tout couper si elle reçoit un message de danger de la tourelle
		ou si après avoir comparé la position actuelle du robot avec celle éstimée 
		il y a une anomalie
		
		@param path la liste des commandes à envoyer à l'asserv sous la forme (cmd, args)
		"""

		goals = []
		for p in path:
			self.addCmd(ID_ASSERV, Q_GOAL_ABS, (p[0],p[1],VITESSE))
			goals.append(p)

		self.debug.log(D_DELETE_PATH, goals)
		self.debug.log(D_SHOW_PATH, goals)

		fifo = self.client.addFifo( MsgFifo(Q_GOAL_ABS, Q_ANGLE_ABS) )
		for i in xrange(len(path)):
			if not fifo.getMsg(0.5): # timeout de 1 seconde pour les accusés de receptions
				self.write("ERROR : Robot.do_path : accusé de reception non reçu")
				return
		else: # si on arrive à la fin
			self.write("Tous les accusés de receptions reçus")
		
		for i in xrange(len(path)):
			fifo.getMsg()
			
		self.client.removeFifo(fifo)
		
		
	def scan(self):
		"""
		@todo gestion si la réponse n'arrive pas assez vite
		demande un scan à la camera, fait toujours deux tentatives

		@return le liste des pions dans le repère absolue avec les pions hors table enlevés
		"""
		l = []

		# pour écouter la réponse de la cam
		fifo = self.client.addFifo( MsgFifo(Q_SCAN_AV, Q_SCAN_AR) )
		
		# première tentative
		self.addCmd(ID_CAM, Q_SCAN_AR)
		l = eval(str(fifo.getMsg().content))
		self.write(l)
			
		# deuxième tentative
		if not l:
			self.addCmd(ID_CAM, Q_SCAN_AR)
			l = eval(str(fifo.getMsg().content))
			self.write(l)

		# arreter d'écouter
		self.client.removeFifo(fifo)

		# récupération de la position
		self.update_pos()
		self.write(l)
		# transformation des valeurs
		Cx = 120
		Cy = -110
		l = self._changeRepere(self.pos[0],self.pos[1],self.pos[2],Cx,Cy,l)
		self.write(l)
		#l = self._filtreInMap(l)
		self.write(l)
		
		self.write(str(l))
		self.debug.log(D_PIONS, l)
		
		return l
		
	def _changeRepere(self, Rx, Ry, Ra, Cx, Cy, l):
		"""
		Effectue le changement de repère pour la réponse de la caméra
		"""
		cosa = math.cos(math.radians(float(Ra)))
		sina = math.sin(math.radians(float(Ra)))
		def f(p):
			p[1] = int(cosa * float(p[1]) - sina * float(p[2])) + Rx + Cx
			p[2] = int(sina * float(p[1]) + cosa * float(p[2])) + Ry + Cy
			return p
		return map(lambda p: f(p), l)
			
	def _filtreInMap(self, l):
		"""
		Filtre les pions hors carte
		"""
		return filter(lambda p: (0 < p[1] < 3000) and (0 < p[2] < 2100), l)

	def treatScan(self,pions):
		""" traitement du scan de la carte avec notre position actuelle 
		@return target,path
		"""
		# algo de détermination quel pion prendre (probablement le plus proche)
		# pathfinding pour esquiver nos pions
		# retourne la cible choisie et le path
		# self.client.send("asserv", Q_GOAL_ABS, <devant le pion>)
		# self.client.Send("asserv", Q_ANGLE_ABS, <faceau pion>)
		if pions:
			def cmp(p1,p2):
				x1,y1 = p1[1],p1[1]
				x2,y2 = p2[1],p2[1]
				
				v1 = abs(x1-self.pos[0])**2+abs(y1-self.pos[1])**2
				v2 = abs(x2-self.pos[0])**2+abs(y2-self.pos[1])**2
				
				return v1 - v2
				
			pions.sort(cmp=lambda p1,p2: cmp(p1,p2))
			
			target = Target(pions[0][1], pions[0][2], pions[0][0])
			
			circles = []
			
			for p in pions:
				circles.append(Circle(Vec2(p[1],p[2]), 400))
			
			path = find_path(Vec2(self.pos[0],self.pos[1]), Vec2(target[1],target[2]), circles)
			
			return target, path
		else:
			return None,None
		
	def dumpObj(self):
		""" ouvrir la pince """
		pass
	
	def takeObj(self, target):
		""" ramasser un objet
		@return True si l'objet a été pris, False sinon
		"""
		# récupération de la position
		self.update_pos()

		# détection du sens des pinces par rapport à l'objet
		if (target.pos.x-self.pos[0]) == 0:
			teta = math.pi
		else:
			teta = math.atan(float(target.pos.y-self.pos[1]) / float(target.pos.x-self.pos[0]))

		if target.pos.x-self.pos[0] < 0: teta += math.pi

		teta = math.degrees(teta)
		if abs(self.pos[2] - teta) < 90:
			id_pince_av, id_pince_ar = PINCE_AV, PINCE_AR
		else:
			id_pince_av, id_pince_ar = PINCE_AR,PINCE_AV
		
		# demander une identification de l'objet
		id_objet = self.addBlockingCmd(1, 1, ID_OTHERS, Q_PION, id_pince_av)
		if not id_objet: # timeout
			self.write("Robot->takeObj : timeout identification de l'objet")
			return False
		
		# checker si on peut le prendre avec la pince avant ou pince arrière
		if self.pinces[id_pince_av].canAccept(id_objet):
			id_pince = id_pince_av
		elif self.pinces[id_pince_ar].canAccept(id_objet):
			id_pince = id_pince_ar
		else:
			# si aucune pince : abandon
			self.write("Robot->takeObj : aucune pince libre")
			return False

		# si pince arrière demi tour
		if id_pince == id_pince_ar:
			r = self.addBlockingCmd(2, (0.5,3), ID_ASSERV, Q_ANGLE_REL, 90, VITESSE-50)
		
		# avance vers l'objet
		r = self.addBlockingCmd(2, (0.5,5), ID_ASSERV, Q_GOAL_ABS, target.pos.x, target.pos.y, VITESSE)
		if not r[1]: # timeout
			self.write("Robot->takeObj : timeout de l'asserv")
			return False
			
		# baisse les pinces
		r = self.addBlockingCmd(2, (0.5,5), ID_OTHERS, Q_PPOSITION, id_pince, 300)
		if not r[1]: # timeout
			self.write("Robot->takeObj : timeout des pinces")
			return False
			
		# ouvre la pince
		r = self.addBlockingCmd(2, (0.5,5), ID_OTHERS, Q_PINCE, id_pince, PINCE_OUVERT)

		# baisse les pinces
		# fermer la pince
		return True
	
if __name__ == '__main__':
	robot = Robot()	
	robot.start()
	
