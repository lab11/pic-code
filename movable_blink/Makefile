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
CFLAGS += -fno-builtin -ffreestanding -nodefaultlibs -nostdlib
# this line seemed pretty unncessary
#CFLAGS += -fshort-enums -fomit-frame-pointer
CFLAGS += -Wall -O0
LDFLAGS += -nostartfiles
LDFLAGS += -T cc2538.ld
LDFLAGS += -Wl,-gc-sections,--sort-section=alignment
LDFLAGS += -Wl,-Map=objs/blink.map,--cref,--no-warn-mismatch
OBJCOPY_BIN += -O binary
OBJCOPY_HEX += -O ihex
OBJCOPY_FLAGS += --gap-fill 0xff
OBJDUMP_FLAGS += --disassemble-all --source --disassembler-options=force-thumb

all: blink.bin debug objs

blink.elf : blink.c objs
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o objs/$@

blink.bin : blink.elf objs
	$(OBJCOPY) $(OBJCOPY_BIN) $(OBJCOPY_FLAGS) objs/$< objs/$@

debug: blink.c objs
	$(CC) $(CFLAGS) -g -w $(LDFLAGS) $< -o objs/blink.debug.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) objs/blink.debug.elf > objs/blink.lst

install: all
ifeq ($(wildcard $(BSL)), )
	@echo "ERROR: Could not find the cc2538-bsl script. Did you run 'git submodule update --init' ?"
else
	python $(BSL) -b 115200 -e -w -v objs/blink.bin
endif

code: code.c objs
	$(CC) $(CFLAGS) -nostartfiles $< -o objs/code.elf
	$(OBJCOPY) $(OBJCOPY_BIN) $(OBJCOPY_FLAGS) objs/code.elf objs/code.bin
	$(OBJCOPY) $(OBJCOPY_HEX) $(OBJCOPY_FLAGS) objs/code.elf objs/code.hex
	$(CC) $(CFLAGS) -g -nostartfiles $< -o objs/code.debug.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) objs/code.debug.elf > objs/code.lst

objs : 
	mkdir objs

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.elf
	rm -f *.bin
	rm -f *.lst
	rm -f *.map
	rm -rf objs/
