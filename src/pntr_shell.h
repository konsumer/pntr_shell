 typedef void (*pntr_sound_callback)(float* audio, int n);



 // initialize the sound-engine (mixer and streaming)
pntr_audio_engine* se pntr_audio_init();

// initialize the window
pntr_window* pntr_screen_init(pntr_image* screen, "my game");

// unload window
void pntr_screen_unload(pntr_window* window);

// check if we should keep running
bool pntr_keep_going(pntr_window* window);

// call in main-loop to fill buffer with audio
void pntr_audio_update(pntr_audio_engine* se);

// call in main-loop to draw the screen on the window
void pntr_screen_update(pntr_window* window, pntr_image* screen);

// call to stream audio. callback(float* audio, int n)
void pntr_audio_process(pntr_audio_engine* se, pntr_sound_callback callback);

// unload the sound-engine
void pntr_audio_unload(pntr_audio_engine* se);

// load WAV, OGG, MP3, FLAC, XM, MOD file
pntr_sound* pntr_sound_load(pntr_audio_engine* se, char* filename);

// unload a sound
void pntr_sound_unload(pntr_audio_engine* se, pntr_sound* sound);

// play a sound
void pntr_sound_play(pntr_sound* sound);

// stop a sound
void pntr_sound_stop(pntr_sound* sound);

// set a sound to loop or not
void pntr_sound_set_loop(pntr_sound* sound, bool loop);

// set a specific offset on a sound (in ms)
void pntr_sound_seek(pntr_sound* sound, int offset);