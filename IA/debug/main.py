#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
DEBUG de l'IA
"""

import sys
import traceback


D_UPDATE_POS	=	0
D_PIONS			=	1
D_SHOW_PATH		=	2
D_DELETE_PATH	=	3

if __name__ == "__main__":
	import Tkinter as Tk
	import threading

	import sys
	sys.path.append("../")
	sys.path.append("./")
	from geometry.vec import *
	from geometry.circle import *
	from geometry.line import *


	class Screen(Tk.Tk):
		def __init__(self,parent=None,w=600,h=420):
			Tk.Tk.__init__(self,parent)
			self.parent = parent
			self.initialize(w,h)
			self.pos = None
			self.pions = []
			self.path = []
		
		def initialize(self,w,h):
			""" create view """
			self.canevas=Tk.Canvas(self)
			self.canevas.configure(width=600,height=420,bg='white')
			self.canevas.pack()
			self.canevas.create_rectangle(0,0,80,80,fill="red")
			self.canevas.create_rectangle(520,0,600,80,fill="blue")
			self.canevas.create_rectangle(0,80,80,420,fill="green")
			self.canevas.create_rectangle(520,80,600,420,fill="green")
			for i in xrange(90,500,70):
				for j in xrange(0,421,70):
					if (i-90)%140:
						if (j%140):
							self.canevas.create_rectangle(i,j,i+70,j+70,fill="blue")
						else:
							self.canevas.create_rectangle(i,j,i+70,j+70,fill="red")
					else:
						if (j%140):
							self.canevas.create_rectangle(i,j,i+70,j+70,fill="red")
						else:
							self.canevas.create_rectangle(i,j,i+70,j+70,fill="blue")
							

		def update_pos(self, pos):
			if self.pos: self.canevas.delete(self.pos)
			x,y,a = pos
			x = self.X(x)
			y = self.Y(y)
			self.pos = self.canevas.create_oval(x-5,y-5,x+5,y+5,outline='black',fill='black')

		def update_pions(self, pions):
			if self.pions:
				for p in self.pions:
					self.canevas.delete(p)
			self.pions = []
			if pions:
				if type(pions[0]) == type((1,1)) or type(pions[0]) == type([1,1]):
					for p in pions:
						x,y,t = p
						x = self.X(x)
						y = self.Y(y)
						self.pions.append(Circle(Vec2(x,y),20).tracer(self.canevas, fill="yellow"))
				else:
					t,x,y = pions
					x = self.X(x)
					y = self.Y(y)
					self.pions.append(Circle(Vec2(x,y),20).tracer(self.canevas, fill="yellow"))

		def show_path(self, path):
			self.delete_path()
			new_path = []
			for i in xrange(0,len(path),2):
				new_path.append(self.X(path[i]))
				new_path.append(self.Y(path[i+1]))
			self.path = self.canevas.create_line(new_path,fill="pink", width=2, arrow=Tk.LAST)
		
		def delete_path(self, params=None):
			if self.path:
				self.canevas.delete(self.path)
			self.path = []

		def X(self, x):
			return 600 - (x * 2 / 10)

		def Y(self, y):
			return y * 2 / 10
			
	def looper():
		while True:
			cmd = raw_input()
			try:
				id_cmd,params = cmd.split('.')
				id_cmd = int(id_cmd)
				params = eval(params)
			except ValueError:
				print "ERROR : mauvais formatage '%s'"%cmd
			except SyntaxError:
				print "ERROR : mauvais formatage '%s'"%cmd
			except Exception as ex:
				print ex
			else:
				try:
					if id_cmd == D_UPDATE_POS:
						screen.update_pos(params)
					elif id_cmd == D_PIONS:
						screen.update_pions(params)
					elif id_cmd == D_SHOW_PATH:
						screen.show_path(params)
					elif id_cmd == D_DELETE_PATH:
						screen.delete_path(params)
				except Exception as ex:
					print "ERROR :",ex
					print traceback.print_tb(sys.exc_info()[2])



	screen = Screen()

	t = threading.Thread(None, looper, None)
	t.start()

	screen.mainloop()
