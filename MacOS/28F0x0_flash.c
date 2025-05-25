// special library for old flash type as  28F010 and 28F020.
#include "28F0x0_flash.h"

static int flash_erase_28F010(uint32_t base) {
	int ret = -1;
//			printf("addr: 0x%08x\n", (base));
//			printf("addr: 0x%08x\n", (base + _28F010_size));
//			printf("addr: 0x%08x\n", (base + (_28F010_size * 2)));

// base 0x100 0000
// filling flash with zero, prepare for erase	
//	if (flash_00_program (base , _28F010_size , 25)== -1)
//		goto out;
// bulk erase flash	 & FF verify 
	if (flash_erase_28F0x0(base , _28F010_size , 200) == -1)
		goto out;
	printf("\n1/3 flash successful erase\n");	
// base 0x108 0000
//	if (flash_00_program ((base + _28F010_size) , _28F010_size , 25 )== -1)
//		goto out;
	
	if (flash_erase_28F0x0((base + _28F010_size) , _28F010_size , 200) == -1)
		goto out;
// base 0x110 0000
	printf("\n2/3 flash successful erase\n");	
	
//	if (flash_00_program ((base + (_28F010_size * 2)), _28F010_size , 25 )== -1)
//		goto out;

	if (flash_erase_28F0x0((base + (_28F010_size * 2)) , _28F010_size , 200) == -1)
		goto out;

	printf("\n3/3 flash successful erase\n");	
	ret = 0;
	
out:
	return ret;
	
}

static int flash_erase_28F020(uint32_t base) {
	int ret = -1;
	
//			printf("addr: 0x%08x\n", (base));
//			printf("addr: 0x%08x\n", (base + _28F020_size));
//			printf("addr: 0x%08x\n", (base + (_28F020_size * 2)));

// base 0x100 0000
// filling flash with zero, prepare for erase	
//	if (flash_00_program (base, _28F020_size,25 )== -1)
//		goto out;
// bulk erase flash	& FF verify
	if (flash_erase_28F0x0(base , _28F020_size , 200) == -1)
		goto out;
	printf("\n1/3 flash successful erase\n");	
// base 0x110 0000
//	if (flash_00_program ((base+_28F020_size), _28F020_size,25 )== -1)
//		goto out;
	if (flash_erase_28F0x0((base + _28F020_size) , _28F020_size , 200) == -1)
		goto out;
// base 0x120 0000
	printf("\n2/3 flash successful erase\n");	
	
//	if (flash_00_program ((base+(_28F020_size*2)), _28F020_size,25 )== -1)
//		goto out;
	if (flash_erase_28F0x0((base + (_28F020_size * 2)) , _28F020_size , 200) == -1)
		goto out;
	
	printf("\n3/3 flash successful erase\n");	
	ret = 0;
	
out:
	return ret;
	
}



