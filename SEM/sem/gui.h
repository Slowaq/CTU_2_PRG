#ifndef __GUI_H_
#define __GUI_H_

void gui_init(void);
void gui_cleanup(void);
void gui_refresh(void);
void gui_resize(int new_w, int new_h);
unsigned char *gui_get_pixels(void);

#endif

