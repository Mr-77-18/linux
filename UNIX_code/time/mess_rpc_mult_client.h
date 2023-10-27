#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/clnt.h>
#include <rpc/svc.h>

static int term = 0;
static int vote = 0;

//定义参数格式
struct Appendrpc{
	//发送信息
	int term;
	//回复信息
	int success;
};

struct Voterpc{
	//发送信息
	int term;
	//回复信息
	int success;
};

int xdr_append(XDR* xdrsp , struct Appendrpc* resp){
	if (!xdr_int(xdrsp , &resp->term)) {
		return 0;
	}

	if (!xdr_int(xdrsp , &resp->success)) {
		return 0;
	}

	return 1;
}

int xdr_vote(XDR* xdrsp , struct Voterpc* resp){
	if (!xdr_int(xdrsp , &resp->term)) {
		return 0;
	}

	if (!xdr_int(xdrsp , &resp->success)) {
		return 0;
	}

	return 1;
}

//注册服务
#define APPEND_PROG ((unsigned long)0x20000003)
#define APPEND_VER ((unsigned long)0x01)
#define APPEND_PROC ((unsigned long)0x01)

#define VOTE_PROG ((unsigned long)0x20000004)
#define VOTE_VER ((unsigned long)0x01)
#define VOTE_PROC ((unsigned long)0x01)


#define CLIENT_SIZE 2

struct client_message{
	char* addr;
	unsigned long service_append;
	unsigned long service_vote;
};

struct client_message Addrs[] = {
	{"192.168.110.129" , 0x20000003 , 0x20000004},
	{"192.168.110.129" , 0x20000005  , 0x20000006}
};

//char* Client_Addr[] = {
//	"192.168.110.129"
//	"192.168.110.128"
//};
