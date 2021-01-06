# zrender
NAME = s
CC = clang
DFLAGS = -g -O0 -Wall -Werror -std=c99 -Wno-unused -fsanitize=address -fsanitize-undefined-trap-on-error -DDEBUG_H
CFLAGS = -g -O0 -Wall -Werror -std=c99 -Wno-unused
SRC = ../zhasher.c ../zwalker.c

main:
	$(CC) $(CFLAGS) -o $(NAME)test $(SRC) simp.c

debug: CFLAGS = $(DFLAGS)
debug:
	$(CC) $(CFLAGS) -o $(NAME)test $(SRC) simp.c
