/*
 * Filename: prg_io_nonblock.c
 * Date:     2019/12/25 14:20
 * Author:   Jan Faigl
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

#include <poll.h>

#include "prg_io_nonblock.h"

/// ----------------------------------------------------------------------------
static int io_open(const char *fname, int flag)
{
   int fd = open(fname, flag | O_NOCTTY | O_SYNC);
   if (fd != -1) {
      // Set fd to non block mode
      int flags = fcntl(fd, F_GETFL);
      flags &= ~O_NONBLOCK;
      if (fcntl(fd, F_SETFL, flags) < 0) {
         fprintf(stderr, "Error in %s(), file %s, line: %d, errno %d\n", __func__, __FILE__, __LINE__, errno);
      }
   }
   return fd;
}

/// ----------------------------------------------------------------------------
int io_open_read(const char *fname)
{
   // as defined for FIFO, open will be blocked for read only unless nonblock is specified
   return io_open(fname, O_RDONLY | O_NONBLOCK); 
}

/// ----------------------------------------------------------------------------
int io_open_write(const char *fname)
{  
   // Be aware that the opening named pipe for writing is blocked until the pipe is opened for reading. 
   // Thus, run a program that opens the pipe or call, e.g., 'tail -f fname', where 'fname' is the filename name of the named pipe being opened for writing.
   return io_open(fname, O_WRONLY);
}

/// ----------------------------------------------------------------------------
int io_close(int fd)
{
   return close(fd);
}

/// ----------------------------------------------------------------------------
int io_putc(int fd, char c)
{
   return write(fd, &c, 1);
}

/// ----------------------------------------------------------------------------
int io_getc(int fd)
{
   char c;
   int r = read(fd, &c, 1);
   return r == 1 ? c : -1;
}

/// ----------------------------------------------------------------------------
ssize_t readn(int fd, void *buf, size_t n)
{
   /*
      Read exactly n number of characters form file descriptor fd and save to buf
   */
   int read_total = 0;
   while (read_total < n)
   {
      ssize_t read_ret = read(fd, buf + read_total, n - read_total);
      if (read_ret > 0) read_total += read_ret;
      if (read_ret == 0) break;
      // TODO: handle if read fails e.g.: read_ret < 0
   }

   if (read_total == n) return read_total;
   else return -1; // TODO: Dunno if it's the correct way
}

/// ----------------------------------------------------------------------------
ssize_t writen(int fd, const void *buf, size_t n)
{
   /*
      Write exactly n bytes from buffer buf to file descriptor fd
   */

   ssize_t written = 0;
   while (written < n)
   {
      ssize_t write_ret = write(fd, buf + written, n - written);
      if (write_ret > 0) written += write_ret;
      if (write_ret == 0) break;
      // TODO: handle if read fails e.g.: write_ret < 0
   }

   if (writen == n) return written;
   else return -1; // TODO: Dunno if it's the correct way
}

/// ----------------------------------------------------------------------------
int io_getc_timeout(int fd, int timeout_ms, unsigned char *c)
{
   struct pollfd ufdr[1];
   int r = 0;
   ufdr[0].fd = fd;
   ufdr[0].events = POLLIN | POLLRDNORM;
   if ((poll(&ufdr[0], 1, timeout_ms) > 0) && (ufdr[0].revents & (POLLIN | POLLRDNORM))) {
      r = read(fd, c, 1);
   }
   return r;
}

/* end of prg_io_nonblock.c */
