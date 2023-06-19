/*! \brief use the function in strings.h and string.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX_LENGTH  1024

//in strings.h
void use_strcasecmp(void);
void use_strncasecmp(void);
void use_index(void);
void use_rindex(void);

//in string.h
void use_stpcpy(void);
void use_strcat(void);
void use_strchr(void);
void use_strcmp(void);
void use_strcoll(void);
void use_strcpy(void);
void use_strcspn(void);
void use_strdup(void);
void use_strfry(void);
void use_strlen(void);
void use_strncat(void);
void use_strncmp(void);
void use_strncpy(void);
void use_strpbrk(void);
void use_strrchr(void);
void use_strsep(void);
void use_strspn(void);
void use_strstr(void);
void use_strtok(void);
void use_strxfrm(void);

int main(int argc, char *argv[])
{
	//use_stpcpy();
	//use_strcpy();
	//use_strdup();
	use_strchr();
	return 0;
}
void use_index(void);
void use_rindex(void);

//copy
void use_stpcpy(void){
	char sbuf[MAX_LENGTH] = {"hello\0"};
	char dbuf[MAX_LENGTH];

	char* point = stpcpy(dbuf , sbuf);

	printf("src is:%s\n" , sbuf);
	printf("dest is:%s\n" , dbuf);
	printf("point to the char:%c\n" ,*point);
}

void use_strcpy(void){
	char sbuf[MAX_LENGTH] = {"hello\0"};
	char dbuf[MAX_LENGTH];

	char* point = strcpy(dbuf , sbuf);

	printf("src is:%s\n" , sbuf);
	printf("dest is:%s\n" , dbuf);
	printf("point to the char:%c\n" ,*point);
}

void use_strncpy(void){
	char sbuf[MAX_LENGTH] = {"hello\0"};
	char dbuf[MAX_LENGTH];

	char* point = strncpy(dbuf , sbuf , MAX_LENGTH);

	printf("src is:%s\n" , sbuf);
	printf("dest is:%s\n" , dbuf);
	printf("point to the char:%c\n" ,*point);
}

//复制，帮你开辟好空间,不用你预先设置
void use_strdup(void){
	char buf[MAX_LENGTH] = {"liusenhong\0"};

	printf("buf is %s\n" , buf);
	printf("and strdup()'s return is %s\n" , strdup(buf));
}

void use_strchr(void){
	char buf[MAX_LENGTH] = {"liusenhong\0"};
	char find = 'o';

	printf("the point to the first occurrence of the char %c is %c\n" , find , *strchr(buf , (int)find));
}



//compare
void use_strcasecmp(void);
void use_strncasecmp(void);
void use_strcmp(void){
	char buf1[MAX_LENGTH] = {"liusenhong\0"};
	char buf2[MAX_LENGTH] = {"liusenhong\0"};

	printf("buf1 is %s\n" , buf1);
	printf("buf2 is %s\n" , buf2);
	printf("the return value of strcmp() is :%d\n" , strcmp(buf1 , buf2));
}

void use_strncmp(void){
	char buf1[MAX_LENGTH] = {"liusenhong\0"};
	char buf2[MAX_LENGTH] = {"liusenhong\0"};

	printf("buf1 is %s\n" , buf1);
	printf("buf2 is %s\n" , buf2);
	printf("the return value of strcmp() is :%d\n" , strncmp(buf1 , buf2 , MAX_LENGTH));
}

void use_strcoll(void);
void use_strcat(void);
void use_strcspn(void);
void use_strfry(void);
void use_strlen(void);
void use_strncat(void);
void use_strpbrk(void);
void use_strrchr(void);
void use_strsep(void);
void use_strspn(void);
void use_strstr(void);
void use_strtok(void);
void use_strxfrm(void);


