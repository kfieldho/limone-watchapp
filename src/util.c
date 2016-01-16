#include "util.h"

void store_string(DictionaryIterator *iter, uint32_t key) {
  Tuple *tuple = dict_find(iter, key);
  if (tuple) {
    persist_write_string(key, tuple->value->cstring);
  }
}

int read_string(uint32_t key, char* buffer, int buffer_size) {
  int written = persist_read_string(key, buffer, buffer_size);
  if (written >= buffer_size * (int)sizeof(char))
    buffer[buffer_size - 1] = '\0';
  return written;
}
