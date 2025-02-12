# this is just for building example

.PHONY: clean

CFLAGS ?= -Wall -Wextra -std=c99 -I src/lib -I src/lib/external

# debug
# CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa -framework AudioToolbox
	else
		LDFLAGS = -lX11 -lasound
	endif
endif

example: src/example.c
	$(CC) $? -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f example
