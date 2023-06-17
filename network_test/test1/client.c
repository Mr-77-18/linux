#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


int buildConnect(const char *lIp, const char *sIp, int sPort)
{
	int skFd;
	if((skFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return 0;

	}

	struct sockaddr_in cliAddr;
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = inet_addr(lIp);
	cliAddr.sin_port = 0;//让内核自己去选择一个端口
	if(bind(skFd,  (struct sockaddr *)&cliAddr, sizeof(cliAddr)) < 0)
	{
		perror("bind()");

	}
	while(1);

	struct sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(sIp);
	srvAddr.sin_port = htons(sPort);
	if(connect(skFd, (struct sockaddr *)&srvAddr, sizeof(srvAddr)) < 0)
	{
		perror("connect()");
		return 0;

	}

	return skFd;

}


int main(int argc, char *argv[])
{
	char lip[16] , dip[16];
	strcpy(lip , argv[1]);
	strcpy(dip , argv[2]);
	int port = atoi(argv[3]);
	
	int fd = buildConnect(lip , dip , port);

	if (close(fd) < 0) {
		perror("close()");
	}

	return 0;
}
