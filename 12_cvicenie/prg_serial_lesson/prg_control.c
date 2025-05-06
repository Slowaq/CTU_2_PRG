#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
 
#include "prg_serial.h"
 
int main(int argc, char *argv[]){
  const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";
  fprintf(stderr, "INFO: open serial port at %s\n", serial);
  int fd = serial_open(serial);
  if (fd != -1) { // read from serial port
    // Procces the initial board message 'i'
    unsigned char init;
    if (serial_getc_timeout(fd, 500, &init) == 1) {
      printf("BOARD: initialization = '%c'\n", init);
    } else {
      fprintf(stderr, "WARN: no init byte received\n");
    }

    _Bool quit = false;
    while (!quit) {
      int c = getchar();
      if (c == '\n' || c == '\r') continue;

      _Bool write_read_response = false;
      switch (c) {
        case 'q':
          printf("Quit the program\n");
          quit = true;
          break;
        case 's':
          printf("Send 's' - LED on\n");
          write_read_response = true;
          break;
        case 'e':
          printf("Send 'e' - LED off\n");
          write_read_response = true;
          break;
        case 'f':
          printf("Send 'f' - LED switch\n");
          write_read_response = true;
          break;
        default:
          printf("Ignoring char '%d'\n", c);
          break;
      }
      if (write_read_response) {
        if (serial_putc(fd, c) != 1) {
          fprintf(stderr, "ERROR: write failed\n");
          break;
        }

        // Read board's response
        unsigned char ack;
        if (serial_getc_timeout(fd, 200, &ack) == 1) {
          printf("DEBUG: Received response '%c'\n", ack);
        } else {
          fprintf(stderr, "WARN: no no ACK {timeout} \n");
        }
      }
    }
  } else {
    fprintf(stderr, "ERROR: Cannot open device %s\n", serial);
  }

  serial_close(fd);
  return 0;
}
