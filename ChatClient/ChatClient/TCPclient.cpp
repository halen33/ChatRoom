#include "stdafx.h"
#include "TCPclient.h"


TCPclient::TCPclient(IMediator *pMediator)
{
	clientSock = 0;
	isClientwork = true;
	m_hThread = 0;
	m_pMediator = pMediator;
}


TCPclient::~TCPclient()
{
}
bool TCPclient::InitNetWork()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return 0;
	}
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr("192.168.88.130");

	// ����socket
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) { perror("sock error"); exit(-1); }
	// ���ӷ����
	if (connect(clientSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("connect error");
		exit(-1);
	}
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, this, 0, 0);
	m_heartThread= (HANDLE)_beginthreadex(NULL, 0, &heart_ThreadProc, this, 0, 0);
	return true;
}
void TCPclient::UnInitNetWork()
{
	    isClientwork = false;
		if (clientSock)
		{
			closesocket(clientSock);
			clientSock = NULL;
		}
		if (m_hThread)//�ر��߳�
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 100))
				TerminateThread(m_hThread, -1);

			CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		WSACleanup();
		
	
}
bool TCPclient::SendData(long lSendIp, char *szbuf, int nlen)//��һ���ǶԷ�ip��˽��
{
/*	struct sockaddr_in serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
*/
	if (!szbuf || nlen <= 0)
		return false;
	STRU_DATAINFO *p = (STRU_DATAINFO*)szbuf;
	if (send(clientSock,szbuf,nlen,0) < 0)//����server
		return false;
	return true;
}
unsigned  __stdcall TCPclient::ThreadProc(void * lpvoid)
{
	//�߳�������Ϣ
	TCPclient *pthis = (TCPclient*)lpvoid;//ǿת����
	char szbuf[BUF_SIZE] = { 0 };//���建����
	sockaddr_in addrClient;//�����ַ
	int nSize = sizeof(addrClient);
	//int nRelRecvNum;//��������
	while (pthis->isClientwork)//init��
	{
		int ret = recv(pthis->clientSock, szbuf, BUF_SIZE, 0);

		// ret= 0 ����˹ر�
		if (ret == 0) {
			printf("Server closed connection: %d\n", pthis->clientSock);
			closesocket(pthis->clientSock);
			pthis->isClientwork = 0;
		}
	//	else printf("%s\n", szbuf);

		if (ret >0)
		{
			//�����н���ȥ����
			pthis->m_pMediator->DealData(szbuf);
		
		}
	}
	return 0;
}
unsigned  __stdcall TCPclient::heart_ThreadProc(void * lpvoid)
{
	int count = 0;  // ����
	TCPclient *pthis = (TCPclient*)lpvoid;//ǿת����
	while (1)
	{
		HEART head;
		head.m_nType = _DEF_PROTOCOL_HEAART_RQ;
		//head.length = 0;
		send(pthis->clientSock, (char *)&head, sizeof(head), 0);
		Sleep(3);     // ��ʱ3��

		++count;      // ���ԣ�����15����������ֹͣ����
		if (count > 15)
			break;
	}
}