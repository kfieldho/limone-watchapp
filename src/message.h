#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

void init_app_message();
void store_string(DictionaryIterator *iter, uint32_t key);
int read_string(uint32_t key, char* buffer, int buffer_size);

#endif /* MESSAGE_H_INCLUDED */
