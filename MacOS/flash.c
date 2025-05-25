#include "flash.h"
#include "28F0x0_flash.c"


static uint32_t Ven_Dev_ID ;
static int abort_requested1 = 0;

#define usSLEEP 10  // in microseconds

static int flash_command_8bit(uint32_t base, uint8_t cmd)   // for only 28F008SA flash.
	{
	uint8_t buf[4];

	base &= ~0xffff;
	buf[0] = cmd;
	buf[1] = cmd;
	buf[2] = cmd;
	buf[3] = cmd;

//	fprintf(stderr, "flash command 0x%02X\n", cmd);
	if (write_memory(base, buf, sizeof(buf)) == -1) {
		fprintf(stderr, "flash command failed\n");
		return -1;
	}
	return 0;
}

static int zero_fill (uint32_t base){
	
	int ret = -1;
	
	if ((base&0xF000000) != 0x1000000) {
		printf("wrong base address, zero fill stopped\n");  // if  base address is wrong (only 0x100 0000 allowed) 
		goto out;
	}
	if ((Ven_Dev_ID) ==0){
		if(flash_identify (0x1000000) == -1)
			goto out;
		printf ("\nFlash zero fill run\n");
		}
	switch (Ven_Dev_ID){
	/*	case 0x89a0:  { if(flash_program_28F016SA(base, data)) goto out; break;}
		case 0x89a2:  { if(flash_program_28F008SA(base, data)) goto out; break;}
		case 0xb0d0:  { if(flash_program_28F160S5(base, data)) goto out; break;}
	*/	case 0x01a7:  { if(flash_00_program(base, (3 * _28F010_size), 10)) goto out; break;}		// AMD 28F010
		case 0x89b4:  { if(flash_00_program(base, (3 * _28F010_size), 10)) goto out; break;}		// Intel 28F010
		case 0x012a:  { if(flash_00_program(base, (3 * _28F020_size), 10)) goto out; break;}		// AMD 28F020
		case 0x89bd:  { if(flash_00_program(base, (3 * _28F020_size), 10)) goto out; break;}		// Intel 28F020
		default: goto out; break;
		}
	ret = 0;	
out:	
	return ret;
	
	
}

