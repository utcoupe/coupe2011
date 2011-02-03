# -*- coding: utf-8 -*-


import serial
import threading
import time
import subprocess
import sys
import glob
#from struct import *
#import binascii

from timer import *
from receveur import *
from envoyeur import *
from gestionnaireerreur import *
from client import *



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
		self.clients			= 	dict()
		self.ports				=	dict()	# pour retrouver le port en cas de deconnection
		self.ports_connection	=	dict()
		self.screens			= 	[]
		self.envoyeurs			=	dict()  # threads
		self.receveurs			=	dict()  # threads
		self.disconnectListener	=	dict()
		
		self._verrou_connectClients = threading.Lock() # sert à ce que les disconnectListener ne tente pas tous une reconnection en m^eme temps
		
		
		self.connectClients(ports)
		
		
		#
		for id_client,client in self.clients.items():
			self.connectToThreads(id_client,client)
		
		
		
		print 'fin init'
	
	
		
	def stop(self):
		print 'stop server'
		for t in threading.enumerate():
			print t
		for n,screen in enumerate(self.screens):
			self.stopScreen(n)
		for it in self.envoyeurs.values():
			it.kill()
		for it in self.receveurs.values():
			it.kill()
		for listener in self.disconnectListener.values():
			listener.kill()
	
	def connectClients(self,clients, verbose=True):
		""" tente de connecter tous les clients de la liste
		
		@params:
			clients: (list)[(port, baudrate)...]
		
		@return: None
		"""
		# lancement des tentatives de connection
		threads = []
		for id_client,refresh in clients:
			#print id_client,refresh
			thread = threading.Thread(None, self.connect, None, (id_client,refresh,verbose,))
			thread.start()
			threads.append(thread)
		
		# attendre que les connections soient établies
		for t in threads:
			t.join()
	
	def connect(self, port, baudrate=None, verbose=True):
		if baudrate:
			id_client, client = self._connectSerial(port, baudrate, verbose)
		else:
			id_client, client = self._connectSubprocess(port)
		return id_client, client
	
	def connectToThreads(self, id_client, client):
		disconnectEvent = threading.Event()
		reconnectEvent = threading.Event()
		reconnectEvent.set()
		# le thread pour rebrancher en cas de deconnection
		self.disconnectListener[id_client]	= DisconnectListener(disconnectEvent, reconnectEvent, self, id_client, self._verrou_connectClients)
		self.disconnectListener[id_client].start()
		# le thread permettant l'envoie
		envoyeur = Envoyeur(id_client, client, disconnectEvent, reconnectEvent)
		envoyeur.start()
		self.envoyeurs[id_client] = envoyeur
		# le thread permettant la reception
		receveur = Receveur(id_client, client, disconnectEvent, reconnectEvent)
		receveur.start()
		self.receveurs[id_client] = receveur
		
		
	def _connectSerial(self, port, refresh, verbose=True):
		""" Connection à un port
		
		@param
			port: numéro du port
			refresh: vitesse de rafraichissement
		
		@return:
			echec: None,None
			success: id_client, client
		"""
		client = None
		for i in range(10):
			try:
				#print 'tentative de connection sur %s'%port
				s = serial.Serial(port, refresh, timeout=1, writeTimeout=1)
			except serial.SerialException as ex:
				if verbose:
					print 'connection echouée sur %s, nouvelle tentative'%port
				#print ex
				pass
			else:
				print "connection '%s' établie"%port
				client = ClientSerial(s,port, refresh)
				break
		if not client:
			print 'echec de la connection %s après 10 tentatives'%port
			return None,None
		else:
			# si on a reussi à se connecter on demande une identification
			time.sleep(1) # necessaire avant d'écrire
			id = self._identification(client)
			print "identification",port," :",id
			self.clients[id] = client
			self.ports[id] = port
			self.ports_connection[port] = True
			return id,client
		
	def _connectSubprocess(self, port):
		try:
			process = subprocess.Popen(port, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		except OSError as ex:
			print "%s not found"%port
			print ex
			return None,None
		else:
			print "connection '%s' établie"%port
			client = ClientSubprocess(process,port,None)
			id = self._identification(client)
			print "identification '%s' : %s"%(port,id)
			self.clients[id] = client
			self.ports[id] = port
			self.ports_connection[port] = True
			return id,client
	
	def _identification(self, client):
		client.write('I') # demande au programme de s'identifier
		try:
			id_client = client.readline().split(',')[1].strip()
		except Exception as ex:
			print "Serial('%s')::identifiaction : (ERROR) %s"%(client.port,ex)
			id_client = client.port
		return id_client
		
	def addCmd(self, cmd, id_client):
		""" ajoute une commande à envoyer
		
		@param:
			cmd: commande à envoyer
			id_client: le client qui doit recevoir
		
		@return: event prevenant de la reception
		"""
		self.envoyeurs[id_client].addCmd(cmd)
		return self.receveurs[id_client].addIdCmd(cmd)
	
	def getReponse(self, cmd, id_client):
		id_cmd = cmd[0]
		return self.receveurs[id_client].reponses[id_cmd]
		
	def sendToCam(self, cmd):
		""" envoyer une commande à la camera """
		try:
			self.camera.stdin.write(str(cmd)+"\n") # envoie au child
			self.camera.stdin.flush()
		except:
			print ("Unexpected error:", sys.exc_info())
	
	def listenCam(self):
		""" récupérer le retour de la caméra """
		output = -1
		try:
			output = self.camera.stdout.readline() # recupération du msg du child
		except:
			print ("Unexpected error:", sys.exc_info())
		return output
	
	def testPing(self, id_client, cmd):
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
			print 'test du port', id_client, 'avec la commande :', cmd
			for i in xrange(nb_iter):
				t = time.time()
				e = self.addCmd(cmd, id_client)
				e.wait()
				self.write(self.getReponse(cmd, id_client), n)
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
	
	def makeLoop(self, id_client, cmd, speed=0.3):
		""" Lance en boucle infinie une commande et l'affiche sur un nouvel écran
		
		@param
			port: le port sur lequel envoyer la commande
			cmd: la commande
			speed: l'interval entre chaque envoi
		"""
		def loop():
			e = self.addCmd(cmd, id_client)
			e.wait(1)
			self.write(self.getReponse(cmd, id_client),n)
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
		


def traiterReponseCamera(msg):
	if msg != 'r':
		lignes = msg.strip().split(';')
		listObjets = []
		for obj in lignes:
			obj_data = obj.split()
			if len(obj_data) == 3:
				listObjets.append(obj_data)
			else:
				print "données corrompues : "+obj
		return listObjets
	else:
		return []
	

def scanPorts():
	pathname = '/dev/ttyACM*'
	if sys.platform == 'darwin':
		pathname = '/dev/tty.ACM*'
	else:
		print "systeme non reconnu, par default on cherche les ports en %s"%pathname

	return glob.glob(pathname)


