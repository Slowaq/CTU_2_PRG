#include "comp_module.h"

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

int main(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;
    const char *fname_pipe_in = argc > 1 ? argv[1] :  "/tmp/computational_module.out";
    const char *fname_pipe_out = argc > 2 ? argv[2] : "/tmp/computational_module.in";

    int pipe_in = io_open_read(fname_pipe_in);
    int pipe_out = io_open_write(fname_pipe_out);

    my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

    // Initialize msg and send MSG_STARTUP
    message msg = { .type = MSG_STARTUP, .data.startup.message =  {'b', 'a', 'l', 'u', 'c', 'w', 'i', 'l'}};
    send_message(pipe_out, &msg);

    // The main loop for reading messages
    uint8_t msg_buf[256];
    int msg_len;
    do
    {
        if (readn(pipe_in, msg_buf, 1) == -1) break; // TODO: not handling error correctly
        msg.type = msg_buf[0];
        if (!get_message_size(msg_buf[0], &msg_len))
        {
            fprintf(stderr, "WARNING: unable to load message size");
            continue;
        }

        if (readn(pipe_in, msg_buf + 1, msg_len - 1) == -1) break; // TODO: not handling error correctly

        if (!parse_message_buf(msg_buf, msg_len, &msg)) continue; // Invalid checksum or wrong length

        switch (msg.type)
        {
            case MSG_GET_VERSION:
                 message out = { .type = MSG_VERSION,
                                .data.version = {1,0,0} }; // TODO: add other, i think it should be just return(msg.data.version)
                send_message(pipe_out, out);
                break;
            case MSG_SET_COMPUTE:
                comp.cid      = msg.data.set_compute.cid; 
                comp.c_re     = msg.data.set_compute.c_re;
                comp.c_im     = msg.data.set_compute.c_im;
                comp.d_re     = msg.data.set_compute.d_re;
                comp.d_im     = msg.data.set_compute.d_im;
                comp.max_iter = msg.data.set_compute.n;
                send_message(pipe_out, ok);
                break;
            case MSG_COMPUTE:
                send_message(pipe_out, ok);
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
                send_message(pipe_out, ok);
                break;
            default:
                send_message(pipe_out, MSG_ERROR);
                break;
        }// switch end

        quit = is_quit();
    } while (!quit);
    
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
            if (comp.abort)
            {
                send_message(pipe_out, MSG_ABORT);
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
            send_message(fd_out, m);

        }
    }
    // Send termination message to signal chunk being complete
    message term = { .type = MSG_DONE };
    send_message(fd_out, term);

    return status;
}
