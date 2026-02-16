# Makefile sencillo
CC = gcc

SRC_DIRS := src
SRCS := $(shell find $(SRC_DIRS) -type f \( -name '*.c' -o -name '*.g' \))
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.g=.o)

CFLAGS = -Wall -Iinclude
LDFLAGS = -lallegro -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf -lallegro_color

TARGET = output_game

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.g
	$(CC) $(CFLAGS) -x c -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

