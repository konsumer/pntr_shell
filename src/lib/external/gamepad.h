/*
Cross platform gamepad library

will auto-detect windows/mac/linux/emscripten, define BUILD_LIBRETRO for libretro-core

TODO: test on emscripen, libretro, and windows

Created by David Konsumer
*/

#ifndef GAMEPAD_H
#define GAMEPAD_H

#define GAMEPAD_MAX 4

typedef enum {
    GAMEPAD_BUTTON_A,
    GAMEPAD_BUTTON_B,
    GAMEPAD_BUTTON_X,
    GAMEPAD_BUTTON_Y,
    GAMEPAD_BUTTON_UP,
    GAMEPAD_BUTTON_DOWN,
    GAMEPAD_BUTTON_LEFT,
    GAMEPAD_BUTTON_RIGHT,
    GAMEPAD_BUTTON_START,
    GAMEPAD_BUTTON_BACK,
    GAMEPAD_BUTTON_GUIDE,
    GAMEPAD_BUTTON_LEFT_SHOULDER,
    GAMEPAD_BUTTON_RIGHT_SHOULDER,
    GAMEPAD_BUTTON_LEFT_THUMB,
    GAMEPAD_BUTTON_RIGHT_THUMB,
    GAMEPAD_BUTTON_COUNT
} GamepadButton;

typedef struct {
    char buttons[GAMEPAD_BUTTON_COUNT];
    float axis_left_x;
    float axis_left_y;
    float axis_right_x;
    float axis_right_y;
    float trigger_left;
    float trigger_right;
    char connected;
} Gamepad;

int gamepad_init(void);
void gamepad_update(Gamepad pads[GAMEPAD_MAX]);
void gamepad_shutdown(void);


#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDDevice.h>

typedef struct {
    IOHIDDeviceRef device;
    uint8_t buttons[GAMEPAD_BUTTON_COUNT];
    float axes[6];  // left_x, left_y, right_x, right_y, trigger_l, trigger_r
} GamepadDevice;

static IOHIDManagerRef hid_manager = NULL;
static GamepadDevice devices[GAMEPAD_MAX];

static void handle_input_value(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
    if (result != kIOReturnSuccess) return;

    IOHIDElementRef element = IOHIDValueGetElement(value);
    IOHIDDeviceRef device = IOHIDElementGetDevice(element);  // This is the correct function
    uint32_t usage_page = IOHIDElementGetUsagePage(element);
    uint32_t usage = IOHIDElementGetUsage(element);
    int value_int = IOHIDValueGetIntegerValue(value);

    // Find which index this device corresponds to
    int device_index = -1;
    for (int i = 0; i < GAMEPAD_MAX; i++) {
        if (devices[i].device == device) {
            device_index = i;
            break;
        }
    }
    if (device_index == -1) return;

    if (usage_page == kHIDPage_Button) {
        // Button inputs
        if (usage > 0 && usage <= GAMEPAD_BUTTON_COUNT) {
            devices[device_index].buttons[usage - 1] = value_int != 0;
        }
    } else if (usage_page == kHIDPage_GenericDesktop) {
        float normalized_value;
        switch (usage) {
            case kHIDUsage_GD_X:
                normalized_value = (float)(value_int - 128) / 128.0f;
                devices[device_index].axes[0] = normalized_value;
                break;
            case kHIDUsage_GD_Y:
                normalized_value = (float)(value_int - 128) / 128.0f;
                devices[device_index].axes[1] = normalized_value;
                break;
            case kHIDUsage_GD_Z:
                normalized_value = (float)(value_int - 128) / 128.0f;
                devices[device_index].axes[2] = normalized_value;
                break;
            case kHIDUsage_GD_Rz:
                normalized_value = (float)(value_int - 128) / 128.0f;
                devices[device_index].axes[3] = normalized_value;
                break;
        }
    }
}

