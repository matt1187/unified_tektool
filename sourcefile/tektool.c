/**************************************************************************
 *
 * This file is part of Buildbot.  Buildbot is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 **************************************************************************/

 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>


#include "tektool.h"
#include "flash.c"







int  Dev;



const char *ErrorMnemonic[] = {"EDVR", "ECIC", "ENOL", "EADR", "EARG",
			       "ESAC", "EABO", "ENEB", "EDMA", "",
			       "EOIP", "ECAP", "EFSO", "", "EBUS",
			       "ESTB", "ESRQ", "", "", "", "ETAB"};

static int abort_requested = 0;
static int debug;

static void sigint_handler(int arg)
	{
	abort_requested = 1;
	}

static void GPIBCleanup(int Dev, char* ErrorMsg)
	{
	printf("Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
	       ErrorMsg, ibsta, iberr, ErrorMnemonic[iberr]);
	if (Dev != -1) {
		printf("Cleanup: Taking device offline\n");
		ibonl (Dev, 0);
	}

	}

static int write_command(char *cmd)
	{
	ibwrt (Dev, cmd, strlen(cmd));
	if (ibsta & ERR)
		return -1;
	return 0;
	}

static int query(char *query, char *buf, int maxsize)
	{
	ibwrt (Dev, query, strlen(query));
	if (ibsta & ERR)
		return -1;

	ibrd (Dev, buf, maxsize);
	if (ibsta & ERR)
		return -1;

	buf[ibcntl - 1] = '\0';
	return ibcntl;
	}

struct cmd_hdr
	{
	uint8_t cmd;
	uint8_t csum;
	uint16_t len;
	};

struct memory_read_cmd
	{
	struct cmd_hdr hdr;
	uint32_t addr;
	uint32_t len;
	};

struct memory_write_cmd
	{
	struct cmd_hdr hdr;
	uint32_t addr;
	uint32_t len;
	uint8_t buf[1024];
	};

static void hexdump(void *_buf, int len)
	{
	int i;
	uint8_t *buf = (uint8_t *)_buf;
	for(i = 0; i < len; i++)
		fprintf(stderr, "%02X ", buf[i]);
	fprintf(stderr, "\n");
	}

static void build_csum(struct cmd_hdr *hdr)
	{
	uint8_t csum = 0;
	uint32_t i;
	for(i = 0; i < be16_to_cpu(hdr->len) + sizeof(struct cmd_hdr); i++)
			csum += ((uint8_t *)hdr)[i];
	hdr->csum = csum;
	}

static int write_memory(uint32_t addr, uint8_t *buf, int len)
{
	struct memory_write_cmd cmd;
	struct cmd_hdr hdr;
	uint16_t responselen;
	char c;

	memset(&cmd, 0, sizeof(cmd));
	cmd.hdr.cmd = 'M';
	cmd.hdr.len = cpu_to_be16(len + 8);
	cmd.addr = cpu_to_be32(addr);
	cmd.len = cpu_to_be32(len);

	memcpy(cmd.buf, buf, len);

	build_csum((struct cmd_hdr *)&cmd);
	if (debug > 1)
		hexdump(&cmd, len + 12);

	ibwrt (Dev, &cmd, len + 12);
	if (ibsta & ERR) {
		fprintf(stderr, "%s: writing command failed\n", __FUNCTION__);
		return -1;
	}

	ibrd(Dev, &c, 1);
	if (ibcntl != 1 || c != '+')

	{
	fprintf(stderr, "%s: response reading failed\n", __FUNCTION__);
	return -1;
	}	

	ibrd(Dev, &hdr, sizeof(struct cmd_hdr));
	if (ibsta & ERR)
	{	
		fprintf(stderr, "%s: response reading failed\n", __FUNCTION__);
		return -1;
	}

	if (ibcntl < (signed)sizeof(hdr))
	{
		fprintf(stderr, "%s: short header\n", __FUNCTION__);
		return -1;
	}

	if (hdr.cmd != '=') {
		fprintf(stderr, "%s: invalid response: %c\n", __FUNCTION__, hdr.cmd);
		return -1;
	}
	c = '+';
	ibwrt(Dev, &c, 1);
	return 0;
}

