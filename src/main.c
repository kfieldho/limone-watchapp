#include <pebble.h>
#include "track.h"

static void init(void) {
  create_track_window();
}

static void deinit(void) {
  destroy_track_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
