// this implements the bare-min raudio implemtenation, for pntr_shell, over fenster-audio

#ifndef TRACELOG
    #define TRACELOG(level, ...)    printf(__VA_ARGS__)
#endif

// Allow custom memory allocators
#ifndef RL_MALLOC
    #define RL_MALLOC(sz)           malloc(sz)
#endif
#ifndef RL_CALLOC
    #define RL_CALLOC(n,sz)         calloc(n,sz)
#endif
#ifndef RL_REALLOC
    #define RL_REALLOC(ptr,sz)      realloc(ptr,sz)
#endif
#ifndef RL_FREE
    #define RL_FREE(ptr)            free(ptr)
#endif


#if defined(SUPPORT_FILEFORMAT_WAV)
    #define DRWAV_MALLOC RL_MALLOC
    #define DRWAV_REALLOC RL_REALLOC
    #define DRWAV_FREE RL_FREE

    #define DR_WAV_IMPLEMENTATION
    #include "external/dr_wav.h"        // WAV loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_OGG)
    // TODO: Remap stb_vorbis malloc()/free() calls to RL_MALLOC/RL_FREE
    #include "external/stb_vorbis.c"    // OGG loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_MP3)
    #define DRMP3_MALLOC RL_MALLOC
    #define DRMP3_REALLOC RL_REALLOC
    #define DRMP3_FREE RL_FREE

    #define DR_MP3_IMPLEMENTATION
    #include "external/dr_mp3.h"        // MP3 loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_QOA)
    #define QOA_MALLOC RL_MALLOC
    #define QOA_FREE RL_FREE

    #if defined(_MSC_VER)           // Disable some MSVC warning
        #pragma warning(push)
        #pragma warning(disable : 4018)
        #pragma warning(disable : 4267)
        #pragma warning(disable : 4244)
    #endif

    #define QOA_IMPLEMENTATION
    #include "external/qoa.h"           // QOA loading and saving functions
    #include "external/qoaplay.c"       // QOA stream playing helper functions

    #if defined(_MSC_VER)
        #pragma warning(pop)        // Disable MSVC warning suppression
    #endif
#endif

#if defined(SUPPORT_FILEFORMAT_FLAC)
    #define DRFLAC_MALLOC RL_MALLOC
    #define DRFLAC_REALLOC RL_REALLOC
    #define DRFLAC_FREE RL_FREE

    #define DR_FLAC_IMPLEMENTATION
    #define DR_FLAC_NO_WIN32_IO
    #include "external/dr_flac.h"       // FLAC loading functions
#endif

#if defined(SUPPORT_FILEFORMAT_XM)
    #define JARXM_MALLOC RL_MALLOC
    #define JARXM_FREE RL_FREE

    #if defined(_MSC_VER)           // Disable some MSVC warning
        #pragma warning(push)
        #pragma warning(disable : 4244)
    #endif

    #define JAR_XM_IMPLEMENTATION
    #include "external/jar_xm.h"        // XM loading functions

    #if defined(_MSC_VER)
        #pragma warning(pop)        // Disable MSVC warning suppression
    #endif
#endif

#if defined(SUPPORT_FILEFORMAT_MOD)
    #define JARMOD_MALLOC RL_MALLOC
    #define JARMOD_FREE RL_FREE

    #define JAR_MOD_IMPLEMENTATION
    #include "external/jar_mod.h"       // MOD loading functions
#endif

typedef void (*AudioCallback)(void *bufferData, unsigned int frames);
typedef struct rAudioBuffer rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

typedef struct AudioStream {
    rAudioBuffer *buffer;
    rAudioProcessor *processor;

    unsigned int sampleRate;
    unsigned int sampleSize;
    unsigned int channels;
} AudioStream;

typedef struct Music {
    AudioStream stream;
    unsigned int frameCount;
    bool looping;

    int ctxType;
    void *ctxData;
} Music;

const char *GetFileExtension(const char *fileName);
void InitAudioDevice(void);
void CloseAudioDevice(void);
Music LoadMusicStreamFromMemory(const char *fileType, const unsigned char* data, int dataSize);
void UpdateMusicStream(Music music);
void UnloadMusicStream(Music music);
void PlayMusicStream(Music music);
bool IsMusicStreamPlaying(Music music);
bool IsMusicReady(Music music);
void StopMusicStream(Music music);
void SeekMusicStream(Music music, float position);
void SetMusicVolume(Music music, float volume);
void SetMusicPitch(Music music, float pitch);
void SetMusicPan(Music music, float pan);
float GetMusicTimeLength(Music music);
float GetMusicTimePlayed(Music music);
void AttachAudioMixedProcessor(AudioCallback processor);
void DetachAudioMixedProcessor(AudioCallback processor);

// TODO: need to fill all these in
