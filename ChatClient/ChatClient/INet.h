#pragma once
#ifndef _INET_H
#define _INET_H
#include <Winsock2.h>
#include "Packdef.h"
#pragma comment(lib,"Ws2_32.lib")
#include <list>
class INet
{
public:
	INet() {}
	virtual ~INet() {}
public:
	virtual bool InitNetWork() = 0;
	virtual void UnInitNetWork() = 0;
	virtual bool SendData(long lSendIp, char *szbuf, int nlen) = 0;//发送数据(发给服务器)
public:
	/*void GetValidIp()
	{
		char szName[20] = { 0 };
		struct hostent *remoteHost = NULL;
		struct in_addr addr;

		//1.获得主机名
		if (!gethostname(szName, sizeof(szName)))
		{
			//2.IPLIST

			remoteHost = gethostbyname(szName);
			int i = 0;
			while (remoteHost->h_addr_list[i] != 0) {
				addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
				//printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
				m_lstIp.push_back(addr.s_addr);
			}

		}


	}
	*/
public:
	std::list<long> m_lstIp;//上线ip
};



#endif