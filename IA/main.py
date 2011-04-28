# -*- coding: utf-8 -*-


from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *
from time import sleep
import math

MAX_MSG		 = 50


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
		self.pos = (0,0)
		self.target_pos = (-1,-1) # la position du pion visé
		
		self._e_stop = threading.Event()
		
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0
	
	
	def addCmd(self, id_device, id_cmd, args=[""]):
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
		vitesse = 160
		
		fifo = self.client.addFifo( MsgFifo(((ID_ASSERV, Q_GOAL_ABS),)) )
		self.do_path((Q_GOAL_ABS, (1000,300,vitesse)),
					 (Q_ANGLE_ABS, (90,vitesse-60)),
					 (Q_GOAL_ABS, (1000,500,vitesse)),
					 (Q_ANGLE_ABS, (45,vitesse-60)),
					)
		
		r = fifo.getMsg()
		r = fifo.getMsg()
		r = fifo.getMsg()
		r = fifo.getMsg()
		self.client.removeFifo(fifo)
		
		sleep(1)
		
		# scan
		self.scan()
		
		
	
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
		r = fifo.getMsg()
		self.write(r)
		self.pos = r
		self.client.removeFifo(fifo)
		
	def do_path(self, *path):
		""" suit un chemin
		La fonction peut tout couper si elle reçoit un message de danger de la tourelle
		ou si après avoir comparé la position actuelle du robot avec celle éstimée 
		il y a une anomalie
		
		@param path la liste des commandes à envoyer à l'asserv sous la forme (cmd, args)
		"""
		goals = []
		for cmd,args in path:
			self.addCmd(ID_ASSERV, cmd, args)
			goals.append(args)
		
		
	def scan(self):
		"""
		demande un scan à la camera
		"""
		fifo = self.client.addFifo( MsgFifo(((ID_CAM, Q_SCAN_AV),)) )
		self.addCmd(ID_CAM, Q_SCAN_AV)
		#r = fifo.getMsg()
		l = eval(str(fifo.getMsg()))
		
		self.update_pos()
		Cx = 120
		Cy = -110
		
		if l:
			l = self.changeRepere(self.pos[0],self.pos[1],self.pos[2],Cx,Cy,l)
			
		self.write(str(l))
		
	def changeRepere(self, Rx, Ry, Ra, Cx, Cy, l):
		result = []
		cosa = math.cos(math.radians(float(Ra)))
		sina = math.sin(math.radians(float(Ra)))
		for t,x,y in l:
			nx = cosa * x - sina * y + Rx + Cx
			ny = sina * x + cosa * y + Ry + Cy
			result.append([t,nx,ny])
		return result
			
	
	def takeObject(self, target):
		"""
		
		"""
		pass
	
	def onEmpty(self):
		""" quand le robot est vide """
		# demande à la camera
		pass
	
	def onFull(self):
		""" quand le robot est plein """
		# aller poser sur des cases
		pass
	
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
		
	def checkAsserv(self, new_pos):
		""" check si l'asserv s'en sort bien 
		@todo coder la fonction si necessaire
		"""
		pass
	
	def treatScan(self):
		""" traitement du scan de la carte avec notre position actuelle 
		@return target,path
		"""
		# algo de détermination quel pion proche (probablement le plus proche)
		# pathfinding pour esquiver nos pions
		# retourne la cible choisie et le path
		# self.client.send("asserv", Q_GOAL_ABS, <devant le pion>)
		# self.client.Send("asserv", Q_ANGLE_ABS, <faceau pion>)
		pass
	
	def onWarning(self, mag):
		""" quand il y a danger """
		# definir une stratégie de coutournement
		# OU
		# nouveau scan, mais seulement derière
		pass
	
	def onStop(self):
		""" quand le robot doit s'arreter """
		self.stop()
	
	def onStart(self):
		""" quand le robot doit demarrer """
		self.start()
	
	def onReset(self):
		""" quand le robot doit se reseter """
		self.reset()
	
if __name__ == '__main__':
	robot = Robot()	
	robot.start()
	