static int flash_program_28F0x0(uint32_t base, uint32_t data, uint32_t flash_size)
{
	int i, ret = -1;
	
	uint32_t data_array[2];			// faster algorithm
	
	data_array[0] = 0x40404040;		// faster algorithm		//  confirm flash
	data_array[1] = data;			// faster algorithm
	if (base == 0x1000000) 	{		// faster algorithm		// only at 0x1000000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command(base, 0x40) == -1)
			goto out;	
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
	else if (base == (0x1000000 + flash_size)) {			// faster algorithm		// only at 0x1800000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command((base -flash_size), 0xFF) == -1)
			goto out;
		
		if (flash_command(base, 0x40) == -1)
			goto out;
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
	else if (base == (0x1000000 + (flash_size*2))) {			// faster algorithm		// only at 0x110000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command((base - flash_size), 0xFF) == -1)
			goto out;

		if (flash_command(base, 0x40) == -1)
			goto out;
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
	else {
		if (write_memory((base-0x4), (uint8_t *)&data_array, sizeof(data_array)) == -1) // faster algorithm
			goto out;
	}

	ret = 0;
out:
	
		
	return ret;
}

static int flash_program_verify_28F0x0(uint32_t base, uint32_t data, uint32_t flash_size)
{
	int i, ret = -1;
	
	uint32_t data_array[2];			// faster algorithm
	
	data_array[0] = 0x40404040;		// faster algorithm		//  confirm flash
	data_array[1] = data;			// faster algorithm
	if (base == 0x1000000) 	{		// faster algorithm		// only at 0x1000000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command(base, 0x40) == -1)
			goto out;	
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
	else if (base == (0x1000000 + flash_size)) {			// faster algorithm		// only at 0x1800000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command((base -flash_size), 0xFF) == -1)
			goto out;
		
		if (flash_command(base, 0x40) == -1)
			goto out;
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
	else if (base == (0x1000000 + (flash_size*2))) {			// faster algorithm		// only at 0x110000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command((base - flash_size), 0xFF) == -1)
			goto out;

		if (flash_command(base, 0x40) == -1)
			goto out;
		if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
			goto out;
	}
//fast flash program (8byte in one telegram instead  4 byte per telegram)
	else {
		if (write_memory((base-0x4), (uint8_t *)&data_array, sizeof(data_array)) == -1) // faster algorithm
			goto out;
	}
// flash program verify
uint32_t	buf = 0xc0c0c0c0;
	if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;
	usleep (6);
// read  flash program verify
	if (read_memory(base,(uint8_t *)buf, 4) == -1)
		goto out;
	if (buf != data){
		printf("data mismatch\r");
	goto out;}
	ret = 0;
out:
	return ret;

}



static int flash_00_program(uint32_t base ,uint32_t flash_size , int tries)  
// size  = size of flash, 28F010 * 4 = 0x80000 , 28F020 * 4 = 0x100000
{
	uint32_t buf = 0;
	int ret = -1;
	uint32_t addr_count = 0;
	time_t start, now;
	
	time(&start);
top:
	while(1) {
// filling of flash with zero.		
		if(flash_program_28F0x0 ((base + addr_count), buf, flash_size) == -1)  
//		if(flash_program_verify_28F0x0 ((addr_count + base), buf, flash_size) == -1)  
			goto try;  		// if flash program error, then decrement try-counter(tries)
		addr_count ++;
		addr_count ++;
		addr_count ++;
		addr_count ++;
		if ((addr_count & 0xFF)==0){
			time(&now);
			fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr_count,  flash_size, ((addr_count) * 100) / flash_size, (int)(now - start));
		}
// last address? Yes -while break;	
		if ((addr_count&0xFFFFFF) == flash_size)    
			break;
		}
// set flash to read array.		
	if (flash_command(base, 0xFF) == -1)		
		goto out;
	
	ret = 0;
out:
	return ret;
	
try:
	printf("addr: 0x%08x", (base+ addr_count));
	printf("tries: %04d\n", tries);
	tries --;
	if ((tries) != 0)
		goto top;
	goto out;
}

static int flash_erase_28F0x0(uint32_t base, uint32_t flash_size, int tries){
	
int32_t 	fail_addr = base;
int 		ret = -1;	
int 		try = tries;
uint32_t	buf = 0;
uint32_t	safe_erase = 0;
uint32_t 	size;
uint32_t 	addr;

	printf("\n");
//	printf("\ntries %04d", try);
top:	
//	printf("\rtries %04d", try);
// bulk erase
	buf = 0x20202020 & safe_erase;
	if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;
	usleep (10);
//bulk erase confirm
	buf = 0x20202020 & safe_erase;
	if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;
	usleep (10000);
// erase verify command
	buf = 0xA0A0A0A0 & safe_erase;
	if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;
	usleep (8);
// reset
	if (flash_command(base, 0xFF) == -1)
		goto out;	
	if (flash_command(base, 0xFF) == -1)
		goto out;
	usleep (100000);

	size = base + flash_size - fail_addr;
	addr = fail_addr ;
		
	if(flash_FF_verify_fast(addr,size,try , &fail_addr, &safe_erase))
		goto out;
	if (fail_addr != -1){
		printf("\r            tries:%04d  addr:%08x",(tries-try), fail_addr);
		try -- ;//100%    0s  tries 0119
		if (try)
			goto top;
		else 
			goto out;
	}
// added erase for reliable flash program (else bits tilt)
safe_erase = (tries - try)/4;
	printf("\n");
	while (safe_erase){
		buf = 0x20202020;
		if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
			goto out;
		usleep (10);
	//bulk erase confirm
		buf = 0x20202020;
		if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
			goto out;
		usleep (10000);
	// erase verify command
		buf = 0xA0A0A0A0;
		if (write_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
			goto out;
		usleep (8);
	// reset
		if (flash_command(base, 0xFF) == -1)
			goto out;	
		if (flash_command(base, 0xFF) == -1)
			goto out;
		usleep (100000);
		printf("\r %2d", safe_erase);
		safe_erase -- ;
	}
	
	
	ret = 0;
out:
	return  ret;
}




static int flash_FF_verify_fast(uint32_t base, uint32_t size, uint32_t try, uint32_t *fail_addr , uint32_t *safe_erase){

int  ret = -1;
uint32_t addr = base;
uint8_t buf[1024] ;
uint32_t  verify_count = 0;
uint32_t len;
time_t start, now;	
	
	
	*fail_addr = -1;
	*safe_erase = 0;
 
	time(&start);
	
	while (base + size - addr){
	
//512 , 0x100 0000 + 0x80000 - 1080000
		len = MIN(512, base + size - addr);
		if (read_memory(addr, buf, len) == -1)
			goto out;
		verify_count = 0;
		while(len - verify_count){ //len instead 512
			if (buf[verify_count] != 0xFF)			// old code, software hang  if not zero = addr % 0x4
			{
				*fail_addr = addr;
				*safe_erase |= 0x000000FF;
				
			}
			verify_count++;				
			
			if (buf[verify_count] != 0xFF)			// old code, software hang  if not zero = addr % 0x4
			{
				*fail_addr = addr;
				*safe_erase |= 0x0000FF00;
		
				
			}
			verify_count++;				
			
			if (buf[verify_count] != 0xFF)			// old code, software hang  if not zero = addr % 0x4
			{
				*fail_addr = addr;
				*safe_erase |= 0x00FF0000;
			}
			verify_count++;					
			
			if (buf[verify_count] != 0xFF)			// old code, software hang  if not zero = addr % 0x4
			{
				*fail_addr = addr;
				*safe_erase |= 0xFF000000;
				goto out1; 
				
			}
			verify_count++;			


		}	
		addr += len;
		if ((addr % 0x100) == 0){
			time(&now);
//			fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr - base, size, ((addr - base) * 100) / size, (int)(now - start));
			fprintf(stderr, "\r %3d%% %4ds",  ((addr - base) * 100) / size, (int)(now - start));
//			printf("  tries:%04d", try);
		}
	}
	
out1:
	ret = 0;
out:	
	return ret;
	
}

