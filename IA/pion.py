# -*- coding: utf-8 -*-


import sys
from protocole import *


import time
from geometry.vec import *

###################
# FLAGS DES PIONS #
###################
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
		self.color = UNKNOWN

	def __iter__(self):
		return iter((self.pos.x,self.pos.y,self.type, self.color))

	def __eq__(self, other):
		""" si les pions sont à peu près au même endroit """
		return (self.pos.x - other.pos.x)**2 + (self.pos.y - other.pos.y)**2 < 200**2
		
	def age(self):
		""" depuis combien de temps avons nous vu ce pion ? """
		return time.time() - self.time
	
	def update(self, pion, robot_color):
		"""
		le time updaté, et eventuellement la pos aussi
		
		@param pion (Pion) le pion vu
		"""
		self.pos = Vec((self.pos.x+pion.pos.x)/2, (self.pos.y+pion.pos.y)/2)
		self.calculColor(robot_color)
		self.calculCase()
		self.time = time.time()

	def calculColor(self, robot_color):
		x,y = self.pos.x, self.pos.y
		if (450 <= x <= 2550) and (0 <= y <= 2100):
			modx = (x-450)%700
			mody = (y%700)
			if robot_color == RED:
				if ((50 < modx < 300) and (50 < mody < 300)) or ((400 < modx < 650) and (400 < mody < 650)):
					self.color = RED
				else:
					self.color = BLUE
			else:
				if ((400 < modx < 650) and (50 < mody < 300)) or ((50 < modx < 300) and (400 < mody < 650)):
					self.color = BLUE
				else:
					self.color = RED
		elif (0 < x < 400) and (0 < y < 400):
			self.color = BLUE
		elif (2600 < x < 3000) and (0 < y < 400):
			self.color = RED
		elif ((0 <= x <= 400) or (2600 < x < 3000)) and (400 < y < 2100):
			self.color = GREEN
		else:
			self.color = UNKNOWN

	def calculCase(self):
		x,y = self.pos.x, self.pos.y
		# la position du centre de la case sur laquelle est le pion
		self.case = Vec((x-450)/350*350+175+450, y/350*350+175)
		
		
	def __repr__(self):
		return "Pion(%s, type=%s)"%(self.pos,self.type)

	
