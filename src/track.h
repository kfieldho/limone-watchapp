#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#define WAKEUP_REASON 0
#define PERSIST_WAKEUP_ID 41
#define PERSIST_REMAINING 43
#define PERSIST_STATE 47
#define PERSIST_TITLE 53
#define PERSIST_FROM 59

#define TITLE 16
#define FROM 17
#define TO 18

#define STARTED 24
#define CANCELED 25
#define FINISHED 26

typedef enum { NOTHING, WORKING, BREAKING, PAUSING } State;

void create_track_window();
void destroy_track_window();
void start_track();

#endif /* TRACK_H_INCLUDED */
