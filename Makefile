# this is just for building example

.PHONY: clean

CFLAGS ?= -std=gnu99

# debug
CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
else
	UNAME_S := $(shell uname -s)
	# First, check if we're using Emscripten
	ifneq (,$(findstring emcc,$(CC)))
		LDFLAGS = -lm
		TARGET_EXT = .html
	else ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa -framework AudioToolbox
	else
		LDFLAGS = -lX11 -lasound -lm
		TARGET_EXT =
	endif
endif

TARGET = example$(TARGET_EXT)

example: src/example.c
	$(CC) $? -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f example example.exe
