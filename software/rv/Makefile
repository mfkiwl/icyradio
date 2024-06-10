# App Config
APP_ADDRESS = 0x00000000
APP_NAME = icyradio-rv

# Multiprocessing
MAX_PARALLEL =

# Directories
TARGETDIR = bin
SOURCEDIR = src
OBJECTDIR = bin/obj
INCLUDEDIR = include

STRUCT := $(shell find $(SOURCEDIR) -type d)

SOURCEDIRSTRUCT := $(filter-out %/include, $(STRUCT))
INCLUDEDIRSTRUCT := $(filter %/include, $(STRUCT))
OBJECTDIRSTRUCT := $(subst $(SOURCEDIR), $(OBJECTDIR), $(SOURCEDIRSTRUCT))

# Build type
BUILD_TYPE ?= debug

# Version
$(shell if ! test -f $(TARGETDIR)/.version; then echo 0 > $(TARGETDIR)/.version; fi)

INC_VERSION ?= n
BUILD_VERSION = $(shell cat $(TARGETDIR)/.version)

ifeq ($(INC_VERSION), y)
$(shell if test -f .vscode/launch.json; then sed -i 's/v$(BUILD_VERSION).$(APP_NAME)/v$(shell echo $$(($(BUILD_VERSION) + 1))).$(APP_NAME)/g' .vscode/launch.json; fi)
$(shell echo $$(($(BUILD_VERSION) + 1)) > $(TARGETDIR)/.version)
$(shell rm -rf $(OBJECTDIR)/*)
BUILD_VERSION = $(shell cat $(TARGETDIR)/.version)
endif

ifeq ($(BUILD_VERSION), 0)
$(shell echo $$(($(BUILD_VERSION) + 1)) > $(TARGETDIR)/.version)
$(shell rm -rf $(OBJECTDIR)/*)
BUILD_VERSION = $(shell cat $(TARGETDIR)/.version)
endif

# Compillers & Linker
CC = riscv64-unknown-elf-gcc
CXX = riscv64-unknown-elf-g++
LD = riscv64-unknown-elf-gcc
AS = riscv64-unknown-elf-as
STRIP = riscv64-unknown-elf-strip
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump
GDB = riscv64-unknown-elf-gdb

# Compillers & Linker flags
ASFLAGS = $(addprefix -I,$(INCLUDEDIRSTRUCT)) -mabi=ilp32 -march=rv32imc
CFLAGS = $(addprefix -I,$(INCLUDEDIRSTRUCT)) -mabi=ilp32 -march=rv32imc -nostdlib -nostartfiles -ffunction-sections -fdata-sections -ffreestanding -Os -std=gnu99 -Wpointer-arith -Werror -DBUILD_VERSION=$(BUILD_VERSION)
CXXFLAGS = $(addprefix -I,$(INCLUDEDIRSTRUCT)) -mabi=ilp32 -march=rv32imc -nostdlib -nostartfiles -ffunction-sections -fdata-sections -ffreestanding -fno-rtti -fno-exceptions -Os -std=c++17 -Wpointer-arith -Werror -DBUILD_VERSION=$(BUILD_VERSION)
LDFLAGS = -mabi=ilp32 -march=rv32imc --specs=nano.specs --specs=nosys.specs -nostdlib -nostartfiles -ffunction-sections -fdata-sections -ffreestanding -Wl,--gc-sections
LDLIBS = -lm -lc -lgcc -lnosys

ifeq ($(BUILD_TYPE), debug)
CFLAGS += -g
CXXFLAGS += -g
endif

## Linker scripts
LDSCRIPT = ld/picorv32_app.ld

# Target
TARGET = $(TARGETDIR)/v$(BUILD_VERSION).$(APP_NAME)

# Sources & objects
SRCFILES := $(addsuffix /*, $(SOURCEDIRSTRUCT))
SRCFILES := $(wildcard $(SRCFILES))

ASSOURCES := $(filter %.s, $(SRCFILES))
ASOBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(ASSOURCES:%.s=%.o))

CASSOURCES := $(filter %.S, $(SRCFILES))
CASOBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(CASSOURCES:%.S=%.o))

CSOURCES := $(filter %.c, $(SRCFILES))
COBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(CSOURCES:%.c=%.o))

CXXSOURCES := $(filter %.cpp, $(SRCFILES))
CXXOBJECTS := $(subst $(SOURCEDIR), $(OBJECTDIR), $(CXXSOURCES:%.cpp=%.o))

SOURCES = $(ASSOURCES) $(CASSOURCES) $(CSOURCES) $(CXXSOURCES)
OBJECTS = $(ASOBJECTS) $(CASOBJECTS) $(COBJECTS) $(CXXOBJECTS)

all: clean-bin make-dir version compile mem-usage

compile:
	@$(MAKE) INC_VERSION=n --no-print-directory -j${MAX_PARALLEL} $(TARGET).elf
	@$(MAKE) INC_VERSION=n --no-print-directory -j${MAX_PARALLEL} $(TARGET).bin $(TARGET).hex $(TARGET).lss
	@$(MAKE) INC_VERSION=n --no-print-directory -j${MAX_PARALLEL} $(TARGET).h

$(TARGET).lss: $(TARGET).elf
	@echo Creating LSS file \'$@\'...
	@$(OBJDUMP) -S --disassemble $< > $@

$(TARGET).bin: $(TARGET).elf
	@echo Creating BIN file \'$@\'...
	@$(OBJCOPY) -O binary --only-section=.irom0.text --only-section=.iram1.text --only-section=.dram1.data $< $(TARGET).flash.bin
	@$(OBJCOPY) -O binary --only-section=.iram0.text --only-section=.dram0.data $< $(TARGET).bram.bin
	@$(OBJCOPY) -O binary --only-section=.drom0.data $< $(TARGET).brom.bin

$(TARGET).hex: $(TARGET).elf
	@echo Creating HEX file \'$@\'...
	@$(OBJCOPY) -O ihex $< $@
	@$(OBJCOPY) -O ihex --only-section=.irom0.text --only-section=.iram1.text --only-section=.dram1.data $< $(TARGET).flash.hex
	@$(OBJCOPY) -O ihex --only-section=.iram0.text --only-section=.dram0.data $< $(TARGET).bram.hex
	@$(OBJCOPY) -O ihex --only-section=.drom0.data $< $(TARGET).brom.hex

$(TARGET).h: $(TARGET).bin
	@echo Generating C header file \'$@\'...
	@xxd -u -i $(TARGET).flash.bin $(TARGET).flash.h
	@sed -i 's/0X/0x/g' $(TARGET).flash.h
	@sed -i 's/\s\s/    /g' $(TARGET).flash.h
	@sed -i 's/unsigned char $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint8_t /g' $(TARGET).flash.h
	@sed -i 's/unsigned int $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint32_t /g' $(TARGET).flash.h
	@xxd -u -i $(TARGET).bram.bin $(TARGET).bram.h
	@sed -i 's/0X/0x/g' $(TARGET).bram.h
	@sed -i 's/\s\s/    /g' $(TARGET).bram.h
	@sed -i 's/unsigned char $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint8_t /g' $(TARGET).bram.h
	@sed -i 's/unsigned int $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint32_t /g' $(TARGET).bram.h
	@xxd -u -i $(TARGET).brom.bin $(TARGET).brom.h
	@sed -i 's/0X/0x/g' $(TARGET).brom.h
	@sed -i 's/\s\s/    /g' $(TARGET).brom.h
	@sed -i 's/unsigned char $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint8_t /g' $(TARGET).brom.h
	@sed -i 's/unsigned int $(TARGETDIR)_v$(BUILD_VERSION)_/static const uint32_t /g' $(TARGET).brom.h
	@cat $(TARGET).flash.h $(TARGET).bram.h $(TARGET).brom.h > $(TARGET).h

$(TARGET).elf: $(OBJECTS)
	@echo ---------------------------------------------------------------------------
	@echo Creating ELF file \'$@\'...
	@$(LD) $(LDFLAGS) -o $@ $^ -T $(LDSCRIPT) $(LDLIBSPATH) $(LDLIBS) -Wl,-Map=$(TARGET).map
ifeq ($(BUILD_TYPE), release)
	@$(STRIP) -g $@
endif

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.s
	@echo Compilling ASM file \'$<\' \> \'$@\'...
	@$(AS) $(ASFLAGS) -MD -c -o $@ $<

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.S
	@echo Compilling ASM file \'$<\' \> \'$@\'...
	@$(CC) $(ASFLAGS) -MD -c -o $@ $<

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@echo Compilling C file \'$<\' \> \'$@\'...
	@$(CC) $(CFLAGS) -MD -c -o $@ $<

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	@echo Compilling C++ file \'$<\' \> \'$@\'...
	@$(CXX) $(CXXFLAGS) -MD -c -o $@ $<

debug: $(TARGET).elf
	$(GDB) $(TARGET).elf

inc-version:
	@echo $$(($(BUILD_VERSION) + 1)) > $(TARGETDIR)/.version
	@if test -f .vscode/launch.json; then sed -i 's/v$(BUILD_VERSION).$(APP_NAME)/v$(shell echo $$(($(BUILD_VERSION) + 1))).$(APP_NAME)/g' .vscode/launch.json; fi

dec-version:
	@echo $$(($(BUILD_VERSION) - 1)) > $(TARGETDIR)/.version
	@if test -f .vscode/launch.json; then sed -i 's/v$(BUILD_VERSION).$(APP_NAME)/v$(shell echo $$(($(BUILD_VERSION) - 1))).$(APP_NAME)/g' .vscode/launch.json; fi

version:
	@echo Build version: v$(BUILD_VERSION)
	@echo ---------------------------------------------------------------------------

mem-usage: $(TARGET).elf
	@echo ---------------------------------------------------------------------------
	@armmem -l $(LDSCRIPT) -d -h $<

make-dir:
	@mkdir -p $(OBJECTDIRSTRUCT)

clean-bin:
	@rm -f $(TARGETDIR)/*.h
	@rm -f $(TARGETDIR)/*.lss
	@rm -f $(TARGETDIR)/*.hex
	@rm -f $(TARGETDIR)/*.bin
	@rm -f $(TARGETDIR)/*.map
	@rm -f $(TARGETDIR)/*.elf

clean: clean-bin
	@rm -rf $(OBJECTDIR)/*

-include $(OBJECTS:.o=.d)

.PHONY: clean clean-bin make-dir mem-usage version dec-version inc-version debug compile all
