//coding by Senhong Liu
//
//time:10/31/2023
//
//内容是关于System V的共享内存

#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/sem.h>

#define BUF_SIZE 1024

pthread_mutex_t mutex;

void init();
void destroy();

//实现的用途是两个进程之间交换数据
void use_shm();
//实现两个进程之间的同步
void use_sem();
//实现的用途是两个进程之间传递消息
void use_msg();

int main(int argc, char *argv[])
{
	init();

#if shm
	use_shm();
#endif

#if sem
	use_sem();
#endif

#if msg
	use_msg();
#endif

	destroy();

	exit(0);
}

void use_shm(){
	int id = shmget(IPC_PRIVATE , BUF_SIZE , IPC_CREAT);
	if (id	< 0) {
		perror("shmget");
		exit(1);
	}

	pid_t pid = fork();
	if (pid == 0) {//in child process
		pthread_mutex_lock(&mutex);

		char* pt = (char*)shmat(id , NULL , 0);
		if ((void*)pt == (void*)-1) {
			perror("shmat");
			exit(1);
		}

		const char* text = "hello world\n";

		strcpy(pt , text);

		pthread_mutex_unlock(&mutex);

		printf("after child process\n");
		
		shmdt((void*)pt);

		exit(0);
	}else{//in parent process

		printf("in parent process\n");

		pthread_mutex_lock(&mutex);

		void* pt = shmat(id , NULL , 0);

		pthread_mutex_unlock(&mutex);

		printf("the share data is %s" , (char*)pt);
		shmdt(pt);
	}
}

void use_sem(){
	int id = semget(IPC_PRIVATE , 1 , IPC_CREAT);
	if (id < 0) {
		perror("semget");
		exit(1);
	}

	struct sembuf ops[2];
	ops[0].sem_num = 0;
	ops[0].sem_op = -1;
	ops[0].sem_flg = SEM_UNDO;

	ops[1].sem_num = 0;
	ops[1].sem_op = 1;
	ops[1].sem_flg = SEM_UNDO;

	pid_t pid = fork();
	if (pid == 0) {
		//child process
		
		semop(id , &ops[0] , 1);

		printf("in child process\n");

		semop(id , &ops[1] , 1);
	}else{
		//parent process
		semop(id , &ops[0] , 1);

		printf("in praent process\n");

		semop(id , &ops[1] , 1);
	}
}

void use_msg(){
	int id = msgget(IPC_PRIVATE , IPC_CREAT);

	pid_t pid = fork();
	if (pid == 0) {
		//child
		const char* test = "hello world\n";
	
		pthread_mutex_lock(&mutex);

		int ret = msgsnd(id , test , sizeof(test) , 0);

		pthread_mutex_unlock(&mutex);

		if (ret < 0) {
			perror("msgsnd");
			exit(1);
		}
		exit(0);
	}else{
		//parent
		char* buf = malloc(sizeof(char)*BUF_SIZE);
		pthread_mutex_lock(&mutex);

		ssize_t size = msgrcv(id , buf , BUF_SIZE , 0 , 0);

		pthread_mutex_unlock(&mutex);
		if (size < 0) {
			perror("msgrcv");
			exit(1);
		}

		printf("the data is:%s" , buf);
	}
}

void init(){
	pthread_mutex_init(&mutex , NULL);
}

void destroy(){
	pthread_mutex_destroy(&mutex);
}
