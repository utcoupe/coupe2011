# -*- coding: utf-8 -*-

from math import *
import time

from geometry.vec import *
from geometry.circle import *
from geometry.line import *
from geometry.rectangle import *

IN_MOTION = True # tracer le chemin meme pendant le mouvement de la souris
DEBUG_FIND_PATH = False


canevas=None
def setCanevas(c):
	""" sert à passer n canevas pour que le debug puisse tracer des lignes """
	global canevas
	canevas = c

def dpr(*o):
	""" debug print """
	if DEBUG_FIND_PATH:
		print o

INVALID = 999999999
maxi = 0
def rec_find_path(A,B, pions, done=[], D=None, type=None, intersect=None):
	"""
	Fonction réccursive pour trouver le plus petit chemin entre 2 points ne passant pas par un obstacle

	@param A,B (Vec2) départ/arrivée
	@param pions (list<Circle>) les obstacles
	@param done (list<Circle>) les obstacles déja évité dans le chemin courant
	@param D (Ligne) la droite A,B
	@param type (-1/1) dessous/dessus
	@param intersect je sais plus
	"""
	global maxi,canevas
	if D==None:
		#print "D(%s) A(%s) B(%s)"%(D,A,B)
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
	rect = Rectangle(Vec2(0,0),Vec2(3000,2100))
	for circle in pions:
		if circle not in done:
			#print circle
			if D & circle:
				if type==-1 or type==None:
					# par dessous
					T1 = A.tangente(circle, -1)
					T2 = circle.tangente(B, -1)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,pions,done+[circle],T1,-1,I)
						d2, path2 = rec_find_path(circle,B,pions,done+[circle],T2,-1,I)
						D1,P1 = d1+d2, path1+path2
					else:
						D1,P1 =  INVALID,[]
				else:
					D1,P1 =  INVALID,[]
				
				if type==1 or type==None:
					# par dessus
					T1 = A.tangente(circle, 1)
					T2 = circle.tangente(B, 1)
					I = T1 & T2
					if I and I in rect:
						d1, path1 = rec_find_path(A,circle,pions,done+[circle],T1,1,I)
						d2, path2 = rec_find_path(circle,B,pions,done+[circle],T2,1,I)
						D2,P2 = d1+d2, path1+path2
					else:
						D2,P2 =  INVALID,[]
				else:
					D2,P2 =  INVALID,[]

				if not IN_MOTION: canevas.delete(l)
				if D1 != INVALID or D2 != INVALID:
					if D2 < D1:
						dpr(D2,P2)
						return D2,P2
					else:
						dpr(D1,P1)
						return D1,P1
				else:
					dpr("Point mort %s,%s"%(str(A),str(B)))
					dpr(INVALID)
					return INVALID,[]
	else:
		if not IN_MOTION: canevas.delete(l)
		if intersect!=None:
			if isinstance(A,Vec2):
				D = Line(A,intersect)
			elif isinstance(B,Vec2):
				D = Line(intersect,B)
		dpr(D.lenght(), [D])
		return D.lenght(), [D]
			
	
def find_path(A,B, pions):
	"""
	Trouve le plus court chemin entre A et B en évitant les obstacles

	@param A,B (Vec2) depart/arrivé
	@param pions (liste<Circle>) les obstacles

	@return liste< <x,y> > le chemin
	"""
	for p in pions:
		if A in p or B in p:
			return []
	global maxi
	maxi = 0
	d, droites = rec_find_path(A, B, pions)
	dpr("droites :",droites)
	path = []
	if droites:
		path.append((A.x, A.y))
		for d1,d2 in zip(droites[:-1],droites[1:]):
			dpr(d1,d2)
			I = d1 & d2
			path.append((I.x,I.y))
		path.append((B.x,B.y))
		
	dpr("path :",path)
	return path
