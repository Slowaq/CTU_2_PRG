#ifndef _KEYBOARD_H__
#define _KEYBOARD_H__

void *keyboard_thread(void*d);
void info(const char *str);
void debug(const char *str);
void error(const char *str);

#endif

