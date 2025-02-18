# this is just for building example

.PHONY: clean

CFLAGS ?= -std=gnu99

# debug
CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
	TARGET_EXT = .exe
	RETRO_EXT = .dll
else
	UNAME_S := $(shell uname -s)
	ifneq (,$(findstring emcc,$(CC)))
		LDFLAGS = -lm -DMA_ENABLE_AUDIO_WORKLETS -sASYNCIFY=1 -sAUDIO_WORKLET=1 -sWASM_WORKERS=1
		TARGET_EXT = .html
		RETRO_EXT = .wasm
	else ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa -framework AudioToolbox -framework IOKit
		TARGET_EXT =
		RETRO_EXT = .dylib
	else
		LDFLAGS = -lX11 -lasound -lm
		TARGET_EXT =
		RETRO_EXT = .so
	endif
endif

example$(TARGET_EXT): src/example.c
	$(CC) $? -o $(TARGET) $(CFLAGS) $(LDFLAGS)

example$(RETRO_EXT): src/example.c
	$(CC) $? -o $(TARGET) $(CFLAGS) $(LDFLAGS) -DBUILD_LIBRETRO

clean:
	rm -f example example.exe example.html example.js example.wasm
