# Echo client program
import socket

HOST = '172.26.75.178'    # The remote host
PORT = 5555              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.send('Hello, world')
#data = s.recv(1024)
s.close()
#print 'Received', repr(data)
