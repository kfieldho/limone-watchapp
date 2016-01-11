#include <pebble.h>
#include "items.h"

static Window *s_window;
static MenuLayer *s_menulayer;

static Item items[MAX_ITEMS_LENGTH];
static int count = 0;

extern char title[MAX_TITLE_LENGTH];

static void add_item(Tuple *tuple_id, Tuple *tuple_name) {
  if (count == MAX_ITEMS_LENGTH) {
    return;
  }
  uint32_t id = tuple_id->value->uint32;
  for (int i = 0; i < count; ++i) {
    if (id == items[i].id) {
      return;
    }
  }
  strncpy(items[count].name, tuple_name->value->cstring, MAX_TITLE_LENGTH - 1);
  if (tuple_name->length > MAX_TITLE_LENGTH - 1) {
    *(items[count].name + MAX_TITLE_LENGTH - 1) = '\0';
  }
  items[count].id = id;
  count++;
}

static void fetch_items() {
  if (count == 0) {
    strcpy(items[count].name, "Pomodoro");
    items[count].id = 0;
    count++;
    menu_layer_reload_data(s_menulayer);
  }
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  if (dict_write_uint8(iter, FETCH_ITEMS, 1) != DICT_OK) {
    return;
  }
  app_message_outbox_send();
}

static void delete_items() {
  for (int i = 0; i < count; ++i) {
    strcpy(items[i].name, "");
    items[i].id = 0;
  }
  count = 0;
}

static uint16_t get_count_callback(struct MenuLayer *menulayer, uint16_t section_index, void *callback_context) {
  return count;
}

#ifdef PBL_ROUND
static int16_t get_cell_height_callback(MenuLayer *s_menulayer, MenuIndex *cell_index, void *callback_context) {
  return 60;
}
#endif

static void draw_row_handler(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  char* name = items[cell_index->row].name;
  menu_cell_basic_draw(ctx, cell_layer, name, NULL, NULL);
}

static void select_callback(struct MenuLayer *s_menulayer, MenuIndex *cell_index, void *callback_context) {
  strcpy(title, items[cell_index->row].name);
  window_stack_pop(false);
}

static void received_handler(DictionaryIterator *iter, void *context) {
  Tuple *tuple_id = dict_find(iter, ITEM_ID);
  Tuple *tuple_name = dict_find(iter, ITEM_NAME);
  if (tuple_id && tuple_name) {
    add_item(tuple_id, tuple_name);
    menu_layer_reload_data(s_menulayer);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menulayer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menulayer, window);
  menu_layer_set_callbacks(s_menulayer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_count_callback,
      .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback, NULL),
      .draw_row = draw_row_handler,
      .select_click = select_callback
  }); 

  layer_add_child(window_layer, menu_layer_get_layer(s_menulayer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menulayer);
}

static void window_appear(Window *window) {
  app_message_register_inbox_received(received_handler);
  fetch_items();
}

static void window_disappear(Window *window) {
  delete_items();
  app_message_register_inbox_received(NULL);
}

void create_item_window() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      .appear = window_appear,
      .disappear = window_disappear,
  });
}

void destroy_item_window() {
  window_destroy(s_window);
}

void show_items() {
  window_stack_push(s_window, false);
}
