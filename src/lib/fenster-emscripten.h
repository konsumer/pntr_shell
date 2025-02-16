// this is an emscripten-only implementation of fenster

#include "emscripten.h"

struct fenster {
  const int width;
  const int height;
  uint32_t *buf;
  int keys[256]; /* keys are mostly ASCII, but arrows are 17..20 */
  int mod;       /* mod is 4 bits mask, ctrl=1, shift=2, alt=4, meta=8 */
  int x;
  int y;
  int mouse;
  const char *title; // I put it at the end, so everything else is fixed-length
};

static bool running = true;

EM_JS(int, fenster_open, (struct fenster *f), {
  if (!Module.canvas) {
    Module.canvas = document.createElement("canvas");
    document.appendElement(Module.canvas);
  }
  const width = Module.HEAP32[f/4];
  const height = Module.HEAP32[(f/4) + 1];
  Module.canvas.width = width;
  Module.canvas.height = height;
  Module.ctx = canvas.getContext("2d");
  Module.screen = Module.ctx.getImageData(0, 0, width, height);
  Module.keys = {};
  Module.mod = 0;
  Module.x = 0;
  Module.y = 0;
  Module.mouse = 0;

  // set alpha to 100%
  const pmax = f + 8 + width * height * 4;
  for (let p=f+8;p < pmax;p+=4) {
    Module.HEAPU8[p+3] = 0xFF;
  }

  for (let i=0;i<256;i++) {
    Module.keys[i] = 0;
  }
  // TODO: add event-callbacks to set things
  console.log(Module);
});

EM_ASYNC_JS(void, emscripten_fenster_loop, (struct fenster *f), {
  const bufSize = Module.canvas.width*Module.canvas.height*4;
  Module.screen.data.set(Module.HEAPU8.slice(f+8, f+8+bufSize));
  Module.ctx.putImageData(Module.screen, 0, 0);
  // TODO: update keys. mod, x, y, mouse

  // this is hack to make it not pin the main-loop (~60fps)
  await new Promise((resolve, reject) => setTimeout(resolve, 16.6));
});

int fenster_loop(struct fenster *f) {
  if (running) {
    emscripten_fenster_loop(f);
  }
  return running ? 1 : 0;
}

void fenster_close(struct fenster *f) {
  running = false;
}

EM_ASYNC_JS(void, fenster_sleep, (int64_t ms), {
  await new Promise((resolve, reject) => setTimeout(resolve, ms));
});

EM_JS(int64_t, fenster_time, (void), {
  return (new Date()).time;
});
