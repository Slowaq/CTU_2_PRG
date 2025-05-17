#include "computation.h"
#include "utils.h"
#include "messages.h"

#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_Z_NORM 128
#define MAX_ITERS 10000
#define EXTRA_ITERS 20


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

} comp = {
    .c_re = -0.5251993,
    .c_im = -0.5251993,

    .n = 600,

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

int base_iters = 60;
double total_zoom = 1.0;

void computation_init(void){
    comp.grid = my_alloc(comp.grid_w * comp.grid_h);
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1. * comp.grid_w); 
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1. * comp.grid_h); 
    comp.nbr_chunks = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im); 
}

void computation_cleanup(void){
    if(comp.grid) free (comp.grid); 
    comp.grid = NULL;
}
bool is_computing(void) {return comp.computing;}
bool is_done(void) {return comp.done;}
bool is_abort(void) {return comp.abort;}

void get_grid_size(int *w, int *h){
    *w = comp.grid_w;
    *h = comp.grid_h;
}

void abort_comp(void){comp.abort =  false;}
void enable_comp(void){comp.abort =  false;}

bool set_compute(message *msg){
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    bool ret = !is_computing();
    if(ret){
        msg->type = MSG_SET_COMPUTE;
        msg->data.set_compute.c_re = comp.c_re;
        msg->data.set_compute.c_im = comp.c_im;
        msg->data.set_compute.d_re = comp.d_re;
        msg->data.set_compute.d_im = comp.d_im;
        msg->data.set_compute.n = comp.n;
        comp.done = false;
    }
    return ret;
}

bool compute(message *msg){
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    if (!is_computing()){ // first chunk
        comp.cid = 0;
        comp.computing = true;
        comp.cur_x = comp.cur_y = 0; // start computation of the whole image
        comp.chunk_re = comp.range_re_min; // upper - "left" corner
        comp.chunk_im = comp.range_im_max; // "upper" - left corner
        msg->type = MSG_COMPUTE;
    } else { // next chunk
        comp.cid += 1;
        if (comp.cid < comp.nbr_chunks){
            comp.cur_x += comp.chunk_n_re;
            comp.chunk_re += comp.chunk_n_re * comp.d_re;
            if(comp.cur_x >= comp.grid_w){
                comp.chunk_re = comp.range_re_min;
                comp.chunk_im += comp.chunk_n_im * comp.d_im;
                comp.cur_x = 0;
                comp.cur_y += comp.chunk_n_im;
            }
            msg->type = MSG_COMPUTE;
        } else {
            // all has been compute
        }
    }

    if (comp.computing && msg->type == MSG_COMPUTE){
        msg->data.compute.cid = comp.cid; 
        msg->data.compute.re = comp.chunk_re;
        msg->data.compute.im = comp.chunk_im;
        msg->data.compute.n_re = comp.chunk_n_re;
        msg->data.compute.n_im = comp.chunk_n_im;
    }

    return is_computing();
}

void update_image(int w, int h, unsigned char *img){
    my_assert(img && comp.grid && w == comp.grid_w && h == comp.grid_h, __func__, __LINE__, __FILE__);
    for (int i = 0; i < w * h; ++i){
        const double t = 1. * comp.grid[i] / (comp.n + 1.0); 
        *(img++) = 9 * (1-t) *t*t*t * 255;
        *(img++) = 15 * (1-t) * (1-t)*t*t * 255;
        *(img++) = 8.5 * (1-t) * (1-t) * (1-t) * t * 255;

    }
}

void update_data(const msg_compute_data *computation_data){
    my_assert(computation_data != NULL, __func__, __LINE__, __FILE__);
    if (computation_data->cid == comp.cid){
        const int idx = comp.cur_x + computation_data->i_re + (comp.cur_y + computation_data->i_im) * comp.grid_w;
        if (idx >= 0 && idx < (comp.grid_w * comp.grid_h)){
            comp.grid[idx] = computation_data->iter;
        }
        if ((comp.cid + 1) >= comp.nbr_chunks && (computation_data->i_re + 1) == comp.chunk_n_re && (computation_data->i_im + 1) == comp.chunk_n_im){
            comp.done = true;
            comp.computing = false;
        }
    } else {
        fprintf(stderr,"Received chunk with unexpected chunk id (cid)%d %d", computation_data->cid, comp.cid);
    };
}

