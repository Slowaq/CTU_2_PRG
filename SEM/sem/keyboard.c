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
                // compute using CPU
                ev.type = EV_COMPUTE_CPU;
                debug("c pressed.");
                break;
            case 'r':
                // reset parameters to origin.
                ev.type = EV_RESET;
                debug("r pressed");
                break;
            case 'y':
                // clear buffer.
                ev.type = EV_CLEAR_BUFFER;
                debug("y pressed");
                break;
            case 'x':
                // refresh window
                ev.type = EV_REFRESH;
                debug("x pressed");
                break;
            case '+':
                ev.type = EV_RES_UP;
                debug("+ pressed, increase resolution");
                break;
            case '-':
                ev.type = EV_RES_DOWN;
                debug("- pressed, decrease resolution");
                break;
            case '1':
                // compute
                ev.type = EV_COMPUTE;
                debug("1 pressed");
                break;

            case 'o': ev.type = EV_ZOOM_IN;    debug("o pressed: zoom in");     break;
            case 'u': ev.type = EV_ZOOM_OUT;   debug("u pressed: zoom out");    break;
            case 'j': ev.type = EV_PAN_LEFT;   debug("j pressed: pan left");    break;
            case 'l': ev.type = EV_PAN_RIGHT;  debug("l pressed: pan right");   break;
            case 'i': ev.type = EV_PAN_UP;     debug("i pressed: pan up");      break;
            case 'k': ev.type = EV_PAN_DOWN;   debug("k pressed: pan down");    break;

            case '7': ev.type = EV_C_RE_UP;    debug("7 pressed: c_re++");      break;
            case '8': ev.type = EV_C_RE_DOWN;  debug("8 pressed: c_re--");      break;
            case '0': ev.type = EV_C_IM_UP;    debug("0 pressed: c_im++");      break;
            case '9': ev.type = EV_C_IM_DOWN;  debug("9 pressed: c_im--");      break;

            case 'z': ev.type = EV_START_ANIM_ZOOM; debug("z pressed: zoom animation");   break;
            case 't': ev.type = EV_START_ANIM; debug("z pressed: animation");   break;

            case '2': ev.type = EV_SAVE_PHOTO; debug("2 pressed: save photo");   break;
            
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

