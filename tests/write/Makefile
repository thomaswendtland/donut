
EXECUTABLE = test

SRCDIR = .
INCDIR = ./../../inc

CCPATH = /usr/bin

CC = $(CCPATH)/g++

INCLUDES += -I$(INCDIR) \

CFLAGS = -O0 -g -Wall $(INCLUDES) -std=c++14

$(EXECUTABLE): test.cpp
	$(CC) $(CFLAGS) $< -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
