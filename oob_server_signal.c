/**
 * oob_server_signal.c
 **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

int fd,cfd;	//server fd和client fd必须在同一级（内存中是在同一个栈中）,分开就会出错
void handle(int s)
{
	int r;
	char data[100];
//	if(s==SIGURG)
//	{
		r=recv(cfd,data,100,MSG_OOB);
		if(r>0)
		{
			data[r]=0;
			printf("接收带外数据：%s\n",data);
		}
//	}
}
main()
{
	int r;
	char buf[1024];
	struct sockaddr_in dr;

	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1) printf("socket err:%m\n"),exit(-1);

	dr.sin_family=AF_INET;
	dr.sin_port=htons(9999);
	dr.sin_addr.s_addr=inet_addr("192.168.245.149");
	
	r=bind(fd,(struct sockaddr*)&dr,sizeof(dr));	
	if(r==-1) printf("bind err:%m\n"),exit(-1);

	r=listen(fd,10);
	if(r==-1) printf("listen err:%m\n"),exit(-1);

	cfd=accept(fd,0,0);	//地址和长度设为0时，表示不限定客户
	if(cfd==-1) printf("accept err:%m\n"),exit(-1);
	
	fcntl(cfd,F_SETOWN,getpid());
	signal(SIGURG,handle);
	while(1)
	{
		r=recv(cfd,buf,1024,0);
		if(r>0)
		{
			buf[r]=0;
			printf("普通数据：%s\n",buf);
		}
		else
		{
			break;
		}
	}
	close(fd);
}
