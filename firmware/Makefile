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

CC := arm-none-eabi-gcc
CPP := arm-none-eabi-g++
LD := arm-none-eabi-g++
OBJCOPY := arm-none-eabi-objcopy

BASEFLAGS += -mcpu=cortex-m4
BASEFLAGS += -mthumb
BASEFLAGS += -mfloat-abi=hard
BASEFLAGS += -mfpu=fpv4-sp-d16
BASEFLAGS += -fmessage-length=0
BASEFLAGS += -fsigned-char
BASEFLAGS += -ffunction-sections
BASEFLAGS += -fdata-sections
BASEFLAGS += -Wall
BASEFLAGS += -Wlogical-op
BASEFLAGS += -Waggregate-return
BASEFLAGS += -Wdouble-promotion
BASEFLAGS += $(foreach d, $(ALLDIRS), -I$d)

CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu11
CFLAGS += $(BASEFLAGS)

CPPFLAGS += -std=c++17
CPPFLAGS += $(BASEFLAGS)

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mthumb
LDFLAGS += -mfloat-abi=hard
LDFLAGS += -mfpu=fpv4-sp-d16
LDFLAGS += -fmessage-length=0
LDFLAGS += -fsigned-char
LDFLAGS += -ffunction-sections
LDFLAGS += -fdata-sections
LDFLAGS += -Wall
LDFLAGS += -Wlogical-op
LDFLAGS += -Waggregate-return
LDFLAGS += -Tpac55xx.ld
LDFLAGS += -nostartfiles
LDFLAGS += -specs=nosys.specs
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -Xlinker -Map="$(BUILDDIR)/tinymovr.map"

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
debug: CFLAGS += -DDEBUG -g -O1
debug: CPPFLAGS += -DDEBUG -g -O1
debug: LDFLAGS += -O1
debug: OBJECTS += $(BUILDDIR)/bootloader.o
debug: binary

# Release target
release: CFLAGS += -g0 -O3 -funroll-loops
release: CPPFLAGS += -g0 -O3 -funroll-loops
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