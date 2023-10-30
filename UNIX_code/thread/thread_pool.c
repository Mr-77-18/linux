#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREAD 4
#define MAX_ELEMENT 10

struct task_t{
	char* argc;
};

struct thread_pool{
	struct task_t* tasks;
	int head , tail , count;

	pthread_mutex_t mutex;
	pthread_cond_t full , empty;

	pthread_t thread_id[MAX_THREAD];
};

struct thread_pool* thp;

void* thread(void*);

void thread_pool_init();

void task_function(void* argc);

void en_task();

int main(int argc, char *argv[])
{
	thread_pool_init();

	en_task('h');
	en_task('w');

	for (int i = 0; i < MAX_THREAD ; i++) {
		pthread_join(thp->thread_id[i] , NULL);
	}

	return 0;
}

void thread_pool_init(){
	thp = (struct thread_pool*)malloc(sizeof(struct thread_pool));
	thp->tasks = (struct task_t*)malloc(sizeof(struct task_t)*MAX_ELEMENT);

	pthread_mutex_init(&(thp->mutex) , NULL);
	pthread_cond_init(&thp->full , NULL);
	pthread_cond_init(&thp->empty , NULL);

	thp->head = 0;
	thp->tail = 0;
	thp->count = 0;

	for (int i = 0; i < MAX_THREAD; i++) {
		pthread_create((thp->thread_id) + i , NULL , thread , NULL);
	}
}

void* thread(void* argc){
	while(1){
		pthread_mutex_lock(&(thp->mutex));

		if (thp->count <= 0) {
			pthread_cond_wait(&(thp->full) , &(thp->mutex));
		}

		struct task_t task = *(thp->tasks + thp->head);

		printf("the args is %c\n" , (char*)task.argc);

		thp->count--;
		thp->head = (thp->head + 1) % MAX_ELEMENT;

		pthread_cond_signal(&(thp->empty));

		pthread_mutex_unlock(&thp->mutex);
	}
}

void en_task(void* argc){
	pthread_mutex_lock(&(thp->mutex));

	struct task_t* task = (struct task_t*)malloc(sizeof(struct task_t));
	task->argc = (char*)argc;

	if (thp->count == MAX_ELEMENT) {
		pthread_cond_wait(&(thp->empty) , &(thp->mutex));
	}

	int size = (thp->tail + 1) % MAX_ELEMENT;
	*(thp->tasks + size) = *task; 

	thp->tail = (thp->tail + 1) % MAX_ELEMENT;
	thp->count++;

	pthread_cond_signal(&(thp->full));

	pthread_mutex_unlock(&(thp->mutex));
}

