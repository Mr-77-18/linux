/*! \brief: the code file is about the read method within standard io
 *
 *	i will use fopen() function to open file,and then use multi read method 
 *	to read , and compare the each function's differece.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("argc is too few");
		exit(1);
	}
	char *path;

	//open file by fopen function;
	FILE* ffd = fopen(path , "agv[1]");

	//use different read function to read
	//fread()
	//

	return 0;
}
