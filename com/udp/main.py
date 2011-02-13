'''
Created on 13 fevr. 2011

@author: HoHen
'''

import serial
from socket import *

usbdev = "/dev/tty.Bluetooth-Modem"

#ser = serial.Serial(usbdev, 115200)

host = "192.168.1.53"
port = 9876
bufsize = 1024
addr = (host,port)

UDPSock = socket(AF_INET,SOCK_DGRAM)
UDPSock.bind(addr)

print "Standby ready !"

while 1:
    data,addr = UDPSock.recvfrom(bufsize)
    print data
    #ser.write(buf)

UDPSock.close()
#ser.close()

if __name__ == '__main__':
    pass