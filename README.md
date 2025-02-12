The idea here is that you want to use [pntr](https://github.com/robloach/pntr) to make a cross-platform game, but don't want to bother too much with build settings & dependencies and such. It should work on Mac, Windows, Linux, BSDs and Emscripten, with no external dependencies.

This is a very light system for drawing things on the screen, receiving input, and making sound (both streaming and 1-shot.) It should work on every major platform, and not really require you to structure your app any particular way. No state is stored anywhere, so you pass your audio-engine or screen-image or whatever around, however you want to.

If you want to target multiple backends and share more state at the app-layer, definitely check out [pntr_app](https://github.com/robloach/pntr_app), as it is more mature, complex, and has a lot more features. For a lot of things it might be easier to work with than this.


## usage

You can include it in you app a few ways, but easiest is probly cmake:

```cmake
PROJECT(mygame
  LANGUAGES C
)

INCLUDE(FetchContent)

FetchContent_Declare(pntr_shell
  URL https://github.com/konsumer/pntr_shell/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(pntr_shell)

ADD_EXECUTABLE(mygame src/main.c)
TARGET_LINK_LIBRARIES(mygame pntr_shell)
```

You can compile like this:

```
cmake -B build -G Ninja
cmnake --build build
```

Use it in your c-program like this:

```c
#include "pntr_shell.h"

int main() {
    // set to true to quit
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_sound_engine* se = pntr_sound_init();
    pntr_screen_init(screen, "my game");

    while(pntr_keep_going() && !exit) {
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);

        // call this in update-loop to keep sound going
        pntr_sound_update(se);
    }

    pntr_screen_unload(screen);
    pntr_sound_unload(se);
    return 0;
}
```

You can also do streaming audio:

```c
#include "pntr_shell.h"

// read more here: https://stellartux.github.io/websynth/guide.html
static int soundgen(int t) {
  return (t>>10^t>>11)%5*((t>>14&3^t>>15&1)+1)*t%99+((3+(t>>14&3)-(t>>16&1))/3*t%99&64);
}

// this is used to track offset for byte-beat
static int32_t u = 0;

// this is what is called by audio-engine
// it will mix current audio with soundgen output
static void app_get_audio(float* audio, int n) {
  for (int i = 0; i < n; i++) {
    audio[i] = (audio[i]/2.0f) + (soundgen(u++/8) / 2048.0f);
  }
}

int main() {
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_sound_engine* se = pntr_sound_init();
    pntr_screen_init(screen, "my game");

    while(pntr_keep_going() && !exit) {
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);

        pntr_sound_process(se, &app_get_audio);
        pntr_sound_update(se);
    }

    pntr_screen_unload(screen);
    pntr_sound_unload(se);
    return 0;
}
```


## api
It has the full [pntr API](https://github.com/RobLoach/pntr) and these:

```c
// initialize the sound-engine (mixer and streaming)
pntr_sound_engine* se pntr_sound_init();

// initialize the window
void pntr_screen_init(pntr_image* screen, "my game");

// check if we should keep running
bool pntr_keep_going();

// call in main-loop to fuill buffer with audio
void pntr_sound_update(pntr_sound_engine* se);

// call to stream audio. callback(float* audio, int n)
void pntr_sound_process(pntr_sound_engine* se, pntr_audio_callback callback);

// unload the screen
void pntr_screen_unload(pntr_image* screen);

// unload the sound-engine
void pntr_sound_unload(pntr_sound_engine* se);

// load WAV, OGG, MP3, XM, MOD file
pntr_sound* pntr_audio_load(pntr_sound_engine* se, char* filename);

// unload sound
void pntr_audio_unload(pntr_sound* sound);

// play a sound
void pntr_audio_play(pntr_sound* sound);

// stop a sound
void pntr_audio_stop(pntr_sound* sound);

// set a sound to loop
void pntr_audio_set_loop(pntr_sound* sound, bool loop);

// set a specific offset on a sound
void pntr_audio_set_offset(pntr_sound_engine* se, pntr_sound*, int offset);
```
