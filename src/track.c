#include <pebble.h>
#include "track.h"
#include "items.h"

extern char title[64];

static Window *s_window;
static Layer *s_layer;
static ActionBarLayer *s_actionbar;
static StatusBarLayer *s_statusbar;
static TextLayer *s_time_layer;
static GBitmap *s_icon_stop, *s_icon_start, *s_icon_pause, *s_icon_menu;
static char s_buffer[32];

static State s_state;
static WakeupId s_wakeup_id;

static char s_started[26], s_ended[26];

static void update_timer() {
  switch(s_state) {
    case NOTHING:
      text_layer_set_text(s_time_layer, "25:00");
      return;
    case PAUSING:
      break;
    case WORKING:
      break;
    case BREAKING:
      break;
  }
  if (s_wakeup_id == 0) {
    s_wakeup_id = persist_read_int(PERSIST_WAKEUP_ID);
  }
  time_t wakeup_timestamp = 0;
  if (wakeup_query(s_wakeup_id, &wakeup_timestamp)) {
    int remaining = wakeup_timestamp - time(NULL);
    int minutes = remaining / 60;
    int seconds = remaining % 60;
    snprintf(s_buffer, sizeof(s_buffer), "%02d:%02d", minutes, seconds);
    text_layer_set_text(s_time_layer, s_buffer);
  }
}

static void post_ifttt() {
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  char buffer[26];
  if (dict_write_cstring(iter, STARTED, s_started) != DICT_OK) {
    return;
  }
  if (dict_write_cstring(iter, ENDED, s_ended) != DICT_OK) {
    return;
  }
  if (dict_write_cstring(iter, TITLE, title) != DICT_OK) {
    return;
  }
  app_message_outbox_send();
}

static void start_work() {
  time_t now = time(NULL);
  if (s_state == NOTHING) {
    struct tm *time_info = localtime(&now);
    strftime(s_started, 26, "%m/%d %H:%M", time_info);
  }
  time_t future_time = now + 1500;
  s_wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true);
  persist_write_int(PERSIST_WAKEUP_ID, s_wakeup_id);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_pause);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, NULL);

  s_state = WORKING;
  persist_write_int(PERSIST_STATE, s_state);
}

static void stop_work() {
  persist_delete(PERSIST_WAKEUP_ID);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_pause);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);

  persist_write_int(PERSIST_STATE, s_state);
  time_t now = time(NULL);
  struct tm *time_info = localtime(&now);
  strftime(s_ended, 26, "%m/%d %H:%M", time_info);

  post_ifttt();
}

static void pause_work() {
  s_wakeup_id = persist_read_int(PERSIST_WAKEUP_ID);
  time_t wakeup_timestamp = 0;
  if (wakeup_query(s_wakeup_id, &wakeup_timestamp)) {
    int remaining = wakeup_timestamp - time(NULL);
    persist_write_int(PERSIST_REMAINING, remaining);
  }

  wakeup_cancel(s_wakeup_id);
  persist_delete(PERSIST_WAKEUP_ID);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_start);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, s_icon_menu);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);

  s_state = PAUSING;
  persist_write_int(PERSIST_STATE, s_state);
}

static void resume_work() {
  int remaining = persist_read_int(PERSIST_REMAINING);
  persist_delete(PERSIST_REMAINING);
  time_t future_time = time(NULL) + remaining;
  s_wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true);
  persist_write_int(PERSIST_WAKEUP_ID, s_wakeup_id);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_pause);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);

  s_state = WORKING;
  persist_write_int(PERSIST_STATE, s_state);
}

static void start_break() {
  time_t future_time = time(NULL) + 300;
  s_wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true);
  persist_write_int(PERSIST_WAKEUP_ID, s_wakeup_id);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, NULL);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);

  s_state = BREAKING;
  persist_write_int(PERSIST_STATE, s_state);
}

static void stop_break() {
  persist_delete(PERSIST_WAKEUP_ID);

  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_start);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, s_icon_menu);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, NULL);

  s_state = NOTHING;
  persist_write_int(PERSIST_STATE, s_state);
}

static void selectclick_handler(ClickRecognizerRef recognizer, void *context) {
  switch(s_state) {
    case NOTHING:
      start_work();
      break;
    case WORKING:
      pause_work();
      break;
    case PAUSING:
      resume_work();
      break;
    case BREAKING:
      break;
  }
  update_timer();
}

static void upclick_handler(ClickRecognizerRef recognizer, void *context) {
  switch(s_state) {
    case NOTHING:
      show_items();
      break;
    case WORKING:
      break;
    case PAUSING:
      show_items();
      break;
    case BREAKING:
      break;
  }
}

