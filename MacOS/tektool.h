/*#define BIG_ENDIAN			4321
#define LITTLE_ENDIAN		1234
#define BYTE_ORDER			LITTLE_ENDIAN
 */


#if !defined (PROG_NAME)
#define PROG_NAME __FILE__
#endif
#if !defined (GIT_VERSION)
#define GIT_VERSION "unknown"
#endif
#if !defined (BUILD_TIME)
#define BUILD_TIME __DATE__ " " __TIME__
#endif

/* change to using htonl, htons, ntohl, ntohs instead? */
#if defined(LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN__) || (BYTE_ORDER ==	LITTLE_ENDIAN)
#define cpu_to_be16(_x) ((((_x) & 0xff) << 8) | (((_x) >> 8) & 0xff))
#define be16_to_cpu cpu_to_be16
#define cpu_to_be32(_x) (cpu_to_be16(((_x) >> 16) & 0xffff) | \
			 (cpu_to_be16(((_x) & 0xffff)) << 16))
#define be32_to_cpu cpu_to_be32
#elif defined(BIG_ENDIAN) || defined(__BIG_ENDIAN) || defined(__BIG_ENDIAN__)
#define cpu_to_be16
#define cpu_to_be32
#define be16_to_cpu
#define be32_to_cpu
#else
#error Unknown endianness !
#endif

#define MIN(_a, _b) ((_a) > (_b) ? (_b) : (_a))

#if defined(__MSDOS__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <malloc.h> /* can this be removed? */
#include <windows.h>
#include "ni488.h"
#elif defined(__linux__)
/* linux with linux-gpib */
//  #include <unistd.h>
#include <gpib/ib.h>
#elif defined(__APPLE__)
/* MacOS with NI GPIB drivers */
//  #include <unistd.h>
#include <ni4882.h>
#else
#error "Unknown compiler environment!"
#endif


static void sigint_handler(int arg);
static void GPIBCleanup(int Dev, char* ErrorMsg);
static int write_command(char *cmd);
static int query(char *query, char *buf, int maxsize);
struct cmd_hdr;
struct memory_read_cmd;
struct memory_write_cmd;
static void hexdump(void *_buf, int len);
static void build_csum(struct cmd_hdr *hdr);
static int write_memory(uint32_t addr, uint8_t *buf, int len);
static int read_memory(uint32_t addr, uint8_t *buf, int len);
static void usage(void);
static uint32_t to_number(char *s);
static int flash_command(uint32_t base, uint8_t cmd);
static int flash_wait_sr(uint32_t base, uint16_t mask, uint16_t result, int tries);
static int flash_wait_gsr(uint32_t base, uint16_t mask, uint16_t result, int tries);
static int flash_wait_bsr(uint32_t base, uint16_t mask, uint16_t result, int tries);

static int flash_load_to_pagebuffer(uint32_t base, uint8_t *data);
static int flash_write_pagebuffer(uint32_t base, uint32_t start);
static int flash_program_page(uint32_t base, uint8_t *data);
int main(int argc, char **argv);

int compareFile(FILE * fPtr1, FILE * fPtr2, uint32_t * line );






