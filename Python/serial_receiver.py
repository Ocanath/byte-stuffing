import serial
from serial.tools import list_ports
from PPP_stuffing import *
import argparse
import binascii


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
			print("failded.")
			pass


	try:
		stuff_buffer = np.array([])
		while(True):
			bytes = slist[0].read()	#should be able to add arg to this
			if(len(bytes) != 0):
				npbytes = np.frombuffer(bytes, np.uint8)
				for b in npbytes:
					# print(b)
					payload, stuff_buffer = unstuff_PPP_stream(b,stuff_buffer)
					if(len(payload) != 0):
						print("Payload = "+str(payload.decode()))
						# print("Payload = "+str(binascii.hexlify(payload)))

	except KeyboardInterrupt:
		pass
	
	print("done");
	for s in slist:
		s.close()