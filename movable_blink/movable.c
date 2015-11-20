
/* GPIO pin definitions */
#define GPIO_C_BASE  0x400DB000  // GPIO C base address
#define GPIO_D_BASE  0x400DC000  // GPIO D base address
#define GPIO_DIR     0x00000400  // Direction offset
#define GPIO_DATA    0x00000000  // Data offset

/* Pin definitions for various cc2538 systems */
#define ATUM_LEDS_BASE GPIO_D_BASE
#define ATUM_RED_LED   3
#define ATUM_BLUE_LED  4
#define ATUM_GREEN_LED 5

#define SDL_LEDS_BASE  GPIO_C_BASE
#define SDL_RED_LED    1
#define SDL_GREEN_LED  0
#define SDL_BIG_LED    5

/* Function prototypes */
void reset_handler(void);
void main(void);

/* Pointers to stack and sections */
static unsigned int stack[512]; // Allocate stack space
extern unsigned long _text;  // Linker construct indicating .text section location
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

/* Boot loader backdoor
 *
 * This is important for loading code through UART (via USB) instead of JTAG.
 * With a JTAG loader, this entire section is unncessary.
 */
#define FLASH_CCA_BOOTLDR_CFG_ENABLE 0xF0FFFFFF        // Enable backdoor function
#define FLASH_CCA_BOOTLDR_CFG_PORT_A_PIN_S 24          // Selected pin on pad A shift ??
#define FLASH_CCA_CONF_BOOTLDR_BACKDOOR_PORT_A_PIN 2   // Pin PA_2 activates the boot loader
#define FLASH_CCA_CONF_BOOTLDR_BACKDOOR_ACTIVE_HIGH 0  // A logic low level activates the boot loader
#define FLASH_CCA_BOOTLDR_CFG_ACTIVE_LEVEL 0
#define FLASH_CCA_IMAGE_VALID 0x00000000               // Indicates valid image in flash
#define FLASH_CCA_BOOTLDR_CFG ( FLASH_CCA_BOOTLDR_CFG_ENABLE \
        | FLASH_CCA_BOOTLDR_CFG_ACTIVE_LEVEL \
        | (FLASH_CCA_CONF_BOOTLDR_BACKDOOR_PORT_A_PIN << FLASH_CCA_BOOTLDR_CFG_PORT_A_PIN_S) )

typedef struct{
    unsigned int bootldr_cfg;
    unsigned int image_valid;
    const void *app_entry_point;
    unsigned char lock[32];
} flash_cca_lock_page_t;

__attribute__ ((section(".flashcca"), used))
const flash_cca_lock_page_t __cca = {
    FLASH_CCA_BOOTLDR_CFG,         // Boot loader backdoor configuration
    FLASH_CCA_IMAGE_VALID,         // Image valid
    &_text,                        // Vector table located at the start of .text
    {   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } // Unlock all pages and debug
};

/* Vector table */
__attribute__ ((section(".vectors"), used))
void(*const vectors[])(void) = {
    (void (*)(void))((unsigned int)stack + sizeof(stack)),   /* Stack pointer */
    reset_handler,    /* Reset handler */
    0,                /* The NMI handler */
    0,                /* The hard fault handler */
    0,                /* 4 The MPU fault handler */
    0,                /* 5 The bus fault handler */
    0,                /* 6 The usage fault handler */
    0,                /* 7 Reserved */
    0,                /* 8 Reserved */
    0,                /* 9 Reserved */
    0,                /* 10 Reserved */
    0,                /* 11 SVCall handler */
    0,                /* 12 Debug monitor handler */
};

/* Reset handler
 *
 * This handler is run on reset of the microcontroller, i.e. it is the code
 *  that runs when the microcontroller starts. It needs to copy over
 *  initialization data for global variables and then enter the user code
 */
