#define PNTR_PIXELFORMAT_RGBA
#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_UTF8
#define PNTR_ENABLE_MATH
#define PNTR_IMPLEMENTATION
// #define CM_USE_STB_VORBIS

#include "pntr.h"
#include "external/fenster.h"
#include "external/fenster_audio.h"
#include "external/cmixer.c"

typedef struct fenster pntr_window;
typedef cm_Source pntr_sound;
typedef struct fenster_audio pntr_audio_engine;
typedef void (*pntr_sound_callback)(float* audio, int n);


// initialize the sound-engine (mixer and streaming)
pntr_audio_engine* pntr_audio_init();

// initialize the window
pntr_window* pntr_window_init(pntr_image* screen, char* title);

// unload window
void pntr_window_unload(pntr_window* window);

// check if we should keep running
bool pntr_window_keep_going(pntr_window* window);

// call in main-loop to fill buffer with audio
void pntr_audio_update(pntr_audio_engine* se);

// call in main-loop to draw the screen on the window
void pntr_window_update(pntr_window* window, pntr_image* screen);

// call to stream audio. callback(float* audio, int n)
void pntr_audio_process(pntr_audio_engine* se, pntr_sound_callback callback);

// unload the sound-engine
void pntr_audio_unload(pntr_audio_engine* se);

// load WAV, OGG, MP3, FLAC, XM, MOD file
pntr_sound* pntr_sound_load(pntr_audio_engine* se, char* filename);

// unload a sound
void pntr_sound_unload(pntr_sound* sound);

// play a sound
void pntr_sound_play(pntr_sound* sound);

// stop a sound
void pntr_sound_stop(pntr_sound* sound);

// set a sound to loop or not
void pntr_sound_set_loop(pntr_sound* sound, bool loop);

// set a specific offset on a sound (in ms)
void pntr_sound_seek(pntr_sound* sound, int offset);



pntr_audio_engine* pntr_audio_init() {
  cm_init(FENSTER_SAMPLE_RATE);
  struct pntr_audio_engine* fa = malloc(sizeof(pntr_audio_engine));
  fenster_audio_open(fa);
  return fa;
}

pntr_window* pntr_window_init(pntr_image* screen, char* title) {
    pntr_window* window = malloc(sizeof(pntr_window));
    const pntr_window temp = {
        .title = title,
        .width = screen->width,
        .height = screen->height,
        .buf = &screen->data->value
    };
    memcpy(window, &temp, sizeof(pntr_window));
    fenster_open(window);
    return window;
}

void pntr_window_unload(pntr_window* window) {
  fenster_close(window);
}

bool pntr_window_keep_going(pntr_window* window) {
  return fenster_loop(window) == 0;
}

void pntr_audio_update(pntr_audio_engine* se) {}

void pntr_window_update(pntr_window* window, pntr_image* screen) {}

void pntr_audio_process(pntr_audio_engine* se, pntr_sound_callback callback) {}

void pntr_audio_unload(pntr_audio_engine* se) {
  fenster_audio_close(se);
}

pntr_sound* pntr_sound_load(pntr_audio_engine* se, char* filename) {
  unsigned int bytesRead = 0;
  unsigned char* bytes = pntr_load_file(filename, &bytesRead);
  // TODO: this does WAV, OGG needs work, but MP3, FLAC, XM, MOD still needs support
  return cm_new_source_from_mem(bytes, bytesRead);
}

void pntr_sound_unload(pntr_sound* sound) {}

void pntr_sound_play(pntr_sound* sound) {}

void pntr_sound_stop(pntr_sound* sound) {}

void pntr_sound_set_loop(pntr_sound* sound, bool loop) {}

void pntr_sound_seek(pntr_sound* sound, int offset) {}
