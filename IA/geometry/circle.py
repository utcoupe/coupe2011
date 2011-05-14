# -*- coding: utf-8 -*-

import sys

from vec import *
from line import *

import copy



class Circle:
	def __init__(self, O, R):
		self.O = copy.deepcopy(O)
		self.R = int(round(R))
	
	def tracer(self, canevas, **options):
		return canevas.create_oval(self.O.x-self.R,self.O.y-self.R,
								   self.O.x+self.R,self.O.y+self.R,
								   options)
		
	def tangente(self, other, type):
		"""
		@param other l'autre objet
		@param type le type (0 ou 1) (dessous/dessus)
		@return (Line) la tangente
		"""
		#print self,other
		if isinstance(other,Circle):
			return self.tangente_circle(other, type)
		elif isinstance(other,Vec):
			return self.tangente_point(other, type)
		else:
			raise Exception("type non géré : %s"%other.__class__.__name__)
		
	def tangente_circle(self, other, type):
		"""
		@param other l'autre cercle
		@param type (-1 ou 1) (dessous/dessus)
		@return (Line) tangente aux deux cercle (self et other)
		"""

		AB = Line(self.O, other.O)
		A = self.O + Vec2(self.R * cos(AB.teta - type * pi/2.0), self.R  * sin(AB.teta - type * pi/2.0))
		B = other.O + Vec2(other.R * cos(AB.teta - type * pi/2.0), other.R * sin(AB.teta - type * pi/2.0))
		
		return Line(A,B)
		
		"""$
		if self.O.x < other.O.x:
			type = -type
		T = Line(self.O, other.O)
		if T.m == None: # vertical
			dx = self.R * type
			T.translate(Vec2(dx,0))
			return T
		if T.m == 0: # horizontal
			dy = self.R * type
			T.translate(Vec2(0,dy))
			return T
		else:
			T.add2K((float(self.R) / T.cosT()) * type)
			Ax = float(2*(self.O.x - (T.k-self.O.y)*T.m)) / float(2*(1.0+T.m**2))
			Ay = T.y(Ax)
			A = Vec2(Ax,Ay)
			Bx = float(2*(other.O.x - (T.k-other.O.y)*T.m)) / float(2*(1.0+T.m**2))
			By = T.y(Bx)
			B = Vec2(Bx,By)
			#print A,B
			return Line(A,B)
		"""
	
	def tangente_point(self, A, type):
		"""
		@param A Vec2 point de l'espace par lequel doit passer la tangente
		@param type (-1 ou 1) le type (dessous/dessus)
		@return (Line) tangente aux cercle passant par le point
		"""

		if A in self: # le point est à l'interieur du cercle
			return None
		# segment (AO)
		AO = Line(A,self.O)
		# distance (A,point tangent)
		dAT = sqrt(AO.lenght()**2 - self.R**2)
		teta = AO.teta + type * atan2(self.R, dAT)
		T = Vec2(A.x + cos(teta) * dAT, A.y + sin(teta) * dAT)
		return Line(A,T)
		"""$
		T = Tangente(A, self, type)
		if T.m==None: # tangente verticale
			#print "!! x=k !!"
			B = Vec2(A.x,self.O.y)
		elif T.m == 0: # tangente horizontale
			B = Vec2(self.O.x, A.y)
		else:
			#print T.m
			Bx = float(2*(self.O.x - (T.k-self.O.y)*T.m)) / float(2*(1.0+T.m**2))
			By = T.y(Bx)
			B = Vec2(Bx,By)
		#print A,B
		return Line(A,B)
		"""
	
	def __and__(self, other):
		if isinstance(other, Line):
			# x = Ax + r * cosT
			# y = Ay + r * sinT
			# (x-Ox)² + (y-Oy)² = R²
			a = 1
			b = 2 * ((other.A.x-self.O.x)*other.cosT() + (other.A.y-self.O.y)*other.sinT())
			c = (other.A.x-self.O.x)**2 + (other.A.y-self.O.y)**2 - self.R**2
			delta = b**2 - 4*a*c
			if delta > 0:
				r1 = (-b+sqrt(delta))/(2*a)
				r2 = (-b-sqrt(delta))/(2*a)
				if 0 < r1 < other.lenght() or 0 < r2 < other.lenght():
					return True
				else:
					return False
			elif delta == 0:
				r = (-b+sqrt(delta))/(2*a)
				if 0 < r < other.lenght():
					return True
				else:
					return False
			else:
				return False
			
			"""$
			#print "line & circle"
			R = self.R
			O = self.O
			if other.m != None:
				# OI.AB = 0
				# => (Ix-Oy)*ABy+(Iy-Oy)*ABy = 0
				# or I sur (AB) donc Iy = m*Ix+k
				# => Ix*ABy-m*Ix*ABy = ABx*Ox-ABy*(Oy-k)
				# d'où Ix = la formule suivante :
				Ix = float(O.x*other.AB.x+other.AB.y*(O.y-other.k)) / float(other.AB.x+other.m*other.AB.y)
				Iy = other.y(Ix)
				#print Ix, Iy
				#canevas.create_oval(Ix-5,Iy-5,Ix+5,Iy+5)
				#print Line(O,Vec2(Ix,Iy)).lenght()
				#print float(Ix-other.A.x),float(other.AB.x)
				if (Line(O,Vec2(Ix,Iy)).lenght() < R) and (0 < (float(Ix-other.A.x) / float(other.AB.x)) < 1):
					return True
				else:
					return False
			else: # x=k
				if abs(other.k-O.x) < R and (min(other.A.y, other.B.y) < O.y < max(other.A.y, other.B.y)):
					return True
				else:
					return False
			"""
		elif not other:
			return None
		else:
			raise Exception("%s.__and__(%s) pas encore gérée"%(self.__class_, other.__class__.__name__))

	def __contains__(self,other):
		if isinstance(other, Vec):
			d = len(Line(self.O, other))
			return d < self.R**2
		elif not other:
			return None
		else:
			raise Exception("%s.__contains__(%s) pas encore gérée"%(self.__class_, other.__class__.__name__))
		
	def __repr__(self):
		return "Circle"+str((self.O,self.R))
