#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED
#include <pebble.h>
#include "constant.h"

typedef enum { NOTHING, WORKING, BREAKING, PAUSING } State;

void update_ifttt(DictionaryIterator *iter);

void create_track_window();
void destroy_track_window();
void start_track();

#endif /* TRACK_H_INCLUDED */
