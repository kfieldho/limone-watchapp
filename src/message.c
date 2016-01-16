#include <pebble.h>
#include "track.h"
#include "items.h"

static bool ready = false;

bool js_ready() {
  return ready;
}

void post_on_ready(DictionaryIterator *iter) {
  Tuple *tuple = dict_find(iter, JS_READY);
  if (tuple) {
    ready = true;
    if(persist_exists(IFTTT_POSTPONED)) {
      uint32_t postponed = (uint32_t) persist_read_int(IFTTT_POSTPONED);
      post_ifttt(postponed);
      persist_delete(IFTTT_POSTPONED);
    }
  }
}

static void received_handler(DictionaryIterator *iter, void *context) {
  update_ifttt(iter);
  update_items(iter);
  update_todoist(iter);
  post_on_ready(iter);
}

void init_app_message() {
  app_message_register_inbox_received(received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
