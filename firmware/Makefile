SOURCEDIR := .
BUILDDIR := $(SOURCEDIR)/build
ELF := $(BUILDDIR)/executable.elf
HEX := $(BUILDDIR)/executable.hex
OBJDIR = $(BUILDDIR)/obj
ALLDIRS = $(shell find $(SOURCEDIR) -maxdepth 3 -type d \
			-not -path "./.git/*" \
			-not -path "./build/*" \
			-not -path "./.vscode/*" \
			-not -path "./.settings/*")

CC := arm-none-eabi-gcc
LD := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

CFLAGS += -Wall
CFLAGS += -Wdouble-promotion
CFLAGS += -Wstrict-prototypes
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -O1
CFLAGS += -fmessage-length=0
CFLAGS += -fsigned-char
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -g
CFLAGS += -std=gnu11
CFLAGS += $(foreach d, $(ALLDIRS), -I$d)

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mthumb
LDFLAGS += -mfloat-abi=hard
LDFLAGS += -mfpu=fpv4-sp-d16
LDFLAGS += -O1
LDFLAGS += -fmessage-length=0
LDFLAGS += -fsigned-char
LDFLAGS += -ffunction-sections
LDFLAGS += -fdata-sections
LDFLAGS += -g
LDFLAGS += -Tpac55xx.ld
LDFLAGS += -nostartfiles
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -Wl,-Map,"$(BUILDDIR)/out.map"

BFLAGS += -O ihex
# ---

# Include all sources
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')

# Get list of object files, with paths
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

# All targets
all: $(HEX) $(ELF)

# Objcopy
$(HEX): $(ELF)
	$(OBJCOPY) $(BFLAGS) $(ELF) $(HEX)
	arm-none-eabi-size $(HEX)

# Link
$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(ELF)

# Compile
$(BUILDDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@
