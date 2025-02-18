The idea here is that you want to use [pntr](https://github.com/robloach/pntr) to make a cross-platform game, but don't want to bother too much with build settings & dependencies and such. It should work on Mac, Windows, Linux, BSDs and Emscripten, and inside a libretro-core, with no external dependencies or config.

> *TODO*: Currently, this is only tested/working on Emscripten, Linux & Mac.

This is a very light system for drawing things on the screen, receiving input, and making sound (both streaming and 1-shot.) It should work on every major platform, and not really require you to structure your app any particular way.

If you want to target multiple backends and share more state at the app-layer, definitely check out [pntr_app](https://github.com/robloach/pntr_app), as it is more mature, complex, and has a lot more features. For a lot of things it might be easier to work with than this.

If you need to do something more advanced with audio, the entire [raudio](https://github.com/raysan5/raudio) API is available.

I also include [cvector](https://github.com/eteran/c-vector), because it's so useful, and I track loaded audio-files with it.

## usage

## build

### cmake

> *TODO*: I have not set this up yet. For now, just use the Makefile.

You can include it in your app a few ways, but easiest is probably cmake:

```cmake
PROJECT(mygame
  LANGUAGES C
)

INCLUDE(FetchContent)

FetchContent_Declare(pntr_shell
  URL https://github.com/konsumer/pntr_shell/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(pntr_shell)

ADD_EXECUTABLE(mygame src/main.c)
TARGET_LINK_LIBRARIES(mygame pntr_shell)
```

You can compile your project like this:

```
cmake -B build -G Ninja
cmake --build build
```

### make

Another option is just to take this whole repo, and put your code in src/example.c, then run `make`.

I have tried to intentionally keep it simple enough that it's self-contained in [lib](src/lib), so you can just include that dir in your project however you like, and make sure you have the platform-specific C/LD flags found in [Makefile](Makefile).

You can do these:

```
# make the native app
make

# make the emscripten app
emmake make

# make the app as libretro-core for mac
make example_libretro.dylib

# make the app as libretro-core for linux
make example_libretro.so

# make the app as libretro-core for windows
make example_libretro.dll

# make the app as libretro-core for emscripten
emmake make example_libretro.wasm
```

### code

> *TODO*: The API is still in flux


## TODO

- get cmake working with static-import (via git) and install, for easier/faster inclusion in user's projects
- support for libretro in raudio: try to use some define that libretro makes, so you just import headers for libretro + pntr_shell, and it detects it
- make sure this works in emscripten
- use this in [null0-pntr](https://github.com/konsumer/null0-pntr)
