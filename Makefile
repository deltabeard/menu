CFLAGS = -Wall -Wextra -Werror -std=c89 -pedantic

all: menu.o ./test/test
menu.o: menu.c menu.h
./test/test: ./test/test.o menu.o
