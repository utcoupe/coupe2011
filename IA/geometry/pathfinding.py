# -*- coding: utf-8 -*-

from math import *
import time
import sys

from vec import *
from circle import *
from line import *
from rectangle import *

#pions = [(250,250), (400,400), (100,100)]
pions = []




maxi = 0
def rec_find_path(A,B, done=[], D=None, type=None, intersect=None):
	global maxi, pions
	R = 50
	if D==None:
		print "D(%s) A(%s) B(%s)"%(D,A,B)
		D = Line(A,B)
	#print "rec_find_path(%s) on %s"%(maxi,D)
	if not IN_MOTION:
		l = D.tracer(canevas)
		c = raw_input()
	if maxi > 200:
		print 'problème'
		maxi = 0
		return D.lenght(), [D]
	maxi += 1
	rect = Rectangle(Vec2(0,0),Vec2(500,500))
	for p in pions:
		if p not in done:
			#print p
			circle = Circle(p, R)
			if D & circle:
				if type==None or type: # type==None or type==0
					# par dessous
					T1 = circle.tangente(A, 0)
					T2 = circle.tangente(B, 1)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,done+[p],T1,0,I)
						d2, path2 = rec_find_path(circle,B,done+[p],T2,1,I)
						D1,P1 = d1+d2, path1+path2
					else:
						D1,P1 =  999999999,[]
				else:
					D1,P1 =  999999999,[]
				
				if not type:
					T1 = circle.tangente(A, 1)
					T2 = circle.tangente(B, 0)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,done+[p],T1,1,I)
						d2, path2 = rec_find_path(circle,B,done+[p],T2,0,I)
						D2,P2 = d1+d2, path1+path2
					else:
						D2,P2 =  999999999,[]
				else:
					D2,P2 =  999999999,[]
				
				if not IN_MOTION: canevas.delete(l)
				if D2 < D1:
					print D2,P2
					return D2,P2
				elif D1 < D2:
					print D1,P1
					return D1,P1
				else:
					print "Point mort %s,%s"%(str(A),str(B))
					print 999999999
					return 999999999,[]
	else:
		if not IN_MOTION: canevas.delete(l)
		if intersect!=None:
			if isinstance(A,Vec2):
				D = Line(A,intersect)
			elif isinstance(B,Vec2):
				D = Line(intersect,B)
		print D.lenght(), [D]
		return D.lenght(), [D]
			
	
def find_path(A,B, done=[]):
	global maxi
	maxi = 0
	d, droites = rec_find_path(A, B)
	#print "droites :",droites
	for d in droites:
		d.tracer
	path = [A.x, A.y]
	for d1,d2 in zip(droites[:-1],droites[1:]):
		print d1,d2
		I = d1 & d2
		path.append(I.x)
		path.append(I.y)
	path.append(B.x)
	path.append(B.y)
		
	#print "path :",path
	return path

if __name__ == "__main__":
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
	IN_MOTION = True # tracer le chemin meme pendant le mouvement de la souris
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
		path = find_path(DEPART,ARRIVE)
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
					path = find_path(DEPART,ARRIVE)
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