static void downclick_handler(ClickRecognizerRef recognizer, void *context) {
  switch(s_state) {
    case NOTHING:
      break;
    case WORKING:
      wakeup_cancel(s_wakeup_id);
      stop_work();
      start_break();
      wakeup_cancel(s_wakeup_id);
      stop_break();
      break;
    case PAUSING:
      resume_work();
      wakeup_cancel(s_wakeup_id);
      stop_work();
      start_break();
      wakeup_cancel(s_wakeup_id);
      stop_break();
      break;
    case BREAKING:
      wakeup_cancel(s_wakeup_id);
      stop_break();
      break;
  }
  wakeup_cancel_all();
  update_timer();
}

static void wakeup_handler(WakeupId id, int32_t reason) {
  switch(s_state) {
    case NOTHING:
      break;
    case WORKING:
      stop_work();
      vibes_short_pulse();
      start_break();
      break;
    case PAUSING:
      break;
    case BREAKING:
      stop_break();
      vibes_long_pulse();
      break;
  }
}

static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, upclick_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, selectclick_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, downclick_handler);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_timer();
}

static void update_display(Layer *layer, GContext *ctx) {
  int remaining;
  time_t wakeup_timestamp = 0;
  if (wakeup_query(s_wakeup_id, &wakeup_timestamp)) {
    remaining = wakeup_timestamp - time(NULL);
  }
  else {
    remaining = persist_read_int(PERSIST_REMAINING);
  }
  GColor color;
  switch(s_state) {
    case NOTHING:
      return;
    case PAUSING:
      remaining = persist_read_int(PERSIST_REMAINING);
      color = PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorLightGray);
      break;
    case WORKING:
      color = PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorLightGray);
      break;
    case BREAKING:
      color = PBL_IF_COLOR_ELSE(GColorYellow, GColorLightGray);
      break;
  }
  int angle = TRIG_MAX_ANGLE - remaining * (TRIG_MAX_ANGLE / ((s_state == BREAKING)? 300: 1500));
  GRect rect = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle, 90, angle, TRIG_MAX_ANGLE);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_layer = layer_create(bounds);
  layer_add_child(window_layer, s_layer);

  s_statusbar = status_bar_layer_create();
  status_bar_layer_set_colors(s_statusbar, GColorClear, GColorBlack);
  layer_add_child(window_layer, status_bar_layer_get_layer(s_statusbar));

  s_actionbar = action_bar_layer_create();
  action_bar_layer_set_background_color(s_actionbar, GColorClear);
  action_bar_layer_add_to_window(s_actionbar, window);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, s_icon_menu);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, s_icon_stop);
  action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_start);
  action_bar_layer_set_click_config_provider(s_actionbar, config_provider);

  s_time_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(55, 72, 70, 30), GRect(39, 67, 66, 30)));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_background_color(s_time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  wakeup_service_subscribe(wakeup_handler);

  layer_set_update_proc(s_layer, update_display);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void window_appear(Window *window) {
  s_state = persist_read_int(PERSIST_STATE);
  int remaining, minutes, seconds;
  switch(s_state) {
    case NOTHING:
      action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, s_icon_menu);
      action_bar_layer_set_icon(s_actionbar, BUTTON_ID_DOWN, NULL);
      break;
    case WORKING:
      action_bar_layer_set_icon(s_actionbar, BUTTON_ID_UP, NULL);
      action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, s_icon_pause);
      break;
    case PAUSING:
      remaining = persist_read_int(PERSIST_REMAINING);
      minutes = remaining / 60;
      seconds = remaining % 60;
      snprintf(s_buffer, sizeof(s_buffer), "%02d:%02d", minutes, seconds);
      text_layer_set_text(s_time_layer, s_buffer);
      break;
    case BREAKING:
      action_bar_layer_set_icon(s_actionbar, BUTTON_ID_SELECT, NULL);
      break;
  }
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    WakeupId id = 0;
    int32_t reason = 0;
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  }
  else {
    s_wakeup_id = persist_read_int(PERSIST_WAKEUP_ID);
  }
  update_timer();
}

static void window_unload(Window *window) {
  action_bar_layer_destroy(s_actionbar);
  text_layer_destroy(s_time_layer);
  layer_destroy(s_layer);
}

void create_track_window() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      .appear = window_appear,
  });

  s_icon_start = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_START);
  s_icon_stop = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_STOP);
  s_icon_pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PAUSE);
  s_icon_menu = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MENU);

  window_stack_push(s_window, false);
}

void destroy_track_window() {
  gbitmap_destroy(s_icon_menu);
  gbitmap_destroy(s_icon_pause);
  gbitmap_destroy(s_icon_stop);
  gbitmap_destroy(s_icon_start);

  window_destroy(s_window);
}
