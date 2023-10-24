#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/clnt.h>
#include <rpc/svc.h>

//定义参数格式
struct Appendrpc{
	int term;
};

struct Voterpc{
	int term;
};

int xdr_append(XDR* xdrsp , struct Appendrpc* resp){
	if (!xdr_int(xdrsp , &resp->term)) {
		return 0;
	}

	return 1;
}

int xdr_vote(XDR* xdrsp , struct Voterpc* resp){
	if (!(!xdr_int(xdrsp , &resp->term))) {
		return 0;
	}

	return 1;
}

#define APPEND_PROG ((unsigned long)0x20000002)
#define APPEND_VER ((unsigned long)0x01)
#define APPEND_PROC ((unsigned long)0x01)

#define VOTE_PROG ((unsigned long)0x20000003)
#define VOTE_VER ((unsigned long)0x01)
#define VOTE_PROC ((unsigned long)0x01)

