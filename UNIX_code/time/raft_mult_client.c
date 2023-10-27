//再raft_1.c的基础上继续完善，一步一步接近真实的raft
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
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


//rpc
#include "mess_rpc_mult_client.h"

//定义状态
#define Leader 0
#define Candidate 1
#define Follower 2

pid_t pid;

void sig_handler(int signo);

//刚开始的时候都是Fllower;
int states = Follower;

//const char* ip, int  port, int time
struct Argc_To_Thread{
	char* ip;
	int port;
	int time;
};

//信号处理
static jmp_buf jmpbuf;

//信号处理
void signal_init();
void signal_mask();

//获取rpc发送的消息
void get_vote_message();
void get_append_message();

//rpc初始化
void rpc_init();

//发送rpc
void send_appendrpc();
void send_requestVrpc();

void _send_appendrpc();
void _send_requestVrpc();

//同样的作为这个group里面的一员，也需要有被调用的rpc函数
static void rec_appendrpc(struct svc_req* rqstp , SVCXPRT* transp);
static void rec_requestVrpc(struct svc_req* rqstp , SVCXPRT* transp);

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
		if (sigsetjmp(jmpbuf , 1) == 0) {

			ret = connect(sockfd , (struct sockaddr*)&address , sizeof(address));
			if (ret == -1) {
				if (errno == EINPROGRESS) {
					printf("in time_thread");

					switch (states) {
						case Leader:
							break;
						case Candidate:
							break;
						case Follower:
							states = Candidate;
							term++;
							vote = 0;
							break;
						default:
							break;

					}
				}
			}
		}	
	}

	pthread_exit(0);
}

void* RPC_time_thread(void* arg)
{
	signal_mask();//忽略SIGALRM的信号处理
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
				switch (states) {
					case Leader:
						//周期执行AppendEntries RPC
						printf("in leader states , and send AppendEntries RPC\n");
						//思考需不需要status
						send_appendrpc();
						break;
					case Candidate:
						//周期执行RequestVote RPC
						printf("in candidate states , and send RequestVote RPC\n");
						//思考需不需要status
						send_requestVrpc();
						break;
					case Follower:
						printf("in Fllower states , nothing to send\n");
						break;
					default:
						break;
				}
			}
	}
	pthread_exit(0);
}

//主线程
int main(int argc, char *argv[])
{
	pid = getpid();
	signal_init();
	rpc_init();

	pthread_t thid[2];
	if (argc <= 2) {
		printf("usage: %s ip_address port_number\n" , argv[0]);
		return 1;
	}
	
	//准备线程1的参数
	struct Argc_To_Thread argc_to_thread1;
	argc_to_thread1.ip = argv[1];
	argc_to_thread1.port = atoi(argv[2]);
	argc_to_thread1.time = 8;

	//准备线程2的参数
	struct Argc_To_Thread argc_to_thread2;
	argc_to_thread2.ip = argv[1];
	argc_to_thread2.port = atoi(argv[2]);
	argc_to_thread2.time = 1;

	pthread_create(&thid[0] , NULL , time_thread , &argc_to_thread1);
	pthread_create(&thid[1] , NULL , RPC_time_thread , &argc_to_thread2);

	signal_mask();//忽略SIGALRM的信号处理

	svc_run();
	
	pthread_join(thid[0] , NULL);
	pthread_join(thid[1] , NULL);

	return 0;
}

void get_append_message(struct Appendrpc* _ap_rpc){
	_ap_rpc->term = term;
	_ap_rpc->success = 0;
}

void get_vote_message(struct Voterpc* _vt_rpc){
	_vt_rpc->term = term;
	_vt_rpc->success = 0;
}

//这个函数调用子函数_send_appendrpc()
//_send_appendrpc是向一个client发送，所以这里需要封装一次，要向所有ip发送
void send_appendrpc(){
	struct Appendrpc ap_rpc;
	get_append_message(&ap_rpc);

	for (int i = 0; i < CLIENT_SIZE; i++) {
		_send_appendrpc(Addrs[i] , ap_rpc);
	}
}

//这个函数调用子函数_send_requestVrpc()
//_send_requestVrpc是向一个client发送，所以这里需要封装一次，要向所有ip发送
void send_requestVrpc(){
	//调试
	printf("in send_requestVrpc");
	struct Voterpc vt_rpc;
	get_vote_message(&vt_rpc);

	for (int i = 0; i < CLIENT_SIZE; i++) {
		_send_requestVrpc(Addrs[i] , vt_rpc);
	}
}

//用户提供一个ipaddr，这个函数负责发送appendrpc，信息也要用户传
void _send_appendrpc(struct client_message _client_addr , struct Appendrpc _ap_rpc){
	//调试
	printf("in _send_appendrpc , the term is:%d\n" , term);
	CLIENT* client = clnt_create(_client_addr.addr , _client_addr.service_append , APPEND_VER , "tcp");
	if (client == NULL) {
		printf("error:clnt_create\n");
	}

	struct timeval timeout;
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;

	enum clnt_stat stat = clnt_call(client , APPEND_PROC , (xdrproc_t)xdr_append , (char*)&_ap_rpc , (xdrproc_t)xdr_append , (char*)&_ap_rpc , timeout);
	if (stat != RPC_SUCCESS) {
		clnt_perror(client , "Call failed");
	}

	clnt_destroy(client);
}

