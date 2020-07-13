CFLAGS = -Wall -Wextra -Werror -std=c89 -pedantic -Og -g3
TARGETS = menu.o ./test/test example

all: $(TARGETS)
example: example.c menu.o
menu.o: menu.c menu.h
./test/test: ./test/test.c menu.o

clean:
	$(RM) $(TARGETS)
