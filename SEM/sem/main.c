#include "main.h"
#include "utils.h"
#include "event_queue.h"
#include "computation.h"
#include "gui.h"
//#include "gui.c"
#include "messages.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ZOOM_AT 0 // 0 - Seahorse Valley, 1 - Elephant Valley, 1 - Spiral 
#define ZOOM_FACTOR 0.9
#define FPS 12
#define FRAMES 200
typedef enum {
   SEAHORSE,
   ELEPHANT,
   SPIRAL
} zoom_at;
// #define SEAHORSE_VALLEY_RE 0.743643887037151
// #define SEAHORSE_VALLEY_IM 0.131825904205330
#define SEAHORSE_VALLEY_RE -0.5251999
#define SEAHORSE_VALLEY_IM -0.525193
#define ELEPHANT_VALLEY_RE 0
#define ELEPHANT_VALLEY_IM 0
#define SPIRAL_RE -1.25066
#define SPIRAL_IM 0.02012

// Jhako
static bool hw_compute_started = false;
static bool waiting_for_compute_ok = false;

static pthread_t anim_th;
static bool is_animating = false;
void *anim_thread(void *_);

static pthread_t anim_th_z;
static bool is_animating_z = false;
void *anim_thread_z(void *_);

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
            case EV_COMPUTE_CPU:
                enable_comp();
                fprintf(stderr,"TUOSM\n");
                info (compute(&msg) ? "compute using CPU" : "fail compute using CPU");
                break;
            case EV_ABORT:
                msg.type = MSG_ABORT;  
                break;
            case EV_SET_COMPUTE:
                // msg.type = MSG_SET_COMPUTE;
                info (set_compute(&msg) ? "set compute" : "fail set compute");
                break;
            case EV_RESET:
                param_reset();
                info("Local compute reset to start");

                queue_push((event){ .type = EV_ABORT});
                msg.data.compute_data.cid  = 0;
                //msg.type = MSG_ABORT;
                queue_push((event){ .type = EV_SET_COMPUTE });
                //queue_push((event){ .type = EV_COMPUTE });
                break;
            case EV_CLEAR_BUFFER:
                {
                int w, h;
                get_grid_size(&w, &h);
                if (computation_grid_ptr()) memset(computation_grid_ptr(), 0, w * h);  
                else debug("Can not clear empty buffer!");
                }
                break;
            case EV_REFRESH:
                gui_refresh();  
                info("Screen cleared");
                break;
            case EV_COMPUTE:
                waiting_for_compute_ok = true;
                // msg.type = MSG_COMPUTE;
                //msg.data.compute.cid = 0;
                compute(&msg);
                break;
            case EV_RES_UP: {
                int w, h;
                get_grid_size(&w, &h);

                w = (w > 80 ? (int)(w * 1.3 + 0.5) : w);
                h = (h > 60 ? (int)(h * 1.3 + 0.5) : h);

                computation_set_resolution(w, h);
                gui_resize(w, h);

                info("Resolution increased");

                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_RES_DOWN: {
                int w, h;
                get_grid_size(&w, &h);
                w = (w > 80 ? (int)(w * 0.7 + 0.5) : w);
                h = (h > 60 ? (int)(h * 0.7 + 0.5) : h);

                computation_set_resolution(w, h);
                gui_resize(w, h);

                info("Resolution decreased");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_C_RE_UP:{
                if (is_computing()) continue;
                double cr = computation_get_c_re();
                double ci = computation_get_c_im();
                computation_set_c(cr + 0.01, ci);
                info("c_re increased");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_C_RE_DOWN:{
                if (is_computing()) continue;
                double cr = computation_get_c_re();
                double ci = computation_get_c_im();
                computation_set_c(cr - 0.01, ci);
                info("c_re decreased");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_C_IM_UP:{
                if (is_computing()) continue;
                double cr = computation_get_c_re();
                double ci = computation_get_c_im();
                computation_set_c(cr, ci + 0.01);
                info("c_im increased");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_C_IM_DOWN:{
                if (is_computing()) continue;
                double cr = computation_get_c_re();
                double ci = computation_get_c_im();
                computation_set_c(cr, ci - 0.01);
                info("c_im decreased");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_PAN_RIGHT:{
                if (is_computing()) continue;
                computation_pan(-0.1, 0);
                info("Pan right");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_PAN_LEFT:{
                if (is_computing()) continue;
                computation_pan(0.1, 0);
                info("Pan left");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_PAN_UP:{
                if (is_computing()) continue;
                computation_pan(0, -0.1);
                info("Pan up");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_PAN_DOWN:{
                if (is_computing()) continue;
                computation_pan(0, 0.1);
                info("Pan down");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_ZOOM_IN:{
                if (is_computing()) continue;
                computation_zoom(0.8);
                int w, h;
                get_grid_size(&w, &h); 
                computation_set_resolution(w, h);
                gui_resize(w, h);
                info("Zoom in");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_ZOOM_OUT:{
                if (is_computing()) continue;
                computation_zoom(1.2);
                int w, h;
                get_grid_size(&w, &h); 
                computation_set_resolution(w, h);
                gui_resize(w, h);
                info("Zoom in");
                queue_push((event){ .type = EV_SET_COMPUTE });
                queue_push((event){ .type = EV_COMPUTE });
                break;
            }
            case EV_START_ANIM:{
                if (!is_animating){
                    int th = pthread_create(&anim_th, NULL, anim_thread, NULL);
                    if (!th){
                        pthread_detach(anim_th);
                        is_animating = true;
                        info("Animation Start!");
                    } else error("Failed to start animation thread");
                } else info("Animation already running");
                break;
            }
            case EV_START_ANIM_ZOOM:{
                if (!is_animating_z){
                    int th = pthread_create(&anim_th_z, NULL, anim_thread_z, NULL);
                    if (!th){
                        pthread_detach(anim_th_z);
                        is_animating_z = true;
                        info("Zoom animation Start!");
                    } else error("Failed to start zoom animation thread");
                } else info("Zoom animation already running");
                break;
            }
            case EV_SAVE_PHOTO:{
                int w, h;
                get_grid_size(&w, &h);
                unsigned char *pixels = gui_get_pixels();
                
                if (!stbi_write_png("fractal.png", w, h, 3, pixels, w*3)) {
                    error("Failed to write PNG!");
                } else info("Photo saved!");
                break;
            }
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

void *anim_thread(void *_){
    double base_re = computation_get_c_re();
    double base_im = computation_get_c_im();
    for (double t = 0; t < 2*M_PI; t += 0.05) {
        // Reset state and set new c
        computation_set_c(base_re + 0.2*cos(t - M_PI/2), base_im + 0.2*sin(t));
        //queue_push((event){ .type = EV_RESET });
        queue_push((event){ .type = EV_SET_COMPUTE });
        queue_push((event){ .type = EV_COMPUTE });

        // Wait until module finishes comunication and gui recieves MSG_DONE
        while (!is_done() && !is_quit()) {
            usleep(10*1000);
        }

        queue_push((event){ .type = EV_REFRESH });

        usleep(100*1000);
    }

    is_animating = false;
    return NULL;
}

void *anim_thread_z(void *_){
    int w, h;
    get_grid_size(&w, &h);

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -y "
        "-f rawvideo "                    // input: raw uncompressed video
        "-pix_fmt rgb24 "                 // pixel format is RGB24
        "-s %dx%d "                       // frame size: WIDTHxHEIGHT
        "-r %d "                          // frame rate
        "-i - "                           // read input from stdin
        "-c:v libx264 "                   // use H.264 encoder
        "-pix_fmt yuv420p "               // output pixel format
        "zoom.mp4",                       // output file
        w, h, FPS
    );

    FILE *ff = popen(cmd, "w");
    if (!ff) {
        error("popen failed!");
        exit(1);
    }

    for (int i = 0; i < FRAMES && !is_quit(); ++i){
        //ccomputation_zoom(zoom_factor);
        if (ZOOM_AT == SEAHORSE) computation_zoom_at(SEAHORSE_VALLEY_RE, SEAHORSE_VALLEY_IM, ZOOM_FACTOR);
        else if (ZOOM_AT == ELEPHANT) computation_zoom_at(ELEPHANT_VALLEY_RE, ELEPHANT_VALLEY_IM, ZOOM_FACTOR);
        else if (ZOOM_AT == SPIRAL) computation_zoom_at(SPIRAL_RE, SPIRAL_IM, ZOOM_FACTOR);

        computation_update_steps();
        
        param_reset();
        computation_compute_full_frame();
        //queue_push((event){ .type = EV_REFRESH });
        gui_refresh();

        unsigned char *pixels = gui_get_pixels();
        fwrite(pixels, 1, w*h*3, ff);
        usleep(50*1000);
    }
    is_animating_z = false;
    
    int status = pclose(ff);
    if (status == -1) {
        perror("pclose");
    } else {
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        info("Video written successfully to zoom.mp4\n");
    } else {
        fprintf(stderr, "ffmpeg exited with status %d\n", WEXITSTATUS(status));
    }
    }

    return NULL;
}


void process_pipe_message(event * const ev){
    my_assert(ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev -> data.msg, __func__, __LINE__, __FILE__);
    ev->type = EV_TYPE_NUM;
    const message *msg = ev->data.msg;
    switch (msg->type){
        case MSG_OK:
            info("OK");
            if (waiting_for_compute_ok) {
                hw_compute_started = true;
                waiting_for_compute_ok = false;
            }
            break;
        case MSG_VERSION:
            fprintf(stderr, "INFO: Module version %d.%d-p%d\n\r", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
            break;
        case MSG_COMPUTE_DATA:
            if (!is_abort()){
                update_data(&(msg->data.compute_data));
            }
            break;
        case MSG_ABORT:
            // info("Computation done.");
            info("Module aborted current chunk—will retry it");
            // re-run the very same chunk
            //queue_push((event){ .type = EV_COMPUTE });
            break;
        case MSG_DONE:
            fprintf(stderr, "Pici co to nejde\n\r");
            gui_refresh();
            if (is_done()){
                info("Computation done");
            } else {
                fprintf(stderr, "ev.type: %d", ev->type);
                event ev = { .type = EV_COMPUTE_CPU};
                queue_push(ev);
            }
            break;
        case MSG_STARTUP:
            fprintf(stderr, "INFO: Startup message: %s\n\r", msg->data.startup.message);
            break;
        default:
            fprintf(stderr, "Unhandled pipe message type %d\n", msg->type);
            break;
    }
    free (ev->data.msg);    
    ev->data.msg = NULL;
}
