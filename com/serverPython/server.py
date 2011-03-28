# -*- coding: utf-8 -*-


from timer import *
import serial
import threading
import time
import subprocess
import sys
import glob
import socket
#from struct import *
#import binascii

from receveur import *
from envoyeur import *
from gestionnaireerreur import *
from device import *
from protocole import *
from client import *
from tcpLoop import *


class Server():
	"""
	Cette classe permet la communication avec les différents ports série
	
	@author Thomas
	"""
	def __init__(self, ports=None):
		""" Constructeur
		
		@param ports tableau de paire (nom du port, vitesse de rafraichissement)
		"""
		self.devices			= 	dict()	# les cartes arduinos, la cam
		self.ports				=	dict()	# pour retrouver le port en cas de deconnection
		self.ports_connection	=	dict()
		self.screens			= 	[]
		self.envoyeurs			=	dict()  # threads
		self.receveurs			=	dict()  # threads
		self.disconnectListener	=	dict()
		self.clients			=	[]		# les interfaces utilisateurs
		
		self._verrou_connectDevices = threading.Lock() # sert à ce que les disconnectListener ne tente pas tous une reconnection en m^eme temps
		self.e_shutdown				= threading.Event()
		
		self.connectDevices(ports)
		
		
		#
		for id_device,device in self.devices.items():
			self.connectToThreads(id_device,device)
		
		
		
		print 'fin init'
	
	
	def __del__(self):
		""" destructeur """
		self.stop()
	
	def start(self):
		host = ''		# Symbolic name meaning all available interfaces
		port = 50000	# Arbitrary non-privileged port
		TCPLoop(self, '', 50000, ).start()
		"""self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.s.settimeout(1.0)
		self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.s.bind((HOST, PORT))
		self.s.listen(1)
		while not self.e_shutdown.isSet():
			try:
				conn, addr = self.TCPSock.accept()
			except socket.timeout:
				pass
			else:
				client = Client(len(self.clients), conn, self)
				client.start()
				self.clients.append(client)"""
	
	def addClient(self, conn):
		client = Client(len(self.clients), conn, self)
		client.start()
		self.clients.append(client)
		
	def stop(self):
		""" couper toutes les connections """
		print 'stop server'
		self.e_shutdown.set()
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
		#time.sleep(5)
		#raise KeyboardInterrupt("shutdown")
	
	def connectDevices(self, devices, verbose=True):
		""" tente de connecter tous les devices de la liste
		
		@params devices: (list)[(port, baudrate)...]
		
		@return None
		"""
		# lancement des tentatives de connection
		threads = []
		for id_device,refresh in devices:
			#print id_device,refresh
			thread = threading.Thread(None, self.connect, None, (id_device,refresh,verbose,))
			thread.start()
			threads.append(thread)
		
		# attendre que les connections soient établies
		for t in threads:
			t.join()
	
	def connect(self, port, baudrate=None, verbose=True):
		""" connecte un device """
		if baudrate:
			id_device, device = self._connectSerial(port, baudrate, verbose)
		else:
			id_device, device = self._connectSubprocess(port)
		return id_device, device
	
	def connectToThreads(self, id_device, device):
		""" connecte les threads d'écoute, d'écriture et de gestion des deconnection au device """
		disconnectEvent = threading.Event()
		reconnectEvent = threading.Event()
		reconnectEvent.set()
		# le thread pour rebrancher en cas de deconnection
		self.disconnectListener[id_device]	= DisconnectListener(disconnectEvent, reconnectEvent, self, id_device, self._verrou_connectDevices)
		self.disconnectListener[id_device].start()
		# le thread permettant l'envoie
		envoyeur = Envoyeur(id_device, device, disconnectEvent, reconnectEvent)
		envoyeur.start()
		self.envoyeurs[id_device] = envoyeur
		# le thread permettant la reception
		receveur = Receveur(id_device, device, disconnectEvent, reconnectEvent, self.clients)
		receveur.start()
		self.receveurs[id_device] = receveur
		
		
	def _connectSerial(self, port, refresh, verbose=True):
		""" Connection à un port
		
		@param port numéro du port
		@param refresh vitesse de rafraichissement
		
		@return
			echec: None,None
			success: id_device, device
		"""
		device = None
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
				device = DeviceSerial(s,port, refresh)
				break
		if not device:
			print 'echec de la connection %s après 10 tentatives'%port
			return None,None
		else:
			# si on a reussi à se connecter on demande une identification
			time.sleep(1) # necessaire avant d'écrire
			id = self._identification(device)
			print "identification",port," :",id
			self.devices[id] = device
			self.ports[id] = port
			self.ports_connection[port] = True
			return id,device
		
	def _connectSubprocess(self, port):
		""" cré un subprocess et ouvre un pipe pour communiquer avec lui """
		try:
			process = subprocess.Popen(port, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		except OSError as ex:
			print "%s not found"%port
			print ex
			return None,None
		else:
			print "connection '%s' établie"%port
			device = DeviceSubprocess(process,port,None)
			id = self._identification(device)
			print "identification '%s' : %s"%(port,id)
			if id:
				self.devices[id] = device
				self.ports[id] = str(port)
				self.ports_connection[str(port)] = True
				return id,device
			else:
				return None,None
	
	def _identification(self, device):
		""" fait une demande d'identification a un device """
		device.write('0'+C_SEP_SEND+str(IDENTIFICATION)) # demande au programme de s'identifier
		r = device.readline()
		try:
			id_device = r.split(C_SEP1)[1].strip()
		except Exception as ex:
			print "Device('%s')::identifiaction : (ERROR) recu: %s, %s"%(device.origin,r,ex)
			id_device = None
		return id_device
	
	def parseMsg(self, id_client, msg):
		if msg:
			msg_split = msg.strip().split(" ",1)
			try:
			    if 'test' == msg_split[0]:
			        id_device,cmd = msg_split[1].split(' ',1)
			        self.testPing(id_device, cmd)
			    elif 'close' == msg_split[0]:
			        self.closeClient(id_client)
			    elif 'loop' == msg_split[0]:
			        msg_split2 = raw_input().split(' ',1)
			        self.makeLoop(msg_split2[0], msg_split2[1], msg_split[1])
			    elif 'stop' == msg_split[0]:
			        self.stopScreen(int(msg_split[1]))
			    else:
			        print msg_split
			        self.addCmd(msg_split[1], msg_split[0])
			except IndexError as ex:
			    print "mauvaise commande, IndexError : %s"%ex
			    traceback.print_exc()
			    return "mauvaise commande, IndexError : %s"%ex
			except KeyError as ex:
			    print "mauvaise commande, KeyError : %s"%ex
			    traceback.print_exc()
			    return "mauvaise commande, KeyError : %s"%ex
		
	def closeClient(self, id):
		self.clients[id]._running = False
		self.clients[id] = None
		
	def addCmd(self, cmd, id_device):
		""" ajoute une commande à envoyer
		
		@param cmd commande à envoyer
		@param id_device le device qui doit recevoir
		
		@return objet de type Reponse, voir le fichier receveur.py
		"""
		id_cmd,reponse = self.receveurs[id_device].addCmd(cmd)
		self.envoyeurs[id_device].addCmd(str(id_cmd)+C_SEP_SEND+str(cmd))
		return reponse
	
	def testPing(self, id_device, cmd):
		""" test la reactivitée de la cmd en envoyant et recevant 
		la sortie est écrite sur un autre écran
		
		@param id_device à quel device balancer la commande
		@param cmd commande
		"""
		n = self.addScreen()
		def loop():
			tot = 0
			nb_iter = 300
			print 'test du port', id_device, 'avec la commande :', cmd
			for i in xrange(nb_iter):
				t = time.time()
				r = self.addCmd(cmd, id_device)
				self.write(r.read(0,1), n)
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
		@param live timer de la loop
		
		@return numero de l'écran créé
		"""
		screens = [ s for s in self.screens if s[0] ]
		self.screens = screens
		screen = subprocess.Popen(["python","screen.py"], shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		self.screens.append([screen, live])
		return len(self.screens)-1 # return l'index du screen créé
	
	def write(self, msg, n=0):
		""" écrit sur un écran 
		
		@param msg le message
		@param n l'écran sur lequel écrire
		"""
		if self.screens[n][0]:
			try:
				self.screens[n][0].stdin.write(str(msg)+"\n") # envoie au child
				self.screens[n][0].stdin.flush()
			except:
				print ("Unexpected error:", sys.exc_info())
	
	def makeLoop(self, id_device, cmd, speed):
		""" Lance en boucle infinie une commande et l'affiche sur un nouvel écran
		
		@param port le port sur lequel envoyer la commande
		@param cmd la commande
		@param speed l'interval entre chaque envoi
		"""
		def loop():
			r = self.addCmd(cmd, id_device)
			self.write(r.read(0,1),n)
		timer = MyTimer(float(speed),loop)
		n = self.addScreen(timer)
		timer.start()
	
	def _stopLoop(self, n):
		""" arreter une loop
		
		@param n numéro du live à stopper
		"""
		if self.screens[n][1]:
			self.screens[n][1].stop()
			self.screens[n][1] = None
	
	def _stopProcess(self, n):
		""" arrete le process faisant tourner un écran
		
		@param n numéro de l'écran
		"""
		if self.screens[n][0]:
			self.screens[n][0].terminate()
			self.screens[n][0] = None
	
	def stopScreen(self, n):
		""" arreter un écran (et la boucle derrière)
		
		@param n numéro de l'écran
		"""
		if self.screens[n][1]:
			self._stopLoop(n)
		self._stopProcess(n)
		


def traiterReponseCamera(msg):
	if msg != 'r':
		decalageX = 200; # mm
		lignes = msg.strip().split(';')
		listObjets = []
		for obj in lignes:
			obj_data = obj.split()
			if len(obj_data) == 3:
				obj_data[1],obj_data[2] = float(obj_data[1])+200.0,float(obj_data[2])
				listObjets.append(obj_data)
			else:
				print "données corrompues : "+obj
		return listObjets
	else:
		return []
	

def scanPorts():
	pathname = '/dev/ttyACM*'
	if sys.platform == 'darwin':
		pathname = '/dev/tty.usbmodemf*'
	elif sys.platform != 'linux2':
		print "systeme non reconnu : % , par default on cherche les ports en %s"%(sys.platform,pathname)

	return glob.glob(pathname)


