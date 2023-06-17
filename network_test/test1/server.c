#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{

	int lfd= socket(AF_INET , SOCK_STREAM , 0);

	if (lfd < 0) {
		perror("socket");
	}

	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(8081);

	if (bind(lfd , (struct sockaddr*)&servAddr , sizeof(servAddr)) < 0) {
		perror("bind()");
	}
	
	if (listen(lfd , 1024) < 0) {
		perror("listen");
	}

	while(1);

	return 0;
}
