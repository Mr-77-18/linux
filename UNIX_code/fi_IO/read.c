/*! \brief: the code file is about the read method within file io
 *
 *	i will use open function to open file,and then use multi read method 
 *	to read , and compare the each function's differece.
 *
 *  function family:
 *  open()
 *  close()
 *  read()
 *  write()
 * 	lseek()
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LENGTH 1024

void copy_file_1(char* spath, char* dpath)
{
	//open spath1;
	int sfd = open(spath , O_RDONLY);
	if (sfd < 0) {
		perror("open() spath");
		exit(1);
	}
	int dfd = open(dpath , O_WRONLY);
	if (dfd < 0) {
		perror("open() dpath");
		exit(1);
	}

	//sfd --> dfd
	char* rbuf = malloc(sizeof(char)*MAX_LENGTH);
	int rn = 0;
	while((rn = read(sfd , rbuf , MAX_LENGTH)) > 0){
		write(dfd , rbuf , rn);
	}
	if (rn < 0) {
		perror("read()");
		exit(1);
	}

	close(sfd);
	close(dfd);
}


/*! \brief this function is proved accoding to copy_file_1 function
 *
 * proved:
 * 1. create file in dpath if have not the file in dpath
 * 2. if write is interrupt by signal or other thing , should write agagin
 *
 * \return Return parameter description
 */

void copy_file_2(char* spath, char* dpath)
{
	//open spath1;
	int sfd = open(spath , O_RDONLY);
	if (sfd < 0) {
		perror("open() spath");
		exit(1);
	}
	int dfd = open(dpath , O_CREAT | O_WRONLY);//creat if there has not file
	if (dfd < 0) {
		perror("open() dpath");
		exit(1);
	}

	//sfd --> dfd
	char* rbuf = malloc(sizeof(char)*MAX_LENGTH);
	int rn = 0;
	int wn = 0;
	while((rn = read(sfd , rbuf , MAX_LENGTH)) > 0){
		wn = 0;
		while((wn = write(dfd , rbuf + wn, rn)) != rn){
			rn = rn - wn;
		}
	}
	if (rn < 0) {
		perror("read()");
		exit(1);
	}

	close(sfd);
	close(dfd);
}

int main(int argc, char *argv[])
{
	if(argc < 3){
		printf("argc is too few , Usage is exefile [spath] [dpath]");
		exit(1);
	}

	copy_file_2(argv[1] , argv[2]);

	return 0;
}
