#pragma once
#include "INet.h"
class IMediator
{
public:
	IMediator() {}
	virtual ~IMediator() {}
public:
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual bool SendData(long lSendIp, char *szbuf, int nlen) = 0;//����inet����������
	virtual void DealData(char *szbuf) = 0;
protected:
	INet *m_pNet;
};

