import struct
import numpy as np
import binascii	#for debugging only. python builtin so that's good


def PPP_stuff(input_barr):
	FRAME_CHAR = np.uint8(0x7E)
	ESC_CHAR = np.uint8(0x7D)
	ESC_MASK = np.uint8(0x20)

	#to start, convert to np array for array ops
	working_buf = np.frombuffer(input_barr, dtype=np.uint8).copy()		#copy bc i think this means otherwise we're writing to the original array, but i want this to be a copy


	#first logical op, find all instances of the ESC char, prepend the escape char, and xor them with 0x20
	inds = np.where(working_buf==ESC_CHAR)[0]
	working_buf[inds] = np.bitwise_xor(working_buf[inds],ESC_MASK)
	working_buf = np.insert(working_buf, inds, ESC_CHAR)

	
	#second, find all frame chars in data, prepend the escape char, and xor them with 0x20
	inds = np.where(working_buf==FRAME_CHAR)[0]
	working_buf[inds] = np.bitwise_xor(working_buf[inds],ESC_MASK)
	working_buf = np.insert(working_buf, inds, ESC_CHAR)

	#finally, prepend and postpend the frame characters 
	working_buf = np.insert(working_buf, 0, FRAME_CHAR)	
	working_buf = np.append(working_buf, FRAME_CHAR)
		
		
	b = working_buf.tobytes()
	return b
	

def PPP_unstuff(input_barr):
	FRAME_CHAR = np.uint8(0x7E)
	ESC_CHAR = np.uint8(0x7D)
	ESC_MASK = np.uint8(0x20)

	wip = np.frombuffer(input_barr, dtype = np.uint8)
	working_input = wip.copy()
	
	
	if(working_input[0] != FRAME_CHAR or working_input[working_input.size-1] != FRAME_CHAR):
		return np.array([])
	
	
	inds = (np.where(working_input==ESC_CHAR)[0] + 1)	#locate all bytes which directly follow an escape character
	working_input[inds] = np.bitwise_xor(working_input[inds], ESC_MASK)	#xor the
	working_input = np.delete(working_input, inds-1)
	
	b = working_input[1:(working_input.size-1)].tobytes()
	return b
	
	
	
def unstuff_PPP_stream(new_byte, stuff_buffer):
	FRAME_CHAR = np.uint8(0x7E)
	
	stuff_buffer = np.append(stuff_buffer,np.uint8(new_byte))
	payload = np.array([]).tobytes()
	if(new_byte == FRAME_CHAR):
		payload = PPP_unstuff(stuff_buffer.tobytes())
		stuff_buffer = np.array([np.uint8(new_byte)])	#reset stuff buffer size and cram the first element with the frame character
		
	return payload, stuff_buffer
	
	

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
		

	
	
