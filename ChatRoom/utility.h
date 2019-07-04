#ifndef T_H_INCLUDED
#define T_H_INCLUDED

#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
list<int> clients_list;

/**********************   macro defintion **************************/
// server ip
#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

//epoll size
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say to everyone>> %s"

#define ERROR_MESSAGE "the client %d you find is not in chatroom yet"
// exit
#define EXIT "EXIT"

#define CAUTION "There is only one int the char room!"

#define ONE_TO_ONE  "clientid %d say to you only>>%s"

#define	TRUE	1
#define	FALSE	0
#define	BUFSIZE	1500
#define	IPSIZE	16
#define	TIMEOUT	3

//int serverfd;
pthread_mutex_t acclock;

typedef struct
{
	void * (*user)(void * arg);
	void * arg;
}task_t;//任务队列
typedef struct
{
	int type;//私聊还是群发
	int fromid;
	int toid;
	char content[BUFSIZE];

}Msg;

typedef struct
{
	int thread_shutdown;

	int thread_max;
	int thread_min;

	int thread_alive;
	int thread_busy;
	int thread_wait;

	int queue_max;
	int queue_size;
	int queue_front;
	int queue_rear;

	pthread_t * tids;
	task_t * queue_task;

	pthread_mutex_t lock;
	pthread_cond_t not_full;
	pthread_cond_t not_empty;

	pthread_t managerid;
}pool_t;
//传线程池 管理者动态扩容缩减


//给任务队列添加任务,并唤醒消费者
int Thread_Add_Task(pool_t * pool,void *(*job)(void*),void *arg)
{
	pthread_mutex_lock(&pool->lock);
	//生产生阻塞于队列满
	while(pool->queue_size == pool->queue_max && pool->thread_shutdown==TRUE)
	{
		pthread_cond_wait(&pool->not_full,&pool->lock);
	}
	if(pool->thread_shutdown == FALSE){
		printf("Producer:0x%x Shutdown is False Exiting...\n",(unsigned int)pthread_self());
		pthread_exit(NULL);
	}
	//被唤醒向任务队列添加一个任务,并且计算任务位置,当前任务数+1
	pool->queue_task[pool->queue_front].user = job;
	pool->queue_task[pool->queue_front].arg = arg;
	++(pool->queue_size);
	pool->queue_front = (pool->queue_front + 1 ) % pool->queue_max;
	pthread_mutex_unlock(&pool->lock);
	//添加任务后,唤醒任意一个消费者
	pthread_cond_signal(&pool->not_empty);
	return 0;
}
//调用任务
void * Thread_Custom_Job(void * arg)
{
	pool_t * pool = (pool_t *)arg;
	task_t task;
	//消费者循环使用
	printf("Custom Thread:0x%x  Waiting Job..\n",(unsigned int)pthread_self());
	while(1){
		//如果队列为空阻塞
		pthread_mutex_lock(&pool->lock);
		while(pool->queue_size == 0 && pool->thread_shutdown == TRUE){
			pthread_cond_wait(&pool->not_empty,&pool->lock);
		}
		if(pool->thread_shutdown==FALSE){
			--pool->thread_alive;
			pthread_mutex_unlock(&pool->lock);
			printf("Custom Thread:0x%x  Shutdown Is False Exiting..\n",(unsigned int)pthread_self());
			pthread_exit(NULL);
		}
		//被唤醒判断是否结束自身
		if(pool->thread_wait > 0)
		{
			--pool->thread_wait;
			--pool->thread_alive;
			pthread_mutex_unlock(&pool->lock);
			printf("Custom Thread:0x%x  Exiting..\n",(unsigned int)pthread_self());
			pthread_exit(NULL);
		}
		//任务队列拿取一个任务,调用执行,计算索引值,当前任务数--,
		task.user = pool->queue_task[pool->queue_rear].user;
		task.arg = pool->queue_task[pool->queue_rear].arg;
		--pool->queue_size;
		pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max;
		++pool->thread_busy;
		pthread_mutex_unlock(&pool->lock);
		//调用任务
		printf("Custom Thread:0x%x Wroking.....\n",(unsigned int)pthread_self());
		(*task.user)(task.arg);
		pthread_mutex_lock(&pool->lock);
		--pool->thread_busy;
		pthread_mutex_unlock(&pool->lock);
		//保证队列非满,唤醒一个生产者继续生产
		pthread_cond_signal(&pool->not_full);
	}
}
//判断线程是否存活
int if_test_threadalive(pthread_t tid)
{
	if((pthread_kill(tid,0))==-1)
		if(errno == ESRCH)
			return FALSE;
	return TRUE;
}

