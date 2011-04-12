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
type = 1

def clic(event):
    global type
    type = -type
    motion(event)

def motion(event):
    global line
    if line: canevas.delete(line)
    T = Vec2(event.x,event.y).tangente(circle,type)
    #T = circle.tangente(Vec2(event.x,event.y),type)
    line = T.tracer(canevas,"black")

canevas.bind('<ButtonPress-1>', clic)
canevas.bind('<Motion>', motion)


canevas.create_oval(pion.x-50,pion.y-50,pion.x+50,pion.y+50,outline='blue',width=1)

# attente des evenements
fenetre.mainloop()