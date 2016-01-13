#include <pebble.h>
#include "track.h"
#include "items.h"
#include "message.h"

char title[MAX_TITLE_LENGTH];

static void init(void) {
  strcpy(title, "Task");
  create_track_window();
  create_item_window();
  init_app_message();
}

static void deinit(void) {
  destroy_item_window();
  destroy_track_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
