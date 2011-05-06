# -*- coding: utf-8 -*-

from math import *
import time

from geometry.vec import *
from geometry.circle import *
from geometry.line import *
from geometry.rectangle import *

IN_MOTION = True # tracer le chemin meme pendant le mouvement de la souris





maxi = 0
def rec_find_path(A,B, pions, done=[], D=None, type=None, intersect=None):
	global maxi
	R = 50
	if D==None:
		print "D(%s) A(%s) B(%s)"%(D,A,B)
		D = Line(A,B)
	#print "rec_find_path(%s) on %s"%(maxi,D)
	if not IN_MOTION:
		print A
		print B
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
				if type==-1 or type==None:
					# par dessous
					T1 = A.tangente(circle, -1)
					T2 = circle.tangente(B, -1)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,pions,done+[p],T1,-1,I)
						d2, path2 = rec_find_path(circle,B,pions,done+[p],T2,-1,I)
						D1,P1 = d1+d2, path1+path2
					else:
						D1,P1 =  999999999,[]
				else:
					D1,P1 =  999999999,[]
				
				if type==1 or type==None:
					# par dessus
					T1 = A.tangente(circle, 1)
					T2 = circle.tangente(B, 1)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,pions,done+[p],T1,1,I)
						d2, path2 = rec_find_path(circle,B,pions,done+[p],T2,1,I)
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
			
	
def find_path(A,B, pions):
	global maxi
	maxi = 0
	d, droites = rec_find_path(A, B, pions)
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
