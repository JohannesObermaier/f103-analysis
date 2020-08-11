D1-A PoC for CKS32F103 (ARM Cortex-M3)

Preparation:
	- Load some firmware into the DuT and enable flash security (readout-protection / RDP-level 1)

Attack execution:
	- Power up the board
	- Connect a debugger
	- Startup OpenOCD
	- Execute the "cksFirmwareExtractor.py" Python script

--------------------------------------------------------------

Further explanation on what the script does:

The CPU can read from flash memory via an ldr instruction IN FLASH MEMORY while single-stepping...
Thus, we just need to know and repeatedly execute one ldr instruction in flash memory to read out the flash.
This might be difficult -- in theory -- since the flash contents are not known.
But: The ldr instruction-gadget is allowed to be in the bootloader... And the bootloader code is readable and executable in RDP level 1.
	1. Download Bootloader
	2. Search for ldr rX, [rY] (rX==rY is allowed. We do not use ldrs with offset (would be possible nevertheless but more complicated))
	3. load the address into rY, execute the instruction via single stepping
	4. get the flash memory contents from rX
	5. repeat...
	==> Implemented as proof of concept.
	(the attack can also be executed via pop instructions, via un-stacking on exception return, etc. etc.)

NB: There are 32-bit instructions that contain a 16-bit-wide ldr instruction. This is also allowed and found by the script.
This experiment shows that finding LDR instructions for such an attack is far from being impossible! This attack fetches several bytes per second. Readout of the entire memory should be possible in a few hours.


--------------------------------------------------------------

manual execution example:

Bootloader has an ldr at 0x1ffff37e
---> Ldr r1, [r0]
	reset halt
	reg pc 0x1ffff37e; #Gadget address
	reg r0 0x124; #address you want to extract from
	step
	reg r1