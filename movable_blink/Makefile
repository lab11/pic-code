CC      = arm-none-eabi-gcc
LD      = arm-none-eabi-ld
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
CFLAGS += -Wall -O3
CFLAGS += -std=c99
LDFLAGS += -nostartfiles
LDFLAGS += -T cc2538.ld
LDFLAGS += -Wl,-gc-sections,--sort-section=alignment
LDFLAGS += -Wl,-Map=objs/movable.map,--cref
# this line seems pretty dangerous
#LDFLAGS += -Wl,--no-warn-mismatch
OBJCOPY_BIN += -O binary
OBJCOPY_HEX += -O ihex
OBJCOPY_FLAGS += --gap-fill 0xff
OBJDUMP_FLAGS += --disassemble-all --source --disassembler-options=force-thumb
OBJDUMP_FLAGS += -C -g --section-headers --disassemble-zeroes
#PIC += -fPIC -msingle-pic-base -mpic-register=r6 -mno-pic-data-is-text-relative
PIC += -fPIE -msingle-pic-base -mpic-register=r6 -mno-pic-data-is-text-relative

PIC_LDFLAGS += -nostartfiles
PIC_LDFLAGS += -pie
PIC_LDFLAGS += -T user_app.ld
PIC_LDFLAGS += -Wl,-gc-sections,--sort-section=alignment


all: code movable.bin debug objs

movable.elf : movable.c objs code
	$(CC) $(CFLAGS) $(LDFLAGS) $< objs/code.bin.o -o objs/$@

movable.bin : movable.elf objs
	$(OBJCOPY) $(OBJCOPY_BIN) $(OBJCOPY_FLAGS) objs/$< objs/$@

debug: movable.c objs
	$(CC) $(CFLAGS) $(LDFLAGS) -g -w $< objs/code.bin.o -o objs/movable.debug.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) objs/movable.debug.elf > objs/movable.lst

install: all
ifeq ($(wildcard $(BSL)), )
	@echo "ERROR: Could not find the cc2538-bsl script. Did you run 'git submodule update --init' ?"
else
	python $(BSL) -b 115200 -e -w -v objs/movable.bin
endif

code: code.c led.c objs
	$(CC) $(CFLAGS) $(PIC) -S code.c -o objs/code.compiled
	$(CC) $(CFLAGS) $(PIC) -S led.c -o objs/led.compiled
	$(CC) $(CFLAGS) $(PIC) -c code.c -o objs/code.o
	$(CC) $(CFLAGS) $(PIC) -c led.c -o objs/led.o
	$(CC) $(CFLAGS) $(PIC) $(PIC_LDFLAGS) objs/led.o objs/code.o -o objs/code.elf
	$(OBJCOPY) $(OBJCOPY_BIN) $(OBJCOPY_FLAGS) objs/code.elf objs/code.bin
	$(OBJCOPY) $(OBJCOPY_HEX) $(OBJCOPY_FLAGS) objs/code.elf objs/code.hex
	$(CC) $(CFLAGS) $(PIC) -g $(PIC_LDFLAGS) -Wl,-Map=objs/code.map code.c led.c -o objs/code.debug.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) objs/code.debug.elf > objs/code.lst
	$(LD) -r -b binary -o objs/code.bin.o objs/code.bin
	$(OBJCOPY) --rename-section .data=.app objs/code.bin.o
	$(OBJDUMP) $(OBJDUMP_FLAGS) objs/code.bin.o > objs/code.bin.lst

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