//线程要做的事 此处为转发 向client发送数据
void * Thread_User_Job(void *arg)
{
	// buf[BUF_SIZE] receive new chat message
	// message[BUF_SIZE] save format message
	int clientfd=*(int *)arg;
	Msg msg;
	bool privateoff=true;
	char buf[BUF_SIZE], message[BUF_SIZE];
	bzero(buf, BUF_SIZE);//存放接收的数据
	bzero(message, BUF_SIZE);//存放给客户端发送的数据

	// receive message
	printf("read from client(clientID = %d)\n", clientfd);
	int len = recv(clientfd, buf, BUF_SIZE, 0);
  	bzero(&msg,sizeof(msg));//清空结构体
	memcpy(&msg.content,buf,sizeof(msg.content));//把收到的消息给结构体
	if(len == 0)  // len = 0 means the client closed connection
	{
		close(clientfd);
		clients_list.remove(clientfd); //server remove the client
		printf("ClientID = %d closed.\n now there are %d client in the char room\n", clientfd, (int)clients_list.size());

	}
	else
	{
		msg.fromid=clientfd;//给结构体赋值
		int leng;
		char id[1000];
		bzero(id,sizeof(id));
	    if(msg.content[0]!='/')
		{
			msg.type=0;
	//		id[0]='0';
		}
		else
		{for(int i=1;i<strlen(msg.content)&&msg.content[i];i++)
			{
				if(isdigit(msg.content[i])&&msg.content[i+1]=='/')
				{
					leng=i;
					msg.type=1;//私聊
					break;
				}
				else if(isdigit(msg.content[i])&&msg.content[i+1]!='/')
				{
					leng=i;
					continue;
				}
				else
				{
					msg.type=0;
	//				id[0]='0';
					break;
				}
			}
		}
		//
		if(msg.type==1)
			//重新覆盖消息的值
			//找到toid
		{
			memcpy(id,msg.content+1,leng);
			msg.toid=atoi(id);
			memccpy(msg.content,msg.content+leng+2,strlen(msg.content),sizeof(msg.content));
		}
		/*
		   if(msg.content[0]=='\\'&&isdigit(msg.content[1]))
		   { msg.type=1;//私聊
		   printf("sssssssssslllllllllll\n");
		   msg.toid=msg.content[1]-'0';
		   memcpy(msg.content,msg.content+2,sizeof(msg.content));
		   }
		   else
		   msg.type=0;//群发
		 */	
		//broadcast message
		//判断是否还有其他聊天室
		if(clients_list.size() == 1) { // this means There is only one int the char room
			send(clientfd, CAUTION, strlen(CAUTION), 0);
//			return len;
		}
		// format message to broadcast
   printf("msg.type%d",msg.type);	
//
//	        send(clientfd,id,strlen(id),0);
//			return len;
			//私聊还是群聊
		
	//memcpy(msg.content,message,BUF_SIZE);
		if(msg.type==1)//私聊
		{
			printf("私聊\n");
			sprintf(message,ONE_TO_ONE,clientfd,msg.content);
			list<int>::iterator ite;
			for(ite=clients_list.begin();ite!=clients_list.end();++ite)
			{
				if(*ite==msg.toid)
				{		privateoff=false;
					//			 bzero(send_buf,sizeof(send_buf));
					//			 memcpy(send_buf,&msg,sizeof(msg));
					send(msg.toid,message,strlen(message),0);  
				}
			}
			if(privateoff==true)
			{
				sprintf(message,ERROR_MESSAGE,msg.toid);
				if(send(msg.fromid,message,strlen(message),0)<0)//告诉请求发消息的客户端 你所找的不在线
					printf("send error message error\n");
			}

		}

		else//群聊
		{
				sprintf(message, SERVER_MESSAGE, clientfd,msg.content);
			printf("群聊\n");
			list<int>::iterator it;
			for(it = clients_list.begin(); it != clients_list.end(); ++it) {
				if(*it != clientfd){
					//     bzero(send_buf,sizeof(send_buf));
					//	 memcpy(send_buf,&msg,sizeof(msg));
					if(send(*it,message,BUF_SIZE,0)<0)  
					{ perror("error"); exit(-1);}
				}
			}
		}
	}
//	return len;
}

// clients_list save all the clients's socket


