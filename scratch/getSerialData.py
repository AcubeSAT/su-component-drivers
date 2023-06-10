from ast import Bytes
import serial
import socket
import os

output_path = '/home/mojo/Downloads/img_output.raw'	# select your output path and file (will be created if not exist)
serial_device = '/dev/ttyACM0'				# ttyACM0 -> USB serial communication device (CDC)
baudrate = 115200					# for ATSAMv71check your baud rate setting for MHC generated file > plib_usart1.c > line201: USART1_REGS->US_BRGR = US_BRGR_CD(81);
timeout_sel = 3000					# set high to avoid re-writing a new file stream if timeout runs out

with open(output_path, 'wb') as fl:
	os.chmod(output_path, 0o777)			# change file permissions to R/W all groups (if py script is called as root or sudo it holds rights to root)
	with serial.Serial(serial_device, baudrate, timeout=timeout_sel) as ser:	
		while 1:
			message = ser.read()
			print(message)
			fl.write(message)
