// this is an emscripten-only implementation of fenster

#include "emscripten.h"

struct fenster {
  const char *title;
  const int width;
  const int height;
  uint32_t *buf;
  int keys[256]; /* keys are mostly ASCII, but arrows are 17..20 */
  int mod;       /* mod is 4 bits mask, ctrl=1, shift=2, alt=4, meta=8 */
  int x;
  int y;
  int mouse;
};

EM_JS(int, fenster_open, (struct fenster *f), {
    if (!Module.canvas) {
        Module.canvas = document.createElement("canvas");
        document.appendElement(Module.canvas);
    }
    Module.ctx = canvas.getContext("2d");
    console.log(Module);

EM_JS(int, fenster_loop, (struct fenster *f), {
    // TODO: update pixels from f->buf
    // TODO: update keys. mod, x, y, mouse
});

EM_JS(void, fenster_close, (struct fenster *f), {
});

EM_ASYNC_JS(void, fenster_sleep, (int64_t ms), {
    await new Promise((resolve, reject) => setTimeout(resolve, ms));
});

EM_JS(int64_t, fenster_time, (void), {
    return (new Date()).time;
});
