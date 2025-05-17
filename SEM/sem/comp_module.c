#include "comp_module.h"
#include "main.h"
#include "utils.h"
#include "keyboard.h"
#include "messages.h"
#include "event_queue.h"
#include "prg_io_nonblock.h"   

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

} comp = {
    .c_re = -0.4,
    .c_im = 0.6,

    .n = 60,

    .range_re_min = -1.6,
    .range_re_max = 1.6,
    .range_im_min = -1.1,
    .range_im_max = 1.1,

    .grid = NULL,
    .grid_w = 640,
    .grid_h = 480,

    .chunk_n_re = 64,
    .chunk_n_im = 48,

    .computing = false,
    .abort = false,
    .done = false,
};

int main(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;
    const char *burst = (argc > 1 ? argv[1] : NULL);

    const char *fname_pipe_out = argc > 2 ? argv[2] :  "/tmp/computational_module.out";
    const char *fname_pipe_in = argc > 3 ? argv[3] : "/tmp/computational_module.in";
    
    int pipe_in = io_open_read(fname_pipe_in);
    int pipe_out = io_open_write(fname_pipe_out);
    
    my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

    // Initialize msg and send MSG_STARTUP
    message msg = { .type = MSG_STARTUP, .data.startup.message =  {'b', 'a', 'l', 'u', 'c', 'w', 'i', 'l'}};
    fprintf(stderr, "INFO: Sending startup message!\n\r");
    send_message(pipe_out, &msg);

    // The main loop for reading messages
    uint8_t msg_buf[256];
    int msg_len;
    bool quit = false;
    do
    {
        if (readn(pipe_in, msg_buf, 1) == -1) break; // TODO: not handling error correctly
        if (msg_buf[0] == MSG_COMPUTE && burst != NULL && burst[0] == 'b') 
        {
            fprintf(stderr, "Jebo!\n");
            msg_buf[0] = MSG_COMPUTE_BURST;
        } 
        msg.type = msg_buf[0];

        if (!get_message_size(msg_buf[0], &msg_len))
        {
            fprintf(stderr, "WARNING: unable to load message size\n\r");
            continue;
        }

        if (msg.type == MSG_COMPUTE_BURST &&  msg_len == -1){
            readn(pipe_in, msg_buf + 1, 1 + 2 );
            //uint8_t chunk_id = msg_buf[1];
            uint8_t len = comp.chunk_n_re * comp.chunk_n_im;
            msg_len = 1 + 1 + 2 + len + 1;

            readn(pipe_in, msg_buf + 4, len + 1);
        } else readn(pipe_in, msg_buf + 1, msg_len - 1);
                
        if (!parse_message_buf(msg_buf, msg_len, &msg)) {
            fprintf(stderr,"BAD checksum or bad size\n\r");
            continue;
        }

        fprintf(stderr, "Burst: %d, my message: %d\n\r", MSG_COMPUTE_BURST, msg.type);
        switch (msg.type)
        {
            case MSG_GET_VERSION: {
                message out = { .type = MSG_VERSION,
                                .data.version = {6,9,0} }; // TODO: add other, i think it should be just return(msg.data.version)
                fprintf(stderr, "INFO: Sending version message!\n\r");
                send_message(pipe_out, &out);
                break;
            }
            case MSG_SET_COMPUTE:
                comp.c_re     = msg.data.set_compute.c_re;
                comp.c_im     = msg.data.set_compute.c_im;
                comp.d_re     = msg.data.set_compute.d_re;
                comp.d_im     = msg.data.set_compute.d_im;
                comp.max_iter = msg.data.set_compute.n;

                comp.cid      = 0;
                comp.cur_x    = 0;
                comp.cur_y    = 0;
                comp.chunk_re = comp.range_re_min;
                comp.chunk_im = comp.range_im_max;
                comp.abort    = false;
                comp.done     = false;
                msg.type = MSG_COMPUTE;

                send_message(pipe_out, &ok);
                break;
            case MSG_COMPUTE: {
                comp.cid      = msg.data.compute.cid;
                comp.chunk_re = msg.data.compute.re;
                comp.chunk_im = msg.data.compute.im;
                comp.chunk_n_re = msg.data.compute.n_re;
                comp.chunk_n_im = msg.data.compute.n_im;

                send_message(pipe_out, &ok);
                fprintf(stderr, "INFO: Starting computation for cid=%d  (%d×%d chunk at %.3f%+.3fj)\n",
                        comp.cid, comp.chunk_n_re, comp.chunk_n_im, comp.chunk_re, comp.chunk_im);

                for (uint8_t x = 0; x < comp.chunk_n_re; ++x) {
                    for (uint8_t y = 0; y < comp.chunk_n_im; ++y) {
                        double z0_re = comp.chunk_re + x * comp.d_re;
                        double z0_im = comp.chunk_im + y * comp.d_im;
                        double z_re = z0_re, z_im = z0_im;
                        uint8_t iter = 0;
                        while (iter < comp.max_iter && (z_re*z_re + z_im*z_im) < MAX_Z_NORM) {
                            double tmp = z_re*z_re - z_im*z_im + comp.c_re;
                            z_im = 2*z_re*z_im + comp.c_im;
                            z_re = tmp;
                            ++iter;
                        }
                    
                        message m = {
                            .type = MSG_COMPUTE_DATA,
                            .data.compute_data = {
                                .cid = comp.cid,
                                .i_re = x,
                                .i_im = y,
                                .iter = iter
                            }
                        };
                        send_message(pipe_out, &m);
                    }
                }
                
                if (comp.abort)
                {
                    message abort = { .type = MSG_ABORT };
                    send_message(pipe_out, &abort);
                    comp.abort = false;
                    comp.cid -= 1;
                    break;
                }

                // fprintf(stderr, "INFO: Chunk %d done, sending MSG_DONE\n", comp.cid);
                message done = { .type = MSG_DONE };
                send_message(pipe_out, &done);
                break;}
            case MSG_COMPUTE_BURST:{
                fprintf(stderr, "debilko!\n");
                comp.cid      = msg.data.compute.cid;
                comp.chunk_re = msg.data.compute.re;
                comp.chunk_im = msg.data.compute.im;
                comp.chunk_n_re = msg.data.compute.n_re;
                comp.chunk_n_im = msg.data.compute.n_im;
                
                send_message(pipe_out, &ok);
                fprintf(stderr, "INFO: Starting BURST computation for cid=%d  (%d×%d chunk)\n",
                        comp.cid, comp.chunk_n_re, comp.chunk_n_im);
                
                uint8_t **iters = malloc(comp.chunk_n_re * sizeof(*iters));
                if (!iters) exit(1);
                for (int i = 0; i < comp.chunk_n_re; ++i){
                    iters[i] = malloc(comp.chunk_n_im * sizeof(*iters[i]));
                    if (!iters[i]) exit(1);
                }

                for (uint8_t x = 0; x < comp.chunk_n_re; ++x) {
                    for (uint8_t y = 0; y < comp.chunk_n_im; ++y) {
                        double z0_re = comp.chunk_re + x * comp.d_re;
                        double z0_im = comp.chunk_im + y * comp.d_im;
                        double z_re = z0_re, z_im = z0_im;
                        uint8_t iter = 0;
                        while (iter < comp.max_iter && (z_re*z_re + z_im*z_im) < MAX_Z_NORM) {
                            double tmp = z_re*z_re - z_im*z_im + comp.c_re;
                            z_im = 2*z_re*z_im + comp.c_im;
                            z_re = tmp;
                            ++iter;
                        }
                    
                        iters[x][y] = iter;
                    }
                }

                msg_compute_data_burst burst_message = { .chunk_id = comp.cid, 
                                                         .length   = comp.chunk_n_re * comp.chunk_n_im,
                                                         .iters    = iters,
                                                         .n_re     = comp.chunk_n_re,
                                                         .n_im     = comp.chunk_n_im};
                send_message(pipe_out, (message*)&burst_message);

                for (int i = 0; i < comp.chunk_n_re; ++i) free(iters[i]);
                free(iters);
                break;}
            case MSG_ABORT:
                comp.abort = true;
                comp.computing = false;
                fprintf(stderr, "TU SOM CHUJU");
                send_message(pipe_out, &ok);
                break;
            default: {
                message err = { .type = MSG_ERROR };
                send_message(pipe_out, &err);
                break;
            }
        }// switch end

        quit = is_quit();
    } while (!quit);
    return status;
}

