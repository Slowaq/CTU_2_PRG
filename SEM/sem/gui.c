#include "gui.h"
#include "xwin_sdl.h"
#include "utils.h"
#include "computation.h" 


static struct{
    int w;
    int h;
    unsigned char *img; 
} gui = { .img = NULL};

void gui_init(void){
    get_grid_size(&gui.w, &gui.h);
    gui.img = my_alloc(gui.w * gui.h * 3);
    my_assert(xwin_init(gui.w, gui.h) == 0, __func__, __LINE__, __FILE__);
}

void gui_cleanup(void){
    if(gui.img){
        free(gui.img);
        gui.img = NULL;
    }
    xwin_close();
}

void gui_refresh(void){
    if (gui.img) {
        update_image(gui.w, gui.h, gui.img);
        xwin_redraw(gui.w, gui.h, gui.img);
    }
}

void gui_resize(int new_w, int new_h){
    if (gui.img) {
      free(gui.img);
      xwin_close();
    }

    gui.w = new_w;
    gui.h = new_h;
    gui.img = my_alloc(gui.w * gui.h * 3);
    my_assert(xwin_init(gui.w, gui.h) == 0, __func__, __LINE__, __FILE__);
}

unsigned char *gui_get_pixels(void){
    return gui.img;
}
