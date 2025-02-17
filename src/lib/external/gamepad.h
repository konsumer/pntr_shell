#ifndef GAMEPAD_H
#define GAMEPAD_H

#ifdef __cplusplus
extern "C" {
#endif

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

#if defined(_WIN32)
    #include <windows.h>
    #include <xinput.h>
    #pragma comment(lib, "xinput.lib")

    int gamepad_init(void) {
        return 1;
    }

    void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
        for (int i = 0; i < GAMEPAD_MAX; i++) {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            if (XInputGetState(i, &state) == ERROR_SUCCESS) {
                pads[i].connected = 1;

                // Sticks
                pads[i].axis_left_x = state.Gamepad.sThumbLX / 32768.0f;
                pads[i].axis_left_y = state.Gamepad.sThumbLY / 32768.0f;
                pads[i].axis_right_x = state.Gamepad.sThumbRX / 32768.0f;
                pads[i].axis_right_y = state.Gamepad.sThumbRY / 32768.0f;

                // Triggers
                pads[i].trigger_left = state.Gamepad.bLeftTrigger / 255.0f;
                pads[i].trigger_right = state.Gamepad.bRightTrigger / 255.0f;

                // Buttons
                pads[i].buttons[GAMEPAD_BUTTON_A] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_B] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_X] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_Y] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_UP] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_DOWN] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_LEFT] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_START] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_BACK] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_LEFT_SHOULDER] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT_SHOULDER] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_LEFT_THUMB] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
                pads[i].buttons[GAMEPAD_BUTTON_RIGHT_THUMB] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
            }
            else {
                pads[i].connected = 0;
                memset(&pads[i], 0, sizeof(Gamepad));
            }
        }
    }
#endif // windows


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

#elif defined(__APPLE__)
    #include <IOKit/hid/IOHIDManager.h>
    #include <IOKit/hid/IOHIDKeys.h>

    static IOHIDManagerRef hid_manager = NULL;
    static IOHIDDeviceRef devices[GAMEPAD_MAX] = {0};
    static CFDictionaryRef device_dictionaries[GAMEPAD_MAX] = {0};

    // Temporary storage for async input data
    static struct {
        Gamepad* gamepad;
        int index;
    } gamepad_state[GAMEPAD_MAX] = {0};

    static void gamepad_input_callback(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
        if (result != kIOReturnSuccess) return;

        IOHIDElementRef element = IOHIDValueGetElement(value);
        uint32_t usage_page = IOHIDElementGetUsagePage(element);
        uint32_t usage = IOHIDElementGetUsage(element);
        int32_t button_state = IOHIDValueGetIntegerValue(value);

        struct { Gamepad* gamepad; int index; }* state = (struct { Gamepad* gamepad; int index; }*)context;
        Gamepad* gamepad = state->gamepad;

        if (usage_page == kHIDPage_Button) {
            switch (usage) {
                case 1: gamepad->buttons[GAMEPAD_BUTTON_A] = button_state; break;
                case 2: gamepad->buttons[GAMEPAD_BUTTON_B] = button_state; break;
                case 3: gamepad->buttons[GAMEPAD_BUTTON_X] = button_state; break;
                case 4: gamepad->buttons[GAMEPAD_BUTTON_Y] = button_state; break;
                case 5: gamepad->buttons[GAMEPAD_BUTTON_LEFT_SHOULDER] = button_state; break;
                case 6: gamepad->buttons[GAMEPAD_BUTTON_RIGHT_SHOULDER] = button_state; break;
                case 7: gamepad->buttons[GAMEPAD_BUTTON_BACK] = button_state; break;
                case 8: gamepad->buttons[GAMEPAD_BUTTON_START] = button_state; break;
                case 9: gamepad->buttons[GAMEPAD_BUTTON_LEFT_THUMB] = button_state; break;
                case 10: gamepad->buttons[GAMEPAD_BUTTON_RIGHT_THUMB] = button_state; break;
            }
        }
        else if (usage_page == kHIDPage_GenericDesktop) {
            float normalized_value = (float)(IOHIDValueGetIntegerValue(value) - IOHIDElementGetLogicalMin(element)) /
                                   (float)(IOHIDElementGetLogicalMax(element) - IOHIDElementGetLogicalMin(element));
            normalized_value = normalized_value * 2.0f - 1.0f; // Convert to -1.0 to 1.0 range

            switch (usage) {
                case kHIDUsage_GD_X: gamepad->axis_left_x = normalized_value; break;
                case kHIDUsage_GD_Y: gamepad->axis_left_y = normalized_value; break;
                case kHIDUsage_GD_Z: gamepad->axis_right_x = normalized_value; break;
                case kHIDUsage_GD_Rz: gamepad->axis_right_y = normalized_value; break;
                case kHIDUsage_GD_Rx: gamepad->trigger_left = (normalized_value + 1.0f) / 2.0f; break;
                case kHIDUsage_GD_Ry: gamepad->trigger_right = (normalized_value + 1.0f) / 2.0f; break;
                case kHIDUsage_GD_DPadUp: gamepad->buttons[GAMEPAD_BUTTON_UP] = normalized_value > 0.5f; break;
                case kHIDUsage_GD_DPadDown: gamepad->buttons[GAMEPAD_BUTTON_DOWN] = normalized_value > 0.5f; break;
                case kHIDUsage_GD_DPadLeft: gamepad->buttons[GAMEPAD_BUTTON_LEFT] = normalized_value > 0.5f; break;
                case kHIDUsage_GD_DPadRight: gamepad->buttons[GAMEPAD_BUTTON_RIGHT] = normalized_value > 0.5f; break;
            }
        }
    }

    static void gamepad_device_matching_callback(void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
        if (result != kIOReturnSuccess) return;

        for (int i = 0; i < GAMEPAD_MAX; i++) {
            if (devices[i] == NULL) {
                devices[i] = device;
                CFRetain(device);

                gamepad_state[i].index = i;
                gamepad_state[i].gamepad = (Gamepad*)context + i;

                IOHIDDeviceRegisterInputValueCallback(device, gamepad_input_callback, &gamepad_state[i]);
                IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                break;
            }
        }
    }

    int gamepad_init(void) {
        hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
        if (!hid_manager) return 0;

        IOHIDManagerSetDeviceMatching(hid_manager, NULL);
        IOHIDManagerRegisterDeviceMatchingCallback(hid_manager, gamepad_device_matching_callback, NULL);
        IOHIDManagerScheduleWithRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);

        return 1;
    }

    void gamepad_update(Gamepad pads[GAMEPAD_MAX]) {
        for (int i = 0; i < GAMEPAD_MAX; i++) {
            pads[i].connected = (devices[i] != NULL);
            if (!pads[i].connected) {
                memset(&pads[i], 0, sizeof(Gamepad));
            }
        }

        // Process events
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
    }

    void gamepad_shutdown(void) {
        if (hid_manager) {
            for (int i = 0; i < GAMEPAD_MAX; i++) {
                if (devices[i]) {
                    IOHIDDeviceUnscheduleFromRunLoop(devices[i], CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                    CFRelease(devices[i]);
                    devices[i] = NULL;
                }
            }

            IOHIDManagerUnscheduleFromRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            IOHIDManagerClose(hid_manager, kIOHIDOptionsTypeNone);
            CFRelease(hid_manager);
            hid_manager = NULL;
        }
    }
#endif // apple

#ifdef __cplusplus
}
#endif // GAMEPAD_H
