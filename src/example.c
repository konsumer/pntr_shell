#include "lib/pntr_shell.h"

static int u = 0;

void bytebeat(void *samplesPointer, unsigned int frames) {
  float *samples = (float *)samplesPointer;
  int t;
  float a;
  for (unsigned int frame = 0; frame < frames; frame++) {
    t = u++ / 8;
    // read more here: https://stellartux.github.io/websynth/guide.html
    // I am mixing left with generated sound, and outputting on left/right
    // (mono)

    a = ((t >> 10 ^ t >> 11) % 5 * ((t >> 14 & 3 ^ t >> 15 & 1) + 1) * t % 99 +
         ((3 + (t >> 14 & 3) - (t >> 16 & 1)) / 3 * t % 99 & 64) / 2048.0f) *
        0.001;

    samples[frame * 2 + 0] = (samples[frame * 2 + 0] / 2.0f) + a; // left
    samples[frame * 2 + 1] = (samples[frame * 2 + 1] / 2.0f) + a; // right
  }
}

int main() {
  // set to true to quit
  bool exit = false;

  pntr_image *screen = pntr_new_image(200, 200);
  pntr_window *window = pntr_shell_init(screen, "my game");

  pntr_sound wub = pntr_sound_load("example.mp3");
  pntr_sound_play(wub);

  pntr_sound_register(&bytebeat);

  pntr_font* font = pntr_load_font_default();

  while (pntr_keep_going(window) && !exit) {
    pntr_clear_background(screen, PNTR_BLACK);
    pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);
    pntr_draw_text(screen, font, "Press space to wub", 30, 100, PNTR_WHITE);

    if (window->keys[PNTR_APP_KEY_SPACE] || window->gamepads[0].buttons[PNTR_APP_BUTTON_A]) {
      pntr_sound_play(wub);
    }
  }

  pntr_unload_image(screen);
  pntr_shell_unload(window);
  pntr_sound_unregister(&bytebeat);
  return 0;
}
