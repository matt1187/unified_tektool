


static int flash_command_8bit(uint32_t base, uint8_t cmd);   // for only 8 bit flash. (base address masking smaller)
static int flash_identify(uint32_t base);
static int flash_identify(uint32_t base);
static int flash_erase (uint32_t base) ;
static int flash_erase_28F160S5(uint32_t base);
static int flash_erase_28F016SA(uint32_t base);
static int flash_erase_28F008SA(uint32_t base);
static int flash_wait_sr_erase(uint32_t base, uint16_t mask, uint16_t result, int tries);

static int flash_program(uint32_t base, uint32_t data);
static int flash_program_28F008SA(uint32_t base, uint32_t data);
static int flash_program_28F016SA(uint32_t base, uint32_t data);
static int flash_program_28F160S5(uint32_t base, uint32_t data);
static int flash_wait_sr_write(uint32_t base, uint16_t mask, uint16_t result, int tries);
