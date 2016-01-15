#ifndef CONSTANT_H_INCLUDED
#define CONSTANT_H_INCLUDED

#define WAKEUP_REASON 0

/* keys for persistent Storage */
#define PERSIST_WAKEUP_ID 41
#define PERSIST_REMAINING 43
#define PERSIST_STATE 47


/* keys for persistent Storage and AppMessage Dictionary*/
#define TODOIST_TOKEN 0

#define ITEM_ID 8
#define ITEM_NAME 9

#define TITLE 16
#define IFTTT_EVENT 17
#define IFTTT_TOKEN 18

#define IFTTT_STARTED 24
#define IFTTT_CANCELED 25
#define IFTTT_FINISHED 26
#define IFTTT_POSTPONED 27

#define JS_READY 32


/* lengths for string */
#define IFTTT_TOKEN_LENGTH 23
#define MAX_EVENT_LENGTH 32

#define MAX_ITEMS_LENGTH 16
#define MAX_TITLE_LENGTH 28

#define TODOIST_TOKEN_LENGTH 41

/* TIMER SECONDS */
#define WORKTIME_SECONDS 1500
#define BREAKTIME_SECONDS 300

#endif /* CONSTANT_H_INCLUDED */
