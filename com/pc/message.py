# -*- coding: utf-8 -*-



#from struct import *
#import binascii
import serial
import threading
import time
import subprocess
from timer import *

class Server():
	def __init__(self, ports):
		self.ser = dict()
		self.screens = []
		self.addScreen()
		
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

		# petite pause avant de pouvoir envoyer et recevoir des données
		time.sleep(2)
		
		print 'fin init'
	
	def connect(self, port, refresh):
		for i in range(10):
			try:
				#print 'tentative de connection sur %s'%port
				s = serial.Serial('/dev/tty'+port, refresh, timeout=1, writeTimeout=1)
			except serial.SerialException as ex:
				print 'connection echouée sur %s, nouvelle tentative'%port, ex
			else:
				print 'connection %s établie'%port
				self.ser[port] = s
				return s
		print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
		self.ser[port] = None
		return self.ser[port]
	
	"""
		cmd: commande brute (sans caractere debut/fin)
	"""
	def sendCmd(self, cmd, port):
		if not cmd:
			cmd = 'p'
		try:
			self.ser[port].write('<'+cmd+'>')
		except serial.SerialException as ex:
			print 'timeout writing on', port, ex
	
	
	def readInput(self, port):
		val = self.ser[port].readline()
		val = val.replace("§","\n")
		if val:
			return port + ':' + val # int(binascii.hexlify(val),16)  #unpack('c', val)
		else:
			return 'timeout on :',port
	
	def testPing(self, port, cmd):
		tot = 0
		nb_iter = 100
		print 'test du port', port, 'avec la commande :', cmd
		for i in xrange(nb_iter):
			t = time.time()
			self.sendCmd(cmd, port)
			self.write(self.readInput(port))
			tEllapsed = time.time() - t
			tot += tEllapsed
			self.writeOn(""+str(i)+" "+str(tEllapsed))
		self.write("temps total"+str(tot))
		self.write("moy"+str(tot/nb_iter))
	
	def addScreen(self, live=None):
		"""
			live: timer du live
		"""
		screens = [ s for s in self.screens if s[0] ]
		self.screens = screens
		screen = subprocess.Popen(["python","screen.py"], shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		self.screens.append([screen, live])
		return len(self.screens)-1 # return l'index du screen créé
	
	def write(self, msg, n=0):
		self.screens[n][0].stdin.write(str(msg)+"\n") # envoie au child
		self.screens[n][0].stdin.flush()
	
	def getLive(self, port, cmd):
		def loop():
			self.sendCmd(cmd, 'ACM0')
			self.write(self.readInput('ACM0'),n)
		timer = MyTimer(0.3,loop)
		n = self.addScreen(timer)
		timer.start()
	
	def stopLive(self, n):
		self.screens[n][1].stop()
		self.screens[n][1] = None
		self.stopProcess(n)
	
	def stopProcess(self, n):
		self.screens[n][0].terminate()
		self.screens[n][0] = None
	
	def stopScreen(self, n):
		if self.screens[n][1]:
			self.stopLive(n)
		else:
			self.stopProcess(n)
			
			
				
		
	
