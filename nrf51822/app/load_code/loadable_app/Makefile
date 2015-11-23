##################################################
# app/*/Makefile
##################################################

PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
APPLICATION_SRCS += led.c

DEVICE = NRF51
SOFTDEVICE_MODEL = s110

LIBRARY_PATHS += ../../../platform
SOURCE_PATHS += ../../../platform

NRF_BASE_PATH ?= ../../../nrf5x-base
#include $(NRF_BASE_PATH)/make/Makefile


##################################################
# movable_blink/Makefile
##################################################

CFLAGS += -mcpu=cortex-m0 -mthumb -march=armv6-m -mlittle-endian
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-builtin -ffreestanding -nodefaultlibs -nostdlib
# this line seemed pretty unncessary
#CFLAGS += -fshort-enums -fomit-frame-pointer
CFLAGS += -Wall -O0
CFLAGS += -std=c99
#LDFLAGS += -nostartfiles
#LDFLAGS += -T cc2538.ld
#LDFLAGS += -Wl,-gc-sections,--sort-section=alignment
#LDFLAGS += -Wl,-Map=objs/movable.map,--cref
# this line seems pretty dangerous
#LDFLAGS += -Wl,--no-warn-mismatch
OBJCOPY_BIN += -O binary
OBJCOPY_HEX += -O ihex
OBJCOPY_FLAGS += --gap-fill 0xff
OBJDUMP_FLAGS += --disassemble-all --source --disassembler-options=force-thumb
OBJDUMP_FLAGS += -C -g --section-headers --disassemble-zeroes
PIC += -fPIC -msingle-pic-base -mpic-register=r6 -mno-pic-data-is-text-relative
#PIC += -fPIE -msingle-pic-base -mpic-register=r6 -mno-pic-data-is-text-relative
CFLAGS += $(PIC)

PIC_LDFLAGS += -nostartfiles
#PIC_LDFLAGS += -pie
PIC_LDFLAGS += -T user_app.ld
PIC_LDFLAGS += -Wl,-gc-sections,--sort-section=alignment
LDFLAGS += $(PIC_LDFLAGS)


##################################################
# nrf5x-base/make/Makefile
##################################################

# Helper for relative paths
#BASE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))..
BASE_DIR := ./../../../nrf5x-base/

# Setup the compilers to use
TOOLCHAIN_PREFIX ?= arm-none-eabi
AS      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-as
CC      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc
LD      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy
OBJDUMP = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objdump
SIZE    = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-size
GDB     = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gdb

# Guess nRF51 unless otherwise set
NRF_MODEL ?= nrf51
NRF_IC    ?= nrf51822

# Set default board
BOARD ?= BOARD_NULL

# Set hardware memory sizes
RAM_KB   ?= 16
FLASH_KB ?= 256

# Default to the newest SDK
SDK_VERSION ?= 10

# Configure which stacks we need for various softdevices
ifeq ($(SOFTDEVICE_MODEL), s110)
    USE_BLE = 1
    SOFTDEVICE_VERSION ?= 8.0.0
    NRF_MODEL = nrf51
endif

# Add useful paths from nRF5x-base
LIBRARY_PATHS += $(BASE_DIR)/advertisement/
LIBRARY_PATHS += $(BASE_DIR)/devices/
LIBRARY_PATHS += $(BASE_DIR)/lib/
LIBRARY_PATHS += $(BASE_DIR)/peripherals/
LIBRARY_PATHS += $(BASE_DIR)/services/

SOURCE_PATHS += $(BASE_DIR)/advertisement/
SOURCE_PATHS += $(BASE_DIR)/devices/
SOURCE_PATHS += $(BASE_DIR)/lib/
SOURCE_PATHS += $(BASE_DIR)/peripherals/
SOURCE_PATHS += $(BASE_DIR)/services/
SOURCE_PATHS += $(BASE_DIR)/startup/

# Add paths for each SDK version
ifeq ($(NRF_MODEL), nrf51)
ifeq ($(SDK_VERSION), 10)

	# Set the path
	SDK_PATH ?= $(BASE_DIR)/sdk/nrf51_sdk_10.0.0/

	# Other knowns about the SDK paths
	SDK_INCLUDE_PATH   = $(SDK_PATH)components/
	SDK_SOURCE_PATH    = $(SDK_PATH)components/
	CMSIS_INCLUDE_PATH = $(SDK_PATH)components/toolchain/gcc/

	# Need to add the paths for all the directories in the SDK.
	# Note that we do not use * because some folders have conflicting files.
	LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)libraries/*/)
	LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_nrf/*/)
	LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)drivers_ext/*/)
	LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)device/)
	LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)serialization/*/)
	LIBRARY_PATHS += $(SDK_INCLUDE_PATH)toolchain/gcc/
	LIBRARY_PATHS += $(SDK_INCLUDE_PATH)toolchain/

	SOURCE_PATHS += $(SDK_SOURCE_PATH)
	SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)*/)
	SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)libraries/*/)
	SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)drivers_nrf/*/)
	SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)drivers_ext/*/)
	SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)serialization/*/)

