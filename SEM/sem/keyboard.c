#include "keyboard.h"
#include "event_queue.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

void *keyboard_thread(void *d){
    fprintf(stderr, "keyboard_thread - start\n");
    call_termios(0);
    int c;
    event ev;
    while ((c = getchar()) != 'q'){
        ev.type = EV_TYPE_NUM;
        switch(c) {
            case 'g':
                // get version
                ev.type = EV_GET_VERSION;
                debug("g pressed.");
                break;
            case 'a':
                //abort 
                ev.type = EV_ABORT;
                debug("a pressed.");
                break;
            case 's':
                // set compute
                ev.type = EV_SET_COMPUTE;
                debug("s pressed.");
                break;
            case 'c':
                // compute
                ev.type = EV_COMPUTE;
                debug("c pressed.");
                break;
            default:
                break;
        } // end switch
        if (ev.type != EV_TYPE_NUM){
            queue_push(ev);
        }
    } // end while
    set_quit();
    ev.type = EV_QUIT;
    queue_push(ev);
    
    call_termios(1); // restore terminal settings
    fprintf(stderr, "keyboard_thread - finished\n\r");
    return NULL;
}