static int flash_identify(uint32_t base)
{
	uint32_t Vendor_ID = 0 ;
	uint32_t Device_ID = 0 ;
	Ven_Dev_ID = 0 ;
	uint32_t buf;
	int ret = -1;
	
// security, check of base address.	
	if ((base&0xF000000) != 0x1000000) {
		printf("wrong base address, verify stopped\n");  // if  base address is wrong (only 0x100 0000 allowed) 
		goto out;
	}
	
// write  identify commando (0x90)
	if (flash_command(base, 0x90) == -1)
		goto out;
// read identify byte at address 0x00 
	if (read_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;

	printf("Flash ID1: 0x%08x\n", buf);
	Vendor_ID = buf;     				//0x89008900 = Intel 28F016SA
										//0x89898989 = Intel 28F008SA
										//0xb000b000 = Intel 28F160S5
	
	usleep (usSLEEP);
// read identify byte at address 0x01 
	if (read_memory(base + 4, (uint8_t *)&buf, sizeof(buf)) == -1)
		goto out;

	printf("Flash ID2: 0x%08x\n", buf);
	Device_ID = buf;					//0xa066a066  = 28F016SA
										//0xa2a2a2a2  = 28F008SA
										//0xd000d000  = 28F160S5
									
	Ven_Dev_ID = ((Device_ID>>8) & 0xFF) | (Vendor_ID & 0xFF00);   // first both nibble is dev_ID ,upper nibble is manufacture ID)
										//0x01a7  = 28F010 (AMD)				
										//0x012a  = 28F020 (AMD)				
										//0x89b4  = 28F010 (intel)				
										//0x89bd  = 28F020 (intel)				
										//0x89a0  = 28F016SA
										//0x89a2  = 28F008SA
										//0xb0d0  = 28F160S5	
										
	printf("Flash ID : 0x%04x\n", Ven_Dev_ID);

	switch (Ven_Dev_ID){
	case 0x01a7: { printf("Flash Type : AMD 28F010\n"); break;}
	case 0x012a: { printf("Flash Type : AMD 28F020\n"); break;}
	case 0x89a0: { printf("Flash Type : 28F016SA\n"); break;}
	case 0x89a2: { printf("Flash Type : 28F008SA\n"); break;}
	case 0x89b4: { printf("Flash Type : Intel 28F010\n"); break;}
	case 0x89bd: { printf("Flash Type : Intel 28F020\n"); break;}
	case 0xb0d0: { printf("Flash Type : 28F160S5\n"); break;}
	
	default: goto out; break;
	
	}	
										
	ret = 0;
out:
	if ((base&0xF000000) == 0x1000000) {
		if (flash_command(base, 0xFF) == -1)
			return -1;		
		if (flash_command(base, 0xFF) == -1)
			return -1;
	}
	return ret;
}

static int flash_program (uint32_t base, uint32_t data) 
{
	int ret = -1;
	
	if ((base&0xF000000) != 0x1000000) {
		printf("wrong base address, program stopped\n");  // if  base address is wrong (only 0x100 0000 allowed) 
		goto out;
	}
	if ((Ven_Dev_ID) ==0){
		if(flash_identify (0x1000000) == -1)
			goto out;
		printf ("\nFlash program process run\n");
		}

		
	switch (Ven_Dev_ID){
	case 0x89a0:  { if(flash_program_28F016SA(base, data)) goto out; break;}
	case 0x89a2:  { if(flash_program_28F008SA(base, data)) goto out; break;}
	case 0xb0d0:  { if(flash_program_28F160S5(base, data)) goto out; break;}
	case 0x01a7:  { if(flash_program_28F0x0(base, data,_28F010_size)) goto out; break;}		// AMD 28F010
	case 0x89b4:  { if(flash_program_28F0x0(base, data,_28F010_size)) goto out; break;}		// Intel 28F010
	case 0x012a:  { if(flash_program_28F0x0(base, data,_28F020_size)) goto out; break;}		// AMD 28F020
	case 0x89bd:  { if(flash_program_28F0x0(base, data,_28F020_size)) goto out; break;}		// Intel 28F020
	
	default: goto out; break;
	}
	ret = 0;	
out:	
	return ret;
}


static int flash_program_28F008SA(uint32_t base, uint32_t data)
{
	int i, ret = -1;

	uint32_t data_array[2];			// faster algorithm
	
	data_array[0] = 0x40404040;		// faster algorithm		//  confirm flash
	data_array[1] = data;			// faster algorithm
	if (base == 0x1000000) 			// faster algorithm		// only at 0x1000000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command(base, 0x40) == -1)
			goto out;

//	if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
	if (write_memory((base-0x4), (uint8_t *)&data_array, sizeof(data_array)) == -1) // faster algorithm
		goto out;

//	if (flash_wait_sr_write(base, 0x8080, 0x8080, 1000) == -1)
//		goto out;

	ret = 0;
out:

	return ret;
}


static int flash_program_28F016SA(uint32_t base, uint32_t data)
{
	int i, ret = -1;

	uint32_t data_array[2];			// faster algorithm
	
	data_array[0] = 0x40404040;		// faster algorithm		//  confirm flash
	data_array[1] = data;			// faster algorithm
	if (base == 0x1000000) 			// faster algorithm		// only at 0x1000000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command(base, 0x40) == -1)
			goto out;

//	if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
	if (write_memory((base-0x4), (uint8_t *)&data_array, sizeof(data_array)) == -1) // faster algorithm
		goto out;

//	if (flash_wait_sr_write(base, 0x0080, 0x0080, 1000) == -1)
//		goto out;
	ret = 0;
out:

	return ret;
}

static int flash_program_28F160S5(uint32_t base, uint32_t data)
{
	int i, ret = -1;
	
	uint32_t data_array[2];			// faster algorithm
	
	data_array[0] = 0x40404040;		// faster algorithm		//  confirm flash
	data_array[1] = data;			// faster algorithm
	if (base == 0x1000000) 			// faster algorithm		// only at 0x1000000 extra command, why : base-0x4 at frist = 0x0FFFFFF, flash CE disabled)
		if (flash_command(base, 0x40) == -1)
			goto out;

//	if (write_memory((base), (uint8_t *)&data, sizeof(data)) == -1)
	if (write_memory((base-0x4), (uint8_t *)&data_array, sizeof(data_array)) == -1) // faster algorithm
		goto out;

//	if (flash_wait_sr_write(base, 0x0080, 0x0080, 1000) == -1)
//		goto out;

	ret = 0;
out:
	
		
	return ret;
}


static int flash_wait_sr_write(uint32_t base, uint16_t mask, uint16_t result, int tries)
	{
	uint32_t buf;
	uint32_t _mask = (mask << 16) | mask;
	uint32_t _result = (result << 16) | result;
	int ret = -1;

	while(tries-- && !abort_requested1)
	{
		if (read_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
			goto out;
		if ((be32_to_cpu(buf) & _mask) == _result)
			break;
	}

	
	if (!tries || abort_requested1)
		return -1;
	ret = 0;
out:
	return ret;
	}



static int flash_erase (uint32_t base) 
{
	
	int ret = -1;
	if (base != 0x1000000) {
		printf("\nWrong base address, erase stopped");  // if  base address is wrong (only 0x100 0000 allowed) 
		goto out;
	}
	
	if (flash_identify(base)) {
		printf("\nIdentify failed, erase stopped");  // if  flash can't be identified, 
		goto out;
	}
	else {printf ("\nFlash erase process run\n");}	
		
		
		
	switch (Ven_Dev_ID){
	case 0x89a0:  { if(flash_erase_28F016SA(base)) goto out; break;}
	case 0x89a2:  { if(flash_erase_28F008SA(base)) goto out; break;}
	case 0xb0d0:  { if(flash_erase_28F160S5(base)) goto out; break;}
	case 0x01a7:  { if(flash_erase_28F010(base)) goto out; break;}		// AMD 28F010
	case 0x89b4:  { if(flash_erase_28F010(base)) goto out; break;}		// Intel 28F010
	case 0x012a:  { if(flash_erase_28F020(base)) goto out; break;}		// AMD 28F020
	case 0x89bd:  { if(flash_erase_28F020(base)) goto out; break;}		// Intel 28F020
	
	
	default: goto out; break;
	}
	ret = 0;	// zero = successful erase
	
out:	
	return ret;
}





static int flash_erase_28F160S5(uint32_t base)
{
	

	int i, ret = -1;

	if (flash_command(base, 0x30) == -1)
		goto out;
			usleep (usSLEEP);

	if (flash_command(base, 0xd0) == -1)
		goto out;
			usleep (usSLEEP);

	if (flash_wait_sr_erase(base, 0x0080, 0x0080, 1000) == -1)
		goto out;

	ret = 0;
out:
	if (flash_command(base, 0xff) == -1)
		return -1;
	return ret;

}


static int flash_erase_28F016SA(uint32_t base)
{
	

	int i, ret = -1;

	if (flash_command(base, 0xa7) == -1)
		goto out;
			usleep (usSLEEP);

	if (flash_command(base, 0xd0) == -1)
		goto out;
			usleep (usSLEEP);

	if (flash_wait_sr_erase(base, 0x0080, 0x0080, 1000) == -1)
		goto out;


	ret = 0;
out:
	if (flash_command(base, 0xff) == -1)
		return -1;
	return ret;

}

static int flash_erase_28F008SA(uint32_t base)  // this device can only block erase  16 erase command with incrementing of address-block
												//  block-size is  0xFFFF (64kb)
{
	uint32_t block_count =0;
	int i, ret = -1;
	
top:
	if (flash_command_8bit((base | (block_count*4)), 0x20) == -1)
		goto out;
	usleep (usSLEEP);

	if (flash_command_8bit((base | (block_count*4)), 0xD0) == -1)
		goto out;
	usleep (usSLEEP);

	if (flash_wait_sr_erase((base | (block_count*4)), 0x8080, 0x8080, 1000) == -1)
		goto out;
	if (block_count != 0xf0000) {   // not all sixteen block erased -> increment and goto top
		block_count = block_count + 0x10000;
		goto top;
	}
	ret = 0;
out:
	if (flash_command(base, 0xff) == -1)
		return -1;
	return ret;

}

static int flash_wait_sr_erase(uint32_t base, uint16_t mask, uint16_t result, int tries)
	{
	uint32_t buf;
	uint32_t _mask = (mask << 16) | mask;
	uint32_t _result = (result << 16) | result;
	int ret = -1;
	
	time_t start, now;
	time(&start);  // start of time
	
	while(tries-- && !abort_requested1)
	{
		if (read_memory(base, (uint8_t *)&buf, sizeof(buf)) == -1)
			goto out;
		time(&now); 																// actually time
		fprintf(stderr, "\rSR: 0x%08x %4ds", be32_to_cpu(buf), (int)(now - start)); //(actually time - start of time)
		if ((be32_to_cpu(buf) & _mask) == _result)
			break;
		usleep(200000);
	}
	printf ("\nerasing successful\n");
	
	if (!tries || abort_requested1)
		return -1;
	ret = 0;
out:
	return ret;
	}