static void handle_device_matching(void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
    int index = -1;
    for (int i = 0; i < GAMEPAD_MAX; i++) {
        if (devices[i].device == NULL) {
            index = i;
            break;
        }
    }
    if (index == -1) return;

    devices[index].device = device;
    IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
    IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    IOHIDDeviceRegisterInputValueCallback(device, handle_input_value, NULL);
}

static void handle_device_removal(void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
    for (int i = 0; i < GAMEPAD_MAX; i++) {
        if (devices[i].device == device) {
            IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
            devices[i].device = NULL;
            memset(&devices[i], 0, sizeof(GamepadDevice));
        }
    }
}

int gamepad_init(void) {
    memset(devices, 0, sizeof(devices));

    hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!hid_manager) return 0;

    CFDictionaryRef matching_dict = NULL;
    CFNumberRef usage_page_ref = NULL;
    CFNumberRef usage_ref = NULL;

    int usage_page = kHIDPage_GenericDesktop;
    int usage = kHIDUsage_GD_GamePad;

    usage_page_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage_page);
    usage_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);

    const void* keys[] = {
        CFSTR(kIOHIDDeviceUsagePageKey),
        CFSTR(kIOHIDDeviceUsageKey)
    };
    const void* values[] = {
        usage_page_ref,
        usage_ref
    };

    matching_dict = CFDictionaryCreate(kCFAllocatorDefault,
                                     keys, values, 2,
                                     &kCFTypeDictionaryKeyCallBacks,
                                     &kCFTypeDictionaryValueCallBacks);

    IOHIDManagerSetDeviceMatching(hid_manager, matching_dict);

    IOHIDManagerRegisterDeviceMatchingCallback(hid_manager, handle_device_matching, NULL);
    IOHIDManagerRegisterDeviceRemovalCallback(hid_manager, handle_device_removal, NULL);

    IOHIDManagerScheduleWithRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);

    CFRelease(matching_dict);
    CFRelease(usage_page_ref);
    CFRelease(usage_ref);

    return 1;
}

void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);

    for (int i = 0; i < GAMEPAD_MAX; i++) {
        memset(&pads[i], 0, sizeof(Gamepad));

        if (devices[i].device) {
            pads[i].connected = 1;
            memcpy(pads[i].buttons, devices[i].buttons, GAMEPAD_BUTTON_COUNT);

            pads[i].axis_left_x = devices[i].axes[0];
            pads[i].axis_left_y = devices[i].axes[1];
            pads[i].axis_right_x = devices[i].axes[2];
            pads[i].axis_right_y = devices[i].axes[3];
            pads[i].trigger_left = devices[i].axes[4];
            pads[i].trigger_right = devices[i].axes[5];
        }
    }
}

void gamepad_shutdown(void) {
    if (hid_manager) {
        for (int i = 0; i < GAMEPAD_MAX; i++) {
            if (devices[i].device) {
                IOHIDDeviceClose(devices[i].device, kIOHIDOptionsTypeNone);
            }
        }

        IOHIDManagerUnscheduleFromRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        IOHIDManagerClose(hid_manager, kIOHIDOptionsTypeNone);
        CFRelease(hid_manager);
        hid_manager = NULL;
    }
}
#endif // apple



