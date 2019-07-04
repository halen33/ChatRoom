// DlgChat.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "DlgChat.h"
#include "afxdialogex.h"


// DlgChat 对话框

IMPLEMENT_DYNAMIC(DlgChat, CDialogEx)

DlgChat::DlgChat(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, c_mcontent(_T(""))
{

}

DlgChat::~DlgChat()
{
}

void DlgChat::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, c_lstcontent);
	DDX_Text(pDX, IDC_EDIT1, c_mcontent);
}


BEGIN_MESSAGE_MAP(DlgChat, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &DlgChat::OnBnClickedButton1)
END_MESSAGE_MAP()


// DlgChat 消息处理程序


void DlgChat::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strIp;
	GetWindowText(strIp);

	UpdateData();
	STRU_DATAINFO sd;
	sd.m_nType = _DEF_PROTOCOL_DATAINFO_RQ;
	sd.toid = _ttoi(strIp);
	sd.type = 0;//私聊
	sd.m_nNum = 0;
	memcpy(sd.m_szBuf, c_mcontent, _DEF_BUFFERSIZE);
	sd.m_nNum = 0;

	if (theApp.GetMediator()->SendData(0, (char*)&sd, sizeof(sd)))
	{
		c_lstcontent.AddString("I say:");
		c_lstcontent.AddString(c_mcontent);
		c_mcontent = "";

		UpdateData(FALSE);
	}
}
