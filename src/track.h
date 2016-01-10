#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#define WAKEUP_REASON 0
#define PERSIST_WAKEUP_ID 41
#define PERSIST_REMAINING 43
#define PERSIST_STATE 47

#define STARTED 3
#define ENDED 4
#define TITLE 5

typedef enum { NOTHING, WORKING, BREAKING, PAUSING } State;

void create_track_window();
void destroy_track_window();
void start_track();

#endif /* TRACK_H_INCLUDED */
