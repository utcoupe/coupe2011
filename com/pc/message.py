# -*- coding: utf-8 -*-


import serial
import threading
import time
import subprocess
from Queue import *
import sys
#from struct import *
#import binascii

from timer import *


class Server():
	"""
	Cette classe permet la communication avec les différents ports série
	
	@author Thomas
	"""
	def __init__(self, ports=None):
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
		
		self.connectCamera()
		
		self.connectSerials(ports)
		
		# créarion de la queue pour l'envoie de commande
		# et du dict pour récupérer les réponses
		for s in self.ser:
			self.queuedCmd[s] = Queue() # on envoi les commandes les unes après les autres
			self.waitRcv[s] = dict() # on reçoit dans n'importe quel ordre
			it = InteruptableThreadedLoop(None, "cmdLoop : "+s)
			it.start(self.loopCmd, (s,))
			self.cmdLoops[s] = it
			it = InteruptableThreadedLoop(None, "rcvLoop : "+s)
			it.start(self.loopRcv, (s,))
			self.rcvLoops[s] = it
		
		# petite pause avant de pouvoir envoyer et recevoir des données
		time.sleep(1)
		
		# lancement des loups
		
		print 'fin init'
	
	def stop(self):
		print 'stop server'
		for t in threading.enumerate():
			print t
		for n,screen in enumerate(self.screens):
			self.stopScreen(n)
		for it in self.cmdLoops.values():
			it.stop()
		for it in self.rcvLoops.values():
			it.stop()
	
	def tryAllSerials(self):
		ports = []
		for i in xrange(10):
			ports.append(('ACM'+str(i), 115200))
		
		# tentative de connection de ports
		connectSerials(ports)
		
		# check
		ser = dict()
		for id,ser in self.ser.items():
			if port:
				addCmd('I', id) # demande à la carte de s'identifier
				
	def connectSerials(self,ports):
		# lancement des tentatives de connection
		threads = []
		for port,refresh in ports:
			print port,refresh
			thread = threading.Thread(None, self.connect, None, (port,refresh,))
			thread.start()
			threads.append(thread)
		
		# attendre que les connections soient établies
		for t in threads:
			t.join()
		
		# check
		"""
		ser = {}
		for s in self.ser:
			if s:
				self.ser[port] = s
		self.ser = ser"""
		
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
				#print 'connection echouée sur %s, nouvelle tentative'%port
				#print ex
				pass
			else:
				print 'connection %s établie'%port
				self.ser[port] = s
				return s
		print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
		self.ser[port] = None
		return self.ser[port]
	
	def connectCamera(self):
		try:
			self.camera = subprocess.Popen("../exe", shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		except OSError as ex:
			print 'camera not found'
			print ex
		else:
			print 'connection camera établie'
		
	def loopCmd(self, port):
		""" la loop envoyant à la suite les commandes de la queue
		si l'envoi échoue (self._sendCmd(..) < 0), stock le retour d'erreur dans self.waitRcv[port]['send']
		"""
		queue = self.queuedCmd[port]
		try:
			cmd = queue.get(True, 2)
		except Empty:
			return
		r = self._sendCmd(cmd, port)
		if r < 0:
			self.waitRcv[port]['send'] = r
		queue.task_done()
	
	def loopRcv(self, port):
		""" la loop récupérant les réponses au différentes commandes """
		di = self.waitRcv[port]
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
				time.sleep(0.001)
		self.waitRcv[port][cmdid] = None
		return rcv
		
	def _sendCmd(self, cmd, port):
		try:
			self.ser[port].write('<'+cmd+'>')
			return 1
		except serial.SerialException as ex:
			print 'timeout, writing on', port
			return -1
		except OSError as ex:
			print 'port ', port, 'débranché'
			return -2
	
	def _readInput(self, port):
		try:
			val = self.ser[port].readline()
		except OSError as ex:
			print 'port ', port, 'débranché'
			return 'E,-1'
		val = val.replace("§","\n")
		if val:
			return val # int(binascii.hexlify(val),16)  #unpack('c', val)
		else:
			return 'timeout, on : '+port
	
	def sendToCam(self, cmd):
		try:
			self.camera.stdin.write(str(cmd)+"\n") # envoie au child
			self.camera.stdin.flush()
		except:
			print ("Unexpected error:", sys.exc_info())
	
	def listenCam(self):
		""" get ouput of child number n """
		output = -1
		try:
			output = self.camera.stdout.readline() # recupération du msg du child
		except:
			print ("Unexpected error:", sys.exc_info())
		return output
	
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
			live: timer de la loop
		
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
		if self.screens[n][0]:
			try:
				self.screens[n][0].stdin.write(str(msg)+"\n") # envoie au child
				self.screens[n][0].stdin.flush()
			except:
				print ("Unexpected error:", sys.exc_info())
	
	def makeLoop(self, port, cmd, speed=0.3):
		""" Lance en boucle infinie une commande et l'affiche sur un nouvel écran
		
		@param
			port: le port sur lequel envoyer la commande
			cmd: la commande
			speed: l'interval entre chaque envoi
		"""
		def loop():
			self.addCmd(cmd, port)
			self.write(self.getRcv(cmd, port, True),n)
		timer = MyTimer(float(speed),loop)
		n = self.addScreen(timer)
		timer.start()
	
	def _stopLoop(self, n):
		""" arreter une loop
		
		@param
			n: numéro du live à stopper
		"""
		if self.screens[n][1]:
			self.screens[n][1].stop()
			self.screens[n][1] = None
	
	def _stopProcess(self, n):
		""" arrete le process faisant tourner un écran
		
		@param
			n: numéro de l'écran
		"""
		if self.screens[n][0]:
			self.screens[n][0].terminate()
			self.screens[n][0] = None
	
	def stopScreen(self, n):
		""" arreter un écran (et la boucle derrière)
		
		@param
			n: numéro de l'écran
		"""
		if self.screens[n][1]:
			self._stopLoop(n)
		self._stopProcess(n)
			
	def bin(self, port):
		""" affiche tout ce que l'ordi reçoit
		"""
		def loop():
			for waitRcv in self.waitRcv.values():
				for rcv in waitRcv.values():
					if rcv:
						self.write(rcv,n)
		timer = MyTimer(0.03,loop)
		n = self.addScreen(timer)
		timer.start()
		
	
def traiterReponseCamera(msg):
	lignes = msg.strip().split(',')
	listObjets = []
	for obj in lignes:
		listObjets.append(obj.split())
	return listObjets
	




