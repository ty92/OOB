/*
 * oob_server_select.c
 * gcc oob_server_select.c -o oob_server_select
 * ./oob_server_select ip port
 */
 #include <stdio.h>
 #include <stdbool.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/epoll.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <assert.h>
 #include <errno.h>
 #include <fcntl.h>
 #include <stdlib.h>
 #include <string.h>
  #include <pthread.h>
 
 int main(int argc, char **argv)
 {
     if(argc <= 2) {
         printf("please input ip address and port num\n");
         exit(EXIT_FAILURE);
     }
 
     const char *ip = argv[1];
     int port = atoi(argv[2]);
 
     int ret = 0;
     struct sockaddr_in address;
     bzero(&address,sizeof(address));
     address.sin_family = AF_INET;
     inet_pton(AF_INET,ip,&address.sin_addr);
     address.sin_port = htons(port);
 
     int listenfd = socket(AF_INET,SOCK_STREAM,0);
     assert(listenfd >= 0);
 
     ret = bind(listenfd,(struct sockaddr*)&address,sizeof(address));
     assert(ret != -1);
 
     ret = listen(listenfd,5);
     assert(ret != -1);
 
     struct sockaddr_in client_address;
     socklen_t client_addresslen = sizeof(client_address);
     int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addresslen);
 
     if(connfd < 0){
         printf("accept error\n");
         exit(EXIT_FAILURE);
     }
 
     char buf[1024];     //buf缓冲区接收数据（普通数据和带外数据）
     fd_set read_fds;    //接收普通数据,放入读事件集合中
     fd_set exceptions_fds;   //接收带外数据，发生异常，放入异常事件集合中
 
     FD_ZERO(&read_fds);
     FD_ZERO(&exceptions_fds);
 
     while(1) {
         FD_SET(connfd,&read_fds);
         FD_SET(connfd,&exceptions_fds);
         ret = select(connfd+1,&read_fds,NULL,&exceptions_fds,NULL);
 
         if(ret < 0) {
             printf("select error\n");
             exit(EXIT_FAILURE);
         }
 
         if(FD_ISSET(connfd,&read_fds)){
             memset(buf,0x00,sizeof(buf));
             ret = recv(connfd,buf,sizeof(buf)-1,0);
             if(ret <= 0)
                 break;
             printf("recv data form %d and buf is %s\n",connfd,buf);
         } 
         
         if(FD_ISSET(connfd,&exceptions_fds)) {
             memset(buf,0x00,sizeof(buf));
             ret =recv(connfd,buf,sizeof(buf)-1,MSG_OOB);
             if(ret <= 0)
                 break;
             printf("recv oob data form %d and buf is %s \n",connfd,buf);
         }
     }
     close(connfd);
     close(listenfd);
     return 0;
 }                        
