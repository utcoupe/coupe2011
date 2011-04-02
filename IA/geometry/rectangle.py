# -*- coding: utf-8 -*-



class Rectangle:
    def __init__(self, A, B):
        if A.x > B.x: A,B = B,A
        self.A = A
        self.B = B
    
    def __contains__(self, v2):
        return ((self.A.x < v2.x < self.B.x) and (min(self.A.y,self.B.y) < v2.y < max(self.A.y,self.B.y)))
