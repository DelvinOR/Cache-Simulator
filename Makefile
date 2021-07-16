all: first

first:
	gcc -g -lm -Wall -Werror -fsanitize=address -std=c99 first.c -o first
clean:
	rm -rf first
