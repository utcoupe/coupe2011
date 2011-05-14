# -*- coding: utf-8 -*-


class Vec:
	def __init__(self, *coords):
		self.__dict__["coords"] = [ int(round(_)) for _ in coords ]

	def __repr__(self):
		return "Vec"+str(tuple(self.coords))

	def __add__(self,other):
		if isinstance(other, Vec):
			return Vec(*map(lambda a: a[0]+a[1], zip(self.coords,other.coords)))
		else:
			return Vec(*map(lambda a: a[0]+a[1], zip(self.coords,other)))

	def __iadd__(self,other):
		if isinstance(other, Vec):
			self.coords = map(lambda a: a[0]+a[1], zip(self.coords,other.coords))
		else:
			self.coords = map(lambda a: a[0]+a[1], zip(self.coords,other))

	def __eq__(self, other):
		if len(self.coords) != len(other.coords):
			return False
		for c1,c2 in zip(self.coords,other.coords):
			if c1 != c2: return False
		return True
		
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

	def __setattr__(self,name,value):
		if name == "x":
			self.__dict__["coords"][0] = value
		elif name == "y":
			self.__dict__["coords"][1] = value
		elif name == "z":
			self.__dict__["coords"][2] = value
		elif name == "t":
			self.__dict__["coords"][3] = value
		else:
			raise AttributeError("Vec n'a pas d'atribut: '%s'"%name)
		
	def __getitem__(self,i):
		return self.coords[i]

	def __str__(self):
		return self.__repr__()
	
	def tracer(self, canevas, **options):
		return canevas.create_oval(self.x-3,self.y-3,self.x+3,self.y+3, options)


	

class Vec2(Vec):
	def __init__(self,x,y):
		Vec.__init__(self, x,y)
	
	def tangente(self, other, type):
		return other.tangente(self, -type)
			
	def __repr__(self):
		return "Vec2"+str((int(self.x),int(self.y)))

