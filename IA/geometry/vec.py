# -*- coding: utf-8 -*-


class Vec:
	def __init__(self, *coords):
		self.coords = [ int(round(_)) for _ in coords ]

	def __repr__(self):
		return "Vec"+str(tuple(self.coords))

	def __add__(self,other):
		return Vec(map(lambda a: a[0]+a[1], zip(self.coords,other.coords)))

	def __iadd__(self,other):
		self.coords = map(lambda a: a[0]+a[1], zip(self.coords,other.coords))

	def __getattr__(self,name):
		if name == "x":
			return self.coords[0]
		elif name == "y":
			return self.coords[1]
		elif name == "z":
			return self.coords[2]
		elif name == "t":
			return self.coords[3]
		else:
			raise AttributeError("Vec n'a pas d'atribut: '%s'"%name)

	def __getitem__(self,i):
		return self.coords[i]

	def __str__(self):
		return self.__repr__()


	

class Vec2(Vec):
	def __init__(self,x,y):
		Vec.__init__(self, x,y)
	
	def tracer(self, canevas, color="black"):
		return canevas.create_oval(self.x-3,self.y-3,self.x+3,self.y+3, fill=color, width=3)
	
	def tangente(self, other, type):
		return other.tangente(self, -type)
			
	def __repr__(self):
		return "Vec2"+str((int(self.x),int(self.y)))

