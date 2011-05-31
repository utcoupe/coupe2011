# -*- coding: utf-8 -*-


import time
from protocole import *


class Case:
	def __init__(self, x,y):
		self.x, self.y = x,y # position du centre de la case
		if (450 <= x <= 2550) and (0 <= y <= 2100):
			modx = (x-450)%700
			mody = (y%700)
			if ((0 < modx < 350) and (0 < mody < 350)) or ((350 < modx < 700) and (350 < mody < 700)):
				self.color = RED
			else:
				self.color = BLUE
		elif (0 < x < 400) and (0 < y < 400):
			self.color = BLUE
		elif (2600 < x < 3000) and (0 < y < 400):
			self.color = RED
		elif ((0 <= x <= 400) or (2600 < x < 3000)) and (400 < y < 2100):
			self.color = GREEN
		else:
			self.color = UNKNOWN

		self._empty = True # si la case est pleine ou pas
		self._time = 0 # date à laquelle on a appellé setFull

	def empty(self):
		"""
		Retourne si on peut poser un pion sur la case ou pas
		"""
		if not self._empty:
			now = time.time()
			if now - self._time > 20: # toutes les x secondes on considère que le pion n'est plus là
				self._empty = True
		return self._empty

	def setFull(self):
		"""
		Pose un pion sur la case
		"""
		self._time = time.time()
		self._empty = False

	def __repr__(self):
		return "Case(x={x},y={y},c={c},e={e})".format(x=self.x,y=self.y,c=self.color,e=self._empty)
		
