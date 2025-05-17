#include <stdbool.h>

#include "messages.h"

#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__



void computation_init(void);
void computation_cleanup(void);

void get_grid_size(int *w, int *h); 
bool is_computing(void);
bool is_done(void);
bool is_abort(void);

void abort_comp(void);
void enable_comp(void);

void abort_comp(void);
void param_reset(void);

bool set_compute(message *msg);
bool compute(message *msg);

void computation_compute_full_frame(void);

void update_image(int w, int h, unsigned char *img);
void update_data(const msg_compute_data *computation_data);

uint8_t *computation_grid_ptr(void); 
void computation_set_resolution(int new_w, int new_h);
void computation_update_steps(void);
// void computation_zoom(double s);

/// Set up new parameter c (real+im)
void computation_set_c(double new_re, double new_im);

/// Shift range of rendered part of complex plane by relative shift [dx,dy]
void computation_pan(double dx, double dy);

/// Zoom in/out to the middle by the given ratio
void computation_zoom(double factor);
void computation_zoom_at(double center_re, double center_im, double factor);

double computation_get_c_re(void);
double computation_get_c_im(void);


#endif

