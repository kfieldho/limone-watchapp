#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED
#include <pebble.h>
#include "constant.h"

typedef struct {
  uint32_t id;
  char name[MAX_TITLE_LENGTH];
} Item;

void update_todoist(DictionaryIterator *iter);
void update_items();
void create_item_window();
void destroy_item_window();
void show_items();

#endif /* ITEMS_H_INCLUDED */
