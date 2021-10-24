SOURCEDIR := .
BUILDDIR := $(SOURCEDIR)/build
ELF := $(BUILDDIR)/tinymovr_fw.elf
HEX := $(BUILDDIR)/tinymovr_fw.hex
OBJDIR = $(BUILDDIR)/obj
ALLDIRS = $(shell find $(SOURCEDIR) -maxdepth 3 -type d \
			-not -path "./.git/*" \
			-not -path "./build/*" \
			-not -path "./.vscode/*" \
			-not -path "./.settings/*")

# Toolchain
CC := arm-none-eabi-gcc
CPP := arm-none-eabi-g++
LD := arm-none-eabi-g++
OBJCOPY := arm-none-eabi-objcopy

# Base flags for all actions
BASEFLAGS += -mcpu=cortex-m4
BASEFLAGS += -mthumb
BASEFLAGS += -mfloat-abi=hard
BASEFLAGS += -mfpu=fpv4-sp-d16
BASEFLAGS += -ffunction-sections # generate a separate ELF section for each function in the source file
BASEFLAGS += -fdata-sections # generate a separate ELF section for each data item in the source file
BASEFLAGS += -Wall -Werror
BASEFLAGS += -Wlogical-op
BASEFLAGS += -Waggregate-return
BASEFLAGS += -Wshadow
BASEFLAGS += -Wdouble-promotion

# Compiler flags (c and c++)
COMPILEFLAGS += $(BASEFLAGS)
COMPILEFLAGS += $(foreach d, $(ALLDIRS), -I$d)
COMPILEFLAGS += -fcommon # legacy behavior for GCC 10
COMpileFLAGS += -fstack-usage # emit stack usage info

# gcc flags
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu11
CFLAGS += $(COMPILEFLAGS)

# g++ flags
CPPFLAGS += -std=c++20
CPPFLAGS += $(COMPILEFLAGS)

 # Link time flags
LDFLAGS += $(BASEFLAGS)
LDFLAGS += -Tpac55xx.ld
LDFLAGS += -nostartfiles
LDFLAGS += -specs=nosys.specs
LDFLAGS += -Xlinker --gc-sections # optimize function and data sections produced during compile...
LDFLAGS += -Xlinker --print-gc-sections # ... and print relevant info
LDFLAGS += -Xlinker -Map="$(BUILDDIR)/tinymovr.map"

# objcopy flags
BFLAGS += -O ihex

# ---

# Include all sources
CSOURCES := $(shell find $(SOURCEDIR) -name '*.c')
CPPSOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')

# Get list of object files, with paths
OBJECTS := $(addprefix $(BUILDDIR)/,$(CSOURCES:%.c=%.o)) $(addprefix $(BUILDDIR)/,$(CPPSOURCES:%.cpp=%.o))

# All targets
all: release

# Debug target
debug: CFLAGS += -DDEBUG -O1
debug: CPPFLAGS += -DDEBUG -O1
debug: LDFLAGS += -O1
debug: OBJECTS += $(BUILDDIR)/bootloader.o
debug: binary

# Release target
release: CFLAGS += -g0 -O3
release: CPPFLAGS += -g0 -O3
release: LDFLAGS += -O3
release: binary

# Binary target
binary: $(HEX) $(ELF)

# Objcopy
$(HEX): $(ELF)
	$(OBJCOPY) $(BFLAGS) $(ELF) $(HEX)
	arm-none-eabi-size $(ELF)
	arm-none-eabi-size $(HEX)

# Link
$(ELF): $(OBJECTS)
	- $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.bl_section,alloc,load,readonly,data,contents $(SOURCEDIR)/bootloader.bin $(BUILDDIR)/bootloader.o
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(ELF)

# Compile C
$(BUILDDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

# Compile C++
$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

# Clean
.PHONY : clean
clean :
	-rm $(OBJECTS)