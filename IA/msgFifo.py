# -*- coding: utf-8 -*-

import threading
from Queue import Queue, Empty



class MsgFifo:
	"""
	FIFO qui récupère les messages
	"""
	def __init__(self, msg2wait):
		"""
		@param msg2wait (list<device,cmd>)
		"""
		self._queue = Queue()
		self._filter = msg2wait
	
	def addMsg(self, id_from, id_msg, id_cmd, msg):
		"""
		ajouter un message à la queue
		"""
		if (id_from, id_cmd) in self._filter:
			self._queue.put((id_msg, id_cmd, msg))
	
	def getMsg(self, time=None):
		try:
			r = self._queue.get(True, time)
		except Empty:
			return None
		else:
			self._queue.task_done()
			return r
	
	
	
	
	
	