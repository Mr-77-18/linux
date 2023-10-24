#include "mess_rpc.h"

static void AppendEntries(struct svc_req* rqstp , SVCXPRT* transp){
	//先试着输出传过来的参数term
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

	struct Appendrpc ap_rpc;
	memset((char*)&ap_rpc , 0 , sizeof(ap_rpc));

	if (!svc_getargs(transp , (xdrproc_t)xdr_append , (caddr_t)&ap_rpc)) {
		svcerr_decode(transp);
		return;
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

	return;
}

static void RequestVote(struct svc_req* rqstp , SVCXPRT* transp){

}

int main(int argc, char *argv[])
{
	SVCXPRT* transpu = svcudp_create(RPC_ANYSOCK);
	if (transpu == NULL) {
		printf("svcudp_create error\n");
		exit(1);
	}

	if (!svc_register(transpu , APPEND_PROG , APPEND_VER , AppendEntries , IPPROTO_UDP)) {
		printf("error: cannot regiter (APPEND_PROG udp)\n")	;
		exit(1);
	}

	SVCXPRT* transpt = svctcp_create(RPC_ANYSOCK , 0 , 0);
	if (transpt == NULL) {
		printf("error: cannot create tcp service\n")	;
		exit(1);
	}

	if (!svc_register(transpt , APPEND_PROG , APPEND_VER , AppendEntries , IPPROTO_TCP)) {
		printf("error: cannot register (APPEND TCP)")	;
		exit(1);
	}

	svc_run();

	printf("error: svc_run return\n");
	exit(1);
}