static int read_memory(uint32_t addr, uint8_t *buf, int len)
{
	
	struct memory_read_cmd cmd;
	struct cmd_hdr hdr;
	int responselen;
	char c;

	memset(&cmd, 0, sizeof(cmd));
	cmd.hdr.cmd = 'm';
	cmd.hdr.len = cpu_to_be16(sizeof(cmd) - 4);
	cmd.addr = cpu_to_be32(addr);
	cmd.len = cpu_to_be32(len);

	build_csum((struct cmd_hdr *)&cmd);
	if (debug > 1)
		hexdump(&cmd, sizeof(cmd));

	ibwrt (Dev, &cmd, sizeof(struct memory_read_cmd));
	if (ibsta & ERR)
	{
		fprintf(stderr, "%s: writing command failed\n", __FUNCTION__);
		return -1;
	}

	ibrd(Dev, &c, 1);
	if (ibcntl != 1 || c != '+')
	{
		fprintf(stderr, "%s: response reading failed\n", __FUNCTION__);
		return -1;
	}
	
	ibrd(Dev, &hdr, sizeof(struct cmd_hdr));
	if (ibsta & ERR)
	{
		fprintf(stderr, "%s: response reading failed\n", __FUNCTION__);
		return -1;
	}

	if (ibcntl < (signed)sizeof(hdr))
	{
		fprintf(stderr, "%s: short header\n", __FUNCTION__);
		return -1;
	}

	if (hdr.cmd != '=')
	{
		fprintf(stderr, "%s: invalid response: %c\n", __FUNCTION__, hdr.cmd);
		return -1;
	}

	responselen = be16_to_cpu(hdr.len);

	if (responselen != len)
	{
		fprintf(stderr, "%s: short response\n", __FUNCTION__);
		return -1;
	}

	ibrd(Dev, buf, responselen);
	if (ibsta & ERR || ibcntl < len)
	{
		fprintf(stderr, "%s: response reading failed\n", __FUNCTION__);
		return -1;
	}

	c = '+';
	ibwrt(Dev, &c, 1);
	if (ibsta & ERR)
	{
		fprintf(stderr, "%s: unable to send ACK\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

static struct option long_options[] =
{
	{ "read", required_argument, 0, 'r' },
	{ "write", required_argument, 0, 'w' },
	{ "base", required_argument, 0, 'b' },
	{ "length", required_argument, 0, 'l' },
	{ "debug", no_argument, 0, 'd' },
	{ "flash-id", no_argument, 0, 'i' },	
	{ "flash-verify", no_argument, 0, 'v' },
	{ "flash-zero-filling", no_argument, 0, 'z' },
	{ "flash-erase", no_argument, 0, 'e' },
	{ "flash-program", required_argument, 0, 'p' },
	{ "help", no_argument, 0, 'h' },
	{ NULL, 0, 0, 0 }
};

static char* ident = PROG_NAME "   Version: " GIT_VERSION "   Build time: " BUILD_TIME;
static void print_version(void)
{
	fprintf(stderr, "# %s\n", ident);
}


static void usage(void)
{	print_version(); /* XXX should have it's own flag */
	fprintf(stderr, "usage:\n"
		"--read         -r <filename>  read from memory to file\n"
		"--write        -w <filename>  read from file to memory\n"
		"--verify       -v <filename>  verify file and memory\n"
		"--zero         -z             flash zero filling\n"
		"--base         -b <base>      base address for read/write/program\n"
		"--length       -l <length>    length of data to be read or written\n"
		"--debug        -d             enable debug logging\n"
		"--flash-id     -i             print ID of flash chips\n"
		"--flash-erase  -e             erase flash at base address\n"
		"--flash-reset  -q             reset flash at base address\n"
		"--flsh-program -p             program flash at base address\n"
		"");
}

static uint32_t to_number(char *s)
{
	uint32_t val;
	char *endp;

	if (*s == '0' && *(s+1) == 'x')
	{
		val = strtoul(s+2, &endp, 16);
		if (*endp != '\0') {
			fprintf(stderr, "failed to parse: [%s]\n", s);
			return 0;
	}
	}	

	 else
	{
		val = strtoul(s, &endp, 10);
		if (*endp != '\0') {
			fprintf(stderr, "failed to parse: [%s]\n", s);
			return 0;
	}
	}
	return val;
}

static int flash_command(uint32_t base, uint8_t cmd)
	{
	uint8_t buf[4];

	base &= ~0x1ffff;
	buf[0] = cmd;
	buf[1] = cmd;
	buf[2] = cmd;
	buf[3] = cmd;


	if (write_memory(base, buf, sizeof(buf)) == -1) {
		fprintf(stderr, "flash command failed\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	
	uint32_t len, addr, base = 0, length = 0;
	char c;
	uint8_t buf[1024];
	int val, optidx;
	FILE *file = NULL;
	FILE *temp_file = NULL;  //added for verify
	int read_op = 0, write_op = 0, identify_flash_op = 0, erase_flash_op = 0, flash_write_op = 0, verify_op =0, zero_fill_op =0; //verify_op &zero_fill_op added
	int readlen, i;
	time_t start, now;

	while((c = getopt_long(argc, argv, "r:w:b:l:p:v:hiedz",  //verify  "r" added
			       long_options, &optidx)) != -1)
	 {
		switch(c)
		{
		case 'h':
			usage();
			return 0;
		case 'l':
			if (length)
		{
				fprintf(stderr, "length given twice");
				return 1;
		}
			length = to_number(optarg);
			break;
		case 'b':
			if (base)
				{
				fprintf(stderr, "base given twice");
				return 1;
			}
			base = to_number(optarg);
			break;
		case 'r':
			if (file) {
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "wb");
			if (!file) {
				fprintf(stderr, "failed to open output file: %s\n", strerror(errno));
				return 1;

			}
			read_op = 1;
			break;
			
// added  for verify
		case 'v':
			if (file) {
				fprintf(stderr, "verify given twice");
				return 1;
			}
			file = fopen(optarg, "rb");
			temp_file = fopen ("temp", "wb");
			if (!file) {
				fprintf(stderr, "failed to open output file: %s\n", strerror(errno));
				return 1;

			}
			verify_op = 1; //this string must also add to program function
			break;
// end of added verify
			
		case 'w':
			if (file)
			{
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "rb");
			if (!file)
			{
				fprintf(stderr, "failed to open input file: %s\n", strerror(errno));
				return 1;

			}

			write_op = 1;
			break;
		case 'p':
			if (file)
			{
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "rb");
			if (!file)
			{
				fprintf(stderr, "failed to open input file: %s\n", strerror(errno));
				return 1;

			}
			flash_write_op = 1;
			break;

		case 'i':
			identify_flash_op = 1;
			break;
		case 'e':
			erase_flash_op = 1;
			break;
//added zerofilling
		case 'z':
			zero_fill_op = 1;
			break;

// end of zerofilling			
			
		case 'd':
			debug++;
			break;
		}
	}

	if (!length)
	{
		fprintf(stderr, "%s: length required\n", __FUNCTION__);
		return 1;
	}

	signal(SIGINT, sigint_handler);

/*device identification - (gpib card, device num, second num, Time out, End of message, disable eos) */

	Dev = ibdev(0, 29, 0, T1s, 1, 0);
	if (ibsta & ERR)
	{
		printf("Unable to open device\nibsta = 0x%x iberr = %d",
		       ibsta, iberr);
		return 1;
	}

	ibclr (Dev);
	if (ibsta & ERR)
	{
		GPIBCleanup(Dev, "Unable to clear device");
		return 1;
	}
// since here is  commando-code

	if (identify_flash_op)
	{
		if (flash_identify(base) == -1)
		{
			fprintf(stderr, "\nidentify failed");
			return -1;
		}
		return 0;
	}
	
//added zero_fill	
	if (zero_fill_op)
	{	printf("addr: 0x%08x\r", (base));
		if (zero_fill(base) == -1)
		{
			fprintf(stderr, "\nzero fill failed");
			return -1;
		}
		return 0;
	}	

// end of zero_fill

	if (erase_flash_op)
	{
		if (flash_erase(base) == -1)
		{
			fprintf(stderr, "\nflash erase failed");
			return -1;
		}
		return 0;
	}
	

	time(&start);
	for(addr = base; addr < base + length && !abort_requested;)
	{
		len = MIN(512, base + length - addr);
		if (read_op) {
			if (read_memory(addr, buf, len) == -1)
				return 1;

			if (fwrite(buf, 1, len, file) != len)
			{
				fprintf(stderr, "\nshort write");
				return 1;
			}
			addr += len;
			if ((addr % 0x100) == 0)
			{
				time(&now);
				fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr - base, length, ((addr - base) * 100) / length, (int)(now - start));
			}
		}
	
//added for verify
		else if (verify_op) {
			if (read_memory(addr, buf, len) == -1)
				return 1;
			
			if (fwrite(buf, 1, len, temp_file) != len)
			{
				fprintf(stderr, "\nshort write");
				return 1;
			}
			addr += len;
			if ((addr % 0x100) == 0)
			{
				time(&now);
				fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr - base, length, ((addr - base) * 100) / length, (int)(now - start));
			}			
		}
// end of added verify

	 else if (write_op)
		{
		readlen = fread(buf, 1, len, file);
		if (readlen == 0)
			break;
		if (readlen == -1) {
			fprintf(stderr, "\nread: %s", strerror(errno));
			return 1;
		}
		if (write_memory(addr, buf, readlen) == -1)
			return 1;
		addr += readlen;
		if ((addr % 0x100) == 0)
			time(&now);
			fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr - base, length, ((addr - base) * 100) / length, (int)(now - start));
	}
	 else if (flash_write_op)
		{
		
		len = MIN(512, base + length -addr);
		readlen = fread(buf, 1, len, file);
		if (readlen == 0){
			printf("\rbreak , program stopped");

			break;}
		if (readlen == -1) {
			fprintf(stderr, "\nread: %s", strerror(errno));
			return 1;
		}

//		if (readlen < 512)
//		{
			for(i = 0; i < readlen; i += 4)
				{
				if (flash_program(addr + i, ((uint32_t *)buf)[i / 4]) == -1) {
					fprintf(stderr, "\nflash programming 4 bytes failed");
					return 1;
				}


			}
//		}
//		 else
/*		{
		if (flash_program_page(addr, buf) == -1)
			{
			fprintf(stderr, "flash programming of 512 bytes failed\n");
			return 1;
			}
		}
*/		addr += readlen;
		if ((addr % 0x100) == 0)
			{
			time(&now);
			fprintf(stderr, "\r%06x/%06x, %3d%% %4ds", addr - base, length, ((addr - base) * 100) / length, (int)(now - start));
			}
//added for faster program algorithm
		if (addr - base == length){
			flash_command(base, 0xff);
			printf("\nFirmware programmed.");
		}
//end of added for faster program algorithm


			
	}
	 else	{
		fprintf(stderr, "\neither read or write required");
		return 1;
	}
}	
	
//added verify
	if (verify_op) {
		int diff;
		int line, col;
		fclose(temp_file);
		temp_file = fopen ("temp", "rb");
		diff = compareFile(temp_file, file, &line);
		if (diff == 0){
			printf("\nFirmware verify: no mismatch.");
		}
		else if (line  == (length +1))
			printf("\nFirmware verify: no mismatch up to end of file.");
		
		else{
        printf("\nFirmware corrupt.");
        printf("\nAddress: 0x%06x", line );
		}
		fclose(temp_file); 
	}
// end of added verify
	
	fclose(file);
	ibonl(Dev, 0);
	return 0;
}


// added verify 


int compareFile(FILE * fPtr1, FILE * fPtr2, uint32_t * line)
{
    uint32_t ch1, ch2;

    *line = 1;
    do   {
		ch1 = fgetc(fPtr1);
		ch2 = fgetc(fPtr2);
		// If characters are not same then return -1
		if (ch1 != ch2)
			return -1;
		// Increment line 
		*line += 1;
    } while (ch1 != EOF && ch2 != EOF);


    /* If both files have reached end */
    if (ch1 == EOF && ch2 == EOF)
        return 0;
    else
        return -1;
}
