# Generic dual-platform Makefile
CC = gcc

# Auto-discover sources
SRC_DIRS := src
SRCS_PC  := $(shell find $(SRC_DIRS) -type f -name '*.c' ! -path 'src/rpi/*')
SRCS_RPI := $(shell find src/rpi -type f -name '*.c')

OBJS_PC  := $(SRCS_PC:.c=.o)
OBJS_RPI := $(SRCS_RPI:.c=.o)

# Common and platform-specific flags
COMMON_CFLAGS = -Wall -Iinclude
PC_CFLAGS = $(COMMON_CFLAGS)
PC_LDFLAGS = -lallegro -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf

RPI_CFLAGS = $(COMMON_CFLAGS) -Iinclude/rpi
RPI_LDFLAGS =

# Targets names
TARGET_PC = output_game
TARGET_RPI = output_game_rpi

# Source lists
SRCS_PC = src/main.c src/init.c src/menu.c src/partida.c src/game_logic.c
OBJS_PC = $(SRCS_PC:.c=.o)

SRCS_RPI = src/rpi/rpi_main.c src/rpi/rpi_game_logic.c src/rpi/rpi_render.c
OBJS_RPI = $(SRCS_RPI:.c=.o)

.PHONY: all pc rpi build toggle clean

# Default: toggle between platforms on each invocation
all: toggle

# Build PC version
pc:
	$(MAKE) build OBJS="$(OBJS_PC)" TARGET="$(TARGET_PC)" CFLAGS="$(PC_CFLAGS)" LDFLAGS="$(PC_LDFLAGS)"

# Build RPi version
rpi:
	$(MAKE) build OBJS="$(OBJS_RPI)" TARGET="$(TARGET_RPI)" CFLAGS="$(RPI_CFLAGS)" LDFLAGS="$(RPI_LDFLAGS)"

# Central build rule (uses OBJS, TARGET, CFLAGS, LDFLAGS provided by callers)
build: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Generic compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Toggle: read .last_platform and switch to the other one
toggle:
	@if [ -f .last_platform ]; then LAST=`cat .last_platform`; else LAST=pc; fi; \
		if [ "$$LAST" = "pc" ]; then $(MAKE) rpi; echo rpi > .last_platform; else $(MAKE) pc; echo pc > .last_platform; fi

# Clean both builds
clean:
	rm -f $(OBJS_PC) $(OBJS_RPI) $(TARGET_PC) $(TARGET_RPI) .last_platform

