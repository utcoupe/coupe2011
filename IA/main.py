# -*- coding: utf-8 -*-
"""
@todo interupts danger, 90s pour la majorité des fonctions


"""

from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *
import time
import math
import threading
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

M_NORMAL	= 0
M_WARNING	= 1
M_ERREUR	= 2

class Robot:
	def __init__(self):
		"""
		Lorsque le robot envoie un message, il envoi aussi un id_msg compris entre 0 et MAX_MSG
		Lorsqu'il reçoit un message, il sait de qui il provient grâce au server,
		il retrouve ensuite à quelle commande il correspond grâce à l'id_msg,
		il active alors deux events : celui self.events[id_device][id_cmd] et celui
		self.msg_events[id_msg]
		"""
		self._lock_write = threading.Lock()
		
		self.pinces = (Pince(), Pince())
		self.client = RobotClient(self)
		
		self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
		self.pos = (0,0,0)
		self.target_pos = (-1,-1) # la position du pion visé
		
		self._e_stop = threading.Event()
		
		self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
		self.id_msg = 0
		
		self.debug = Debug()

		self.color = RED
	
	def addCmd(self, id_device, id_cmd, *args):
		"""
		Envoyer une commande
		(non blockant)
		
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
			else: print str(msg).strip()
		finally:
			self._lock_write.release()
	
	def start(self):
		""" démarage du robot """
		self.client.start()

		while 1:
		self.do_path(((400,0),(0,0)))
		
		"""
		self.write("* CALIBRATION MANUELLE *")
		self.addBlockingCmd(1, 1, ID_ASSERV, Q_MANUAL_CALIB, 1850,700,180)
		self.write("")
		#self.calibCam()
		#self.testCam()
		#"""

		#while 1:
		#	self.do_path(((1150,800),(1850,700)))

		"""
		self.write("* RÉCUPÉRATION COULEUR *")
		self.color = int(self.addBlockingCmd(1, 1, ID_OTHERS, Q_COLOR).content)
		if self.color == RED:
			self.write("COULEUR ROUGE !")
		else:
			self.write("COULEUR BLEU !")
		self.addCmd(ID_OTHERS, Q_LED, self.color)
		self.write("")

		
		self.write("* JACK POUR RECALAGE *")
		r = self.addBlockingCmd(2, (0.5,None), ID_OTHERS, Q_JACK)
		self.write("* RECALAGE *")
		r = self.addBlockingCmd(2, (0.5,None), ID_ASSERV, Q_AUTO_CALIB, self.color)
		self.write("")
		
		self.update_pos()
		
		self.write("* ATTENTE DU JACK *")
		self.addCmd(ID_OTHERS, Q_LED, -1)
		r = self.addBlockingCmd(2, (0.5,None), ID_OTHERS, Q_JACK)
		self.addCmd(ID_OTHERS, Q_LED, self.color)
		self.write("ON Y VAS !")
		self.write("")
		threading.Timer(88, self.stop, ("90s !",)).start()
		self.do_path(((1500,350),))
		self.addBlockingCmd(1, 10, ID_ASSERV, Q_ANGLE_ABS, 90)
		#"""
		
		while not self._e_stop.isSet():
			self.update_pos()
			self.debug.log(D_UPDATE_POS,self.pos)
			self.write("* SCAN *")
			l = self.scan()
			self.write("")

			self.updatePions(l)
			
			self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))

			if self.pions:
				target, objectif, path = self.findTarget(self.pions)
				
				self.write("cible : %s"%target)
				self.write("objectif : %s"%objectif)
				self.write("path : %s"%path)

				if target:
					self.write("* MOVE *")
					if self.do_path(zip(path[2::2],path[3::2])):
						# on update la pos actuelle de la target
						target.pos = objectif
						self.write("objectif : %s"%objectif)
						self.write("target.pos : %s"%target.pos)
						target.calculCase()
						target.calculColor(self.color)
						self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))
					self.write("")
					raw_input("press")
					continue
					
				
			self.write("* TOURNE *")
			self.addBlockingCmd(2, (0.5,5), ID_ASSERV, Q_ANGLE_REL, 30, VITESSE - 30)
			self.write("")

	def updatePions(self, l):
		# update des pions
		pions_to_add = []
		for new_p in l:
			for p in self.pions:
				if new_p == p:
					new_p.update(p,self.color)
					break
			else:
				pions_to_add.append(new_p)
		self.pions += pions_to_add

		self.pions = filter(lambda p: p.age() < 2, self.pions)
		
		self.debug.log(D_DELETE_PATH)
		self.debug.log(D_PIONS, map(lambda p: tuple(p), self.pions))
		
	def stop(self, msg=None):
		""" arret du robot """
		if msg: self.write(msg, colorConsol.FAIL)
		self.addCmd(-1, Q_KILL)
		self.addCmd(ID_ASSERV,Q_STOP)
		self.addBlockingCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -1)
		self.addBlockingCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -2)
		self._e_stop.set()
		self.client._treat("%s.%s.%s"%(ID_IA,W_STOP,Q_KILL))
	
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
				target, objectif, path = self.findTarget(self.pions)
				
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

			if '1' == raw_input():
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
		@todo interuption danger, interuption 90s, interuption si erreur parceque la position n'est pas la bonne
		suit un chemin
		La fonction peut tout couper si elle reçoit un message de danger de la tourelle
		ou si après avoir comparé la position actuelle du robot avec celle éstimée 
		il y a une anomalie
		
		@param path<(x,y)> la liste des points
		"""
		if not self._e_stop.isSet():
			self.update_pos()
			
			fifo = self.client.addFifo( MsgFifo(Q_GOAL_ABS, Q_ANGLE_ABS, Q_GETSENS, Q_KILL, W_PING_AV, W_PING_AR) )
			
			goals = []
			for p in path:
				self.addCmd(ID_ASSERV, Q_GOAL_ABS, p[0],p[1],VITESSE)
				goals.append(p)

			nb_accuse_recep = 0
			timeLastPing = 0
			inPause = False
			while nb_accuse_recep<len(path):
				if time.time() - timeLastPing > 0.5:
					self.addCmd(ID_ASSERV, Q_RESUME)
					inPause = False
				m = fifo.getMsg(0.5) # timeout de 0.5 seconde pour les accusés de receptions
				if not m:
					self.write("ERROR : Robot.do_path : accusé de reception non reçu", colorConsol.FAIL)
					return
				if m.id_cmd == Q_GOAL_ABS:
					nb_accuse_recep += 1
				elif m.id_cmd == Q_KILL: # arret
					self.write("WARNING : Robot.do_path : arret du robot", colorConsol.WARNING)
					return
				elif m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR:
					if not inPause :
						self.addCmd(ID_ASSERV, Q_PAUSE)
						self.write("WARNING : Robot.do_path : detection adversaire", colorConsol.WARNING)
						inPause = True
					timeLastPing = time.time()
				
			self.write("Tous les accusés de receptions reçus")

			self.addCmd(ID_ASSERV, Q_GETSENS)
			
			nb_point_reach = 0
			while nb_point_reach<len(path):
				m = fifo.getMsg(0.5)
				if time.time() - timeLastPing > 0.5:
					self.addCmd(ID_ASSERV, Q_RESUME)
					inPause = False
				if m:
					if m.id_cmd == Q_GOAL_ABS:
						self.addCmd(ID_ASSERV, Q_GETSENS)
						nb_point_reach += 1
					elif m.id_cmd == Q_GETSENS:
						self.addCmd(ID_OTHERS, Q_ULTRAPING, m.content)
					elif m.id_cmd == Q_KILL: # arret
						self.write("WARINING : Robot.do_path : arret du robot")
						return
					elif m.id_cmd == W_PING_AV or m.id_cmd == W_PING_AR:
						if not inPause:
							self.addCmd(ID_ASSERV, Q_PAUSE)
							self.write("WARINING : Robot.do_path : detection adversaire", colorConsol.WARNING)
							inPause = True
						timeLastPing = time.time()
				
			self.client.removeFifo(fifo)
			# arret de l'écoute des pings
			self.addBlockingCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -1)
			self.addBlockingCmd(1, 1, ID_OTHERS, Q_ULTRAPING, -2)

			return True

			
	
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



	def findTarget(self,pions):
		"""
		(blockant)
		traitement du scan de la carte avec notre position actuelle

		@param pions liste<Pion>
		
		@return target(Pion),objectif(Vec),path([x1,y1,x2,y2,...])
		"""
		# algo de détermination quel pion prendre (probablement le plus proche)
		# pathfinding pour esquiver nos pions
		# retourne la cible choisie et le path
		# self.client.send("asserv", Q_GOAL_ABS, <devant le pion>)
		# self.client.Send("asserv", Q_ANGLE_ABS, <faceau pion>)
		self.debug.log(D_DELETE_PATH)
		if not pions:
			return None, None
			
		def plusProche(p1,p2):
			x1,y1 = p1.pos.x,p1.pos.y
			x2,y2 = p2.pos.x,p2.pos.y
			
			v1 = abs(x1-self.pos[0])**2+abs(y1-self.pos[1])**2
			v2 = abs(x2-self.pos[0])**2+abs(y2-self.pos[1])**2
			
			return v1 - v2

		# tri pour avoir les pions par proximité
		pions.sort(cmp=lambda p1,p2: plusProche(p1,p2))

		# on va maintenant chercher le path
		# comme obstacles on ne prend que nos pions
		circles = filter(lambda p: p.color == self.color, pions)
		# on remplace ces Pion par des cercles pour la fonction find_path
		circles = map(lambda p: Circle(p.pos, 300), circles)
		self.write(circles)
		
		# on cherche une cible
		for p in pions:
			if (self.color == BLUE and p.color == RED) or (self.color == RED and p.color == BLUE):
				target = p
				# on va essayer de pousser cette cible par selon l'axe 0x
				case_direction_pousse = Vec(target.case.x+350, target.case.y) if self.pos[0] < target.pos.x else Vec(target.case.x-350, target.case.y)
				print "case_direction_pousse",case_direction_pousse
				# position devant le pion
				pos_pousse = self._position_pousse(target,case_direction_pousse)
				# vérification qu'il n'y a pas un pion à nous sur la case devant ou derrière
				case_p_pousse = Vec(target.case.x-350, target.case.y) if self.pos[0] < target.pos.x else Vec(target.case.x+350, target.case.y)
				if  case_p_pousse not in map(lambda p: p.case, pions) and case_direction_pousse not in map(lambda p: p.case, pions):
					# appelle de la fonction
					path = find_path(Vec2(self.pos[0],self.pos[1]), pos_pousse, circles)
					if self._pathValid(path):
						break
				else: # tentative selon l'axe Oy
					case_direction_pousse = Vec(target.case.x, target.case.y+350) if self.pos[1] < target.pos.y else Vec(target.case.x, target.case.y-350)
					# position de pousse
					pos_pousse = self._position_pousse(target,case_direction_pousse)
					# vérif pour nos pions
					case_p_pousse = Vec(target.case.x, target.case.y-350) if self.pos[1] < target.pos.y else Vec(target.case.x, target.case.y+350)
					if case_p_pousse not in map(lambda p: p.case, pions) and case_direction_pousse not in map(lambda p: p.case, pions):
						# appelle de la fonction
						path = find_path(Vec2(self.pos[0],self.pos[1]), pos_pousse, circles)
						if self._pathValid(path):
							break
		else: # aucune cible ne convient
			return None, None, None


		case_stop = Line(case_direction_pousse, target.pos).pointFrom(case_direction_pousse,160)
		path += [target.pos.x,target.pos.y, case_stop.x, case_stop.y, target.pos.x,target.pos.y]

		
		"""# décalage du point d'arrivée (à 120mm du pion)
		Ax,Ay, Bx,By = path[-4], path[-3], path[-2], path[-1]
		dAB = math.sqrt((Bx-Ax)**2 + (By-Ay)**2)
		path[-2] -= int(120 * (Bx-Ax) / dAB)
		path[-1] -= int(120 * (By-Ay) / dAB)"""
		
		self.debug.log(D_SHOW_PATH,path)
		
		return target, case_direction_pousse, path


	def _pathValid(self, path):
		for x in path[::2]:
			if not (250 < x < 2750):
				return False
		for y in path[1::2]:
			if not (250 < y < 1850):
				return False
		return True

		
	def _position_pousse(self, target, case):
		"""
		Renvoie la position qui permettra de pousser la target sur la case en avançant droit

		@param target (Pion)
		@param case (Vec)

		@return (Vec) la position de pousse
		"""
		l = Line(case, target.pos)
		return l.pointFrom(target.pos,300)
	
		
		
	def dumpObj(self, id_pince):
		"""
		(blockant)
		ouvrir la pince
		"""
		r = self.addBlockingCmd(1, 1, ID_OTHERS, Q_PINCE, id_pince, PINCE_OUVERT)
		if not r: # timeout
			self.write("Robot->dumpObj : timeout ouvrir les pinces")

	
	def takeObj(self, target):
		"""
		(blockant)
		@todo remplir bien les pinces
		ramasser un objet
		@return True si l'objet a été pris, False sinon
		"""
		# récupération de la position
		self.update_pos()

		# détection du sens des pinces par rapport à l'objet (on calcul l'angle dans le repère objet)
		if (target.pos.x-self.pos[0]) == 0:
			teta = math.pi
		else:
			teta = math.atan(float(target.pos.y-self.pos[1]) / float(target.pos.x-self.pos[0]))

		if target.pos.x-self.pos[0] < 0: teta += math.pi

		teta = math.degrees(teta)
		if abs(self.pos[2] - teta) < 90:
			id_pince_av, id_pince_ar = PINCE_AV, PINCE_AR
			self.write("l'objet est devant")
		else:
			id_pince_av, id_pince_ar = PINCE_AR,PINCE_AV
			self.write("l'objet est derrière")
			
		# demander une identification de l'objet
		#id_objet = self.addBlockingCmd(1, 1, ID_OTHERS, Q_PION, id_pince_av)
		id_objet = PION_1
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
			self.id_pince = None
			return False
		self.id_pince = id_pince
		
		# si pince arrière demi tour
		if id_pince == id_pince_ar:
			self.write("demi tour")
			r = self.addBlockingCmd(2, (0.5,3), ID_ASSERV, Q_ANGLE_REL, 90, VITESSE-50)
		
		# avance vers l'objet
		self.write("go to %s"%target)
		r = self.addBlockingCmd(2, (0.5,5), ID_ASSERV, Q_GOAL_ABS, target.pos.x, target.pos.y, VITESSE)
		if not r[1]: # timeout
			self.write("Robot->takeObj : timeout de l'asserv")
			return False
			
		# ouvre la pince
		self.write("ouvre les pinces")
		r = self.addBlockingCmd(1, 0.5, ID_OTHERS, Q_PINCE, id_pince, PINCE_OUVERT)
		if not r: # timeout
			self.write("Robot->takeObj : timeout ouvrir les pinces")
			return False
		
		# baisse les pinces
		self.write("baisse les pinces")
		r = self.addBlockingCmd(2, (0.5,10), ID_OTHERS, Q_PRECAL, id_pince)
		if not r[1]: # timeout
			self.write("Robot->takeObj : timeout baisser les pinces")
			return False

		# fermer la pince
		self.write("ouvre les pinces")
		r = self.addBlockingCmd(1, 0.5, ID_OTHERS, Q_PINCE, id_pince, PINCE_FERME)
		if not r: # timeout
			self.write("Robot->takeObj : timeout fermer pinces")
			return False
		
		# lève les pinces
		self.write("lève les pinces")
		r = self.addBlockingCmd(2, (0.5,10), ID_OTHERS, Q_SETPOSITION, id_pince, 9999)
		if not r[1]: # timeout
			self.write("Robot->takeObj : timeout lever les pinces")
			return False

		self.pinces[id_pince].objet = PION_2_T
		
		return True
	
if __name__ == '__main__':
	robot = Robot()	
	robot.start()
	