void compute_and_stream(
    int fd_out, 
    uint8_t cid, 
    double start_re, 
    double start_im, 
    uint8_t n_re, 
    uint8_t n_im)
{
    // Iteration through every pixel
    for (uint8_t x = 0; x < n_re; ++x) {
        for (uint8_t y = 0; y < n_im; ++y) {
            fprintf(stderr, "n_re, n_im: %d %d", n_re, n_im);
            if (comp.abort)
            {
                message abort = { .type = MSG_ABORT };
                send_message(fd_out, &abort);
                comp.abort = false;
                return;
            }

            double z0_re = start_re + x * comp.d_re;
            double z0_im = start_im + y * comp.d_im;
            double z_re = z0_re;
            double z_im = z0_im;
            uint8_t iter = 0;

            // Determine number of iterations of current pixel
            while (iter < comp.max_iter && (z_re*z_re + z_im*z_im) < MAX_Z_NORM) {
                double tmp = z_re*z_re - z_im*z_im + comp.c_re;
                z_im = 2*z_re*z_im + comp.c_im;
                z_re = tmp;
                ++iter;
            }

            // Send result to out pipe
            message m = { .type = MSG_COMPUTE_DATA,
                          .data.compute_data.cid = cid,
                          .data.compute_data.i_re = x,
                          .data.compute_data.i_im = y,
                          .data.compute_data.iter = iter
                        };
            fprintf(stderr, "INFO: Sending message to fd_out:\n\r");
            send_message(fd_out, &m);

        }
    }
    // Send termination message to signal chunk being complete
    fprintf(stderr, "INFO: Chunk computed!\n\r");
    message term = { .type = MSG_DONE };
    send_message(fd_out, &term);
}