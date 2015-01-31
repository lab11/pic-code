CC      = arm-none-eabi-gcc
LD      = arm-none-eabi-gcc
AR      = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
NM      = arm-none-eabi-nm

LDSCRIPT = cc2538.ld
BSL = ../cc2538-bsl/cc2538-bsl.py

CFLAGS += -mcpu=cortex-m3 -mthumb -mlittle-endian
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-strict-aliasing
# this line seemed pretty unncessary
#CFLAGS += -fshort-enums -fomit-frame-pointer
CFLAGS += -Wall -O0
LDFLAGS += -nostartfiles
LDFLAGS += -T cc2538.ld
LDFLAGS += -Wl,-gc-sections,--sort-section=alignment
LDFLAGS += -Wl,-Map=blink.map,--cref,--no-warn-mismatch
OBJCOPY_FLAGS += -O binary --gap-fill 0xff
OBJDUMP_FLAGS += --disassemble --source --disassembler-options=force-thumb


all: blink.bin debug

blink.elf : blink.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

blink.bin : blink.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

debug: blink.c
	$(CC) $(CFLAGS) -g $(LDFLAGS) $< -o blink.debug.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) blink.debug.elf > blink.lst

install: all
ifeq ($(wildcard $(BSL)), )
	@echo "ERROR: Could not find the cc2538-bsl script. Did you run 'git submodule update --init' ?"
else
	python $(BSL) -b 115200 -e -w -v blink.bin
endif

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.elf
	rm -f *.bin
	rm -f *.lst
