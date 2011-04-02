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
pion = Vec2(250,250)
circle = Circle(pion,50)
A = None
B = None

def clic_A(event):
    global A
    A = Vec2(event.x,event.y)

def clic_B(event):
    global A, line
    if line: canevas.delete(line)
    B = Vec2(event.x,event.y)
    l = Line(A,B)
    if l & circle and circle & l:
        print "intersection"
        line = l.tracer(canevas,"red")
    elif l & circle or circle & l:
        print "probleme"
        line = l.tracer(canevas,"blue")
    else:
        print "pas intersection"
        line = l.tracer(canevas,"green")
    A=None
    
def motion(event):
    global A, line
    if A:
        if line: canevas.delete(line)
        B = Vec2(event.x,event.y)
        l = Line(A,B)
        if l & circle and circle & l:
            print "intersection"
            line = l.tracer(canevas,"red")
        elif l & circle or circle & l:
            print "probleme"
            line = l.tracer(canevas,"blue")
        else:
            print "pas intersection"
            line = l.tracer(canevas,"green")
        

canevas.bind('<ButtonPress-1>', clic_A)
canevas.bind('<ButtonRelease-1>', clic_B)
canevas.bind('<Motion>', motion)


canevas.create_oval(pion.x-50,pion.y-50,pion.x+50,pion.y+50,outline='blue',width=1)

# attente des evenements
fenetre.mainloop()