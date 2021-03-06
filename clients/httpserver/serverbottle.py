#!/usr/bin/python

from bottle import route, run
from bottle import static_file

import threading
import sys
import socket

# http://bottlepy.org/docs/dev/tutorial.html

# -----------------------------
# Connexion au serveur robot
# -----------------------------

HOST = 'localhost'
PORT = 50000
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((HOST, PORT))

# -----------------------------
# API
# -----------------------------

@route('/api/ping/:id')
def ping(id):
	s.send('asserv 1')
	data = s.recv(1024)
	return '{pong:"%s"}' % data


@route('/api/recalageRouge/:id')
def recalage(id):
	s.send('asserv 7:1')
	#data = s.recv(1024)
	return '{recalage:"done"}' % id

@route('/api/recalageBleu/:id')
def recalage(id):
	print "recalage bleu"
	s.send('asserv 7:0')
	print "cmd sent"
	data = s.recv(1024)
	return '{recalage:"done"}'

@route('/api/stop/:id')
def recalage(id):
	s.send('asserv 13')
	data = s.recv(1024)
	return '{recalage:"done"}'

@route('/api/gorelative/:x/:y')
def gorelative(x, y):
	s.send('3.42.2.'+x+'.'+y+'.100')
	data = s.recv(1024)
	return '{gorelative:"done"}'

# -----------------------------
# fichiers statiques
# -----------------------------

@route('/')
@route('/:filename#.*#')
def send_static(filename='index.html'):
    return static_file(filename, root='../viewmonitor')

# -----------------------------
# on lance le serveur
# -----------------------------

run(host='', port=8080)
