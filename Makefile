# this is just for building example

.PHONY: clean

CFLAGS ?= -std=gnu99

# debug
CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa -framework AudioToolbox
	else
		LDFLAGS = -lX11 -lasound -lm
	endif
endif

example: src/example.c
	$(CC) $? -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f example example.exe
