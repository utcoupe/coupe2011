# -*- coding: utf-8 -*-


from protocole import *
import sys
import threading

i_am = "itsme"
if len(sys.argv) > 1:
	i_am = sys.argv[1]


def send(id_cmd,reponse):
	print str(id_cmd)+C_SEP1+reponse
	sys.stdout.flush()

def identification(args):
	return i_am
def ping(args):
	return "Pong"
def goalAbsolu(args):
	t = threading.Timer(5,send,(args[0],"ok2"))
	t.start()
	return "ok"	
cmd = dict()
cmd[IDENTIFICATION] = identification
cmd[PING]			= ping
cmd[GOAL_A]			= goalAbsolu

while True:
	message = raw_input()
	message = [ int(data) for data in message.split(C_SEP_SEND) ]
	id_cmd = message[0]
	command	= message [1]
	args = [id_cmd]
	if len(message) > 2:
		args += message[2:]

	send(id_cmd,cmd[command](args))
	

	
