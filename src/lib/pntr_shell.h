// you can custmize these defines, if you want, but I just want to enable everything possible, by default

#define PNTR_PIXELFORMAT_RGBA
#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_UTF8
#define PNTR_ENABLE_MATH
#define PNTR_IMPLEMENTATION
#define CVECTOR_LOGARITHMIC_GROWTH
#define SUPPORT_MODULE_RAUDIO
#define RAUDIO_STANDALONE
#define SUPPORT_FILEFORMAT_WAV
#define SUPPORT_FILEFORMAT_OGG
#define SUPPORT_FILEFORMAT_MP3
#define SUPPORT_FILEFORMAT_QOA
#define SUPPORT_FILEFORMAT_FLAC
#define SUPPORT_FILEFORMAT_XM
#define SUPPORT_FILEFORMAT_MOD

// this disables raudio logging
#define TRACELOG(level, ...)

#include "external/cvector.h"
#include "pntr.h"
#include "external/fenster.h"
#include "raudio.c"

// a "window" is a fenster instance
typedef struct fenster pntr_window;

// pntr_sound is a bit confusing with raudio, it's Music, not Sound or Wave
typedef Music pntr_sound;

typedef AudioCallback pntr_sound_callback;

typedef enum pntr_app_key {
    PNTR_APP_KEY_INVALID          = 0,
    PNTR_APP_KEY_FIRST            = 32,
    PNTR_APP_KEY_SPACE            = 32,
    PNTR_APP_KEY_APOSTROPHE       = 39,  /* ' */
    PNTR_APP_KEY_COMMA            = 44,  /* , */
    PNTR_APP_KEY_MINUS            = 45,  /* - */
    PNTR_APP_KEY_PERIOD           = 46,  /* . */
    PNTR_APP_KEY_SLASH            = 47,  /* / */
    PNTR_APP_KEY_0                = 48,
    PNTR_APP_KEY_1                = 49,
    PNTR_APP_KEY_2                = 50,
    PNTR_APP_KEY_3                = 51,
    PNTR_APP_KEY_4                = 52,
    PNTR_APP_KEY_5                = 53,
    PNTR_APP_KEY_6                = 54,
    PNTR_APP_KEY_7                = 55,
    PNTR_APP_KEY_8                = 56,
    PNTR_APP_KEY_9                = 57,
    PNTR_APP_KEY_SEMICOLON        = 59,  /* ; */
    PNTR_APP_KEY_EQUAL            = 61,  /* = */
    PNTR_APP_KEY_A                = 65,
    PNTR_APP_KEY_B                = 66,
    PNTR_APP_KEY_C                = 67,
    PNTR_APP_KEY_D                = 68,
    PNTR_APP_KEY_E                = 69,
    PNTR_APP_KEY_F                = 70,
    PNTR_APP_KEY_G                = 71,
    PNTR_APP_KEY_H                = 72,
    PNTR_APP_KEY_I                = 73,
    PNTR_APP_KEY_J                = 74,
    PNTR_APP_KEY_K                = 75,
    PNTR_APP_KEY_L                = 76,
    PNTR_APP_KEY_M                = 77,
    PNTR_APP_KEY_N                = 78,
    PNTR_APP_KEY_O                = 79,
    PNTR_APP_KEY_P                = 80,
    PNTR_APP_KEY_Q                = 81,
    PNTR_APP_KEY_R                = 82,
    PNTR_APP_KEY_S                = 83,
    PNTR_APP_KEY_T                = 84,
    PNTR_APP_KEY_U                = 85,
    PNTR_APP_KEY_V                = 86,
    PNTR_APP_KEY_W                = 87,
    PNTR_APP_KEY_X                = 88,
    PNTR_APP_KEY_Y                = 89,
    PNTR_APP_KEY_Z                = 90,
    PNTR_APP_KEY_LEFT_BRACKET     = 91,  /* [ */
    PNTR_APP_KEY_BACKSLASH        = 92,  /* \ */
    PNTR_APP_KEY_RIGHT_BRACKET    = 93,  /* ] */
    PNTR_APP_KEY_GRAVE_ACCENT     = 96,  /* ` */
    PNTR_APP_KEY_WORLD_1          = 161, /* non-US #1 */
    PNTR_APP_KEY_WORLD_2          = 162, /* non-US #2 */
    PNTR_APP_KEY_ESCAPE           = 256,
    PNTR_APP_KEY_ENTER            = 257,
    PNTR_APP_KEY_TAB              = 258,
    PNTR_APP_KEY_BACKSPACE        = 259,
    PNTR_APP_KEY_INSERT           = 260,
    PNTR_APP_KEY_DELETE           = 261,
    PNTR_APP_KEY_RIGHT            = 262,
    PNTR_APP_KEY_LEFT             = 263,
    PNTR_APP_KEY_DOWN             = 264,
    PNTR_APP_KEY_UP               = 265,
    PNTR_APP_KEY_PAGE_UP          = 266,
    PNTR_APP_KEY_PAGE_DOWN        = 267,
    PNTR_APP_KEY_HOME             = 268,
    PNTR_APP_KEY_END              = 269,
    PNTR_APP_KEY_CAPS_LOCK        = 280,
    PNTR_APP_KEY_SCROLL_LOCK      = 281,
    PNTR_APP_KEY_NUM_LOCK         = 282,
    PNTR_APP_KEY_PRINT_SCREEN     = 283,
    PNTR_APP_KEY_PAUSE            = 284,
    PNTR_APP_KEY_F1               = 290,
    PNTR_APP_KEY_F2               = 291,
    PNTR_APP_KEY_F3               = 292,
    PNTR_APP_KEY_F4               = 293,
    PNTR_APP_KEY_F5               = 294,
    PNTR_APP_KEY_F6               = 295,
    PNTR_APP_KEY_F7               = 296,
    PNTR_APP_KEY_F8               = 297,
    PNTR_APP_KEY_F9               = 298,
    PNTR_APP_KEY_F10              = 299,
    PNTR_APP_KEY_F11              = 300,
    PNTR_APP_KEY_F12              = 301,
    PNTR_APP_KEY_F13              = 302,
    PNTR_APP_KEY_F14              = 303,
    PNTR_APP_KEY_F15              = 304,
    PNTR_APP_KEY_F16              = 305,
    PNTR_APP_KEY_F17              = 306,
    PNTR_APP_KEY_F18              = 307,
    PNTR_APP_KEY_F19              = 308,
    PNTR_APP_KEY_F20              = 309,
    PNTR_APP_KEY_F21              = 310,
    PNTR_APP_KEY_F22              = 311,
    PNTR_APP_KEY_F23              = 312,
    PNTR_APP_KEY_F24              = 313,
    PNTR_APP_KEY_F25              = 314,
    PNTR_APP_KEY_KP_0             = 320,
    PNTR_APP_KEY_KP_1             = 321,
    PNTR_APP_KEY_KP_2             = 322,
    PNTR_APP_KEY_KP_3             = 323,
    PNTR_APP_KEY_KP_4             = 324,
    PNTR_APP_KEY_KP_5             = 325,
    PNTR_APP_KEY_KP_6             = 326,
    PNTR_APP_KEY_KP_7             = 327,
    PNTR_APP_KEY_KP_8             = 328,
    PNTR_APP_KEY_KP_9             = 329,
    PNTR_APP_KEY_KP_DECIMAL       = 330,
    PNTR_APP_KEY_KP_DIVIDE        = 331,
    PNTR_APP_KEY_KP_MULTIPLY      = 332,
    PNTR_APP_KEY_KP_SUBTRACT      = 333,
    PNTR_APP_KEY_KP_ADD           = 334,
    PNTR_APP_KEY_KP_ENTER         = 335,
    PNTR_APP_KEY_KP_EQUAL         = 336,
    PNTR_APP_KEY_LEFT_SHIFT       = 340,
    PNTR_APP_KEY_LEFT_CONTROL     = 341,
    PNTR_APP_KEY_LEFT_ALT         = 342,
    PNTR_APP_KEY_LEFT_SUPER       = 343,
    PNTR_APP_KEY_RIGHT_SHIFT      = 344,
    PNTR_APP_KEY_RIGHT_CONTROL    = 345,
    PNTR_APP_KEY_RIGHT_ALT        = 346,
    PNTR_APP_KEY_RIGHT_SUPER      = 347,
    PNTR_APP_KEY_MENU             = 348,
    PNTR_APP_KEY_LAST
} pntr_app_key;

