# -*- coding: utf-8 -*-


import time



###################
# FLAGS DES PIONS #
###################
VIDE			= 0
PION_1		  = 1 # pion simple
PION_2		  = 2 # double pion
PION_1_T		= 3 # pion simple + tour
PION_2_T		= 4 # pion double + tour
TOUR			= 5 # tour simple

class Pion:
	def __init__(self, x ,y, type):
		self.pos = (x,y)
		self.type = type # flag
		self.time = time.time()
	
	def age(self):
		""" depuis combien de temps avons nous vu ce pion ? """
		return time.time() - self.time
	
	def update(self, pos=None):
		"""
		le time updaté, et eventuellement la pos aussi
		
		@param pos la nouvelle position, None pour garder l'ancienne
		"""
		if pos:
			self.pos = pos
		self.time = time.time()
	
	