#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

main()
{
	int fd;
	int r;
	struct sockaddr_in dr;

	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1) printf("socket err:%m\n"),exit(-1);


	dr.sin_family=AF_INET;
	dr.sin_port=htons(9999);
	dr.sin_addr.s_addr=inet_addr("192.168.234.129");
	
	r=connect(fd,(struct sockaddr*)&dr,sizeof(dr));
	if(r==-1) printf("connect err:%m\n"),exit(-1);

//	r=send(fd,"hello tty",9,0/*MSG_OOB*/);
	r=send(fd,"hello tty",9,MSG_OOB);
	if(r==-1) printf("send err:%m\n"),exit(-1);

	close(fd);
}
