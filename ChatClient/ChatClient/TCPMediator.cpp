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
	{ _DEF_PROTOCOL_ONLINE_RQ,&TCPMediator::OnlineRq },//��Ϣ������
	{ _DEF_PROTOCOL_ONLINE_RS,&TCPMediator::OnlineRs },
	{ _DEF_PROTOCOL_OFFLINE_RQ,&TCPMediator::OfflineRq },
	{ _DEF_PROTOCOL_DATAINFO_RQ,&TCPMediator::DataInfoRq },
	{ 0,0 }

};
void TCPMediator::OnlineRq(char *szbuf)
{
	STRU_ONLINE *p = (STRU_ONLINE* )szbuf;//6
	AfxGetApp()->m_pMainWnd->SendMessage(UM_ONLINEMSG, 0, p->rsid);//˭�����Ļظ�
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
		//���������������Ĳ���Ҫ�ظ�
		//�յ�����Ϣ ��ʾ��ip�ǶԷ�һ��ʼ����fromid
//����3 2������ ��ʾ2
	else
	{
		//���Է����ظ�
		STRU_ONLINE rq;
		//��Ҫ����2 ����
		rq.m_nType = _DEF_PROTOCOL_ONLINE_RQ;
		rq.clientfd = p->clientfd;//clientfd ��6
		rq.rsid = p->rsid;//��5
		m_pNet->SendData(0, (char*)&rq, sizeof(rq));
	}
}
bool TCPMediator::Open()
{
//��ʼ������
//����������Ϣ
	if (!m_pNet->InitNetWork())
		return 0;

	else
		return 1;
}
void TCPMediator::Close()
{
	m_pNet->UnInitNetWork();//�ͻ�������
//��������Ϣ
}
void TCPMediator::DataInfoRq(char *szbuf)
{
	STRU_DATAINFO *psd = (STRU_DATAINFO*)szbuf;
	if (psd->type == 1)
	{
		AfxGetApp()->m_pMainWnd->SendMessage(UM_DATAINFOMSG, (WPARAM)psd->m_szBuf, psd->fromid);//�յ�����Ϣ ��ʾ��ip�ǶԷ�һ��ʼ����fromid
	}
	else if(psd->type==0)
	{ 
		AfxGetApp()->m_pMainWnd->SendMessage(UM_DATAINFOMSG_O, (WPARAM)psd->m_szBuf, psd->fromid);//˽��
	}
}
bool TCPMediator::SendData(long lSendIp, char *szbuf, int nlen)
{
	STRU_DATAINFO *p = (STRU_DATAINFO*)szbuf;
	if (!m_pNet->SendData(lSendIp, szbuf, nlen))
		return false;

	return true;
}//����inet����������
void TCPMediator::DealData(char *szbuf)
{
    //��������
	PackdefType *pType = (PackdefType*)szbuf;//������յ��������������
	//����Э��ӳ���---��������
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