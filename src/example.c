#include "pntr_shell.h"

int main() {
    // set to true to quit
    bool exit = false;

    pntr_image* screen = pntr_new_image(200, 200);
    pntr_audio_engine* se = pntr_audio_init();
    pntr_window* window = pntr_window_init(screen, "my game");

    while(pntr_window_keep_going(window) && !exit) {
        pntr_clear_background(screen, PNTR_BLACK);
        pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);

        pntr_window_update(window, screen);
        pntr_audio_update(se);
    }

    pntr_unload_image(screen);
    pntr_window_unload(window);
    pntr_audio_unload(se);
    return 0;
}
