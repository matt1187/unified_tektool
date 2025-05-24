# Project  unified tektool
"universial Tektronix TDSxxx memory-tools "

A heavy modififaction of tektool & tekfwtool from author "sschelle" .  It is published under GPL V2, pleas follow it and don't make it closed and/or commercial variant.

Full history -> (https://www.eevblog.com/forum/repair/unified-tektool-released-!-(firmware-flash-tools-for-old-tds-series)/)

- executable file for Windows 
Win XP up to 10 works fine with GPIB adapter with installed VISA driver from Agilent or NI.
Win 98 need msvcrt.dll 
- executable file for linux and other system
  [ ] i am planing.

# how to using
Unified Tektool  use auto-detection of flash type and have memory-write-protect.  (Erase, program, zero-fill works only at base address 0x1000000)

- Flash identify `tektool -i -b 0x1000000 -l 0x10`
-	Flash zero fill (only  28F010 and 28F020) `tektool -z -b 0x1000000 -l 0x10`
- Flash erase  `tektool -e -b 0x1000000 -l 0x10`
- Flash program `tektool -p example.bin -b  0x1000000 -l 0x180000`
- Flash verify `tektool -v example.bin -b  0x1000000 -l 0x180000`
- Memory write (it doesn't work at flash)  `tektool -w example.bin -b 0x4000000 -l 0x20000`
- Memory read  (it works also at flash) `tektool -r  example.bin -b 0x1000000 -l 0x300000`
- HEEEELLLLP ! `tektool -h`

- Note: length is depent of flash type.
 ` 0x180000`                      0x01a7  = 28F010 (AMD)            
 ` 0x300000`                      0x012a  = 28F020 (AMD)            
 ` 0x180000`                      0x89b4  = 28F010 (Intel)            
 ` 0x300000`                      0x89bd  = 28F020 (Intel)            
 ` 0x400000`                      0x89a0  = 28F016SA
 ` 0x400000`                      0x89a2  = 28F008SA
 ` 0x400000`                      0xb0d0  = 28F160S5

- tipp for using of 28F010/020 flash, which it is tilted again.



	
