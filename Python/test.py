from PPP_stuffing import *
import binascii	#for debugging only. python builtin so that's good

# payload = bytearray([1,0xFF,0x7E,0x5,0x7D,3,0x7E,0xAB,0xEF,0x7D,4])
payload = np.uint8(np.random.randint(255,size=10)).tobytes()
print("input: "+str(binascii.hexlify(payload)))
stuffed_message = PPP_stuff(payload)


wbf = np.frombuffer(stuffed_message, dtype = np.uint8).copy()
nbf = np.frombuffer(PPP_stuff(bytearray([0x7E,0x7D,0xDE,0xAD])),np.uint8).copy()
wbf = np.append(wbf, nbf)
stuffed_message = wbf.tobytes()

wbf = np.frombuffer(stuffed_message, dtype = np.uint8).copy()
secondrandompayload = np.uint8(np.random.randint(255,size=1)).tobytes()
nbf = np.frombuffer(PPP_stuff(secondrandompayload),np.uint8).copy()
wbf = np.append(wbf, nbf)
stuffed_message = wbf.tobytes()
print("other random input: "+str(binascii.hexlify(secondrandompayload)))

wbf = np.frombuffer(stuffed_message, dtype=np.uint8).copy()
wbf = np.append(wbf, np.uint8([0,0x7E,0,0,0x7E,0,0,0,0x7E,0x7D,0x7E,0x7E,0x7D,0x45,0x7E]))
stuffed_message=wbf.tobytes()


print("Value to send to over a stream: " + str(binascii.hexlify(stuffed_message)))




"""
Test of unstuffing directly 
"""
# unstuffed = PPP_unstuff(stuffed)
# print(binascii.hexlify(unstuffed))




"""
	Test unstuffing stream function with an emulated stream
"""
stuff_buffer = np.array([])
for b in stuffed_message:
	# print(hex(b))
	payload, stuff_buffer = unstuff_PPP_stream(b,stuff_buffer)
	if(len(payload) != 0):
		print("Payload = "+str(binascii.hexlify(payload)))