/**********************   some function **************************/
/**
 * @param sockfd: socket descriptor
 * @return 0
 **/
int setnonblocking(int sockfd)
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
	return 0;
}

/**
 * @param epollfd: epoll handle
 * @param fd: socket descriptor
 * @param enable_et : enable_et = true, epoll use ET; otherwise LT
 **/
void addfd( int epollfd, int fd, bool enable_et )
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if( enable_et )
		ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	setnonblocking(fd);
	printf("fd added to epoll!\n\n");
}

/**
 * @param clientfd: socket descriptor
 * @return : len
 **/
int sendBroadcastmessage(int clientfd)
{
	// buf[BUF_SIZE] receive new chat message
	// message[BUF_SIZE] save format message
	Msg msg;
	bool privateoff=true;
	char buf[BUF_SIZE], message[BUF_SIZE];
	bzero(buf, BUF_SIZE);//存放接收的数据
	bzero(message, BUF_SIZE);//存放给客户端发送的数据

	// receive message
	printf("read from client(clientID = %d)\n", clientfd);
	int len = recv(clientfd, buf, BUF_SIZE, 0);
  	bzero(&msg,sizeof(msg));//清空结构体
	memcpy(&msg,buf,sizeof(msg));//把收到的消息给结构体
	if(len == 0)  // len = 0 means the client closed connection
	{
		close(clientfd);
		clients_list.remove(clientfd); //server remove the client
		printf("ClientID = %d closed.\n now there are %d client in the char room\n", clientfd, (int)clients_list.size());

	}
	else
	{
		msg.fromid=clientfd;//给结构体赋值
		int leng;
		char id[1000];
		bzero(id,sizeof(id));
	    if(msg.content[0]!='/')
		{
			msg.type=0;
			id[0]='0';
		}
		else
		{for(int i=1;i<strlen(msg.content)&&msg.content[i];i++)
			{
				if(isdigit(msg.content[i])&&msg.content[i+1]=='/')
				{
					leng=i;
					msg.type=1;//私聊
					break;
				}
				else if(isdigit(msg.content[i])&&msg.content[i+1]!='/')
				{
					leng=i;
					continue;
				}
				else
				{
					msg.type=0;
					id[0]='0';
					break;
				}
			}
		}
		//
		if(msg.type==1)
			//重新覆盖消息的值
			//找到toid
		{
			memcpy(id,msg.content+1,leng);
			msg.toid=atoi(id);
			memccpy(msg.content,msg.content+leng+2,strlen(msg.content),sizeof(msg.content));
		}
		 
		//broadcast message
		//判断是否还有其他聊天室
		if(clients_list.size() == 1) { // this means There is only one int the char room
			send(clientfd, CAUTION, strlen(CAUTION), 0);
			return len;
		}
		// format message to broadcast
     cout<<msg.type<<endl;	

//	        send(clientfd,id,strlen(id),0);
//			return len;
			//私聊还是群聊
		
	//memcpy(msg.content,message,BUF_SIZE);
		if(msg.type==1)//私聊
		{
			printf("私聊\n");
		sprintf(message,ONE_TO_ONE,clientfd,msg.content);
			cout<<message<<endl;
			list<int>::iterator ite;
			for(ite=clients_list.begin();ite!=clients_list.end();++ite)
			{
				if(*ite==msg.toid)
				{		
				          cout<<message;
						  privateoff=false;
					//			 bzero(send_buf,sizeof(send_buf));
					//			 memcpy(send_buf,&msg,sizeof(msg));
					send(msg.toid,message,BUF_SIZE,0);  
				}
			}
			if(privateoff==true)
			{
				sprintf(message,ERROR_MESSAGE,msg.toid);
				if(send(msg.fromid,message,strlen(message),0)<0)//告诉请求发消息的客户端 你所找的不在线
					printf("send error message error\n");
			}

		}

		else//群聊
		{
			printf("群聊\n");
			sprintf(message, SERVER_MESSAGE, clientfd,msg.content);
			list<int>::iterator it;
			for(it = clients_list.begin(); it != clients_list.end(); ++it) {
				if(*it != clientfd){
					//     bzero(send_buf,sizeof(send_buf));
					//	 memcpy(send_buf,&msg,sizeof(msg));
					if(send(*it,message,BUF_SIZE,0)<0)  
					{ perror("error"); exit(-1);}
				}
			}
		}
	}
	return len;
}
#endif // THREAD_H_INCLUDED

