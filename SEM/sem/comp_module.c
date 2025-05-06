#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "main.h"
#include "utils.h"
#include "keyboard.h"
#include "messages.h"
#include "event_queue.h"
#include "prg_io_nonblock.h"   

// TODO: create header file for comp_module

#define IO_PUTC_ERROR 106
#define MAX_Z_NORM 4        // To stop iteration during computing pixel value

static struct {
    double c_re;            // Fractal Parameter
    double c_im;
    int n;

    double range_re_min;
    double range_re_max;
    double range_im_min;
    double range_im_max;

    int grid_w;
    int grid_h;

    int cur_x;
    int cur_y;

    double d_re;            // Horizontal step between pixels
    double d_im;            // Vertical step between pixels

    int nbr_chunks;
    int cid;
    double chunk_re;
    double chunk_im;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;
    
    uint8_t *grid;
    uint8_t max_iter;       // Maximal number of iterations in a loop
    bool computing;
    bool abort;             // Abort flag
    bool done;              // Done flag

} comp;

void compute_and_stream(
    int fd_out,             // Pipe for writing messages to control module
    int8_t cid,             // Chunk identifier
    double start_re,        // Real part of top left corner of chunk
    double start_im,        // Imaginary part of top left corner of chunk
    uint8_t n_re,           // Chunk width (number of horizontal pixels)                   
    uint8_t n_im            // Chunk height (number of vertical pixels)
);

int main(int argc, char *argv[])
{
    int return = EXIT_SUCCESS;
    const char *fname_pipe_in = argc > 1 ? argv[1] :  "/tmp/computational_module.out";
    const char *fname_pipe_out = argc > 2 ? argv[2] : "/tmp/computational_module.in";

    int pipe_in = io_open_read(fname_pipe_in);
    int pipe_out = io_open_write(fname_pipe_out);

    my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

    // Initialize msg and send MSG_STARTUP
    message msg = { .type = MSG_STARTUP, .data.startup.message =  {'b', 'a', 'l', 'u', 'c', 'w', 'i', 'l'}};
    send_message(fname_pipe_out, &msg)

    // The main loop for reading messages
    uint8_t msg_buf[256];
    do
    {
        if (readn(pipe_in, msg_buf, 1) == -1) break; // TODO: not handling error correctly
        msg.type = msg_buf[0];
        if (!get_message_size(msg_buf[0], &msg_len))
        {
            fprintf(stderr, "WARNING: unable to load message size")
            continue;
        }

        if (readn(pipe_in, msg_buf + 1, msg_len - 1) == -1) break; // TODO: not handling error correctly

        if (!parse_message_buf(msg_buf, msg_len, &msg)) continue; // Invalid checksum or wrong lenght

        switch (msg.type)
        {
        case MSG_GET_VERSION:
            message out = { .type = MSG_VERSION} // TODO: add other, i think it should be just return(msg.data.version)
            send_message(pipe_out, out)
            break;
        case MSG_SET_COMPUTE:
            comp.cid = msg->data.compute.cid; 
            comp.chunk_re = msg->data.compute.re;
            comp.chunk_im = msg->data.compute.im;
            comp.chunk_n_re = msg->data.compute.n_re;
            comp.chunk_n_im = msg->data.compute.n_im;
            send_message(pipe_out, MSG_OK);
            break;
        case MSG_COMPUTE:
            compute_and_stream( pipe_out, 
                                comp.cid, 
                                comp.chunk_re, 
                                comp.chunk_im, 
                                comp.chunk_n_re,
                                comp.chunk_n_im
                                );
            break;
        case MSG_ABORT:
            msg.type = MSG_ABORT;
            send_message(pipe_out, MSG_OK);
            break;
        default:
            send_message(pipe_out, MSG_ERROR);
            break;
        }

        quit = is_quit();
    } while (!quit);
    
}

void compute_and_stream(
    int fd_out, 
    int8_t cid, 
    double start_re, 
    double start_im, 
    uint8_t n_re, 
    uint8_t n_im)
{
    // Iteration through every pixel
    for (int i = 0; i < n_re; ++i)
    {
        for (int j = 0; j < n_im; ++j)
        {
            if (comp.abort)
            {
                send_message(pipe_out, MSG_ABORT);
                comp.abort = false;
                return;
            }

            double z0_re = start_re + x * n_re;
            double z0_re = start_im + y * n_im;
            double z_re = z0_re;
            double z_im = z0_im;
            uint8_t iter = 0;

            // Determine number of iterations of current pixel
            do
            {
                double tmp = z_re*z_re - z_im*z_im + c_re;  
                z_im = 2*z_re*z_im + c_im;  
                z_re = tmp;  
                iter++;  
            } while (iter < comp.max_iter && (z_re**2 + z_im**2) < MAX_Z_NORM);

            // Send result to out pipe
            message m = { .type = MSG_COMPUTE_DATA,
                          .data.compute_data.cid = cid,
                          .data.compute_data.i_re = x,
                          .data.compute_data.i_im = y,
                          .data.compute_data.iter = iter
                        };
            send_message(fd_out, m);

            // Send termination message to signal chunk being complete
            message term = { .type = MSG_DONE };
            send_message(fd_out, term);
        }
    }
}
