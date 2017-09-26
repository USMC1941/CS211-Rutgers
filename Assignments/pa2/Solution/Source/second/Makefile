all:second

second: trie.h second.h trie.c second.c
	gcc -Wall -Werror -fsanitize=address trie.c second.c -o second

clean:
	rm -rf second 
