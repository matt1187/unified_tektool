# Universal Tektronix TDSxxx memory tools "

Unified tektool is a tektool & tekfwtool from author "sschelle" with heavy modification.
Unified tektool is capable to flashing of difference flash type (from old TDS540 to new TDS794D).
A disadvantage of unified tektool is low flash speed (With PCI GPIB card ~15 minute for TDS700C/D flash and up to 40 Minute for TDS500B/700A with 28F020)
It is published under GPL V2, please follow it and don't make it closed and/or commercial variant.

Full history -> https://www.eevblog.com/forum/repair/unified-tektool-released-!-(firmware-flash-tools-for-old-tds-series) and https://www.eevblog.com/forum/repair/tektronix-tdsxxx-tektool-tekfwtool-legal-modification/

- Executable file for **Windows** -> [![download](https://github.com/matt1187/unified_tektool/tree/main/Windows)]
- unified tektool works on Win XP up to 10 fine with GPIB adapter with installed VISA driver from Agilent or NI.
- Using on Win 98 need msvcrt.dll
- source file and executable files for **MacOS** ->  https://github.com/tantratron/unified_tektool/tree/main/MacOS
- you can use terminal window under MacOS then for example run command in order to find out what flash model is installed `./tektool -i -b 0x1000000 -l 0x10`
- executable file for linux and other system -> not available yet
- **Sourcefile** (written for windows system with minGW )  ->  [![sourcefile](https://github.com/matt1187/unified_tektool/tree/main/sourcefile)]

# how to
Unified Tektool  use auto-detection of flash type and have memory-write-protect.  (Erase, program, zero-fill works only at base address 0x1000000)
Tektool works with command line in cmd from windows or terminal of linux.  Please switch Protection swith to *unprotected* and power up.  Device show no life, it is OK. Don't forgot switch it back to  *protected* after successful flash/read.

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

# to do 
Improvement of flashing speed.
Only 28F016SA offer block flash -> speedup with "minor" modification on my sourcefile
For most flash type, it need  68k Code, which it is uploaded to Tek TDSxxx and 680x0 CPU controlled flash and received data.
GPIB is always for ~1ms after receiving not aviable. This is why, bytewise programming algorithm can reach only 4kb/s  at best case. Flash need up to 100µs time to save of bit and byte. Writespeed (Burst) is 1 MB/s (1µs cycle, too fast for flash) 

# note
Only use these software if you have experience with tektronix TDSxxx repair and upgrade. It is at your own risk if your oscilloscope bricks.

	
