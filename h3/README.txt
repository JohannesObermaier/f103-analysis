H3 PoC

Preparation:
	- Wire up the devices as described below
	- Load the firmware to the the attack board
	- Load some firmware into the DuT and enable flash security (readout-protection / RDP-level 1)

Attack execution:
	- Power up both boards
	- Open the serial interface connected to the DuT (9600 Baud, 8 bits, no parity, 1 stop bit)
	- Startup the debugger for the DuT (e.g., with openocd)
	- Upload the shellcode into the DuT's SRAM, e.g.,
		- telnet localhost 4444 (connect to openocd)
		- load_image shellcode.bin 0x20000000
	- Disconnect the debugger
	- Push the BLUE button on the attack board, LEDs flash shortly
	- A shell appears on the serial interface

--------------------------------------------------

Electrical setup:

Attack Board: STM32F3 Discovery
DuT: STM32F1 or APM32F1 based device.

Attack Board 	<-> 	DuT
PD0+PD1		<->	3V3 (Power input)
PD2		<->	BOOT0
PD4		<->	~RESET

CN4-Pin1	<->	SWD-Clock
CN4-Pin3	<->	SWD-Data

JP4-TX		<->	PA10 (RX)
JP4-RX		<->	PA9 (TX)


The debug interface to the DuT is provided by the (integrated) debugger of the Attack Board.
The serial interface to the DuT is also provided by the Attack Board.
Please note that PD0 and PD1 have to be connected in parallel. This is necessary due to the high amount of current required by some DuTs.

--------------------------------------------------

Attack Board Firmware: ./attackboard
SRAM-Firmware/shellcode: ./rootshell