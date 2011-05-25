# -*- coding: utf-8 -*-

import threading
import socket
import sys
from protocole import *
import colorConsol

MOD_TCP = 0
MOD_CIN = 1



class RobotClient(threading.Thread):
	def __init__(self, robot, mod):
		threading.Thread.__init__(self,None,None,"RobotClient")

		self.robot = robot
		self.mod = mod
		self._listFifo = list()
		self._lock_fifo = threading.Lock()
		
		self._partialMsg = ""
		
		self._e_close = threading.Event()
		
		# connection
		if self.mod == MOD_TCP:
			host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
			port = int(sys.argv[2]) if len(sys.argv) > 2 else 50000			# The same port as used by the server
			self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self._socket.settimeout(1.0)
			self._socket.connect((host, port))
		

	def combineWithPartial(self, msg):
		"""
		@return [] si le message ne peut pas etre exploité (pas de \n à la fin)
		@return [m1,m2,m3, ...] sinon
		"""
		#print (self._partialMsg,msg)
		msg = self._partialMsg + str(msg)
		index = msg.rfind('\n')
		if index < 0:
			self._partialMsg = msg
			#print self._partialMsg,[]
			return []
		else:
			self._partialMsg = msg[index+1:]
			#print self._partialMsg,[ m for m in msg[:index].split('\n') ]
			return [ m for m in msg[:index].split('\n') ]
		
	def send(self, msg):
		""" 
		Envoie le message au serveur
		"""
		if not self._e_close.isSet():
			msg = str(msg).strip()
			if self.mod == MOD_TCP:
				self._socket.send(msg+"\n")
			else:
				print msg
			self.write("Send : %s"%msg)
	
	def run(self):
		""" 
		Récupère ce qu'envoie le serveur
		"""
		while not self._e_close.isSet():
			try:
				if self.mod == MOD_TCP:
					data = self._socket.recv(1024) # en octets/chars
				else:
					data = raw_input()+"\n"
			except socket.timeout:
				pass
			else:
				if not data or str(data) == 'close':
					if self.mod == MOD_TCP:
						self._socket.close()
						self._e_close.set()
					self.write("break recv")
				else:
					for msg in self.combineWithPartial(data):
						self._treat(msg)
				
	def _treat(self, msg):
		""" fonction appellée quand un message est reçu """
		self.write("Received : '%s'"%msg)
		msg = str(msg).strip()
		msg_split = msg.split(C_SEP_SEND,2)
		try:
			id_from = int(msg_split[0]) # l'id du client qui a envoyé le message
			if id_from != ID_SERVER: # si ce n'est pas le serveur
				id_msg = int(msg_split[1])
				if id_msg >= 0: # les erreurs sont en negatif
					id_cmd = self.robot.cmd[id_msg]
				else:
					id_cmd = id_msg
				msg = msg_split[2].strip() if len(msg_split) > 2 else None

				# remplissage des fifo qui attendent
				for fifo in self._listFifo:
					fifo.addMsg(id_from, id_msg, id_cmd, msg)
					
			else: # si c'est le serveur qui a envoyé la commande
				id_cmd = int(msg_split[2])
				if msg_split[1] == "scan":
					self.robot.scan()
				elif id_cmd == Q_IDENT:
					self.send("{id_server}{sep}{id_msg}{sep}{id_ia}{sep}ia".format(id_server=ID_SERVER,id_msg=msg_split[1], sep=C_SEP_SEND, id_ia=ID_IA))
		except ValueError as ex:
			self.write(ex, colorConsol.FAIL)
		except Exception as ex:
			self.write(ex, colorConsol.FAIL)

	def stop(self):
		"""
		Envoi la commande Q_KILL à toutes les fifo,
		de cette manière toutes les actions en cours du robot devraient s'arreter
		"""
		for fifo in self._listFifo:
			fifo.addMsg(ID_IA, -1, Q_KILL, "kill")
			
	def reset(self):
		"""
		Appellée lors du reset du robot, cette fonction n'a pas pour
		effet de reset la connection avec le serveur, elle va juste reset
		ce qui est utilisé par le robot (le partialMsg et les fifo)
		"""
		self._partialMsg = ""
		self._listFifo = []

	def write(self, msg):
		""" pour écrire sans se marcher sur les doigts """
		self.robot.write(msg)
	
	def addFifo(self, msgFifo):
		self._listFifo.append(msgFifo)
		return msgFifo
	
	def removeFifo(self, msgFifo):
		self._lock_fifo.acquire()
		try:
			self._listFifo.remove(msgFifo)
		except ValueError:
			pass
		finally:
			self._lock_fifo.release()
	
		
