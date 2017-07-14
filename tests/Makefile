
PROJECT = rye
BUILDDIR = ./build

SRCDIR = ./src
INCDIR = ./inc

TARGET_CCPATH = /opt/gcc-arm-none-eabi/bin

TARGET_CC = $(TARGET_CCPATH)/arm-none-eabi-gcc
TARGET_LD = $(TARGET_CCPATH)/arm-none-eabi-gcc
TARGET_AR = $(TARGET_CCPATH)/arm-none-eabi-ar
TARGET_OBJCOPY = $(CCPATH)/arm-none-eabi-objcopy

TARGET_CFLAGS  = -O0 -g -Wall -I. -mcpu=cortex-m4 -mthumb $(INCLUDES) -DUSE_STDPERIPH_DRIVER -D$(DEFINES) -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

LDSCRIPT = ./STM32F767ZITx_FLASH.ld
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m4 -specs=nosys.specs

INCLUDES += -I$(INCDIR) \

SOURCES += $(wildcard src/*.c)

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

#DEFINES +=

ELF = $(BUILDDIR)/$(PROJECT).elf
HEX = $(BUILDDIR)/$(PROJECT).hex
BIN = $(BUILDDIR)/$(PROJECT).bin

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf build
