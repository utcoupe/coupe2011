# -*- coding: utf-8 -*-


import sys
sys.path.append('../com/serverPython/')
from protocole import *


import time
from geometry.vec import *

###################
# FLAGS DES PIONS #
###################
UNKNOWN			= -1
VIDE			= 0
PION_1			= 1 # pion simple
PION_2			= 2 # double pion
PION_1_T		= 3 # pion simple + tour
PION_2_T		= 4 # pion double + tour
TOUR			= 5 # tour simple

class Pion:
	def __init__(self, x ,y, t=PION_1):
		self.pos = Vec(x,y)
		self.type = t # flag
		self.time = time.time()

		self.calculColor()
		self.calculCase()

	def __iter__(self):
		return iter((self.pos.x,self.pos.y,self.type, self.color))
		
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

	def calculColor(self):
		x,y = self.pos.x, self.pos.y
		if (450 <= x <= 2550) and (0 <= y <= 2100):
			if ((x-450)%700) < 350:
				if (y%700) < 350:
					self.color = RED
				else:
					self.color = BLUE
			else:
				if (y%700) < 350:
					self.color = BLUE
				else:
					self.color = RED
		elif (0 <= x <= 400) and (0 <= y <= 400):
			self.color = BLUE
		elif (2600 <= x <= 3000) and (0 <= y <= 400):
			self.color = RED
		else:
			self.color = UNKNOWN

	def calculCase(self):
		x,y = self.pos.x, self.pos.y
		# la position du centre de la case sur laquelle est le pion
		self.case = Vec((x-450)/350*350+175+450, y/350*350+175)
		
		
	def __repr__(self):
		return "Pion(%s, type=%s)"%(self.pos,self.type)

	
