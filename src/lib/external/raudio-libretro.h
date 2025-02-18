// this implements a miniaudio backend for libretro, and adds InitLibretroAudioDevice to raudio

void InitLibretroAudioDevice();

//////////////////////////// below here is miniaudio implementation

// see https://miniaud.io/docs/examples/custom_backend.html for exmaple

typedef struct {
    ma_context context; /* Make this the first member so we can cast between ma_context and ma_context_ex. */
    struct {
        // TODO:  put any functions needed for libretro audio here
    } libretro;
} ma_context_ex;

// TODO: these are example callbacks that pull the data out of miniaudio
// you need to connect libretro up to these so it can get the data

void ma_audio_callback_capture__libretro(void* pUserData, ma_uint8* pBuffer, int bufferSizeInBytes) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pUserData;
    MA_ASSERT(pDeviceEx != NULL);
    ma_device_handle_backend_data_callback((ma_device*)pDeviceEx, NULL, pBuffer, (ma_uint32)bufferSizeInBytes / ma_get_bytes_per_frame(pDeviceEx->device.capture.internalFormat, pDeviceEx->device.capture.internalChannels));
}

void ma_audio_callback_playback__libretro(void* pUserData, ma_uint8* pBuffer, int bufferSizeInBytes) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pUserData;
    MA_ASSERT(pDeviceEx != NULL);
    ma_device_handle_backend_data_callback((ma_device*)pDeviceEx, pBuffer, NULL, (ma_uint32)bufferSizeInBytes / ma_get_bytes_per_frame(pDeviceEx->device.playback.internalFormat, pDeviceEx->device.playback.internalChannels));
}


static ma_result ma_context_uninit__libretro(ma_context* pContext) {
    ma_context_ex* pContextEx = (ma_context_ex*)pContext;
    MA_ASSERT(pContext != NULL);

    // TODO: close audio system

    return MA_SUCCESS;
}

static ma_result ma_context_enumerate_devices__libretro(ma_context* pContext, ma_enum_devices_callback_proc callback, void* pUserData) {
    ma_context_ex* pContextEx = (ma_context_ex*)pContext;
    ma_bool32 isTerminated = MA_FALSE;
    ma_bool32 cbResult;
    int iDevice;

    MA_ASSERT(pContext != NULL);
    MA_ASSERT(callback != NULL);

    // TODO: for each device run
    //
    // ma_device_info deviceInfo;
    // MA_ZERO_OBJECT(&deviceInfo);
    // callback(pContext, ma_device_type_playback, &deviceInfo, pUserData);
    //
    // ma_device_info deviceInfo;
    // MA_ZERO_OBJECT(&deviceInfo);
    // callback(pContext, ma_device_type_capture, &deviceInfo, pUserData);
    //
    // there should be at least 1 in each with deviceInfo.isDefault = MA_TRUE;

    return MA_SUCCESS;
}


static ma_result ma_device_init_internal__libretro(ma_device_ex* pDeviceEx, const ma_device_config* pConfig, ma_device_descriptor* pDescriptor) {
    ma_context_ex* pContextEx = (ma_context_ex*)pDeviceEx->device.pContext;
    MA_ASSERT(pDeviceEx  != NULL);
    MA_ASSERT(pDescriptor != NULL);

    // you can do this to fail:
    //  return MA_FAILED_TO_OPEN_BACKEND_DEVICE;

     // TODO: fill in pDescriptor
    if (pDescriptor->sampleRate == 0) {
        pDescriptor->sampleRate = MA_DEFAULT_SAMPLE_RATE;
    }
    // this is max, but it should be calculated from pDescriptor->sampleRate & pConfig->performanceProfile
    pDescriptor->periodSizeInFrames = 32768;

    // TODO: set up things like callback for libretro or whatever
    // this is where you bind the miniaudio device directly to driver
    // ma_device_init_internal__libretro is a bit confusing to me. I see it binds callback in SDL, but never mentions it again

    return MA_SUCCESS;
}


static ma_result ma_device_init__libretro(ma_device* pDevice, const ma_device_config* pConfig, ma_device_descriptor* pDescriptorPlayback, ma_device_descriptor* pDescriptorCapture) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pDevice;
    ma_context_ex* pContextEx = (ma_context_ex*)pDevice->pContext;
    ma_result result;
    MA_ASSERT(pDevice != NULL);

    /* I don't think libretro supports loopback mode */
    if (pConfig->deviceType == ma_device_type_loopback) {
        return MA_DEVICE_TYPE_NOT_SUPPORTED;
    }

    if (pConfig->deviceType == ma_device_type_capture || pConfig->deviceType == ma_device_type_duplex) {
        result = ma_device_init_internal__libretro(pDeviceEx, pConfig, pDescriptorCapture);
        if (result != MA_SUCCESS) {
            return result;
        }
    }

    if (pConfig->deviceType == ma_device_type_playback || pConfig->deviceType == ma_device_type_duplex) {
        result = ma_device_init_internal__libretro(pDeviceEx, pConfig, pDescriptorPlayback);
        if (result != MA_SUCCESS) {
            return result;
        }
    }

    return MA_SUCCESS;
}

