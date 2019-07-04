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
 map<int, pair<string, int> > mmap;
/**********************   macro defintion **************************/
// server ip
//#pragma once

#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

//epoll size
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF
//消息大小
#define _DEF_BUFFERSIZE  1024

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

#define UM_ONLINEMSG   WM_USER + 1
#define UM_OFFLINEMSG  WM_USER + 2
#define UM_DATAINFOMSG  WM_USER + 3


//协议

#define _DEF_PROTOCOL_BASE  100

#define _DEF_PROTOCOL_ONLINE_RQ _DEF_PROTOCOL_BASE  +1
#define _DEF_PROTOCOL_ONLINE_RS _DEF_PROTOCOL_BASE  +2

#define _DEF_PROTOCOL_OFFLINE_RQ _DEF_PROTOCOL_BASE  +3


#define _DEF_PROTOCOL_DATAINFO_RQ _DEF_PROTOCOL_BASE  +4
#define _DEF_PROTOCOL_HEAART_RQ _DEF_PROTOCOL_BASE  +5

//协议包
typedef char PackdefType;
//上线请求 上线回复 下线请求

typedef struct HEART
{
    PackdefType m_nType;
}

typedef struct STRU_ONLINE
{
	PackdefType m_nType;
	int clientfd;
    int rsid;
}STRU_OFFLINE;


struct STRU_DATAINFO
{
	PackdefType m_nType;
	int type;//0 私聊 1 群聊
	int fromid;
	int  toid;
	char        m_szBuf[_DEF_BUFFERSIZE];
	int         m_nNum;  //序号
};


//int serverfd;
pthread_mutex_t acclock;

typedef struct
{
	void * (*user)(void * arg);
	void * arg;
}task_t;//任务队列
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
printf("我在工作，传进来了发来消息的是%d\n",clientfd);
   //	Msg msg;
	fcntl(clientfd, F_SETFL, fcntl(clientfd, F_GETFD, 0)| O_NONBLOCK);
	bool privateoff=true;
	char buf[BUF_SIZE], message[BUF_SIZE];
	bzero(buf, BUF_SIZE);//存放接收的数据
	bzero(message, BUF_SIZE);//存放给客户端发送的数据

//	printf("read from client(clientID = %d)\n", clientfd);
	int len = recv(clientfd, buf, BUF_SIZE, 0);
	printf("len %d",len);
	//STRU_DATAINFO *p=(STRU_DATAINFO*)buf;//获取协议包
	//	bzero(&msg,sizeof(msg));//清空结构体
	//	memcpy(&msg.content,buf,sizeof(msg.content));//把收到的消息给结构体
	if(len <= 0)  // len = 0 means the client closed connection
	{
		printf("errrrrrrrrrr");
		char mes[BUF_SIZE];
		close(clientfd);
		clients_list.remove(clientfd); //server remove the client
		sprintf(mes,"ClientID = %d closed. now there are %d client in the char room", clientfd, (int)clients_list.size());
		//发送下线消息
		list<int>::iterator itt;
		STRU_OFFLINE offline;
		offline.clientfd=clientfd;
		offline.m_nType=_DEF_PROTOCOL_OFFLINE_RQ;
		for(itt = clients_list.begin(); itt != clients_list.end(); ++itt) {
			//  if(*it != clientfd){
			//     bzero(send_buf,sizeof(send_buf));
			//   memcpy(send_buf,&msg,sizeof(msg));
			if(send(*itt,(char *)&offline,sizeof(offline),0)<0)
			{ perror("error"); exit(-1);
			}

            else
			{
			printf("send offline message success\n");
			}
		}
		}
		else if(len>0)
		{
			//判断协议类型
			PackdefType *pType=(PackdefType *)buf;
			//如果是心跳包发过来
			if(*pType==_DEF_PROTOCOL_HEAART_RQ)
            {
                mmap[clientfd].second = 0;        // 每次收到心跳包，count置0
                cout << "Received heart-beat from" <<clientfd;
            }

			else if(*pType==_DEF_PROTOCOL_ONLINE_RQ)
			{
			STRU_ONLINE *r=(STRU_ONLINE*)buf;
		   printf("client:%d rsid:%d",r->clientfd,r->rsid);
		    //	rq->rsid=clientfd;
		STRU_ONLINE rq;
		rq.m_nType=_DEF_PROTOCOL_ONLINE_RQ;
		rq.clientfd=r->clientfd;
		rq.rsid=r->rsid;
            if(send(rq.clientfd,(char*)&rq,sizeof(rq),0)<0)
			{
			perror("send online rq error");
			}
			else
				printf("send onlinerq success\n");
			}
			else if(*pType==_DEF_PROTOCOL_DATAINFO_RQ)
			{
			STRU_DATAINFO *p=(STRU_DATAINFO*)buf;//获取协议包
	//		msg.fromid=clientfd;//给结构体赋值
			p->fromid=clientfd;
			printf("消息来自%d\n",p->fromid);
			printf("消息type %d\n",p->type);

			printf("内容%s\n",p->m_szBuf);
			if(p->type==0)//私聊
			{
				printf("私聊\n");
	  printf("消息发给%d\n",p->toid);
//				sprintf(message,ONE_TO_ONE,clientfd,msg.content);
				list<int>::iterator ite;
				for(ite=clients_list.begin();ite!=clients_list.end();++ite)
				{
					if(*ite==p->toid)
					{		privateoff=false;
						//			 bzero(send_buf,sizeof(send_buf));
						//			 memcpy(send_buf,&msg,sizeof(msg));
					if(	send(p->toid,buf,sizeof(buf),0)>0)
					{
					printf("私发成功\n");

					}
					}
				}
				if(privateoff==true)
				{
					sprintf(message,ERROR_MESSAGE,p->toid);
					if(send(p->fromid,message,strlen(message),0)<0)//告诉请求发消息的客户端 你所找的不在线
						printf("send error message error\n");
				}

			}

			else//群聊
			{
//				sprintf(message, SERVER_MESSAGE, clientfd,msg.content);
				printf("群聊\n");
				list<int>::iterator it;
				for(it = clients_list.begin(); it != clients_list.end(); ++it) {
					if(*it != clientfd){
						//     bzero(send_buf,sizeof(send_buf));
						//	 memcpy(send_buf,&msg,sizeof(msg));
						if(send(*it,buf,sizeof(buf),0)<0)
						{ perror("error"); exit(-1);}
					else
					{
					printf("群发成功\n");
					}
					}
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
#endif // THREAD_H_INCLUDED

