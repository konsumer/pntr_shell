# this is just for building example

.PHONY: clean

CFLAGS ?= -I src/lib -I src/lib/external

# debug
# CFLAGS += -g

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lgdi32
	TARGET_EXT = .exe
	RETRO_EXT = .dll
else
	UNAME_S := $(shell uname -s)
	ifneq (,$(findstring emcc,$(CC)))
		LDFLAGS = -lm -DMA_ENABLE_AUDIO_WORKLETS -sAUDIO_WORKLET=1 -sWASM_WORKERS=1 -sASYNCIFY -DPLATFORM_WEB --preload-file assets
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
	$(CC) $? $(CFLAGS) $(LDFLAGS) -o $@

example_libretro$(RETRO_EXT): src/example.c
	$(CC) $? $(CFLAGS) $(LDFLAGS) -DBUILD_LIBRETRO -o $@

clean:
	rm -f example *.exe *.html *.js *.wasm *.data *.so *.dylib *.dll
