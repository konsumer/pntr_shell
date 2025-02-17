The idea here is that you want to use [pntr](https://github.com/robloach/pntr) to make a cross-platform game, but don't want to bother too much with build settings & dependencies and such. It should work on Mac, Windows, Linux, BSDs and Emscripten, with no external dependencies or config.

This is a very light system for drawing things on the screen, receiving input, and making sound (both streaming and 1-shot.) It should work on every major platform, and not really require you to structure your app any particular way.

If you want to target multiple backends and share more state at the app-layer, definitely check out [pntr_app](https://github.com/robloach/pntr_app), as it is more mature, complex, and has a lot more features. For a lot of things it might be easier to work with than this.

If you need to do something more advanced with audio, the entire [raudio](https://github.com/raysan5/raudio) API is available.

I also include [cvector](https://github.com/eteran/c-vector), because it's so useful.

## usage

## build

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
cmnake --build build
```

Another option is just to take this whole repo, and put your code in src/example.c, then run `make`.

I have tried to intentionally keep it simple enough that it's self-contained in [lib](src/lib), so you can just include that dir in your project however you like, and make sure you have the platform-specific C/LD flags found in [Makefile](Makefile).

### code


Use it in your c-program like this:

```c
#include "lib/pntr_shell.h"

int main() {
    // set to true to quit
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_window* window = pntr_shell_init(screen, "my game");

    pntr_sound wub = pntr_sound_load("example.mp3");
    pntr_sound_play(wub);

    while(pntr_keep_going(window) && !exit) {
        pntr_clear_background(screen, PNTR_BLACK);
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);

        if (window->keys[PNTR_APP_KEY_SPACE]) {
            pntr_sound_play(wub);
        }
    }

    pntr_unload_image(screen);
    pntr_shell_unload(window);
    return 0;
}

```

You can also do streaming audio:

```c
#include "lib/pntr_shell.h"

static int u = 0;

void bytebeat(float* bufferData, unsigned int frames) {
  int t;
  float a;
  for (unsigned int frame = 0; frame < frames; frame++){
    t = u++/8;
    // read more here: https://stellartux.github.io/websynth/guide.html
    a = (audio[i]/2.0f) + ( (t>>10^t>>11)%5*((t>>14&3^t>>15&1)+1)*t%99+((3+(t>>14&3)-(t>>16&1))/3*t%99&64) / 2048.0f);

    samples[frame * 2 + 0] = a; // left
    samples[frame * 2 + 1] = a; // right
  }
}

int main() {
    // set to true to quit
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_window* window = pntr_shell_init(screen, "my game");

    pntr_sound_register(bytebeat);

    while(pntr_keep_going(window) && !exit) {
        pntr_clear_background(screen, PNTR_BLACK);
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);
    }

    pntr_unload_image(screen);
    pntr_shell_unload(window);
    ntr_sound_unregister(bytebeat);
    return 0;
}
```


## api
It has the full [pntr API](https://github.com/RobLoach/pntr) and these:

```c
// initialize the window & audio-system
pntr_window* pntr_shell_init(pntr_image* screen, char* title);

// unload window & audio-system
void pntr_shell_unload(pntr_window* window);

// check if we should keep running (also updates all loaded audio)
bool pntr_keep_going(pntr_window* window);

// load a sound file
pntr_sound pntr_sound_load(char* filename);

// unload a sound
void pntr_sound_unload(pntr_sound sound);

// Checks if a sound is ready
bool pntr_sound_ready(pntr_sound sound);

// Check if sound is playing
bool pntr_sound_isplaying(pntr_sound sound);

// Stop sound playing
void pntr_sound_stop(pntr_sound sound);

// Start sound playing
void pntr_sound_play(pntr_sound sound);

// Seek sound to a position (in seconds)
void pntr_sound_seek(pntr_sound sound, float position);

// Set volume for sound (1.0 is max level)
void pntr_sound_volume(pntr_sound sound, float volume);

// Set pitch for a sound (1.0 is base level)
void pntr_sound_pitch(pntr_sound sound, float pitch);

// Set pan for a sound (0.0 to 1.0, 0.5=center)
void pntr_sound_pan(pntr_sound sound, float pan);

// Get sound time length (in seconds)
float pntr_sound_get_length(pntr_sound sound);

// Get current sound time played (in seconds)
float pntr_sound_get_played(pntr_sound sound);

// register a sound-callback
void pntr_sound_register(pntr_sound_callback cb);

// unregister a sound-callback
void pntr_sound_unregister(pntr_sound_callback cb);
```


## TODO

- get cmake working with static-import (via git) and install, for easier/faster inclusion in user's projects
- support for libretro in raudio: try to use some define that libretro makes, so you just import headers for libretro + pntr_shell, and it detects it
- make sure this works in emscripten
- figure out how to deal with raudio `Sound`/`Music` properly (streaming `Music` is nice, but `Sound` can play multiple copies of same sound at once)
- use this in [null0-pntr](https://github.com/konsumer/null0-pntr)