ifdef USE_BLE
    LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)ble/*/)
    LIBRARY_PATHS += $(wildcard $(SDK_INCLUDE_PATH)ble/ble_services/*/)
    SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)ble/*/)
    SOURCE_PATHS += $(wildcard $(SDK_SOURCE_PATH)ble/ble_services/*/)
    CFLAGS += -DBLE_STACK_SUPPORT_REQD
endif

endif # SDK 10
endif # nrf51

print-% : ; @echo $* = $($*)

LIBRARY_INCLUDES = $(addprefix -I,$(LIBRARY_PATHS))
CMSIS_INCLUDE = $(addprefix -I,$(CMSIS_INCLUDE_PATH))

VPATH = $(SOURCE_PATHS)
OBJDIR = _build/

OUTPUT_PATH = _build/

CFLAGS += -g -D$(DEVICE) -D$(BOARD) $(LIBRARY_INCLUDES) -MD
COMPILE_ONLY += -c

OUTPUT_NAME ?= $(addsuffix _$(SOFTDEVICE_MODEL), $(PROJECT_NAME))
HEX = $(OUTPUT_PATH)$(OUTPUT_NAME).hex
ELF = $(OUTPUT_PATH)$(OUTPUT_NAME).elf
BIN = $(OUTPUT_PATH)$(OUTPUT_NAME).bin
LST = $(OUTPUT_PATH)$(OUTPUT_NAME).lst

SRCS = $(SYSTEM_FILE) $(notdir $(APPLICATION_SRCS))
OBJS = $(addprefix $(OUTPUT_PATH), $(SRCS:.c=.o)) $(addprefix $(OUTPUT_PATH),$(APPLICATION_LIBS))
DEPS = $(addprefix $(OUTPUT_PATH), $(SRCS:.c=.d))
LSTS = $(addprefix $(OUTPUT_PATH), $(SRCS:.c=.lst))
#SRCS_AS = $(START_CODE)
#OBJS_AS = $(addprefix $(OUTPUT_PATH), $(SRCS_AS:.s=.os))

#all: $(OBJS) $(OBJS_AS) $(HEX)
all: $(OBJS) $(LSTS) $(HEX)

#$(HEX): $(OBJS) $(OBJS_AS)
#	$(LD) $(LDFLAGS) $(OBJS_AS) $(OBJS) -o $(ELF)
#	$(OBJCOPY) -Oihex $(ELF) $(HEX)
#	$(OBJCOPY) -Obinary $(ELF) $(BIN)
#	$(SIZE) $(ELF)
$(HEX): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(ELF)
	$(OBJCOPY) -Oihex $(ELF) $(HEX)
	$(OBJCOPY) -Obinary $(ELF) $(BIN)
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(ELF) > $(LST)
	arm-none-eabi-ld -r -b binary -o ../code_to_load.o $(BIN)
	$(OBJCOPY) --rename-section .data=.app ../code_to_load.o
	$(SIZE) $(ELF)

size: $(ELF)
	$(SIZE) $(ELF)

$(OUTPUT_PATH)%.o: %.c
	$(MAKE_BUILD_FOLDER)
	$(CC) $(LDFLAGS) $(CFLAGS) $(COMPILE_ONLY) $< -o $@

$(OUTPUT_PATH)%.os: %.s
	$(MAKE_BUILD_FOLDER)
	$(AS) $< -o $@

$(OUTPUT_PATH)%.lst: %.o
	$(MAKE_BUILD_FOLDER)
	$(OBJDUMP) $(OBJDUMP_FLAGS) $< > $@

.PHONY: all size clean


##################################################
# nrf5x-base/make/Makefile.posix
##################################################

MAKE_BUILD_FOLDER = mkdir -p $(OUTPUT_PATH)

clean:
	rm -rf $(OUTPUT_PATH)
	rm -f *.jlink
	rm -f JLink.log
	rm -f .gdbinit
	rm -f *.d








