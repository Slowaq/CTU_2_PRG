#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "main.h"
#include "utils.h"
#include "keyboard.h"
#include "messages.h"
#include "event_queue.h"
#include "prg_io_nonblock.h"   

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

} comp

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

    // Send desired number of bytes to pipe
    for (int i = 0; i < msg_len)
    {
        if (io_putc(fname_pipe_out, buf[i]) != 1)
        {
            fprintf(stderr, "ERROR during sending byte number %d\n", i);
            exit(IO_PUTC_ERROR);
        }

    }

    // Main loop for reading messages
    uint8_t buf[256];
    do
    {
        io_getc(pipe_in)
        quit = is_quit();
    } while (!quit);
    
}