void try_tran_leader(int _success){
	if (_success) {
		vote++;
	}	
	if (vote >= CLIENT_SIZE % 2) {//允许成为leader
		states = Leader;//这里有一个思考，是不是要立马回到主流程啊，不必再向后续的client发送vote了
	}
}

//send vote rpc
void _send_requestVrpc(struct client_message _client_addr , struct Voterpc _vt_rpc){
	//调试
	printf("in _send_requestVrpc\n");
	CLIENT* client = clnt_create(_client_addr.addr , _client_addr.service_vote , VOTE_VER , "tcp");
	if (client == NULL) {
		printf("error:clnt_create\n");
	}

	struct timeval timeout;
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;

	enum clnt_stat stat = clnt_call(client , VOTE_PROC , (xdrproc_t)xdr_vote , (char*)&_vt_rpc , (xdrproc_t)xdr_vote , (char*)&_vt_rpc , timeout);
	if (stat != RPC_SUCCESS) {
		clnt_perror(client , "Call failed");
	}

	try_tran_leader(_vt_rpc.success);

	clnt_destroy(client);
}

static void rec_appendrpc(struct svc_req* rqstp , SVCXPRT* transp){
	//调试
	printf("in rec_appendrpc\n");
	switch (rqstp->rq_proc) {
		case NULLPROC:
			svc_sendreply(transp , (xdrproc_t)xdr_void , NULL);
			return ;
		case APPEND_PROC:
			break;
		default:
			svcerr_noproc(transp);
			return ;
	}

	//重新开始记（变为candidate的计时器）
	//alarm(1);

	struct Appendrpc ap_rpc;
	memset((char*)&ap_rpc , 0 , sizeof(ap_rpc));

	if (!svc_getargs(transp , (xdrproc_t)xdr_append , (caddr_t)&ap_rpc)) {
		svcerr_decode(transp);
		return;
	}

	if (ap_rpc.term >= term) {
		states = Follower;
		term = ap_rpc.term;
		kill(pid , 14);
	}


	printf("the term is %d\n" , ap_rpc.term);

	if (!svc_sendreply(transp , (xdrproc_t)xdr_append , (caddr_t)&ap_rpc)) {
		printf("errro:unbale to send reply\n");
		exit(1);
	}
	if (!svc_freeargs(transp , (xdrproc_t)xdr_append , (caddr_t)&ap_rpc)) {
		printf("error:unbale to free args\n");
		exit(1);
	}
}

static void rec_requestVrpc(struct svc_req* rqstp , SVCXPRT* transp){
	//调试
	printf("in rec_requestVrpc\n");
	switch (rqstp->rq_proc) {
		case NULLPROC:
			svc_sendreply(transp , (xdrproc_t)xdr_void , NULL);
			return ;
		case VOTE_PROC:
			break;
		default:
			svcerr_noproc(transp);
			return ;
	}

	struct Voterpc vt_rpc;
	memset((char*)&vt_rpc , 0 , sizeof(vt_rpc));

	if (!svc_getargs(transp , (xdrproc_t)xdr_vote , (caddr_t)&vt_rpc)) {
		svcerr_decode(transp);
		return;
	}

	if (vt_rpc.term > term) {//真实情况是需要根据不同点状态进行处理的
		states = Follower;
		term = vt_rpc.term;
		vt_rpc.success = 1;
		//kill(pid , 14);
	}

	if (!svc_sendreply(transp , (xdrproc_t)xdr_vote , (caddr_t)&vt_rpc)) {
		printf("errro:unbale to send reply\n");
		exit(1);
	}
	if (!svc_freeargs(transp , (xdrproc_t)xdr_vote , (caddr_t)&vt_rpc)) {
		printf("error:unbale to free args\n");
		exit(1);
	}
}

//rpc初始化
void rpc_init(){
	SVCXPRT* transpu = svcudp_create(RPC_ANYSOCK);
	if (transpu == NULL) {
		printf("svcudp_create error\n");
		exit(1);
	}

	//append
	if (!svc_register(transpu , APPEND_PROG , APPEND_VER , rec_appendrpc , IPPROTO_UDP)) {
		printf("error: cannot regiter (APPEND_PROG udp)\n") ;
		exit(1);
	}

	//vote
	if (!svc_register(transpu , VOTE_PROG , VOTE_VER , rec_requestVrpc , IPPROTO_UDP)) {
		printf("error: cannot regiter (VOTE_PROG udp)\n") ;
		exit(1);
	}

	SVCXPRT* transpt = svctcp_create(RPC_ANYSOCK , 0 , 0);
	if (transpt == NULL) {
		printf("error: cannot create tcp service\n")  ;
		exit(1);
	}

	//append
	if (!svc_register(transpt , APPEND_PROG , APPEND_VER , rec_appendrpc , IPPROTO_TCP)) {
		printf("error: cannot register (APPEND TCP)") ;
		exit(1);
	}

	//vote
	if (!svc_register(transpt , VOTE_PROG , VOTE_VER , rec_requestVrpc , IPPROTO_TCP)) {
		printf("error: cannot register (VOTE TCP)") ;
		exit(1);
	}


}

void signal_init(){
	struct sigaction sa;
	sa.sa_flags = 0;
	sa.sa_handler = sig_handler;
	sigaction(SIGALRM , &sa , NULL);
}

void signal_mask(){
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask , SIGALRM);

	pthread_sigmask(SIG_BLOCK , &signal_mask , NULL);
}

void sig_handler(int signo){
	printf("here\n");
	siglongjmp(jmpbuf , 1);
}

