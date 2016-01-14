#include <pebble.h>
#include "track.h"
#include "items.h"

static void received_handler(DictionaryIterator *iter, void *context) {
  update_ifttt(iter);
  update_items(iter);
  update_todoist(iter);
}

void init_app_message() {
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_received(received_handler);
}
