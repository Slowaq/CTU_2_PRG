#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "main.h"
#include "utils.h"
#include "keyboard.h"
#include "messages.h"
#include "event_queue.h"
#include "prg_io_nonblock.h"   

#ifndef __COMP_MODULE_H__
#define __COMP_MODULE_H__

const message ok = { .type = MSG_OK };

/// ----------------------------------------------------------------------------
/// @brief Compute a chunk of the Julia fractal and stream the results back to the controller.
///        Iterates over a grid of pixels defined by the complex-plane origin (start_re, start_im),
///        the dimensions (n_re × n_im), and the step sizes (comp.d_re, comp.d_im). For each pixel,
///        it computes the escape-iteration count up to comp.max_iter, then sends a MSG_COMPUTE_DATA
///        message containing (cid, x, y, iter). If comp.abort is set during computation, the function
///        immediately sends MSG_ABORT and returns. Once all pixels are processed, it sends a single
///        MSG_DONE message to signal completion of this chunk.
///
/// @param fd_out           // Pipe for writing messages to control module
/// @param cid              // Chunk identifier
/// @param start_re         // Real part of top left corner of chunk
/// @param start_im         // Imaginary part of top left corner of chunk
/// @param n_re             // Chunk width (number of horizontal pixels)
/// @param n_im             // Chunk height (number of vertical pixels)
/// ----------------------------------------------------------------------------
void compute_and_stream(
    int fd_out,             
    uint8_t cid,             
    double start_re,        
    double start_im,        
    uint8_t n_re,                              
    uint8_t n_im            
);



#endif