#pragma once
#include "IMediator.h"
class TCPMediator;
typedef void (TCPMediator::*PFUN)(char *);
//Э��ӳ���
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
	 bool SendData(long lSendIp, char *szbuf, int nlen);//����inet����������
	 void OnlineRs(char *szbuf);
	 void OnlineRq(char *szbuf);
	 void OfflineRq(char *szbuf);
	 void DealData(char *szbuf);
	 void DataInfoRq(char *szbuf);
	 int n;
};

