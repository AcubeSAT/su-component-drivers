from ast import Bytes
import serial
import socket
import os

output_path = '/home/mojo/Downloads/img_output.raw'	# select your output path and file (will be created if not exist)

ser = serial.Serial(
	port='/dev/ttyACM0',		# Serial Port to read the data from
	baudrate = 115200,		# Rate at which the information is shared to the communication channel
	parity=serial.PARITY_NONE,	# Applying Parity Checking (none in this case)
	stopbits=serial.STOPBITS_ONE,	# Pattern of Bits to be read
	bytesize=serial.EIGHTBITS,	# Total number of bits to be read
	timeout=500			# Number of serial commands to accept before timing out
)

with open(output_path, 'wb') as fl:
	os.chmod(output_path, 0o777)			# change file permissions to R/W all groups (if py script is called as root or sudo it holds rights to root)
	while 1:
		message = ser.read()
		print(message)
		fl.write(message)