static cvector_vector_type(pntr_sound*) loaded_sounds = NULL;

// initialize the window & audio-system
pntr_window* pntr_shell_init(pntr_image* screen, char* title) {
    pntr_window* window = malloc(sizeof(pntr_window));
    const pntr_window temp = {
        .title = title,
        .width = screen->width,
        .height = screen->height,
        .buf = &screen->data->value
    };
    memcpy(window, &temp, sizeof(pntr_window));
    fenster_open(window);
    InitAudioDevice();
    return window;
}

// unload window & audio-system
void pntr_shell_unload(pntr_window* window) {
  fenster_close(window);
  CloseAudioDevice();
  for (int i = 0; i < cvector_size(loaded_sounds); ++i) {
    if (loaded_sounds[i] != NULL) {
      UnloadMusicStream(*loaded_sounds[i]);
    }
  }
  cvector_free(loaded_sounds);
}

// check if we should keep running (also updates all loaded audio)
bool pntr_keep_going(pntr_window* window) {
  // TODO: audio loops
  for (int i = 0; i < cvector_size(loaded_sounds); ++i) {
    if (loaded_sounds[i] != NULL) {
      UpdateMusicStream(*loaded_sounds[i]);
    }
  }
  return fenster_loop(window) == 0;
}

// load a sound file
pntr_sound pntr_sound_load(char* filename) {
  unsigned int dataSize = 0;
  const unsigned char* fileData = pntr_load_file(filename, &dataSize);
  if (dataSize > 0) {
    // I prefer magic-bytes to extension, but raudio already works this way
    // see https://github.com/RobLoach/pntr/issues/191
    pntr_sound sound = LoadMusicStreamFromMemory(GetFileExtension(filename), fileData, dataSize);
    cvector_push_back(loaded_sounds, &sound);
    sound.looping = false;
    return sound;
  } else {
    // is there a better way to return "none" for sound?
    pntr_sound empty = {};
    return empty;
  }
}

