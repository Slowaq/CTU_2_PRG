#ifndef __MAIN_H__
#define __MAIN_H__

#include "event_queue.h"
#include "utils.h"

void *main_thread(void *d);
void info(const char *str);
void debug(const char *str);
void error(const char *str);
void process_pipe_message(event * const ev);


#endif