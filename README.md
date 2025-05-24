# Project  unified tektool
"universial Tektronix TDSxxx memory-tools "

Unified tektool is a tektool & tekfwtool from author "sschelle" with heavy modififaction.
Unified tektool is capable to flashing of difference flash type (from old TDS540 to  TDS794D).
It is published under GPL V2, pleas follow it and don't make it closed and/or commercial variant. Small notice: i am only autodictat, i have never learned programming on shool/university. This is why, i have wreid coding style.

Full history -> (https://www.eevblog.com/forum/repair/unified-tektool-released-!-(firmware-flash-tools-for-old-tds-series)/)

- Executable file for **Windows** -> [![download](https://github.com/matt1187/unified_tektool/tree/main/Windows)]
- Win XP up to 10 works fine with GPIB adapter with installed VISA driver from Agilent or NI.
- Win 98 need msvcrt.dll 
- executable file for linux and other system -> not aviable yet. I'll published him, if i make one or i got executable file.
- **Sourcefile** (written for windows system with minGW )  ->  [![sourcefile](https://github.com/matt1187/unified_tektool/tree/main/sourcefile)]



# how to
Unified Tektool  use auto-detection of flash type and have memory-write-protect.  (Erase, program, zero-fill works only at base address 0x1000000)
Tektool works with command line in cmd from windows or terminal of linux.
![faster_program, but yet too slow(without 0xFFcommando)](https://github.com/user-attachments/assets/afbe8051-3cee-4a15-90a0-46fe7c8aa6b6)

- Flash identify `tektool -i -b 0x1000000 -l 0x10`
- Flash zero fill (only  28F010 and 28F020) `tektool -z -b 0x1000000 -l 0x10`
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
Replay proccess without zerofilling, if it is not yet successful.
(not conform with datasheet of FLASH and in future: you would get difficult with reprogramming in future. But few zerofilling & erasing cycle may help)

# note
It is your risk. Pleas don't  hit me, if your unit is a brick.

	
