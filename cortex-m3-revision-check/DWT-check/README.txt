See errata: PR141-PRDC-007452 -> Behavior of the DWT has been updated/improved


Execute this in gdb after connecting to openocd+microcontroller:

p {int*}0xe0001000=0x400A0001 # Enable DWT (Tracing/Profiling unit, enable cycle counter, CPI and SLEEP counter)
p {int*}0x20000000=0x21002000 # write to SRAM: mov r0, 0; mov r1, 0
p {int*}0x20000004=0x0001f100 # add r0, 1
p {int*}0x20000008=0xbf20bf20 # wfe; wfe
p {int*}0x2000000C=0xbe2abf20 # wfe; bkpt
p $pc=0x20000000 # set PC to SRAM/0x20000000
p $primask=1 # primask 1
p $faultmask=1 # faultmask 1
p {int*}0xe0001008=0 # reset CPI counter
cont # continue execution
#break with ctrl+c
x/16x 0xe0001000 # dump counters

If the value at 0xe0001008 is DIFFERENT each time (has any value between 0 and 0xFF), then the core is affected by the errata. ==> This is the old version of the core.

If the value at 0xe0001008 is THE SAME each time (has a small value), then the core is fixed ==> New version of the core.