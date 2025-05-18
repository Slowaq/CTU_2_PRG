# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Werror -std=gnu99 -g -pedantic -I./stb
LDFLAGS := -fsanitize=address -lpthread -lm -lSDL2 -lSDL2_image

# Targets
PRG      := prgsem
COMP     := comp_module

# Source files
PRG_SRCS := computation.c event_queue.c gui.c keyboard.c main.c messages.c \
            prg_io_nonblock.c prgsem.c utils.c xwin_sdl.c
PRG_OBJS := $(PRG_SRCS:.c=.o)

COMP_SRCS := comp_module.c prg_io_nonblock.c utils.c messages.c event_queue.c
COMP_OBJS := $(COMP_SRCS:.c=.o)

all: $(PRG) $(COMP)

# prgsem needs stb_image_write.o
$(PRG): $(PRG_OBJS) stb_image_write.o
	$(CC) $(PRG_OBJS) stb_image_write.o $(LDFLAGS) -o $@

$(COMP): $(COMP_OBJS)
	$(CC) $(COMP_OBJS) $(LDFLAGS) -o $@

# Generic .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the single stb_image_write.o from the header with the implementation macro
stb_image_write.o: stb/stb_image_write.h
	$(CC) $(CFLAGS) -DSTB_IMAGE_WRITE_IMPLEMENTATION -x c -c stb/stb_image_write.h -o stb_image_write.o

clean:
	rm -f $(PRG) $(COMP) *.o