#if defined(__linux__)
    #include <linux/input.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <dirent.h>
    #include <stdio.h>

    static int ev_fds[GAMEPAD_MAX] = {-1, -1, -1, -1};

    // Helper function to find gamepad devices
    static int is_gamepad(const char* device_path) {
        int fd = open(device_path, O_RDONLY);
        if (fd == -1) return 0;

        unsigned char evtype_bitmask[EV_MAX/8 + 1];
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask) != -1) {
            // Check if device has key and absolute axis events (typical for gamepads)
            if (evtype_bitmask[EV_KEY/8] & (1<<(EV_KEY%8)) &&
                evtype_bitmask[EV_ABS/8] & (1<<(EV_ABS%8))) {
                close(fd);
                return 1;
            }
        }
        close(fd);
        return 0;
    }

    int gamepad_init(void) {
        int found = 0;
        DIR* dir;
        struct dirent* ent;
        char path[256];

        dir = opendir("/dev/input");
        if (dir == NULL) return 0;

        int pad_index = 0;
        while ((ent = readdir(dir)) != NULL && pad_index < GAMEPAD_MAX) {
            if (strncmp(ent->d_name, "event", 5) == 0) {
                snprintf(path, sizeof(path), "/dev/input/%s", ent->d_name);
                if (is_gamepad(path)) {
                    ev_fds[pad_index] = open(path, O_RDONLY | O_NONBLOCK);
                    if (ev_fds[pad_index] != -1) {
                        printf("Gamepad found at %s\n", path);
                        found = 1;
                        pad_index++;
                    }
                }
            }
        }
        closedir(dir);
        return found;
    }

    void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
        struct input_event ev;

        for (int i = 0; i < GAMEPAD_MAX; i++) {
            if (ev_fds[i] == -1) {
                pads[i].connected = 0;
                continue;
            }

            pads[i].connected = 1;

            while (read(ev_fds[i], &ev, sizeof(ev)) > 0) {
                printf("Event: type=%d, code=%d, value=%d\n",
                       ev.type, ev.code, ev.value);

                switch (ev.type) {
                    case EV_KEY:
                        switch (ev.code) {
                            // Note A/B are swapped!
                            case BTN_B:
                                pads[i].buttons[GAMEPAD_BUTTON_A] = ev.value;
                                printf("A button: %d\n", ev.value);
                                break;
                            case BTN_A:
                                pads[i].buttons[GAMEPAD_BUTTON_B] = ev.value;
                                printf("B button: %d\n", ev.value);
                                break;
                            case BTN_X:
                                pads[i].buttons[GAMEPAD_BUTTON_X] = ev.value;
                                printf("X button: %d\n", ev.value);
                                break;
                            case BTN_Y:
                                pads[i].buttons[GAMEPAD_BUTTON_Y] = ev.value;
                                printf("Y button: %d\n", ev.value);
                                break;
                        }
                        break;

                    case EV_ABS:
                        switch (ev.code) {
                            case ABS_X:
                                pads[i].axis_left_x = ev.value / 32767.0f;
                                break;
                            case ABS_Y:
                                pads[i].axis_left_y = ev.value / 32767.0f;
                                break;
                            case ABS_RX:
                                pads[i].axis_right_x = ev.value / 32767.0f;
                                break;
                            case ABS_RY:
                                pads[i].axis_right_y = ev.value / 32767.0f;
                                break;
                            case ABS_Z:
                                pads[i].trigger_left = ev.value / 255.0f;
                                break;
                            case ABS_RZ:
                                pads[i].trigger_right = ev.value / 255.0f;
                                break;
                        }
                        break;
                }
            }
        }
    }

    void gamepad_shutdown(void) {
        for (int i = 0; i < GAMEPAD_MAX; i++) {
            if (ev_fds[i] != -1) {
                close(ev_fds[i]);
                ev_fds[i] = -1;
            }
        }
    }
#endif // linux




#ifdef _WIN32
#include <windows.h>
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

static XINPUT_STATE states[GAMEPAD_MAX];

int gamepad_init(void) {
    ZeroMemory(states, sizeof(states));
    return 1;
}