// unload a sound
void pntr_sound_unload(pntr_sound sound) {
  for (int i = 0; i < cvector_size(loaded_sounds); ++i) {
    if (loaded_sounds[i] == &sound) {
      cvector_erase(loaded_sounds, i);
      break;
    }
  }
  UnloadMusicStream(sound);
}

// Checks if a sound is ready
bool pntr_sound_ready(pntr_sound sound) {
  return IsMusicReady(sound);
}

// Check if sound is playing
bool pntr_sound_isplaying(pntr_sound sound) {
  return IsMusicStreamPlaying(sound);
}

// Stop sound playing
void pntr_sound_stop(pntr_sound sound) {
  StopMusicStream(sound);
}

// Start sound playing
void pntr_sound_play(pntr_sound sound){
  StopMusicStream(sound);
  PlayMusicStream(sound);
}

// Seek sound to a position (in seconds)
void pntr_sound_seek(pntr_sound sound, float position) {
  SeekMusicStream(sound, position);
}

// Set volume for sound (1.0 is max level)
void pntr_sound_volume(pntr_sound sound, float volume) {
  SetMusicVolume(sound, volume);
}

// Set pitch for a sound (1.0 is base level)
void pntr_sound_pitch(pntr_sound sound, float pitch) {
  SetMusicPitch(sound, pitch);
}

// Set pan for a sound (0.0 to 1.0, 0.5=center)
void pntr_sound_pan(pntr_sound sound, float pan) {
  SetMusicPan(sound, pan);
}

// Get sound time length (in seconds)
float pntr_sound_get_length(pntr_sound sound) {
  return GetMusicTimeLength(sound);
}

// Get current sound time played (in seconds)
float pntr_sound_get_played(pntr_sound sound) {
  return GetMusicTimePlayed(sound);
}

// register a sound-callback
void pntr_sound_register(pntr_sound_callback cb) {
  AttachAudioMixedProcessor(cb);
}

// unregister a sound-callback
void pntr_sound_unregister(pntr_sound_callback cb) {
  DetachAudioMixedProcessor(cb);
}
