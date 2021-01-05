# zrender
NAME = s
CC = clang
CFLAGS = -g -O0 -Wall -Werror -std=c99 -Wno-unused -fsanitize=address -fsanitize-undefined-trap-on-error -DDEBUG_H
CFLAGS = -g -O0 -Wall -Werror -std=c99 -Wno-unused -DDEBUG_H
SRC = ../zhasher.c ../zwalker.c ../zrender.c

main:
	$(CC) $(CFLAGS) -o $(NAME)test $(SRC) simp.c
