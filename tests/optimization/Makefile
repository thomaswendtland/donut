
EXECUTABLE = test

SRCDIR = .
INCDIR = ./../../inc

CCPATH = /opt/gcc6-arm-none-eabi/bin

CC = $(CCPATH)/arm-none-eabi-c++
SIZE = $(CCPATH)/arm-none-eabi-size
OBJDUMP = $(CCPATH)/arm-none-eabi-objdump

INCLUDES += -I$(INCDIR) \

CFLAGS = -Os -g -Wall $(INCLUDES) -std=c++14 -nostdlib -mcpu=cortex-m3 -mthumb

$(EXECUTABLE): $(EXECUTABLE).cpp
	$(CC) $(CFLAGS) $< -o $(EXECUTABLE)
	$(OBJDUMP) -d $(EXECUTABLE) > asm.s
	$(SIZE) $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
