# -*- coding: utf-8 -*-


import threading
import serial
from protocole import *


class Receveur(threading.Thread):
	""" Thread qui va lire en permanence sur la sortie de la carte
	"""
	def __init__(self, id_client, client, disconnect_event, reconnect_event):
		"""
		@param:
			id_client: l'identifiant du client (récupérer après une demande d'identification '<I>')
			client: le serial ou subprocess associé
			disconnect_event: desactivé en temps normal, activé pendant une deconnection
		"""
		threading.Thread.__init__(self, None, None, "Receveur(%s)"%str(id_client))
		self._kill_event = threading.Event()
		self._client = client
		self._id_cmd_actuel = 0
		self.reponses = dict()
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
	
	def addCmd(self, cmd):
		"""
		par default la réponse est à None
		
		@return:
			event qui previendra de la reception de la réponse
		"""
		self._id_cmd_actuel += 1
		self.reponses[self._id_cmd_actuel] = Reponse()
		return self._id_cmd_actuel, self.reponses[self._id_cmd_actuel]
		
	def run(self):
		while not self._kill_event.isSet():
			self._loop()
			self._kill_event.wait(0.0001)
		print "%s arreté"%self.name
			
	def kill(self):
		self._kill_event.set()
		self._client.close()
	
	def _readLine(self):
		"""
		@return:
			None si rien, timeout ou deconnection
			(string) si on a réussi à lire
		"""
		try:
			val = self._client.readline()
		except serial.SerialException as ex:
			print ex
		except OSError as ex:
			print "%s : (ERROR) deconnection..."%self.name
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return None
		except IOError as ex:
			print "%s : (ERROR) %s"%(self.name,ex)
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return -1
		else:
			val = val.replace("§","\n")
			if val:
				return val # int(binascii.hexlify(val),16)  #unpack('c', val)
	
	def _loop(self):
		self._reconnect_event.wait()
		r = self._readLine()
		if r:
			#print r
			try:
				id_cmd,rep_cmd = self._parseReponse(r)
			except Exception as ex:
				print "%s : (ERROR) données reçues corrompues, %s"%(self.name,ex)
			id_cmd = int(id_cmd)
			self.reponses[id_cmd].add(rep_cmd)

	def _parseReponse(self, msg):
		msg = msg.strip()
		id_cmd,reponse = msg.split(C_SEP1)
		return id_cmd,reponse
			

class Reponse():
	def __init__(self):
		self._events = {}
		self._reponses = []
		self._verrou = threading.Lock()

	def add(self,val):
		self._verrou.acquire()
		n = len(self._reponses) # le numéro de la réponse
		self._reponses.append(val)
		self._addEvent(n)
		self._events[n].set()
		self._verrou.release()

	def read(self,n,timeout=None):
		self._verrou.acquire()
		self._addEvent(n)
		self._verrou.release()
		self._events[n].wait(timeout)
		if self._events[n].isSet():
			return self._reponses[n]
		else:
			return None

	def _addEvent(self,n):
		if n not in self._events:
			self._events[n] = threading.Event()
		