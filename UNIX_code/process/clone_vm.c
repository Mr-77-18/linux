#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024

void clone_function();
void fork_function();
int child_function(void*);

char* buf;

int main(int argc, char *argv[])
{
	//clone_function();

	fork_function();
	
	return 0;
}

void clone_function(){
	buf = malloc(sizeof(char)*BUF_SIZE);
	char* stack;
	stack = malloc(65536);

	int id = clone(child_function , stack + 65536, CLONE_VM | SIGCHLD , NULL);
	if (id < 0) {
		perror("clone");
		printf("%s\n" , strerror(errno));
		exit(1);
	}
	
	sleep(5);

	printf("the messaeg from child is:%s\n" , buf);
}

int child_function(void* arg){
	const char* text = "hello world\n";

	strcpy(buf , text);

	return 0;
}

void fork_function(){
	buf = malloc(sizeof(char)*BUF_SIZE);

	pid_t pid = fork();
	if (pid == 0) {
		//child
		const char* text = "hello world\n";
		strcpy(buf , text);

		printf("in child , the message is:%s" , buf);
		exit(0);
	}else{
		//parent
		sleep(5);
		printf("in parent the message from child is:%s\n" , buf);
	}
}
