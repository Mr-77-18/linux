#include "mess_rpc.h"

void usage(){
	printf("usage: [name] hostname/IPaddr\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage();
	}	

	struct Appendrpc ap_rpc;
	ap_rpc.term = 77;

	CLIENT* client = clnt_create(argv[1] , APPEND_PROG , APPEND_VER , "tcp");
	if (client == NULL) {
		printf("error:clnt_create\n");
		exit(1);
	}
	
	struct timeval timeout;
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;

	enum clnt_stat stat = clnt_call(client , APPEND_PROC , (xdrproc_t)xdr_append , (char*)&ap_rpc , (xdrproc_t)xdr_append , (char*)&ap_rpc , timeout);
	if (stat != RPC_SUCCESS) {
		clnt_perror(client , "Call failed");
		exit(1);
	}
	
	clnt_destroy(client);
	return 0;
}
