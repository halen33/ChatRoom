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
	 bool SendData(long lSendIp, char *szbuf, int nlen);//��������(����������)
public:
	static  unsigned  __stdcall ThreadProc(void *);//�����̺߳��� ��������Ϣ
	static  unsigned  __stdcall heart_ThreadProc(void *);//������������
private:
	SOCKET clientSock;
	bool isClientwork;
	HANDLE m_hThread;//����Ϣ
	HANDLE m_heartThread;//��������
	IMediator *m_pMediator;
};

