
# -------
# Helpers
# -------

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ifeq ($(OS),Windows_NT)
	OS := Windows
else
	OS := $(shell uname)
endif

# ---------
# Variables
# ---------

PROJECTDIR := .
BUILDDIR := $(PROJECTDIR)/build
ELF := $(BUILDDIR)/tinymovr_fw.elf
HEX := $(BUILDDIR)/tinymovr_fw.hex
BIN := $(BUILDDIR)/tinymovr_fw.bin
OBJDIR = $(BUILDDIR)/obj
GIT_VERSION := $(shell git describe --abbrev=7 --dirty --always --tags)

# Toolchain
CC := arm-none-eabi-gcc
CPP := arm-none-eabi-g++
LD := arm-none-eabi-g++
OBJCOPY := arm-none-eabi-objcopy
SIZE_EX := arm-none-eabi-size
ifeq ($(OS),Windows)
	dir_guard=@if not exist $(@D) @mkdir "$(@D)"
	rmdir_cmd=@rmdir /S /Q
else
	dir_guard=@mkdir -p $(@D)
	rmdir_cmd=@rm -rf
endif

# Base flags for all actions
BASEFLAGS += -mcpu=cortex-m4
BASEFLAGS += -mthumb
BASEFLAGS += -mfloat-abi=hard
BASEFLAGS += -mfpu=fpv4-sp-d16
BASEFLAGS += -ffunction-sections # generate a separate ELF section for each function in the source file
BASEFLAGS += -fdata-sections # generate a separate ELF section for each data item in the source file
BASEFLAGS += -fno-tree-loop-distribute-patterns # disable replacing with calls to builting functions
BASEFLAGS += -Wall
BASEFLAGS += -Wlogical-op
BASEFLAGS += -Waggregate-return
BASEFLAGS += -Wshadow
BASEFLAGS += -Wdouble-promotion
BASEFLAGS += -DBOARD_REV_$(REV)
BASEFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
BASEFLAGS += $(INCLUDEFLAGS)

# Compiler flags (c and c++), inherit BASEFLAGS
COMPILEFLAGS += $(BASEFLAGS)
COMPILEFLAGS += -fcommon # legacy behavior for GCC 10
COMPILEFLAGS += -fstack-usage # emit stack usage info

# gcc flags, inherit COMPILEFLAGS
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu11
CFLAGS += $(COMPILEFLAGS)

# g++ flags, inherit COMPILEFLAGS
CPPFLAGS += -std=c++20
CPPFLAGS += $(COMPILEFLAGS)

 # Link time flags, inherit BASEFLAGS
LDFLAGS += $(BASEFLAGS)
LDFLAGS += -Tpac55xx.ld
LDFLAGS += -nostartfiles
LDFLAGS += -specs=nosys.specs
LDFLAGS += -Xlinker --gc-sections # optimize function and data sections produced during compile...
# LDFLAGS += -Xlinker --print-gc-sections # ... and print relevant info
LDFLAGS += -Xlinker -Map="$(BUILDDIR)/tinymovr.map"

# objcopy flags
HEXFLAGS += -O ihex
BINFLAGS += -O binary

# Headers
INCLUDEDIRS := . ./cmsis_core/ ./pac55xx_sdk/ ./pac55xx_sdk/device/ ./pac55xx_sdk/driver/ ./pac55xx_sdk/peripheral/
INCLUDEFLAGS := $(foreach d, $(INCLUDEDIRS), -I$d)

# Source Files
CSOURCES := $(call rwildcard,./src,*.c)
CSOURCES += $(call rwildcard,./pac55xx_sdk,*.c)
CPPSOURCES := $(call rwildcard,./src,*.cpp)
CPPSOURCES += $(call rwildcard,./pac55xx_sdk,*.cpp)

# List of object files, with paths
OBJECTS := $(addprefix $(BUILDDIR)/,$(CSOURCES:%.c=%.o)) $(addprefix $(BUILDDIR)/,$(CPPSOURCES:%.cpp=%.o))

# -------
# Targets
# -------

# All targets
all: release

# Debug target
debug: CFLAGS += -DDEBUG -g2 -O1
debug: CPPFLAGS += -DDEBUG -g2 -O1
debug: LDFLAGS += -O1
debug: OBJECTS += $(BUILDDIR)/bootloader.o
debug: binary

# Upgrade target
upgrade: CFLAGS += -DNDEBUG -g0 -O3
upgrade: CPPFLAGS += -DNDEBUG -g0 -O3
upgrade: LDFLAGS += -O3
upgrade: binary

# Release target
release: OBJECTS += $(BUILDDIR)/bootloader.o
release: upgrade

# Binary target
binary: print_board_rev $(HEX) $(BIN) $(ELF)

# Objcopy to HEX
$(HEX): $(ELF)
	$(OBJCOPY) $(HEXFLAGS) $(ELF) $(HEX)
	$(SIZE_EX) $(HEX)

# Objcopy to BIN
$(BIN): $(ELF)
	$(OBJCOPY) $(BINFLAGS) $(ELF) $(BIN)

# Link
$(ELF): $(OBJECTS)
	- $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.bl_section,alloc,load,readonly,data,contents $(PROJECTDIR)/bootloader.bin $(BUILDDIR)/bootloader.o
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(ELF)
	$(SIZE_EX) --format=berkeley $(ELF)

# Compile C
$(BUILDDIR)/%.o: %.c
	$(dir_guard)
	$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

# Compile C++
$(BUILDDIR)/%.o: %.cpp
	$(dir_guard)
	$(CPP) $(CPPFLAGS) -I$(dir $<) -c $< -o $@

# Clean
.PHONY : clean
clean :
	- $(rmdir_cmd) "$(BUILDDIR)"

# Print board revision
.PHONY: print_board_rev
print_board_rev:
	@echo "===Building Tinymovr==="
	@echo "Board revision is $(REV)"
	@echo "======================="
