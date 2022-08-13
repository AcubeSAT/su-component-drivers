from ast import Bytes
import serial
import socket

# while 1:
#     with open('/home/mojo/wspace/output2.raw', 'ab') as fl:
#             with serial.Serial('/dev/ttyACM0', 115200, timeout=1) as ser:
#                 message = ser.read()
#                 amsg = message.hex()
#                 bmsg = bytes(amsg, 'ascii')
#                 print(bmsg)
#                 fl.write(bmsg)

# with open('/home/mojo/wspace/output2.raw', 'ab') as fl:
#         with serial.Serial('/dev/ttyACM0', 115200, timeout=1) as ser:
#             while 1:
#                 message = ser.read()
#                 amsg = message.hex()
#                 bmsg = bytes(amsg, 'ascii')
#                 print(message)
#                 fl.write(bmsg)

with open('/home/mojo/wspace/output4.raw', 'ab') as fl:
        with serial.Serial('/dev/ttyACM0', 115200, timeout=3000) as ser:
            while 1:
                message = ser.read()
                print(message)
                fl.write(message)