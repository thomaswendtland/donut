
EXECUTABLE = test

SRCDIR = .
INCDIR = ./../../inc

CCPATH = /usr/bin

CC = $(CCPATH)/gcc

INCLUDES += -I$(INCDIR) \

CFLAGS = -O0 -g -Wall $(INCLUDES) -std=c++14

$(EXECUTABLE): test.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE)
