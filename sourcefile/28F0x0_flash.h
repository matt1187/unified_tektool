

#define _28F010_size  0x80000
#define _28F020_size  0x100000

static int flash_erase_28F010(uint32_t base);
static int flash_erase_28F020(uint32_t base);

static int flash_program_28F0x0(uint32_t base, uint32_t data, uint32_t flash_size);
static int flash_program_verify_28F0x0(uint32_t base, uint32_t data, uint32_t flash_size);
static int flash_00_program(uint32_t base,uint32_t size, int tries);
static int flash_erase_28F0x0(uint32_t base, uint32_t flash_size, int tries);
//static int flash_FF_verify_fast(uint32_t base, uint32_t size, uint32_t try, uint32_t *fail_addr);

static int flash_FF_verify_fast(uint32_t base, uint32_t size, uint32_t try, uint32_t *fail_addr, uint32_t *safe_erase);