void param_reset(void){
  comp.cid       = 0;
  comp.cur_x     = 0;
  comp.cur_y     = 0;
  comp.chunk_re  = comp.range_re_min;
  comp.chunk_im  = comp.range_im_max;
  comp.computing = false;
  comp.abort     = false;
  comp.done      = false;

  memset(comp.grid, 0, comp.grid_w * comp.grid_h);
}

uint8_t *computation_grid_ptr(void){
  return comp.grid;
}

void computation_set_resolution(int new_w, int new_h){
    if (comp.grid) free(comp.grid);

    comp.grid_w = new_w;
    comp.grid_h = new_h;

    comp.grid = my_alloc(comp.grid_w * comp.grid_h);
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (double)comp.grid_w;
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (double)comp.grid_h;

    comp.nbr_chunks = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im);

    param_reset();
}

void computation_update_steps(void) {
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (double)comp.grid_w;
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (double)comp.grid_h;
    comp.nbr_chunks = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im);
}


double computation_get_c_re(void) {return comp.c_re;}
double computation_get_c_im(void) {return comp.c_im;}

void computation_set_c(double new_re, double new_im){
    comp.c_re = new_re;
    comp.c_im = new_im;

    param_reset();
}

void computation_pan(double dx, double dy){
    double w = comp.range_re_max - comp.range_re_min;
    double h = comp.range_im_max - comp.range_im_min;

    comp.range_re_max += w * dx;
    comp.range_re_min += w * dx;
    comp.range_im_max += h * dy;
    comp.range_im_min += h * dy;

    param_reset();
}

void computation_zoom(double factor){
    double cx = 0.5 * (comp.range_re_min + comp.range_re_max);
    double cy = 0.5 * (comp.range_im_min + comp.range_im_max);
    double w  = (comp.range_re_max - comp.range_re_min) * factor;
    double h  = (comp.range_im_max - comp.range_im_min) * factor;
    
    comp.range_re_min = cx - 0.5 * w;
    comp.range_re_max = cx + 0.5 * w;
    comp.range_im_min = cy - 0.5 * h;
    comp.range_im_max = cy + 0.5 * h;

    param_reset();
}

void computation_zoom_at(double center_re, double center_im, double factor) {
    double re_min = comp.range_re_min;
    double re_max = comp.range_re_max;
    double im_min = comp.range_im_min;
    double im_max = comp.range_im_max;

    double w = re_max - re_min;
    double h = im_max - im_min;

    double w_new = w * factor;
    double h_new = h * factor;

    double rx = (center_re - re_min) / w;
    double ry = (center_im - im_min) / h;

    comp.range_re_min = center_re - rx * w_new;
    comp.range_re_max = comp.range_re_min + w_new;
    comp.range_im_min = center_im - ry * h_new;
    comp.range_im_max = comp.range_im_min + h_new;

    total_zoom *= factor;
    double zoom_level = -log10(total_zoom);
    comp.n = base_iters + (int)(zoom_level * EXTRA_ITERS);

    if (comp.n < base_iters) comp.n = base_iters;
    if (comp.n > MAX_ITERS)      comp.n = MAX_ITERS;

    param_reset();
}

void computation_compute_full_frame(void) {
    // const int W = comp.grid_w, H = comp.grid_h;
    int W, H;
    get_grid_size(&W, &H);
    const double dr = (comp.range_re_max - comp.range_re_min) / W;
    const double di = -(comp.range_im_max - comp.range_im_min) / H;

    for (int y = 0; y < H; ++y) {
      double zy = comp.range_im_max + y * di;
      for (int x = 0; x < W; ++x) {
        double zx = comp.range_re_min + x * dr;
        double zr = zx, zi = zy;
        int iter = 0;
        while (iter < comp.n && (zr*zr + zi*zi) < MAX_Z_NORM) {
          double tmp = zr*zr - zi*zi + comp.c_re;
          zi = 2*zr*zi + comp.c_im;
          zr = tmp;
          ++iter;
        }
        comp.grid[y*W + x] = iter;
      }
    }
}

