#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
#include <pebble.h>

void store_string(DictionaryIterator *iter, uint32_t key);
int read_string(uint32_t key, char* buffer, int buffer_size);

#endif /* UTIL_H_INCLUDED */
