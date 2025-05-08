CFLAGS += -Wall -Werror -std=gnu99 -g -pedantic
LDFLAGS = -lpthread

HW = prgsemestral
TARGET = prgsem
COMP_MODULE = comp_module

# Add SDL flags only to prgsem
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image

# Object files for prgsem, excluding comp_module.o
OBJS = $(filter-out comp_module.o, $(patsubst %.c,%.o,$(wildcard *.c)))

all: $(TARGET) $(COMP_MODULE)

# Rule to build prgsem (with SDL and pthread)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(SDL_LDFLAGS) -o $@

# Rule to build comp_module
$(COMP_MODULE): comp_module.o
	$(CC) comp_module.o $(LDFLAGS) -o $@

# Compile all .c to .o
%.o: %.c
	$(CC) -c $(CFLAGS) $(SDL_CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(COMP_MODULE) *.o
