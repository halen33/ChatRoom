#include "t.h"
int main(int argc,char *argv[])
{
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(Port);
	serveraddr.sin_addr.s_addr=inet_addr(Ip);
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	//监听绑定
	if(listener<0)
	{
		perror("create listener error\n"); 
	}
	bind(listener,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	int recv=listen(listener,128);
	if(recv<0)
	{
		perror("listen error");
		exit(-1);
	}
	printf("listen is created\n");
	//epoll绑定
	int epollfd=epoll_create(Epoll_Size);//创建最大数句柄
	if(epollfd<0)
	{
		perror("epollfd wrong");
	}
	printf("epollfd is %d\n",epollfd);
	static struct epoll_event events[Epoll_Size];
	addfd(epollfd,listener,true);
	//循环等待
	while(1)
	{
		char buf[Buf_Size];
		bzero(buf,Buf_Size);
		int ep_count=epoll_wait(epollfd,events,Epoll_Size,-1);
		if(ep_count<0)
		{
			perror("get ep_count fault");
		}
		for(int i=0;i<ep_count;i++)
		{
			int socket=events[i].data.fd;
			//有新用户连接
			//接收
			if(listener==socket)
			{
				struct sockaddr_in 	clientaddr;
				socklen_t client_Length=sizeof(struct sockaddr_in);
				//接收
				int clientfd=accept(socket,(struct sockaddr *)&clientaddr,&client_Length);
				printf("client connection from:%s:%d(IP:port),clientfd=%d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port),clientfd);
				//添加到epoll
				addfd(epollfd,clientfd,true);
				//将clientfd添加到链表
				client_list.push_back(clientfd);
				printf("welcome new clientfd=%d to epoll\n",clientfd);
				printf("now there is %d client in the chatroom\n ",(int)client_list.size());
				sprintf(buf,server_Welcome,clientfd);	
				//向用户发送欢迎消息
				int rec=send(clientfd,buf,sizeof(buf),0);	
				if(rec<0)
				{
					perror("send error");
					exit(-1);
				}
			}
			else//传消息
			{
				int rec=sendboradcast(socket);
				if(rec<0)
				{
					perror("send message error");
					exit(-1);
				}

			}
		}	
	}
	close(listener);
	close(epollfd);
}

