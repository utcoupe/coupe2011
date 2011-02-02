# -*- coding: utf-8 -*-


import threading

class Receveur(threading.Thread):
	""" Thread qui va lire en permanence sur la sortie de la carte
	"""
	def __init__(self, id_client, client, disconnect_event, reconnect_event):
		"""
		@param:
			id_client: l'identifiant du client (récupérer après une demande d'identification '<I>')
			client: le serial associé
			disconnect_event: desactivé en temps normal, activé pendant une deconnection
		"""
		threading.Thread.__init__(self, None, None, "Receveur(%s)"%str(id_client))
		self._stopevent = threading.Event()
		self._client = client
		self.reponses = dict()
		self._reception_events = dict()
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
	
	def addIdCmd(self, cmd):
		""" previens qu'on attend une répons epour la commande <id_cmd>
		par default la réponse est à None
		
		@return:
			event qui previendra de la reception de la réponse
		"""
		id_cmd = cmd[0]
		self.reponses[id_cmd] = None
		self._reception_events[id_cmd] = threading.Event()
		return self._reception_events[id_cmd]
		
	def run(self):
		while not self._stopevent.isSet():
			self._loop()
			self._stopevent.wait(0.0001)
		print "%s arreté"%self.name
			
	def stop(self):
		self._stopevent.set()
	
	def _readLine(self):
		"""
		@return:
			None si rien (timeout) ou deconnection
			(string) si on a réussi à lire
		"""
		try:
			val = self._client.readline()
		except OSError as ex:
			print "%s : deconnection..."%self.name
			self._disconnect_event.set()
			return None
		else:
			val = val.replace("§","\n")
			if val:
				return val # int(binascii.hexlify(val),16)  #unpack('c', val)
	
	def _loop(self):
		self._reconnect_event.wait()
		r = self._readLine()
		if r:
			print r
			id_cmd,rep_cmd = r.strip().split(',')
			self.reponses[id_cmd] = rep_cmd
			self._reception_events[id_cmd].set()

