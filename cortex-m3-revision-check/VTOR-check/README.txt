The size of VTOR has been extended from 29:7 to 31:7 in Version 2

http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100165_0201_00_en/ric1414056327082.html

Use openocd to verify this: Write 0xFFFFFFFF into the VTOR register and read back the value

mww 0xe000ed08 0xFFFFFFFF
mdw 0xe000ed08 1

If result == 0xFFFFFF80 --> Core Version 2 (APM32, CKS32, GD32)
If result == 0x3FFFFF80 --> Core Version 1 (STM32)