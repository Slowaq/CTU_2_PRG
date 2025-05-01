#include "main.h"
#include "utils.h"
#include "event_queue.h"
#include "computation.h"
#include "gui.h"
#include "messages.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void *main_thread(void *d){

    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_out = *(int*)d;
    message msg;
    uint8_t msg_buf[sizeof(message)];
    int msg_len;
    bool quit = false; 

    computation_init();
    gui_init();
    // initilize computation, visualization
    do {
        event ev = queue_pop();
        msg.type = MSG_NBR;
        switch (ev.type){
            case EV_QUIT:
                debug("Quit received");
                break;
            case EV_GET_VERSION:
                msg.type = MSG_GET_VERSION;
                break;
            case EV_COMPUTE:
                enable_comp();
                info (compute(&msg) ? "compute" : "fail compute");
                break;
            case EV_ABORT:
                msg.type = MSG_ABORT;  
                break;
            case EV_SET_COMPUTE:
                info (set_compute(&msg) ? "set compute" : "fail set compute");
                break;
            case EV_PIPE_IN_MESSAGE:
                process_pipe_message(&ev);
                break;
            default:
                break;
        }// switch end
        if (msg.type != MSG_NBR){
            my_assert(fill_message_buf(&msg, msg_buf, sizeof(message), &msg_len), __func__, __LINE__, __FILE__);
            if (write(pipe_out,msg_buf, msg_len) == msg_len){
                debug("send data to pipe_out");
            } else{
                error("send message fail!");
            }
        }
        quit = is_quit();
    } while(!quit);
    gui_cleanup();
    computation_cleanup();
    //  cleanup computation, visualization
    return NULL;
}

void process_pipe_message(event * const ev){
    my_assert(ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev -> data.msg, __func__, __LINE__, __FILE__);
    ev->type = EV_TYPE_NUM;
    const message *msg = ev->data.msg;
    switch (msg->type){
        case MSG_OK:
            info("OK");
            break;
        case MSG_VERSION:
            fprintf(stderr, "INFO: Module version %d.%d-p%d\n", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
            break;
        case MSG_COMPUTE_DATA:
            if (!is_abort()){
                update_data(&(msg->data.compute_data));
            }
            break;
        case MSG_ABORT:
            info("Computation done.");
            break;
        case MSG_DONE:
            gui_refresh();
            if (is_done()){
                info("Computation done");
            } else {
                event ev = { .type = EV_COMPUTE};
                queue_push(ev);
            }
            break;
        default:
            fprintf(stderr, "Unhandled pipe message type %d\n", msg->type);
            break;
    }
    free (ev->data.msg);    
    ev->data.msg = NULL;
}