static ma_result ma_device_uninit__libretro(ma_device* pDevice) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pDevice;
    ma_context_ex* pContextEx = (ma_context_ex*)pDevice->pContext;
    MA_ASSERT(pDevice != NULL);

    if (pDevice->type == ma_device_type_capture || pDevice->type == ma_device_type_duplex) {
        // TODO: do any other per-device uninit you need here
    }

    if (pDevice->type == ma_device_type_playback || pDevice->type == ma_device_type_duplex) {
        // TODO: do any other per-device uninit you need here
    }

    return MA_SUCCESS;
}

static ma_result ma_device_start__libretro(ma_device* pDevice) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pDevice;
    ma_context_ex* pContextEx = (ma_context_ex*)pDevice->pContext;
    MA_ASSERT(pDevice != NULL);

    if (pDevice->type == ma_device_type_capture || pDevice->type == ma_device_type_duplex) {
       // TODO: do your start here
    }

    if (pDevice->type == ma_device_type_playback || pDevice->type == ma_device_type_duplex) {
        // TODO: do your start here
    }

    return MA_SUCCESS;
}

static ma_result ma_device_stop__libretro(ma_device* pDevice) {
    ma_device_ex* pDeviceEx = (ma_device_ex*)pDevice;
    ma_context_ex* pContextEx = (ma_context_ex*)pDevice->pContext;
    MA_ASSERT(pDevice != NULL);

    if (pDevice->type == ma_device_type_capture || pDevice->type == ma_device_type_duplex) {
       // TODO: do your stop here
    }

    if (pDevice->type == ma_device_type_playback || pDevice->type == ma_device_type_duplex) {
        // TODO: do your stop here
    }

    return MA_SUCCESS;
}



static ma_result ma_context_get_device_info__libretro(ma_context* pContext, ma_device_type deviceType, const ma_device_id* pDeviceID, ma_device_info* pDeviceInfo) {
    ma_context_ex* pContextEx = (ma_context_ex*)pContext;
    MA_ASSERT(pContext != NULL);

    // TODO: fill in stuff like this
    pDeviceInfo->nativeDataFormatCount = 3;
    pDeviceInfo->nativeDataFormats[0].format     = ma_format_s16;
    pDeviceInfo->nativeDataFormats[0].channels   = 0;   /* All channel counts supported. */
    pDeviceInfo->nativeDataFormats[0].sampleRate = 0;   /* All sample rates supported. */
    pDeviceInfo->nativeDataFormats[0].flags      = 0;
    pDeviceInfo->nativeDataFormats[1].format     = ma_format_s32;
    pDeviceInfo->nativeDataFormats[1].channels   = 0;   /* All channel counts supported. */
    pDeviceInfo->nativeDataFormats[1].sampleRate = 0;   /* All sample rates supported. */
    pDeviceInfo->nativeDataFormats[1].flags      = 0;
    pDeviceInfo->nativeDataFormats[2].format     = ma_format_u8;
    pDeviceInfo->nativeDataFormats[2].channels   = 0;   /* All channel counts supported. */
    pDeviceInfo->nativeDataFormats[2].sampleRate = 0;   /* All sample rates supported. */
    pDeviceInfo->nativeDataFormats[2].flags      = 0;

    return MA_SUCCESS;
}

static ma_result ma_context_init__libretro(ma_context* pContext, const ma_context_config* pConfig, ma_backend_callbacks* pCallbacks) {
    // this is how you will get the conext, later. Cram any libretro functions in here.
    ma_context_ex* pContextEx = (ma_context_ex*)pContext;

    MA_ASSERT(pContext != NULL);

    // TODO: add whatever is needed later (especially functions) to pContextEx->libretro

    // set callbacks for context & device
    pCallbacks->onContextInit             = ma_context_init__libretro; // circular ref back to this function
    pCallbacks->onContextUninit           = ma_context_uninit__libretro;
    pCallbacks->onContextEnumerateDevices = ma_context_enumerate_devices__libretro;
    pCallbacks->onContextGetDeviceInfo    = ma_context_get_device_info__libretro;
    pCallbacks->onDeviceInit              = ma_device_init__libretro;
    pCallbacks->onDeviceUninit            = ma_device_uninit__libretro;
    pCallbacks->onDeviceStart             = ma_device_start__libretro;
    pCallbacks->onDeviceStop              = ma_device_stop__libretro;

    return MA_SUCCESS;
}

// ENTRY: this is bound in parent init as custom "backend" and should just call the init funciton of the backend
static ma_result ma_context_init__libretro_loader(ma_context* pContext, const ma_context_config* pConfig, ma_backend_callbacks* pCallbacks) {
   // this is how you say "failed"
   ma_result result = MA_NO_BACKEND;

   result = ma_context_init__libretro(pContext, pConfig, pCallbacks);

   // this is how you say "success"
   // return MA_SUCCESS;

   return result;
}


void InitLibretroAudioDevice(){
    // TODO: call retro_set_audio_sample_batch to setup callback
    // TODO: init similar to InitAudioDevice but with custom miniaudio device
}
