#include "pntr_shell.h"

int main() {
    // set to true to quit
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_audio_engine* se = pntr_audio_init();
    pntr_window* window = pntr_screen_init(screen, "my game");

    while(pntr_keep_going(window) && !exit) {
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);

        pntr_screen_update(window, screen);
        pntr_audio_update(se);
    }

    pntr_unload_image(screen);
    pntr_screen_unload(window);
    pntr_audio_unload(se);
    return 0;
}