# zrender
NAME = zrender
CC = clang
CFLAGS = -Wall -Werror -std=c99 -Wno-unused
#CC = gcc
#CFLAGS = -Wall -Werror -Wno-unused #-DDEBUG_H 
SRC = zhasher.c zwalker.c zrender.c

main:
	$(CC) $(CFLAGS) -o $(NAME)-test $(SRC) main.c

debug: CFLAGS += -g -O0 -fsanitize=address -fsanitize-undefined-trap-on-error -DDEBUG_H
debug: main 
	@printf '' >/dev/null

examples:
	$(CC) $(CFLAGS) -o ex example.c zrender.c vendor/zhasher.c vendor/zwalker.c

clean:
	rm -f *.o vendor/*.o
