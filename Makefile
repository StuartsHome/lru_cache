run: build link  
	@echo complete
	
build:
	gcc -c lru_cache.c -o lru_cache.o

link:
	gcc lru_cache.o -L ../hashmap/ -lhashmap
