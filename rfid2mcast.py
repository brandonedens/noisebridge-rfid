# Software for reading tags from SAMSys RFID Reader serial port and
# multicasting tag information to local network.
# Brandon Edens
# 2013-01-29

import re
import serial
import socket
import sys

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 11332
PORT = '/dev/ttyUSB0'

if __name__ == '__main__':
    port = PORT
    if len(sys.argv) > 1:
        port = sys.argv[1]
    ser = serial.Serial(port=port, baudrate=57600, timeout=0.2)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

    while True:
        line = ser.readline()
        # Match RFID tag specified in read line akin to:
        # {Rd,d:3005FB63AC1F3841EC880467,t:EPC1G2;DF
        # where d:THIS_IS_TAG_NUMBER
        matches = re.match("{Rd,d:([0-9A-F]+),", line)
        if matches:
            rfid_tag = matches.group(1)
            print(rfid_tag)
            sock.sendto(rfid_tag, (MCAST_GRP, MCAST_PORT))
    ser.close()
