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
LD := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy


CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -O3
CFLAGS += -fmessage-length=0
CFLAGS += -fsigned-char
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall
CFLAGS += -Wshadow
CFLAGS += -Wlogical-op
CFLAGS += -Waggregate-return
CFLAGS += -Wdouble-promotion
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu11
CFLAGS += $(foreach d, $(ALLDIRS), -I$d)

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mthumb
LDFLAGS += -mfloat-abi=hard
LDFLAGS += -mfpu=fpv4-sp-d16
LDFLAGS += -O3
LDFLAGS += -fmessage-length=0
LDFLAGS += -fsigned-char
LDFLAGS += -ffunction-sections
LDFLAGS += -fdata-sections
LDFLAGS += -Wall
LDFLAGS += -Wshadow
LDFLAGS += -Wlogical-op
LDFLAGS += -Waggregate-return
LDFLAGS += -Wdouble-promotion
LDFLAGS += -Wstrict-prototypes
LDFLAGS += -Tpac55xx.ld
LDFLAGS += -std=gnu11
LDFLAGS += -nostartfiles
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -Wl,-Map,"$(BUILDDIR)/tinymovr.map"

BFLAGS += -O ihex
# ---

# Include all sources
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')

# Get list of object files, with paths
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

# All targets
all: release

# Debug target
debug: CFLAGS += -DDEBUG -g3
debug: binary

# Release target
release: CFLAGS += -g0
release: binary

# Release target
binary: $(HEX) $(ELF)

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

# Clean
.PHONY : clean
clean :
	-rm $(OBJECTS)