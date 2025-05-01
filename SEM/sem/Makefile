CFLAGS+= -Wall -Werror -std=gnu99 -g -pedantic
LDFLAGS=-lpthread

HW=prgsemestral
TARGET=prgsem

CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) -lSDL2_image


all: $(TARGET)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

prgsem: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
