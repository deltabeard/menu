CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -Og -g3

override example_sdl: CFLAGS += $(shell sdl2-config --cflags) -DPICOFONT_9x15
override example_sdl: LDLIBS += $(shell sdl2-config --libs)

all: example example_sdl
example: ../menu.o
example_sdl: ../menu.o sdl2_picofont.o
clean:
	$(RM) example example_sdl
