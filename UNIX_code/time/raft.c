#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//定义状态
#define Leader 0
#define Candidate 1
#define Follower 2

//刚开始的时候都是Fllower;
int states = Follower;

//const char* ip, int  port, int time
struct Argc_To_Thread{
	char* ip;
	int port;
	int time;
};

void* time_thread(void* arg)
{
	//处理参数：
	struct Argc_To_Thread* p_argc_to_thread = (struct Argc_To_Thread*)arg;

	char* ip = p_argc_to_thread->ip;
	int port = p_argc_to_thread->port;
	int time = p_argc_to_thread->time;


	int ret = 0;
	struct sockaddr_in address;
	bzero(&address , sizeof(address));

	//设置地址
	address.sin_family = AF_INET;
	inet_pton(AF_INET , ip , &address.sin_addr);
	address.sin_port = htons(port);

	int sockfd = socket(PF_INET , SOCK_STREAM , 0);
	assert(sockfd >= 0);

	//设置超时时间
	struct timeval timeout;
	timeout.tv_sec = time;
	timeout.tv_usec = 0;
	socklen_t len = sizeof(timeout);
	ret = setsockopt(sockfd , SOL_SOCKET , SO_SNDTIMEO , &timeout , len);
	assert(ret != -1);

	while(1){
		//启动连接
		ret = connect(sockfd , (struct sockaddr*)&address , sizeof(address));

		if (ret == -1) {
			if (errno == EINPROGRESS) {
				printf("connecting timeout , process timeout logic\n");
				switch (states) {
					case Leader:
						break;
					case Candidate:
						break;
					case Follower:
						states = Candidate;
						break;
					default:
						break;
				}
			}
			printf("error occur when connecting to server\n");
		}
	}
	pthread_exit(0);
}

//主线程
int main(int argc, char *argv[])
{
	pthread_t thid;
	if (argc <= 2) {
		printf("usage: %s ip_address port_number\n" , argv[0]);
		return 1;
	}
	
	//准备线程参数
	struct Argc_To_Thread argc_to_thread;
	argc_to_thread.ip = argv[1];
	argc_to_thread.port = atoi(argv[2]);
	argc_to_thread.time = 1;

	pthread_create(&thid , NULL , time_thread , &argc_to_thread);


	while(1){
		switch (states) {
			case Leader:
				printf("in leader states\n");
				break;
			case Candidate:
				printf("in candidate states\n");
				break;
			case Follower:
				printf("in Fllower states\n");
				break;
			default:
				break;
		}
	}

	pthread_join(thid , NULL);

	return 0;
}
