# -*- coding: utf-8 -*-

import copy
from vec import *

class Rectangle:
	def __init__(self, A, B):
		if isinstance(A, Vec2):
			self.A = copy.deepcopy(A)
		else:
			self.A = Vec2(A[0],A[1])
		if isinstance(B, Vec2):
			self.B = copy.deepcopy(B)
		else:
			self.B = Vec2(B[0],B[1])
		
		if self.A.x > self.B.x:
			self.A,self.B = self.B,self.A
	
	def __contains__(self, v2):
		return ((self.A.x < v2.x < self.B.x) and (min(self.A.y,self.B.y) < v2.y < max(self.A.y,self.B.y)))
