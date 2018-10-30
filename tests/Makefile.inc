
# Basic makefile for all tests

BUILDDIR = build

BIN = $(BUILDDIR)/$(addsuffix .bin, $(NAME))
ELF = $(BUILDDIR)/$(addsuffix .elf, $(NAME))
HEX = $(BUILDDIR)/$(addsuffix .hex, $(NAME))

INCDIR = ../../inc
SRCDIR = .

CCPREFIX =

CC = $(CCPREFIX)gcc
LD =  $(CCPREFIX)ld
OBJDUMP =  $(CCPREFIX)objdump
OBJCOPY =  $(CCPREFIX)objcopy
SIZE =  $(CCPREFIX)size

INCLUDES = -I$(INCDIR)

SOURCES = $(wildcard *.cpp)

OBJECTS += $(addsuffix .o, $(basename $(SOURCES)))

# LDSCRIPT = ../device.ld
LDFLAGS = -lc

CFLAGS = -O0 -g -Wall -Wpedantic $(INCLUDES) -std=c++14

all:
	mkdir -p $(BUILDDIR)
	gcc $(CFLAGS) $(SOURCES) -o $(BUILDDIR)/$(NAME)

clean:
	rm -rf $(BUILDDIR)

asm: $(ELF)
	$(OBJDUMP) -fhD $(ELF) > asm.txt
