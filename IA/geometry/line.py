# -*- coding: utf-8 -*-

import sys

from vec import *

from math import *
import copy


class Line:
	def __init__(self,A, B):
		self.A = copy.deepcopy(A)
		self.B = copy.deepcopy(B)
		self.AB = Vec2(B.x - A.x, B.y - A.y)
		self.teta = atan2(self.AB.y, self.AB.x)
		"""$
		A = copy.deepcopy(A)
		B = copy.deepcopy(B)
		self.invers = 1 # pour la fonction pointFrom(..)
		if A.x > B.x:
			A,B = B,A
			self.invers = -1
		self.A = A
		self.B = B
		self.AB = Vec2(B.x-A.x,B.y-A.y)
		if self.AB.x != 0: # x=k
			self.m = float(self.AB.y)/float(self.AB.x)
			if abs(self.m) > 1e+15:
				#print "! x=k !"
				self.m = None 
		else:
			#print "! x=k !"
			self.m = None
		
		if self.m!=None: self.k = A.y - self.m*A.x # y=m*x+k
		else: self.k = self.A.x # x=K
		"""
	
	def tracer(self,canevas, **options):
		return canevas.create_line(self.A.x,self.A.y,self.B.x,self.B.y,options)
		
	
	def y(self,x):
		return self.m*x+self.k
	
	def translate(self, v):
		"""
		translation de vecteur v
		@param v (Vec2) vecteur
		"""
		A = self.A + v
		B = self.B + v
		self.__init__(A, B)
		
	def lenght(self):
		""" return (float) distance AB """
		return sqrt(self.AB.x**2+self.AB.y**2)
	
	def cosT(self):
		""" @return cos teta """
		return cos(self.teta)
		#return float(self.AB.x) / float(self.lenght())
	
	def sinT(self):
		""" @return sin teta """
		return sin(self.teta)
		#return float(self.AB.y) / float(self.lenght())
	
	def add2K(self, K):
		self.k += K
		self.A.y += K
		self.B.y += K
		self.AB = Vec2(self.B.x-self.A.x,self.B.y-self.A.y)
		
	def __len__(self):
		""" @return (int) distance AB² """
		return int(self.AB.x)**2+int(self.AB.y)**2
		
	def __and__(self, other):
		if isinstance(other, Line):
			if other.teta != self.teta:
				tanTb = tan(other.teta)
				r1 = ((other.A.y - self.A.y) + ((self.A.x - other.A.x) * tanTb)) / (self.sinT() - self.cosT()*tanTb)
				return self.A + Vec2(r1 * self.cosT(), r1 * self.sinT())
			else:
				return None
			
			"""$
			if self.m and other.m:
				if (self.m-other.m != 0):
					x = float(other.k-self.k)/float(self.m-other.m)
					return Vec2(x, self.y(x))
				else:
					return None
			else:
				if self.m: # other.x=k
					return Vec2(other.k, self.y(other.k))
				elif other.m: # self.x=k
					return Vec2(self.k, other.y(self.k))"""
		elif isinstance(other,Vec):
			raise Exception("not implemented yet")
		elif not other:
			return None
		else:
			return other & self
	
	def __repr__(self):
		return "Line"+str((self.A,self.B,self.teta,self.lenght()))

	def pointFrom(self, M, d):
		"""
		Calcul la position du point de la droite situé à une distance d de M
		@param M (Vec) point de depart sur la droite
		@param d (int) distance

		@return (Vec) le point
		"""
		dAB = self.lenght()
		x = self.invers * int(d * (self.B.x-self.A.x) / dAB)
		y = self.invers * int(d * (self.B.y-self.A.y) / dAB)
		return M + (x,y)
		

'''$
class Tangente(Line):
	def __init__(self, A, C, type):
		"""
		@param A (Vec2) point par lequel doit passer la tangente
		@param C (Circle) le cercle
		@param type (0 ou 1) le type (dessous/dessus)
		"""

		
		O = C.O
		R = C.R
		AO = Vec2(O.x-A.x, O.y-A.y)
		d2AO = AO.x**2+AO.y**2
		if d2AO != 0:
			dAO = sqrt(d2AO)
			#print A,C
			t1 = asin(float(AO.y)/(dAO))
			
			if (O.x < A.x): t1 = pi - t1
			#print "t1",degrees(t1)
			frac = float(R)/(dAO)
			if frac < -1.0:
				t2 = pi/2.0
			elif 1.0 < frac:
				t2 = -pi/2.0
			else:
				t2 = asin(frac)
				
			teta = t1+type*t2
			
			if cos(teta) != 0:
				self.m = sin(teta)/cos(teta)
				self.k = A.y - self.m*A.x
			else: # x=k
				self.m = None
				self.k = 0
			
			B = Vec2(500,self.y(500))
			Line.__init__(self, A, B)
		else:
			print "ERROR : Tangente.__init__(%s,%s,%s) distance entre le point et le centre null"%(A,C,type)
			Line.__init__(self,A,A)
		"""
		
	def __repr__(self):
		return "Tangente"+str((self.A,self.B,self.AB))
'''
