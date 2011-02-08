import time
from threads import *
import threading
import serial
import thread
import threads
import select
import subprocess
import timer



"""
ser = serial.Serial("/dev/ttyACM0", 115200)
if not ser: exit()"""


process = subprocess.Popen("./a.out", shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

"""
def f():
	try:
		while True:
			#print "coucou"
			r = ser.readline()
			print r
			output = process.stdout.readline()
			print (output,output)
	except select.error:
		print "regarde"
	finally:
		print "OUECH"
	print "fin"
	"""

def f():
	output = process.stdout.readline()
	print (output,)
	
t = timer.InteruptableThreadedLoop()
t.start(f, pause=1)
#ser.close()

time.sleep(2)
process.terminate()
#process.kill()
t.stop()

print threading.enumerate()
t.join()




"""
t = Thread(target = f)
t.start()
print t.isAlive()

print threading.enumerate()
time.sleep(1)


#t.raise_exc(Exception)
t.terminate()
t.join()

print t.isAlive()
"""


