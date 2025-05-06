#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
 
#include "prg_serial.h"

void set_raw(_Bool set)
{
   static struct termios tio, tioOld;
   tcgetattr(STDIN_FILENO, &tio);
   if (set) { // put the terminal to raw 
      tioOld = tio; //backup 
      cfmakeraw(&tio);
      tio.c_lflag &= ~ECHO; // assure echo is disabled
      tio.c_oflag |= OPOST; // enable output postprocessing
      tcsetattr(STDIN_FILENO, TCSANOW, &tio);
   } else {    // set the previous settingsreset
      tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
   }
}
 
int main(int argc, char *argv[]){
  const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";
  fprintf(stderr, "INFO: open serial port at %s\n", serial);
  int fd = serial_open(serial);
  if (fd != -1) { // read from serial port
    // Procces the initial board message 'i'
    unsigned char init;
    if (serial_getc_timeout(fd, 500, &init) >= 0) {
      printf("BOARD: initialization = '%c'\n", init);
    } else {
      fprintf(stderr, "WARN: no init byte received\n");
    }

    _Bool quit = false;
    set_raw(true);
    while (!quit) {
      int c = getchar();

      _Bool write_read_response = false;
      switch (c) {
        case 'q':
          printf("Quit the program\n");
          quit = true;
          break;
        case 'S':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '0':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '1':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '2':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '3':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '4':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        case '5':
          printf("Sending '%c' to Nucleo\n", c);
          write_read_response = true;
          break;
        default:
          printf("Ignoring char '%c'\n", c);
          break;
      }
      if (write_read_response) {
        if (serial_putc(fd, c) != 1) {
          fprintf(stderr, "ERROR: write failed\n");
          break;
        }

        // Read board's response
        unsigned char ack;
        if (serial_getc_timeout(fd, 200, &ack) >= 0) {
          printf("DEBUG: Received response '%c'\n", ack);
        } else {
          fprintf(stderr, "WARN: no no ACK {timeout} \n");
        }
      }
    }
  } else {
    fprintf(stderr, "ERROR: Cannot open device %s\n", serial);
  }

  set_raw(false);
  serial_close(fd);
  return 0;
}
