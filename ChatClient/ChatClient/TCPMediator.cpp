#include "stdafx.h"
#include "TCPclient.h"
#include"ChatClient.h"
#include "TCPMediator.h"


TCPMediator::TCPMediator()
{
	m_pNet = new TCPclient(this);
}


TCPMediator::~TCPMediator()
{
}
static const ProtocolMap m_ProtocolMapEntries[] =
{
	{ _DEF_PROTOCOL_ONLINE_RQ,&TCPMediator::OnlineRq },//消息处理函数
	{ _DEF_PROTOCOL_ONLINE_RS,&TCPMediator::OnlineRs },
	{ _DEF_PROTOCOL_OFFLINE_RQ,&TCPMediator::OfflineRq },
	{ _DEF_PROTOCOL_DATAINFO_RQ,&TCPMediator::DataInfoRq },
	{ 0,0 }

};
void TCPMediator::OnlineRq(char *szbuf)
{
	STRU_ONLINE *p = (STRU_ONLINE* )szbuf;//6
	AfxGetApp()->m_pMainWnd->SendMessage(UM_ONLINEMSG, 0, p->rsid);//谁发来的回复
}
void TCPMediator::OfflineRq(char *szbuf)
{
	STRU_OFFLINE *p = (STRU_OFFLINE *)szbuf;
	AfxGetApp()->m_pMainWnd->SendMessage(UM_OFFLINEMSG, 0, p->clientfd);
}
void TCPMediator::OnlineRs(char *szbuf)
{
	STRU_ONLINE *p = (STRU_ONLINE *)szbuf;
	AfxGetApp()->m_pMainWnd->SendMessage(UM_ONLINEMSG, 0, p->clientfd);
	if (p->rsid == 0)
	{
		return;
	}
		//服务器给他发来的不需要回复
		//收到的消息 显示的ip是对方一开始赋的fromid
//我是3 2发来了 显示2
	else
	{
		//给对方发回复
		STRU_ONLINE rq;
		//我要告诉2 我在
		rq.m_nType = _DEF_PROTOCOL_ONLINE_RQ;
		rq.clientfd = p->clientfd;//clientfd 是6
		rq.rsid = p->rsid;//是5
		m_pNet->SendData(0, (char*)&rq, sizeof(rq));
	}
}
bool TCPMediator::Open()
{
//初始化网络
//发送上线消息
	if (!m_pNet->InitNetWork())
		return 0;

	else
		return 1;
}
void TCPMediator::Close()
{
	m_pNet->UnInitNetWork();//客户端离线
//发离线消息
}
void TCPMediator::DataInfoRq(char *szbuf)
{
	STRU_DATAINFO *psd = (STRU_DATAINFO*)szbuf;
	if (psd->type == 1)
	{
		AfxGetApp()->m_pMainWnd->SendMessage(UM_DATAINFOMSG, (WPARAM)psd->m_szBuf, psd->fromid);//收到的消息 显示的ip是对方一开始赋的fromid
	}
	else if(psd->type==0)
	{ 
		AfxGetApp()->m_pMainWnd->SendMessage(UM_DATAINFOMSG_O, (WPARAM)psd->m_szBuf, psd->fromid);//私聊
	}
}
bool TCPMediator::SendData(long lSendIp, char *szbuf, int nlen)
{
	STRU_DATAINFO *p = (STRU_DATAINFO*)szbuf;
	if (!m_pNet->SendData(lSendIp, szbuf, nlen))
		return false;

	return true;
}//调用inet发给服务器
void TCPMediator::DealData(char *szbuf)
{
    //处理数据
	PackdefType *pType = (PackdefType*)szbuf;//处理接收到缓冲区里的数据
	//查找协议映射表---遍历数组
	int i = 0;
	while (1)
	{
		if (m_ProtocolMapEntries[i].m_nType == *pType)
		{
			(this->*m_ProtocolMapEntries[i].m_pfun)(szbuf);
			return;
		}
		else if (m_ProtocolMapEntries[i].m_nType == 0 && m_ProtocolMapEntries[i].m_pfun == 0)
			return;

		i++;
	}

}