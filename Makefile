# zrender
NAME = zrender
CC = gcc
CFLAGS = -Wall -Werror -std=c99 -DDEBUG_H -Wno-unused

main:
	$(CC) $(CFLAGS) -o $(NAME)-test zrender.c vendor/zhasher.c vendor/zwalker.c vendor/util.c main.c
