all: ipc clone_vm

ipc: ipc.c
	gcc -o ipc ipc.c -Dshm=0 -Dsem=1 -Dmsg=0

clone_vm: clone_vm.c
	gcc -o clone_vm clone_vm.c
	

PHONY: clean

clean:
	rm ipc clone_vm
