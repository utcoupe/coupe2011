# -*- coding: utf-8 -*-


import serial
import threading
import time
import subprocess
from Queue import *
#from struct import *
#import binascii

from timer import *


class Server():
	"""
	Cette classe permet la communication avec les différents ports série
	
	@author Thomas
	"""
	def __init__(self, ports):
		""" Constructeur
		
		@param
			ports: tableau de paire (nom du port, vitesse de rafraichissement)
		"""
		self.ser		= 	dict()
		self.screens	= 	[]
		self.queuedCmd 	= 	dict() 	# liste des commandes à envoyer par port
		self.waitRcv 	=	dict() 	# liste des réponses attendues par port
		self.cmdLoops	=	dict() # threads
		self.rcvLoops	=	dict() # threads
		
		# lancement des tentatives de connection
		threads = []
		for port,refresh in ports:
			print port,refresh
			thread = threading.Thread(None, self.connect, None, (port,refresh,))
			thread.start()
			threads.append(thread)
		
		# attendre que les connections soient établies
		while True:
			for t in threads:
				if t.isAlive():
					time.sleep(2)
					break # break que le for
			break
		
		# Verification que les connections ont reussi
		for s in self.ser.values():
			if not s: 
				exit()
		
		# créarion de la queue pour l'envoie de commande
		# et du dict pour récupérer les réponses
		for s in self.ser:
			self.queuedCmd[s] = Queue() # on envoi les commandes les unes après les autres
			self.waitRcv[s] = dict() # on reçoit dans n'importe quel ordre
			self.cmdLoops[s] = threading.Thread(None, self.loopCmd, None, (s,))
			self.cmdLoops[s].start()
			self.rcvLoops[s] = threading.Thread(None, self.loopRcv, None, (s,))
			self.rcvLoops[s].start()
		
		# petite pause avant de pouvoir envoyer et recevoir des données
		time.sleep(1)
		
		# lancement des loups
		
		print 'fin init'
	
	def connect(self, port, refresh):
		""" Connection à un port
		
		@param
			port: numéro du port
			refresh: vitesse de rafraichissement
		"""
		for i in range(10):
			try:
				#print 'tentative de connection sur %s'%port
				s = serial.Serial('/dev/tty'+port, refresh, timeout=1, writeTimeout=1)
				#s = serial.Serial('/dev/tty'+port, refresh, writeTimeout=1)
			except serial.SerialException as ex:
				print 'connection echouée sur %s, nouvelle tentative'%port, ex
			else:
				print 'connection %s établie'%port
				self.ser[port] = s
				return s
		print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
		self.ser[port] = None
		return self.ser[port]
	
	def loopCmd(self, port):
		""" la loop envoyant à la suite les commandes de la queue """
		queue = self.queuedCmd[port]
		while True:
			cmd = queue.get()
			r = self._sendCmd(cmd, port)
			if r < 0:
				self.waitRcv[port] = 'timeout'
			queue.task_done()
	
	def loopRcv(self, port):
		""" la loop récupérant les réponses au différentes commandes """
		di = self.waitRcv[port]
		while True:
			r = self._readInput(port).split(',')
			#print r
			if len(r) == 2 and r[0] != 'timeout':
				cmd,recv = r
				di[cmd] = recv
	
	def addCmd(self, cmd, port):
		""" ajoute une commande à envoyer dans la queue des commandes
		
		@param
			cmd: commande à envoyer
			port: port sur lequel envoyer
		"""
		cmdid = cmd[0]
		self.queuedCmd[port].put(cmd)
		self.waitRcv[port][cmdid] = None
	
	def getRcv(self, cmd, port, bloquant=False):
		""" renvoie la réponse à la commande
		
		@param
			cmd: la commande dont on veut la réponse
			port: port sur lequel on veut écouter la réponse
			bloquant: True <=> boucle en attendant la réponse
		"""
		cmdid = cmd[0]
		rcv = self.waitRcv[port][cmdid]
		if bloquant:
			while not rcv:
				rcv = self.waitRcv[port][cmdid]
		return rcv
		
	def _sendCmd(self, cmd, port):
		try:
			self.ser[port].write('<'+cmd+'>')
			return 1
		except serial.SerialException as ex:
			print 'timeout, writing on', port, ex
			return -1
	
	
	def _readInput(self, port):
		val = self.ser[port].readline()
		val = val.replace("§","\n")
		if val:
			return val # int(binascii.hexlify(val),16)  #unpack('c', val)
		else:
			return 'timeout, on : '+port
	
	def testPing(self, port, cmd):
		""" test la reactivitée de la cmd en envoyant et recevant 
		la sortie est écrite sur un autre écran
		
		@param
			port: sur quel port balancer la commande
			cmd: commande
		"""
		n = self.addScreen()
		def loop():
			tot = 0
			nb_iter = 300
			print 'test du port', port, 'avec la commande :', cmd
			for i in xrange(nb_iter):
				t = time.time()
				self.addCmd(cmd, port)
				self.write(self.getRcv(cmd, port,True), n)
				tEllapsed = time.time() - t
				tot += tEllapsed
				self.write(""+str(i)+" "+str(tEllapsed), n)
			self.write("nb d'iters : " + str(nb_iter))
			self.write("temps total"+str(tot), n)
			self.write("moy"+str(tot/nb_iter), n)
		
		t = threading.Thread(None, loop, None)
		t.start()
	
	def addScreen(self, live=None):
		""" 
		@param
			live: timer du live
		
		@return numero de l'écran créé
		"""
		screens = [ s for s in self.screens if s[0] ]
		self.screens = screens
		screen = subprocess.Popen(["python","screen.py"], shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		self.screens.append([screen, live])
		return len(self.screens)-1 # return l'index du screen créé
	
	def write(self, msg, n=0):
		""" écrit sur un écran 
		
		@param
			msg: le message
			n: l'écran sur lequel écrire
		"""
		self.screens[n][0].stdin.write(str(msg)+"\n") # envoie au child
		self.screens[n][0].stdin.flush()
	
	def getLive(self, port, cmd, speed=0.3):
		""" Lance en boucle infinie une commande et l'affiche sur un nouvel écran
		
		@param
			port: le port sur lequel envoyer la commande
			cmd: la commande
			speed: l'interval entre chaque envoi
		"""
		def loop():
			self.addCmd(cmd, 'ACM0')
			self.write(self.getRcv(cmd, 'ACM0', True),n)
		timer = MyTimer(float(speed),loop)
		n = self.addScreen(timer)
		timer.start()
	
	def _stopLive(self, n):
		""" arreter un "live"
		
		@param
			n: numéro du live à stopper
		"""
		self.screens[n][1].stop()
		self.screens[n][1] = None
	
	def _stopProcess(self, n):
		""" arrete le process faisant tourner un écran
		
		@param
			n: numéro de l'écran
		"""
		self.screens[n][0].terminate()
		self.screens[n][0] = None
	
	def stopScreen(self, n):
		""" arreter un écran (et la boucle derrière)
		
		@param
			n: numéro de l'écran
		"""
		if self.screens[n][1]:
			self._stopLive(n)
		self._stopProcess(n)
			
			
				
		
	
