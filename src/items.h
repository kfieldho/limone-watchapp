#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

#define TODOIST_TOKEN 0

#define ITEM_ID 8
#define ITEM_NAME 9

#define MAX_ITEMS_LENGTH 16
#define MAX_TITLE_LENGTH 28

#define TODOIST_TOKEN_LENGTH 41

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
