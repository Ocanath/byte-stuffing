import serial
from serial.tools import list_ports
from PPP_stuffing import *
import argparse
import time
import string
import random


"""
Intended use of this file:
	if you run serial_receiver and serial_transmitter simultaneously,
	with two CP2102's connected to your computer with TX-RX, RX-TX, GND-GND connections,
	the message produced by the transmitter will be parsed and decoded correctly by the receiver
	
	You should be able to do whatever you want to the wires and the framing will always recover if the signal is good
	
	no checksum addition/parsing, but that's added on a higher implementation layer


	TODO: improve portability of receiver ? or implement as-is in various serial plotting demos
"""



if __name__ == "__main__":

	parser = argparse.ArgumentParser(description='Serial PPP stuffing application parser')
	parser.add_argument('--CP210x_only', help="If you're finding a non serial adapter, use this to filter", action='store_true')
	args = parser.parse_args()

	slist = []	
	""" 
		Find all serial ports.
	"""
	com_ports_list = list(list_ports.comports())
	port = []

	for p in com_ports_list:
		if(p):
			pstr = ""
			pstr = p
			port.append(pstr)
			print("Found:", pstr)
	if not port:
		print("No port found")

	for p in port:
		try:
			ser = []
			if( (args.CP210x_only == False) or  (args.CP210x_only == True and (p[1].find('CP210x') != -1) or p[1].find('USB Serial Port') != -1) ):
				ser = (serial.Serial(p[0],'460800', timeout = 1))
				slist.append(ser)
				print ("connected!", p)
				break
		except:
			print("failed.")
			pass

	# payload = bytearray("what the ground loop...",encoding='utf8')
	# stuffed_payload = PPP_stuff(payload)
	# slist[0].write(stuffed_payload)
	
	try:
		iteration = 0
		while(True):
			strpayload = "this is a message" + str(iteration)
			iteration = iteration + 1
			print(strpayload)
			
			payload = bytearray(strpayload,encoding='utf8')
			stuffed_payload = PPP_stuff(payload)
			slist[0].write(stuffed_payload)
			time.sleep(0.05)	#note: removing this delay doesn't break the software per-se (the algorithm to frame and decode chugs away without issue!) but it does cause windows to BSOD, probably due to a memory overrun of some sort on the CP2102 VCP driver.
	except KeyboardInterrupt:
		pass
	
	print("done");
	for s in slist:
		s.close()