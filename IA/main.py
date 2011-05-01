# -*- coding: utf-8 -*-


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
vitesse 	= 160


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
	
	
	def addCmd(self, id_device, id_cmd, args=[]):
		"""
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
	
	def write(self, msg):
		self.client.write(msg)
	
	def start(self):
		""" démarage du robot """
		self.client.start()
		
		"""
		# les pinces en haut
		self.addCmd(ID_OTHERS, Q_ASCENSEUR, (0,1000))
		
		# demande de position
		self.update_pos()
		
		# calibrage
		fifo = self.client.addFifo( MsgFifo(((ID_ASSERV, Q_AUTO_CALIB),)) )
		self.addCmd(ID_ASSERV, Q_AUTO_CALIB, (RED,))
		r = fifo.getMsg() # accusé de reception
		r = fifo.getMsg()
		self.write(r)
		self.client.removeFifo(fifo)
		
		# sortie
		self.write("c'est parti")
		
		self.do_path((Q_GOAL_ABS, (1000,300,vitesse)),
					 (Q_ANGLE_ABS, (0,vitesse-60)),
					)
		"""
		
		self.write("scan know !")
		sleep(0.5)
		
		# scan
		pions = self.scan()
		
		self.debug.log(D_PIONS, pions)
		
		if pions:
			target = self._treatScan(pions)
			
			self.write("cible : %s"%target)
			
	
	def stop(self, msg=None):
		""" arret du robot """
		if msg: self.write(msg)
		pass
	
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
		fifo = self.client.addFifo( MsgFifo(((ID_ASSERV, Q_POSITION),)) )
		self.addCmd(ID_ASSERV, Q_POSITION)
		m = fifo.getMsg()
		self.pos = tuple(int(_) for _ in m.content.split(C_SEP_SEND))
		self.write(self.pos)
		self.client.removeFifo(fifo)
		
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
		for cmd,args in path:
			self.addCmd(ID_ASSERV, cmd, args)
			goals.append(args)

		fifo = self.client.addFifo( MsgFifo(((ID_ASSERV, Q_GOAL_ABS), (ID_ASSERV, Q_ANGLE_ABS),)) )
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
		"""
		l = []

		# pour écouter la réponse de la cam
		fifo = self.client.addFifo( MsgFifo(((ID_CAM, Q_SCAN_AV),(ID_CAM, Q_SCAN_AR))) )
		
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
	
	def dumpObj(self):
		""" ouvrir la pince """
		self.addCmd(ID_OTHERS,Q_OPEN_PINCE) # ouvre la pince
		self.addCmd(CAM, Q_SCAN) # demande un scan et c'est raparti !
	
	def takeObj(self):
		""" ramasser un objet """
		# demander une identification de l'objet
		# checker si on peut le prendre avec la pince avant ou pince arrière
		# si aucune pince : abandon, demande de scan
		# si pince arrière : rotation
		# avancer vers l'objet
		# fermer la pince
	
	def _treatScan(self,pions):
		""" traitement du scan de la carte avec notre position actuelle 
		@return target,path
		"""
		# algo de détermination quel pion proche (probablement le plus proche)
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
			
			target = pions[0]
			
			circles = []
			
			for p in pions:
				circles.append(Circle(Vec2(p[1],p[2]), 400))
			
			path = find_path(Vec2(self.pos[0],self.pos[1]), Vec2(target[1],target[2]), circles)
			
			return target, path
		else:
			return None,None
	
if __name__ == '__main__':
	robot = Robot()	
	robot.start()
	
