#!/usr/bin/env python
# -*- coding: utf-8 -*-



import os
ROOT_DIR  = os.path.split(os.path.split(os.path.dirname(os.path.abspath(__file__)))[0])[0]


import sys
sys.path.append(os.path.join(ROOT_DIR,"com","serverPython"))
from protocole import *
import colorConsol

import Tkinter as Tk
import threading


class Screen(Tk.Tk):
	def __init__(self,parent=None):
		Tk.Tk.__init__(self,parent)
		self.current_face = Tk.IntVar()
		self.current_face.set(AVANT)
		rb_pince_av = Tk.Radiobutton(self, text="Face avant", variable=self.current_face, value=AVANT, indicatoron=0)
		rb_pince_av.pack()
		rb_pince_ar = Tk.Radiobutton(self, text="Face arriere", variable=self.current_face, value=ARRIERE, indicatoron=0)
		rb_pince_ar.pack()
		b_ascenseur_up = Tk.Button(self, text="Monter ascenseur", command=self.b_ascenseur_up)
		b_ascenseur_up.pack()
		b_ascenseur_up = Tk.Button(self, text="Descendre ascenseur", command=self.b_ascenseur_down)
		b_ascenseur_up.pack()
		b_close = Tk.Button(self, text="Fermer les pinces", command=self.b_pinces_close)
		b_close.pack()
		b_serre = Tk.Button(self, text="Serrer les pinces", command=self.b_pinces_serre)
		b_serre.pack()
		b_open = Tk.Button(self, text="Ouvrir les pinces", command=self.b_pinces_open)
		b_open.pack()
		while True:
			msg = raw_input()
			msg_split = msg.split(C_SEP_SEND)
			if len(msg_split) > 2 and int(msg_split[1]) == -999:
				print "%s%s%s%s%s"%(ID_SERVER,C_SEP_SEND,-999,C_SEP_SEND,ID_IA)
				sys.stdout.flush()
				break

	def send(self, id_device, id_cmd, *args):
		msg = str(id_device)+C_SEP_SEND+"42"+C_SEP_SEND+str(id_cmd)
		for a in args:
			msg += C_SEP_SEND+str(a)
		print msg
		sys.stdout.flush()
	
	def b_ascenseur_up(self):
		self.send(ID_OTHERS, Q_SETPOSITION, self.current_face.get(), HAUT)
		
	def b_ascenseur_down(self):
		self.send(ID_OTHERS, Q_SETPOSITION, self.current_face.get(), BAS)
		
	def b_pinces_close(self):
		self.send(ID_AX12, Q_CLOSE, self.current_face.get())
		
	def b_pinces_serre(self):
		self.send(ID_AX12, Q_SERRE, self.current_face.get())
		
	def b_pinces_open(self):
		self.send(ID_AX12, Q_OPEN_MAX, self.current_face.get())



s = Screen()
s.mainloop()
