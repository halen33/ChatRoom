#pragma once
#include "IMediator.h"
class TCPMediator;
typedef void (TCPMediator::*PFUN)(char *);
//协议映射表
struct ProtocolMap
{
	PackdefType m_nType;
	PFUN        m_pfun;

};
class TCPMediator :
	public IMediator
{
public:
	TCPMediator();
	~TCPMediator();
public:
	 bool Open() ;
	 void Close() ;
	 bool SendData(long lSendIp, char *szbuf, int nlen);//调用inet发给服务器
	 void OnlineRs(char *szbuf);
	 void OnlineRq(char *szbuf);
	 void OfflineRq(char *szbuf);
	 void DealData(char *szbuf);
	 void DataInfoRq(char *szbuf);
	 int n;
};

