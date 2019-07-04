#pragma once
#include "INet.h"
#include "IMediator.h"
class TCPclient :public INet
{
public:
	TCPclient(IMediator *m_pMediator);
	~TCPclient();
public:
	bool InitNetWork();
	 void UnInitNetWork();
	 bool SendData(long lSendIp, char *szbuf, int nlen);//发送数据(发给服务器)
public:
	static  unsigned  __stdcall ThreadProc(void *);//创建线程函数 用来收消息
	static  unsigned  __stdcall heart_ThreadProc(void *);//用来发心跳包
private:
	SOCKET clientSock;
	bool isClientwork;
	HANDLE m_hThread;//收消息
	HANDLE m_heartThread;//发心跳包
	IMediator *m_pMediator;
};

