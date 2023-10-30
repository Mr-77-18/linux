all: thread_pool

thread_pool: thread_pool.c
	gcc -o thread_pool thread_pool.c -pthread

PHONY: clean

clean:
	rm thread_pool
