CFLAGS = -Wall -Wextra -Werror -std=c89 -pedantic -Og -g3

all: menu.o test/test examples
menu.o: menu.c menu.h
test/test: ./test/test.c menu.o
examples:
	$(MAKE) -C examples

clean:
	$(RM) menu.o ./test/test

.PHONY: examples
