#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

#define FETCH_ITEMS 0

#define ITEM_ID 1
#define ITEM_NAME 2

#define MAX_ITEMS_LENGTH 32
#define MAX_TITLE_LENGTH 60

typedef struct {
  uint32_t id;
  char name[MAX_TITLE_LENGTH];
} Item;

void create_item_window();
void destroy_item_window();
void show_items();

#endif /* ITEMS_H_INCLUDED */
