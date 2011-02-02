# -*- coding: utf-8 -*-


import threading
import serial
from Queue import *

class Envoyeur(threading.Thread):
	""" Thread qui va envoyer en permanence les ordre stocké dans sa Queue()
	permet d'empecher plusieurs threads d'écrire en meme temps sur un port
	"""
	def __init__(self, id_client, client, disconnect_event, reconnect_event):
		"""
		@param:
			id_client: l'identifiant du client (récupérer après une demande d'identification '<I>')
			client: le serial associé
			disconnect_event: desactivé en temps normal, activé pendant une deconnection
		"""
		threading.Thread.__init__(self, None, None, "Envoyeur(%s)"%str(id_client))
		self._stopevent = threading.Event()
		self._client = client
		self._queue = Queue()
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
	
	def addCmd(self, cmd):
		""" ajoute une commande en fin de queue """
		self._queue.put(cmd)		
		
	def run(self):
		while not self._stopevent.isSet():
			self._loop()
			self._stopevent.wait(0.0001)
		print "%s arreté"%self.name
			
	def stop(self):
		self._stopevent.set()
	
	def _sendCmd(self, cmd):
		""" envoie la commande de debut de queue """
		try:
			self._client.write('<'+cmd+'>')
			return 1
		except serial.SerialException as ex:
			raise Exception, "timeout, writing on"+str(self._client)
		except OSError as ex:
			print "%s : (ERROR) deconnection..."%self.name
			self._disconnect_event.set()
			return -1
	
	def _loop(self):
		self._reconnect_event.wait()
		try:
			cmd = self._queue.get(True, 2)
		except Empty:
			return
		r = self._sendCmd(cmd)
		self._queue.task_done()
