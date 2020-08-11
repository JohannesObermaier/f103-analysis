The APM32, CKS32, and GD32 claim to have a newer core revision according to their CPUID.
To validate the CPUID, since it could also be faked, we have two tools in this folder.

VTOR-check: Uses an implementation change in the VTOR register to detect the core revision.
DWT-check: Uses a bug in the DWT module to detect the core revision. This is something that noone would really like to fix "manually" since this has very little benefit to the user and only is observable under some special conditions.

------

Both checks are harmless and do not expose any security issues so far.

The APM32, CKS32, GD32 (F103 and F130) have CPUID: 0x412fc231
The original STM32 has 0x411fc231

Thus, APM32, CKS32, GD32 are Revision 2, Patch 1, but the STM32 if Revision 1, Patch 1
