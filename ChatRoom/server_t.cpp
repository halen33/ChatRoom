#include "utility.h"
void * Thread_Manager_Job(void *arg)
{
	int alive,busy,size;
	pool_t * pool = (pool_t*)arg;
	//接收线程池阀值
	while(pool->thread_shutdown)
	{
		int i,add;
		pthread_mutex_lock(&pool->lock);
		alive = pool->thread_alive;
		busy = pool->thread_busy;
		size = pool->queue_size;
		pthread_mutex_unlock(&pool->lock);
		//扩容线程
		if((size > (alive - busy) || ((float)busy/alive)*(float)100 >= (float)70) && alive < pool->thread_max)
		{
			//创建线程时使用tids数组,如果对应位为0,直接使用,如果对应位>0,测试该线程是否存活,存活忽略使用,否则覆盖
			for(i=add=0;i<pool->thread_max,add<pool->thread_min;i++){
				if(pool->tids[i]==0 || !if_test_threadalive(pool->tids[i])){
					pthread_create(&pool->tids[i],NULL,Thread_Custom_Job,(void*)pool);
					++add;

					pthread_mutex_lock(&pool->lock);
					++pool->thread_alive;
					pthread_mutex_unlock(&pool->lock);
				}
			}
		}
		//缩减线程
		if(busy * 2 < (alive - busy && pool->thread_alive-10 > pool->thread_min)){
			//修改回收数
			pthread_mutex_lock(&pool->lock);
			pool->thread_wait = pool->thread_min;
			pthread_mutex_unlock(&pool->lock);
			for(i=0;i<pool->thread_min,pool->thread_alive > pool->thread_min;i++)
				pthread_cond_signal(&pool->not_empty);
		}
		printf("Manager thread:0x%x output Pool info [ALIVE=%d  BUSY=%d  LEI=%d  B/A=%.2f%%  A/ALL=%.2f%%\n",(unsigned int)pthread_self(),alive,busy,alive - busy,(float)busy/alive * 100,(float)alive/(pool->thread_max) * 100);
		sleep(TIMEOUT);
	}
	printf("Manager Thread:0x%x Shutdown is Flase Exiting...\n",(unsigned int)pthread_self());
	pthread_exit(NULL);
}
pool_t* Thread_Create(int max,int min,int quemax)
{
	int err;
	pool_t * pool;
	//初始化线程池阀值
	if((pool = (pool_t *)malloc(sizeof(pool_t)))==NULL)
	{
		perror("Thread_Create Malloc Error");
		return NULL;
	}
	pool->thread_max = max;
	pool->thread_min = min;
	pool->queue_max = quemax;
	pool->thread_shutdown = TRUE;
	pool->thread_alive = 0;
	pool->thread_busy = 0;
	pool->queue_size = 0;
	pool->queue_front = 0;
	pool->queue_rear = 0;

	//线程id数组申请空间
	if((pool->tids = (pthread_t *)malloc(sizeof(pthread_t) * max))==NULL){
		perror("Thread_Create Malloc Thread tids Error");
		return NULL;
	}
	memset(pool->tids,0,sizeof(pthread_t)*max);
	//任务队列申请空间
	if((pool->queue_task = (task_t *)malloc(sizeof(task_t) * quemax))==NULL){
		perror("Thread_Create Malloc Queue Task Error");
		return NULL;
	}
	//初始化锁,环境变量
	if(pthread_mutex_init(&(pool->lock),NULL)!=0 || pthread_cond_init(&(pool->not_full),NULL)!=0 || pthread_cond_init(&(pool->not_empty),NULL)!=0){

		perror("Thread_Create Init mutex or cond Error");
		return NULL;
	}
	//循环创建消费者,按线程池最小线程数创建
	for(int i=0;i<min;i++){
		if((err = pthread_create(&(pool->tids[i]),NULL,Thread_Custom_Job,(void *)pool))>0){
			printf("Pthread_Create Custom Error:%s\n",strerror(err));
			return NULL;
		}
		/*每次创建存活线程数++*/
		++(pool->thread_alive);
	}
	//创建管理者线程
	if((err = pthread_create(&pool->managerid,NULL,Thread_Manager_Job,(void*)pool))>0){
		printf("Pthread_Create Manager Error:%s\n",strerror(err));
		return NULL;
	}
	return pool;
}
int main(int argc, char *argv[])
{
	//线程池
	pool_t * pool;
	//创建线程池
	pool = Thread_Create(200,10,100);

	//服务器IP + port
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//创建监听socket
	int listener = socket(PF_INET, SOCK_STREAM, 0);
	if(listener < 0) { perror("listener"); exit(-1);}
	printf("listen socket created \n");
	//绑定地址
	if( bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind error");
		exit(-1);
	}
	//监听
	int ret = listen(listener, 5);
	if(ret < 0) { perror("listen error"); exit(-1);}
	printf("Start to listen: %s\n", SERVER_IP);
	//在内核中创建事件表
	int epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0) { perror("epfd error"); exit(-1);}
	printf("epoll created, epollfd = %d\n", epfd);
	static struct epoll_event events[EPOLL_SIZE];
	//往内核事件表里添加事件
	addfd(epfd, listener, true);
	//主循环

	while(1)
	{
		//epoll_events_count表示就绪事件的数目
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		if(epoll_events_count < 0) {
			perror("epoll failure");
			break;
		}

		printf("epoll_events_count = %d\n", epoll_events_count);
		//处理这epoll_events_count个就绪事件
		for(int i = 0; i < epoll_events_count; ++i)
		{
			int sockfd = events[i].data.fd;
			//新用户连接
			if(sockfd == listener)
			{
				struct sockaddr_in client_address;
				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = accept( listener, ( struct sockaddr* )&client_address, &client_addrLength );
  //如果是新用户连接事件的发生 就建立连接，然后再把这个事件加入epfd监听后续事件
				printf("client connection from: %s : % d(IP : port), clientfd = %d \n",
						inet_ntoa(client_address.sin_addr),
						ntohs(client_address.sin_port),
						clientfd);

				addfd(epfd, clientfd, true);
                     
				// 服务端用list保存用户连接
				clients_list.push_back(clientfd);
				printf("Add new clientfd = %d to epoll\n", clientfd);
				printf("Now there are %d clients int the chat room\n", (int)clients_list.size());

				// 服务端发送欢迎信息
				printf("welcome message\n");
				char message[BUF_SIZE];
				bzero(message, BUF_SIZE);
				sprintf(message, SERVER_WELCOME, clientfd);
				int ret = send(clientfd, message, BUF_SIZE, 0);
				if(ret < 0) { perror("send error"); exit(-1); }
			}
			//处理用户发来的消息，并广播，使其他用户收到信息
			else
			{
				if(pool->thread_shutdown)

			  Thread_Add_Task(pool,Thread_User_Job,(void*)&sockfd);
			//	if(ret < 0) { perror("error");exit(-1); }
			}
		}
	}
	close(listener); //关闭socketvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	close(epfd);    //关闭内核
	return 0;
}

