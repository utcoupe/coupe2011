# -*- coding: utf-8 -*-

import threading
from Queue import Queue, Empty

class Message:
	def __init__(self, id_from, id_msg, id_cmd, content):
		self.id_from = id_from
		self.id_msg = id_msg
		self.id_cmd = id_cmd
		self.content = content

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
			m = (Message(id_msg, id_msg, id_cmd, msg))
			self._queue.put(m)
	
	def getMsg(self, time=None):
		try:
			r = self._queue.get(True, time)
		except Empty:
			return None
		else:
			self._queue.task_done()
			return r
	
	
	
	
	
	
