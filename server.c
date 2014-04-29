#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(){
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
}