void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
    for (DWORD i = 0; i < GAMEPAD_MAX; i++) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        // Clear the gamepad state
        memset(&pads[i], 0, sizeof(Gamepad));

        if (XInputGetState(i, &state) == ERROR_SUCCESS) {
            pads[i].connected = 1;

            // Buttons
            WORD buttons = state.Gamepad.wButtons;
            pads[i].buttons[GAMEPAD_BUTTON_A] = (buttons & XINPUT_GAMEPAD_A) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_B] = (buttons & XINPUT_GAMEPAD_B) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_X] = (buttons & XINPUT_GAMEPAD_X) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_Y] = (buttons & XINPUT_GAMEPAD_Y) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_UP] = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_DOWN] = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_LEFT] = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_RIGHT] = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_START] = (buttons & XINPUT_GAMEPAD_START) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_BACK] = (buttons & XINPUT_GAMEPAD_BACK) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_LEFT_SHOULDER] = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_RIGHT_SHOULDER] = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_LEFT_THUMB] = (buttons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
            pads[i].buttons[GAMEPAD_BUTTON_RIGHT_THUMB] = (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

            // Axes (normalize from -32768 to 32767 to -1 to 1)
            pads[i].axis_left_x = state.Gamepad.sThumbLX / 32768.0f;
            pads[i].axis_left_y = -state.Gamepad.sThumbLY / 32768.0f;  // Flip Y
            pads[i].axis_right_x = state.Gamepad.sThumbRX / 32768.0f;
            pads[i].axis_right_y = -state.Gamepad.sThumbRY / 32768.0f; // Flip Y

            // Triggers (normalize from 0 to 255 to 0 to 1)
            pads[i].trigger_left = state.Gamepad.bLeftTrigger / 255.0f;
            pads[i].trigger_right = state.Gamepad.bRightTrigger / 255.0f;
        }
    }
}

void gamepad_shutdown(void) {
    // Nothing needed for XInput shutdown
}
#endif // windows



#ifdef BUILD_LIBRETRO
#include <libretro.h>

static struct retro_input_state_callback input_state_cb;
static int16_t libretro_input_state(unsigned port, unsigned device, unsigned index, unsigned id);

int gamepad_init(void) {
    // LibRetro cores don't need explicit initialization
    return 1;
}

void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
    for (int i = 0; i < GAMEPAD_MAX; i++) {
        // Clear the gamepad state
        memset(&pads[i], 0, sizeof(Gamepad));

        // Check if controller is connected (any button pressed)
        int16_t any_input = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
        pads[i].connected = (any_input != 0);

        if (pads[i].connected) {
            // Buttons
            pads[i].buttons[GAMEPAD_BUTTON_A] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
            pads[i].buttons[GAMEPAD_BUTTON_B] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
            pads[i].buttons[GAMEPAD_BUTTON_X] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
            pads[i].buttons[GAMEPAD_BUTTON_Y] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
            pads[i].buttons[GAMEPAD_BUTTON_UP] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
            pads[i].buttons[GAMEPAD_BUTTON_DOWN] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
            pads[i].buttons[GAMEPAD_BUTTON_LEFT] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
            pads[i].buttons[GAMEPAD_BUTTON_RIGHT] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
            pads[i].buttons[GAMEPAD_BUTTON_START] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
            pads[i].buttons[GAMEPAD_BUTTON_BACK] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
            pads[i].buttons[GAMEPAD_BUTTON_LEFT_SHOULDER] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
            pads[i].buttons[GAMEPAD_BUTTON_RIGHT_SHOULDER] = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);

            // Analog sticks (normalize from -32768 to 32767 to -1 to 1)
            int16_t lx = input_state_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
            int16_t ly = input_state_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
            int16_t rx = input_state_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
            int16_t ry = input_state_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

            pads[i].axis_left_x = lx / 32768.0f;
            pads[i].axis_left_y = ly / 32768.0f;
            pads[i].axis_right_x = rx / 32768.0f;
            pads[i].axis_right_y = ry / 32768.0f;

            // Triggers (L2/R2)
            pads[i].trigger_left = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2) / 32768.0f;
            pads[i].trigger_right = input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2) / 32768.0f;
        }
    }
}

void gamepad_shutdown(void) {
    // Nothing needed for libretro shutdown
}
#endif // BUILD_LIBRETRO



#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>

static const char* gamepadId[GAMEPAD_MAX] = {NULL};
static int numGamepads = 0;

