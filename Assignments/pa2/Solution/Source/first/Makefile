all:first

first: trie.h first.h trie.c first.c
	gcc -Wall -Werror -fsanitize=address trie.c first.c -o first

clean:
	rm -rf first 
