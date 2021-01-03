# zrender
NAME = zrender
CC = clang 
CFLAGS = -Wall -Werror -std=c99 -Wno-unused -fsanitize=address \
	-fsanitize-undefined-trap-on-error
#CC = gcc
#CFLAGS = -Wall -Werror -Wno-unused #-DDEBUG_H 

main:
	$(CC) $(CFLAGS) -o $(NAME)-test zrender.c vendor/zhasher.c vendor/zwalker.c vendor/util.c main.c

examples:
	$(CC) $(CFLAGS) -o ex example.c zrender.c vendor/zhasher.c vendor/zwalker.c vendor/util.c
	