void reset_handler(void) {
    
    // Copy the data segment intializers from Flash to SRAM
    unsigned long* data_src = &_etext;
    unsigned long* data_dst;
    for (data_dst = &_data; data_dst < &_edata;) {
        *data_dst++ = *data_src++;
    }

    // Zero-fill the bss segment
    __asm("    ldr     r0, =_bss         \n"
          "    ldr     r1, =_ebss        \n"
          "    mov     r2, #0            \n"
          "    .thumb_func               \n"
          "zero_loop:                    \n"
          "        cmp     r0, r1        \n"
          "        it      lt            \n"
          "        strlt   r2, [r0], #4  \n"
          "        blt     zero_loop     \n"
          );

    // Call user code
    main();

    // End here if code returns
    while (1);
}

typedef struct {
    unsigned int* entry_loc;        /* Entry point for user application */
    unsigned int* init_data_loc;    /* Data initialization information in flash */
    unsigned int init_data_size;    /* Size of initialization information */
    unsigned int got_start_offset;  /* Offset to start of GOT */
    unsigned int got_end_offset;    /* Offset to end of GOT */
    unsigned int plt_start_offset;  /* Offset to start of PLT */
    unsigned int plt_end_offset;    /* Offset to end of PLT */
    unsigned int bss_start_offset;  /* Offset to start of BSS */
    unsigned int bss_end_offset;    /* Offset to end of BSS */
} Load_Info;

extern unsigned char* _apps;
extern unsigned char* _eapps;

/* Main code
 *
 * This is the user's application
 */
void main(void) {
    // Select which LED to blink
    const unsigned int LED_BASE = ATUM_LEDS_BASE;

    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_RED_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_BLUE_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(LED_BASE | GPIO_DIR)) |= (1 << ATUM_GREEN_LED); // Sets the LED pin to be an output
    *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_GREEN_LED) << 2))) = 0x00; // LED on


    /* Perform application load functions */
    Load_Info* code_info = (Load_Info*)&_apps;
    unsigned int* flash_location = (unsigned int*)&_apps;
    unsigned int* sram_location = (unsigned int*)0x20001000; // arbitrary choice for testing
    // copy data into data section
    //  Data start location assumes .text starts at address 0, and needs to be
    //  updated to the actual location in flash
    unsigned int* init_data_src = (unsigned int*)((unsigned int)(*code_info).init_data_loc + (unsigned int)flash_location);
    unsigned int* init_data_end = (unsigned int*)((unsigned int)init_data_src + (*code_info).init_data_size);
    unsigned int* data_dst = sram_location;
    while (init_data_src < init_data_end) {
        *data_dst++ = *init_data_src++;
    }
    // zero out bss section
    unsigned int* bss_start = (unsigned int*)((unsigned int)sram_location + (*code_info).bss_start_offset);
    unsigned int bss_size = (*code_info).bss_end_offset - (*code_info).bss_start_offset;
    unsigned int* bss_end   = (unsigned int*)((unsigned int)bss_start + bss_size);
    while (bss_start < bss_end) {
        *bss_start++ = 0;
    }
    // fixup GOT
    unsigned int* got_start = (unsigned int*)((unsigned int)sram_location + (*code_info).got_start_offset);
    unsigned int got_size = (*code_info).got_end_offset - (*code_info).got_start_offset;
    unsigned int* got_end   = (unsigned int*)((unsigned int)got_start + got_size);
    while (got_start < got_end) {
        *got_start++ += (unsigned int)sram_location;
    }
    //TODO: fixup PLT to enable shared libraries


    /* Context Switch */
    // set base register (r6)
    //  Needs to point to the beginning of the GOT section, which is at the
    //  start of the SRAM for the data section.
    //  Hard coded to sram_location for now
    __asm(" ldr     r6,=0x20001000      \n"
          );
    //TODO: set stack pointer
    //TODO: move to user mode
    // Jump into application
    //  Entry location assumes .text starts at address 0, and needs to be
    //  updated to the actual location in flash
    unsigned int jmp = ((unsigned int)(*code_info).entry_loc + (unsigned int)flash_location);
    void (*fn)(void) = (void (*)(void))(((unsigned int)jmp) | 1); // |1 is very important!! Must stay in thumb mode
    fn();


    /* Never reached */
    *((volatile unsigned int*)(((LED_BASE) | GPIO_DATA) + ((1 << ATUM_RED_LED) << 2))) = 0x00; // LED on
}

