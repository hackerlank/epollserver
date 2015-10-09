#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <pthread.h>

#define PTHREAD_MAX 300
#define SERVER_IP   "0.0.0.0"
void *connect_server(void *arg)
{
	int cfd;
	int recbytes;
	int sin_size;
	char buffer[1024]={0};
	struct sockaddr_in s_addr, c_addr;
	unsigned short portnum = 0x8888;
	int connect_times = *(int *)arg;
	printf ("Hello,welcome to client connect_num = %d!\r\n", connect_times);
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd < 0)
	{
		printf("socket fail!\r\n");
		return;
	}
	
	printf("socket ok!\r\n");
	
	bzero(&s_addr, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	s_addr.sin_port = htons(portnum);
	printf("s_addr=%#x, port:%#x\r\n", s_addr.sin_addr.s_addr, s_addr.sin_port);
	
	if(connect(cfd, (struct sockaddr *)(&s_addr), sizeof(struct sockaddr)) < 0)
	{
		printf("connect fail ! \r\n");
		return;
	}
	
	//sleep (10);
	if((recbytes = read(cfd, buffer, sizeof(buffer))) < 0)
	{
		printf("read data fail! \r\n");
		return;
	}
	printf("read ok\r\nREC:\r\n");
	buffer[recbytes]='\0';
	printf("%s\r\n", buffer);
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "this is client id=%d", connect_times);
	write(cfd, buffer, sizeof(buffer));
	//getchar();
	close(cfd);
	//sleep(100);
	return;
}

int connect_num[PTHREAD_MAX];
int main(int agrc, char* argv[])
{
	int i;
	for(i = 0; i <PTHREAD_MAX; i++)
	{
		connect_num[i] = i;
		pthread_t pid;
		if(pthread_create(&pid, NULL, connect_server, (void *)&connect_num[i]) < 0)
		{	
			printf("create pthread error!  = %ld\r\n", pid);
			continue;
		}
		printf("create pthread ok! i = %u\r\n", (unsigned int)pid);
	}
	sleep(100);
	return 0;

}