EM_BOOL gamepad_callback(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData) {
    if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED) {
        if (numGamepads < GAMEPAD_MAX) {
            gamepadId[numGamepads++] = strdup(gamepadEvent->id);
            printf("Gamepad connected: %s\n", gamepadEvent->id);
        }
    }
    else if (eventType == EMSCRIPTEN_EVENT_GAMEPADDISCONNECTED) {
        for (int i = 0; i < numGamepads; i++) {
            if (gamepadId[i] && strcmp(gamepadId[i], gamepadEvent->id) == 0) {
                free((void*)gamepadId[i]);
                gamepadId[i] = NULL;

                // Shift remaining gamepads
                for (int j = i; j < numGamepads - 1; j++) {
                    gamepadId[j] = gamepadId[j + 1];
                }
                numGamepads--;
                break;
            }
        }
        printf("Gamepad disconnected: %s\n", gamepadEvent->id);
    }
    return EM_TRUE;
}

int gamepad_init(void) {
    numGamepads = 0;
    memset(gamepadId, 0, sizeof(gamepadId));

    EMSCRIPTEN_RESULT result;
    result = emscripten_set_gamepadconnected_callback(NULL, EM_TRUE, gamepad_callback);
    if (result != EMSCRIPTEN_RESULT_SUCCESS) return 0;

    result = emscripten_set_gamepaddisconnected_callback(NULL, EM_TRUE, gamepad_callback);
    if (result != EMSCRIPTEN_RESULT_SUCCESS) return 0;

    return 1;
}

void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
    EmscriptenGamepadEvent gamepadState;

    for (int i = 0; i < GAMEPAD_MAX; i++) {
        memset(&pads[i], 0, sizeof(Gamepad));

        if (i < numGamepads && gamepadId[i] != NULL) {
            if (emscripten_get_gamepad_status(i, &gamepadState) == EMSCRIPTEN_RESULT_SUCCESS) {
                pads[i].connected = 1;

                // Standard mapping buttons
                pads[i].buttons[GAMEPAD_BUTTON_A] = gamepadState.digitalButton[0];
                pads[i].buttons[GAMEPAD_BUTTON_B] = gamepadState.digitalButton[1];
                pads[i].buttons[GAMEPAD_BUTTON_X] = gamepadState.digitalButton[2];
                pads[i].buttons[GAMEPAD_BUTTON_Y] = gamepadState.digitalButton[3];
                pads[i].buttons[GAMEPAD_BUTTON_LEFT_SHOULDER] = gamepadState.digitalButton[4];
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT_SHOULDER] = gamepadState.digitalButton[5];
                pads[i].buttons[GAMEPAD_BUTTON_BACK] = gamepadState.digitalButton[8];
                pads[i].buttons[GAMEPAD_BUTTON_START] = gamepadState.digitalButton[9];
                pads[i].buttons[GAMEPAD_BUTTON_LEFT_THUMB] = gamepadState.digitalButton[10];
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT_THUMB] = gamepadState.digitalButton[11];
                pads[i].buttons[GAMEPAD_BUTTON_UP] = gamepadState.digitalButton[12];
                pads[i].buttons[GAMEPAD_BUTTON_DOWN] = gamepadState.digitalButton[13];
                pads[i].buttons[GAMEPAD_BUTTON_LEFT] = gamepadState.digitalButton[14];
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT] = gamepadState.digitalButton[15];

                // Axes
                pads[i].axis_left_x = gamepadState.axis[0];
                pads[i].axis_left_y = gamepadState.axis[1];
                pads[i].axis_right_x = gamepadState.axis[2];
                pads[i].axis_right_y = gamepadState.axis[3];

                // Triggers (axes 6 and 7 in standard mapping)
                pads[i].trigger_left = gamepadState.analogButton[6];
                pads[i].trigger_right = gamepadState.analogButton[7];
            }
        }
    }
}

void gamepad_shutdown(void) {
    emscripten_set_gamepadconnected_callback(NULL, EM_FALSE, NULL);
    emscripten_set_gamepaddisconnected_callback(NULL, EM_FALSE, NULL);

    for (int i = 0; i < GAMEPAD_MAX; i++) {
        if (gamepadId[i]) {
            free((void*)gamepadId[i]);
            gamepadId[i] = NULL;
        }
    }
    numGamepads = 0;
}
#endif // EMSCRIPTEN


#endif // GAMEPAD_H
