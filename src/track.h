#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#define WAKEUP_REASON 0
#define PERSIST_WAKEUP_ID 41
#define PERSIST_REMAINING 43
#define PERSIST_STATE 47
#define PERSIST_TITLE 53
#define PERSIST_FROM 59

#define TITLE 16
#define IFTTT_EVENT 17
#define IFTTT_TOKEN 18

#define IFTTT_STARTED 24
#define IFTTT_CANCELED 25
#define IFTTT_FINISHED 26

#define IFTTT_TOKEN_LENGTH 23
#define MAX_EVENT_LENGTH 32

typedef enum { NOTHING, WORKING, BREAKING, PAUSING } State;

void update_ifttt(DictionaryIterator *iter);

void create_track_window();
void destroy_track_window();
void start_track();

#endif /* TRACK_H_INCLUDED */
