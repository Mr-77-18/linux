all: ipc

ipc: ipc.c
	gcc -o ipc ipc.c -Dshm=0 -Dsem=1 -Dmsg=0

PHONY: clean

clean:
	rm ipc 
