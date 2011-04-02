# -*- coding: utf-8 -*-


import time
import sys
sys.path.append("../")


from geometry.vec import *
from geometry.circle import *
from geometry.line import *
from pathfinding import *

#pions = [(250,250), (400,400), (100,100)]


# importation de la bibliotheque graphique Tkinter
from Tkinter import *
# creation d’une fenetre
fenetre=Tk()
fenetre.title('dessin')
# creation d’un canevas
canevas=Canvas(fenetre)
canevas.configure(width=500,height=500,bg='white')
canevas.pack()

DEPART = None
ARRIVE = None
line = None
lines = None
left_press = False
right_press = False
pion = None
pions = [Vec2(250,250)]
def clic_A(event):
    global DEPART, line, left_press
    left_press = True
    DEPART = Vec2(event.x, event.y)
    if line: canevas.delete(line)
    line = canevas.create_line(DEPART.x,DEPART.y,DEPART.x,DEPART.y,fill="red")
    
def clic_B(event):
    global DEPART, B, line, lines, left_press
    left_press = False
    if line: canevas.delete(line)
    if lines: canevas.delete(lines)
    ARRIVE = Vec2(event.x, event.y)
    
    #print DEPART,ARRIVE
    
    t = time.time()
    path = find_path(DEPART,ARRIVE,pions)
    print (time.time() - t)*100,"ms"
    lines = canevas.create_line(path,fill="green",width=1)
    DEPART = None
    B = None

def press_right(event):
    global right_press
    right_press = True

def addPion(event):
    global right_press, pions
    right_press = False
    canevas.create_oval(event.x-50, event.y-50,event.x+50, event.y+50, fill="yellow")
    pions.append(Vec2(event.x,event.y))
    
def motion(event):
    global DEPART, line, pion, lines
    if line: canevas.delete(line)
    if pion: canevas.delete(pion)
    if left_press:
        if lines: canevas.delete(lines)
        if DEPART:
            line = canevas.create_line(DEPART.x,DEPART.y,event.x, event.y,fill="red")
            
            if IN_MOTION:
                ARRIVE = Vec2(event.x, event.y)
                t = time.time()
                path = find_path(DEPART,ARRIVE,pions)
                print (time.time() - t)*100,"ms"
                lines = canevas.create_line(path,fill="green",width=1)
    elif right_press:
        pion = canevas.create_oval(event.x-50, event.y-50,event.x+50, event.y+50, fill="red")
    
    
canevas.bind('<ButtonPress-1>', clic_A)
canevas.bind('<ButtonRelease-1>', clic_B)
canevas.bind('<Motion>', motion)
canevas.bind('<ButtonPress-3>', press_right)
canevas.bind('<ButtonRelease-3>', addPion)


for p in pions:
    canevas.create_oval(p.x-50,p.y-50,p.x+50,p.y+50,outline='blue',width=1)

# attente des evenements
fenetre.mainloop()
