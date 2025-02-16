# this is just for building example

.PHONY: clean

CFLAGS ?= -std=gnu99

# debug
CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
	TARGET_EXT = .exe
else
	UNAME_S := $(shell uname -s)
	ifneq (,$(findstring emcc,$(CC)))
		LDFLAGS = -lm -sASYNCIFY=1
		TARGET_EXT = .html
	else ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa -framework AudioToolbox
	else
		LDFLAGS = -lX11 -lasound -lm
		TARGET_EXT =
	endif
endif

TARGET = example$(TARGET_EXT)

$(TARGET): src/example.c
	$(CC) $? -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f example example.exe example.html example.js example.wasm
