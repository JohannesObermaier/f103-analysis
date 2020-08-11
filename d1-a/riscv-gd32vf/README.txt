D1-A PoC for GD32VF103 (RISC-V)

Preparation:
	- Load some firmware into the DuT and enable flash security (readout-protection / RDP-level 1)
	- Wire up the system as described below

Attack execution:
	- Start up the serial interface (115200 Baud, 8 bits, no parity, 1 stop bit)
	- Power up the DuT
	- Connect a debugger to the DuT
	- Load the attack firmware into the SRAM
	- Execute the attack firmware
	- Firmware appears on the serial interface


--------------------------------------------------------------

Electrical connections:
Serial cable (e.g., "FTDI")	<->	DuT
RX					PA2 (TX)