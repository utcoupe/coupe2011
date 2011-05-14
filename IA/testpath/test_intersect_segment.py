# -*- coding: utf-8 -*-


import sys
sys.path.append("../")
from geometry.vec import *
from geometry.circle import *
from geometry.line import *
from pathfinding import *


# importation de la bibliotheque graphique Tkinter
from Tkinter import *
# creation d’une fenetre
fenetre=Tk()
fenetre.title('dessin')
# creation d’un canevas
canevas=Canvas(fenetre)
canevas.configure(width=500,height=500,bg='white')
canevas.pack()

line = None
A = None
B = None
segment = Line(Vec2(100,100), Vec2(300,300))

def clic_A(event):
	global A
	A = Vec2(event.x,event.y)

def clic_B(event):
	global A, line
	if line: canevas.delete(line)
	B = Vec2(event.x,event.y)
	l = Line(A,B)
	if l & segment and segment & l:
		print "intersection"
		line = l.tracer(canevas,fill="red")
	elif l & segment or segment & l:
		print "probleme"
		line = l.tracer(canevas,fill="blue")
	else:
		print "pas intersection"
		line = l.tracer(canevas,fill="green")
	A=None
	
def motion(event):
	global A, line
	if A:
		if line: canevas.delete(line)
		B = Vec2(event.x,event.y)
		l = Line(A,B)
		if l & segment and segment & l:
			print "intersection"
			line = l.tracer(canevas,fill="red")
			(l & segment).tracer(canevas)
			(segment & l).tracer(canevas)
		elif l & segment or segment & l:
			print "probleme"
			line = l.tracer(canevas,fill="blue")
		else:
			print "pas intersection"
			line = l.tracer(canevas,fill="green")
		

canevas.bind('<ButtonPress-1>', clic_A)
canevas.bind('<ButtonRelease-1>', clic_B)
canevas.bind('<Motion>', motion)

segment.tracer(canevas)

# attente des evenements
fenetre.mainloop()
