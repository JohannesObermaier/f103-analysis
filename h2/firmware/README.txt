H2 PoC

Preparation:
	- Load some firmware into the DuT and disable the debug interface (high security / RDP-level 2)
	- Gain access to the IO2 bonding wire (sanding paper, etching, etc.)
		(- Tip: To detect the bonding wire -> Compare the traffic with the data provided for H1! The bootloader should be identical.)
	- Load the firmware to the attack board (STM32F3 Discovery)
	- Wire up the devices as described below

Attack execution:
	- Power up the attack board
	- Power up the DuT
	- The attack is executed automatically (Red LED illuminated: RDP loading detected and fault injected)
	- Attach to the DuT via the debugger -> SUCCESS
	- (if unsuccessful: disconnect the DuT from the power, push the black RESET button on the attack board to re-arm the attack board, and try again)


--------------------------------------------------------------

Electrical connections:
Attack Board 		<->	DuT
PD0+PD1				QSPI-IO2 bonding wire

The debugger is connected to the DuT.
PD0 and PD1 must be connected in parallel due to the power demand of some DuTs.

