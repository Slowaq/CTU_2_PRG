#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "messages.h"
#include <stdbool.h>
#include <pthread.h>

typedef enum {
   EV_NUCLEO,
   EV_KEYBOARD,
   EV_NUM
} event_source;

typedef enum {
   EV_COMPUTE, // request compute on nucleo with particular 
   EV_RESET_CHUNK, //reset the chunk id
   EV_RESET,
   EV_ABORT,
   EV_GET_VERSION,
   EV_THREAD_EXIT,
   EV_QUIT,
   EV_SERIAL,
   EV_SET_COMPUTE,
   EV_COMPUTE_CPU,
   EV_CLEAR_BUFFER,
   EV_REFRESH,
   EV_PIPE_IN_MESSAGE,
   EV_RES_UP,       
   EV_RES_DOWN,
   EV_C_RE_UP,      // increase real part of c
   EV_C_RE_DOWN,
   EV_C_IM_UP,      // increase imaginary part of c
   EV_C_IM_DOWN,
   EV_ZOOM_IN,      // zoom in
   EV_ZOOM_OUT,     // ozoom out
   EV_PAN_LEFT,     // shift
   EV_PAN_RIGHT,
   EV_PAN_UP,
   EV_PAN_DOWN,
   EV_START_ANIM,
   EV_START_ANIM_ZOOM,
   EV_SAVE_PHOTO,
   EV_SAVE_VIDEO,
   EV_TYPE_NUM
} event_type;

typedef struct {
   int param;
} event_keyboard;

typedef struct {
   message *msg;
} event_serial;

typedef struct {
   event_source source;
   event_type type;
   union {
      int param;
      message *msg;
   } data;
} event;

void queue_init(void);
void queue_cleanup(void);

event queue_pop(void);

void queue_push(event ev);

bool is_quit(void);

void set_quit(void);
#endif

/* end of event_queue.h */

