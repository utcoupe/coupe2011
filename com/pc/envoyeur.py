# -*- coding: utf-8 -*-


import threading
from Queue import *
import serial


class Envoyeur(threading.Thread):
	""" Thread qui va envoyer en permanence les ordre stocké dans sa Queue()
	permet d'empecher plusieurs threads d'écrire en meme temps sur un port
	"""
	def __init__(self, id_client, client, disconnect_event, reconnect_event):
		"""
		@param:
			id_client: l'identifiant du client (récupérer après une demande d'identification '<I>')
			client: le serial ou subprocess associé
			disconnect_event: desactivé en temps normal, activé pendant une deconnection
		"""
		threading.Thread.__init__(self, None, None, "Envoyeur(%s)"%str(id_client))
		self._kill_event = threading.Event()
		self._client = client
		self._queue = Queue()
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
	
	def addCmd(self, cmd):
		""" ajoute une commande en fin de queue """
		self._queue.put(cmd)		
		
	def run(self):
		while not self._kill_event.isSet():
			self._loop()
			self._kill_event.wait(0.0001)
		print "%s arreté"%self.name
			
	def kill(self):
		self._kill_event.set()
		self._client.close()
	
	def _sendCmd(self, cmd):
		""" envoie la commande de debut de queue """
		try:
			self._client.write('<'+cmd+'>')
			return 1
		except serial.SerialException as ex:
			print ex
			return -1
		except OSError as ex:
			print "%s : (ERROR) %s"%(self.name,ex)
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return -1
		except IOError as ex:
			print "%s : (ERROR) %s"%(self.name,ex)
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return -1
	
	def _loop(self):
		self._reconnect_event.wait()
		try:
			cmd = self._queue.get(True, 2)
		except Empty:
			return
		r = self._sendCmd(cmd)
		self._queue.task_done()
