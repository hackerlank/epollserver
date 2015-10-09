#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAXLINE 32
//#define OPEN_MAX 100
#define LISTENQ 200
//#define SERV_PORT 5000
//#define INFTIM 1000


void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts < 0)
	{
		printf("fcntl(sock, GETFL)");
		exit(1);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0)
	{
		printf("fcntl(sock, SETFL, opts)");
		exit(1);
	}
	printf("set nonblocking ok!\r\n");
}


int main(int argc, char* argv[]){
	int i, listenfd, connfd, sockfd, epfd, nfds;
	ssize_t n;
	char line[MAXLINE];
	socklen_t clilen;
	unsigned short portnumber = 0x8888;
	
	struct epoll_event ev,events[20];
	
	if((epfd = epoll_create(256)) < 0)
	{
		printf("epoll_create fail!");
		exit(1);
	}
	printf("epoll_create ok\r\n");

	struct sockaddr_in s_addr, c_addr;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket fail!\r\n");
		exit(1);
	}
	printf("socket ok\r\n");
	
	setnonblocking(listenfd);
	
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
	{
		printf("epoll_ctl ADD fail!\r\n");
		exit(1);
	}
	bzero(&s_addr, sizeof(struct sockaddr_in));

	s_addr.sin_family = AF_INET;
	char *local_addr = "0.0.0.0";
	inet_aton(local_addr, &(s_addr.sin_addr));
	s_addr.sin_port = htons(portnumber);

	if(bind(listenfd, (struct sockaddr *)(&s_addr), sizeof(struct sockaddr)) < 0)	
	{
		printf("bind fail!\r\n");
                exit(1);
	}
	printf("bind ok!\r\n");
	if((listen(listenfd, LISTENQ)) < 0)
	{
		printf("listen fail!\r\n");
		exit(1);
	}
	printf("listen OK!\r\n");

	for(;;)
	{
		nfds = epoll_wait(epfd, events, 20, 500);
		printf("epoll_wait: nfds = %d\r\n", nfds);
		for(i = 0; i<nfds; ++i)
		{
			if(events[i].data.fd == listenfd)
			{	
				printf("before accept!\r\n");
				connfd = accept(listenfd, (struct sockaddr *)&c_addr, &clilen);
				if(connfd < 0)
				{
					printf("connfd<0\r\n");
					exit(1);
				}	
				
				//char *str = inet_ntoa(c_addr.sin_addr);
				//printf("accept a connect from %s\r\n", str);

				printf("accept ok! connect from %#x:%#x\r\n", ntohl(c_addr.sin_addr.s_addr), ntohs(c_addr.sin_port));

				if(write(connfd, "welcome to server!\r\n", 32) < 0)
				{
					printf("write connfd fail!\r\n");
					continue;
				}
				ev.data.fd = connfd;
				
				ev.events = EPOLLIN | EPOLLET;

				if(epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) < 0) 
				{
					printf("epoll_ctl ADD fail\r\n");
					continue;
				}
			}
			else if(events[i].events & EPOLLIN)
			{
				printf("EPOLLIN\r\n");
				if((sockfd = events[i].data.fd) < 0)
					continue;
				if((n = read(sockfd, line, MAXLINE)) < 0)
				{
					printf("readline error\r\n");
					close(sockfd);
					events[i].data.fd = -1;
					continue;
				}
				else if(n == 0)
				{
					printf("no data to read\r\n");
					close(sockfd);
					events[i].data.fd = -1;
					continue;
				}
				line[n] = '\0';
				printf("read data: %s\r\n", line);
				memset(line, 0 , sizeof(line));

				ev.data.fd = sockfd;
				ev.events = EPOLLOUT | EPOLLET;

				//if(epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev) < 0)
				//{
				//	printf("epoll_ctl MOD EPOLLOUT fail\r\n");
				//	continue;
				//}
			}
			else if(events[i].events & EPOLLOUT)
			{
				printf("EPOLLOUT\r\n");
				sockfd = events[i].data.fd;
				write(sockfd, "server receive data ", 32);
				ev.data.fd = sockfd;
				
				ev.events = EPOLLIN | EPOLLET;
				
				//if(epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev) < 0)
				//{
				//	printf("epoll_ctl MOD EPOLLIN fail\r\n");
				//	continue;
				//}
			}
		}
	}
	return 0;

/*
	int sfp,nfp;
	struct sockaddr_in s_addr,c_addr;
	int sin_size;
	unsigned short portnum = 0x8888;
	char buffer[1024];
	printf("hello,welcome to my server !\r\n");
	sfp = socket(AF_INET, SOCK_STREAM, 0);
	if (sfp < 0 )
	{
		printf("socket fail! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");
	
	bzero(&s_addr, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(portnum);

	if(bind(sfp, (struct sockaddr *)(&s_addr), sizeof(struct sockaddr)) <0)
	{
		printf ("bind fail! \r\n");
		return -1;
	}
	printf("bind ok!\r\n");
	if(listen(sfp, 100) < 0)
	{
		printf("listen failed!\r\n");
		return -1;
	}
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);

		nfp = accept(sfp, (struct sockaddr*)(&c_addr), &sin_size);
		if(nfp < 0)
		{
			printf("accept fail! \r\n");
			return -1;
		}
		printf("accept ok!\r\n Server start get connect from %#x:%#x\r\n", ntohl(c_addr.sin_addr.s_addr), ntohs(c_addr.sin_port));
		
		if(write(nfp, "hello, welcome to my server \r\n", 32) < 0)
		{
			printf("write fail!\r\n");
			return -1;
		}
		printf("write ok!\r\n");
		read(nfp, buffer, sizeof(buffer));
		printf("pid = %s",buffer);
		close(nfp);
	}
	close(sfp);
	return 0;
*/
}
