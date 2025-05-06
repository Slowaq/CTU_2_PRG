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

static struct {
    double c_re;
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

    double d_re;
    double d_im;

    int nbr_chunks;
    int cid;
    double chunk_re;
    double chunk_im;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;
    
    uint8_t *grid;
    bool computing;
    bool abort;
    bool done;

} comp;

void compute_and_stream(int fd_out, 
                        int8_t cid, 
                        double start_re, double start_im, 
                        uint8_t n_re, uint8_t n_im)

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
    uint8_t buf[16];
    int msg_len;
    fill_message_buf(&msg, buf, sizeof(uint8_t), &msg_len);

    if (send(fname_pipe_out, buf, msg_len) != msg_len)
    {
        fprintf(stderr, "ERROR during sending byte number %d\n", i);
        exit(IO_PUTC_ERROR);
    }

    // The main loop for reading messages
    uint8_t buf[256];
    do
    {
        if (readn(pipe_in, buf, 1) == -1) break; // TODO: not handling error correctly
        msg.type = buf[0];
        if (!get_message_size(buf[0], &msg_len))
        {
            fprintf(stderr, "WARNING: unable to load message size")
            continue;
        }

        if (readn(pipe_in, buf + 1, msg_len - 1) == -1) break; // TODO: not handling error correctly

        if (!parse_message_buf(buf, msg_len, &msg)) continue; // Invalid checksum or wrong lenght

        switch (msg.type)
        {
        case MSG_GET_VERSION:
            message out = { .type = MSG_VERSION} // TODO: add other, i think it should be just return(msg.data.version)
            send(pipe_out, out, sizeof(out));
            break;
        case MSG_SET_COMPUTE:
            comp.cid = msg->data.compute.cid; 
            comp.chunk_re = msg->data.compute.re;
            comp.chunk_im = msg->data.compute.im;
            comp.chunk_n_re = msg->data.compute.n_re;
            comp.chunk_n_im = msg->data.compute.n_im;
            send(pipe_out, MSG_OK, sizeof(MSG_OK));
            break;
        case MSG_COMPUTE:
            
            break;
        case MSG_ABORT:
            msg.type = MSG_ABORT;
            send(pipe_out, MSG_OK, sizeof(MSG_OK));
            break;
        default:
            send(pipe_out, MSG_ERROR, sizeof(MSG_ERROR));
            break;
        }

        quit = is_quit();
    } while (!quit);
    
}

void compute_and_stream(int fd_out, 
    int8_t cid, 
    double start_re, double start_im, 
    uint8_t n_re, uint8_t n_im)
{
    for (int i = 0; i < n_re; ++i)
    {
        for (int j = 0; j < n_im; ++j)
        {
            if (comp.abort)
            {
                send(pipe_out, MSG_ABORT, sizeof(MSG_ABORT));
                comp.abort = false;
            }

            
        }
    }
}
