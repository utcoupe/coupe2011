#!/usr/bin/env python
# -*- coding: utf-8 -*-


import Tkinter as Tk
import threading


class Screen(Tk.Tk):
	def __init__(self,parent=None,w=50,h=25):
		Tk.Tk.__init__(self,parent)
		self.parent = parent
		self.initialize(w,h)
	
	def initialize(self,w,h):
		""" create view """
		self.text = Tk.Text(self, width=w, height=h)
		self.text.pack()
	
	def write(self,msg):
		self.text.insert("%d.%d" % (0, 0), msg)
	


screen = Screen()

def looper():
	while True:
		cmd = raw_input()
		if cmd == 'q':
			break
		cmd = cmd + '\n'
		screen.write(cmd)

t = threading.Thread(None, looper, None)
t.start()

screen.mainloop()

