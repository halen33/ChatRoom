
// ChatClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CChatClientDlg �Ի���



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
	, m_content(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_content);
	DDX_Control(pDX, IDC_LIST1, m_listContent);
	DDX_Control(pDX, IDC_LIST2, m_lstIp);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_DATAINFOMSG, &CChatClientDlg::DataInfoMsg)
	ON_MESSAGE(UM_ONLINEMSG, &CChatClientDlg::OnlineRs)
	ON_MESSAGE(UM_OFFLINEMSG, &CChatClientDlg::OfflineRq)
	ON_MESSAGE(UM_DATAINFOMSG_O, &CChatClientDlg::DataInfoMsg_O)
	ON_BN_CLICKED(IDC_BUTTON1, &CChatClientDlg::OnBnClickedButton1)
	ON_LBN_DBLCLK(IDC_LIST2, &CChatClientDlg::OnLbnDblclkList2)
END_MESSAGE_MAP()


// CChatClientDlg ��Ϣ�������

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (!theApp.GetMediator()->Open())
	{
		MessageBox("���ӷ���������ʧ��");
		return FALSE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//�յ���Ϣ����Ϣ��ʾ������
LRESULT CChatClientDlg::DataInfoMsg_O(WPARAM w, LPARAM l)
{
	char *szContent = (char*)w;
	//lparam --ip
	int a = l;
	char szbuf[20];
	LPCSTR text;
	sprintf(szbuf, "%d", a);
	text = LPCSTR(szbuf);

	//�򿪴���
	DlgChat* pDlg = GetDlg(text);
	//CString strip = szip;
	
	LPCSTR text2;
	sprintf(szbuf, "%d say:", a);
	text2 = LPCSTR(szbuf);

	if (pDlg)
	{
		pDlg->c_lstcontent.AddString(text2);
		pDlg->c_lstcontent.AddString(szContent);
	}

	return 0;
}
LRESULT CChatClientDlg::DataInfoMsg(WPARAM w, LPARAM l)//���յ�����Ϣ��ʾ��������
{
	//��������ʾ��������
	char *szContent = (char*)w;
	//lparam --ip
	in_addr addr;
	/*CString strIp;
	addr.S_un.S_addr = l;
	char *szip = inet_ntoa(addr);
	*/
	//CDlgChat* pDlg = GetDlg(szip);
	int a = l;
	char szbuf[20];
	LPCSTR text;
	sprintf(szbuf, "%d say:", a);
	text = LPCSTR(szbuf);
	m_listContent.AddString(text);
	m_listContent.AddString(szContent);
	

	return 0;

}
LRESULT CChatClientDlg::OfflineRq(WPARAM w, LPARAM l)
{
	int a = l;
	char *szbuf=(char *)malloc(sizeof(char)*4);
	//LPCSTR text;
	sprintf(szbuf, "%d", a);//char*
	//text = LPCSTR(szbuf);
	CString strIp;
	for (int i = 0;i < m_lstIp.GetCount();i++)
	{
		m_lstIp.GetText(i, strIp);
		if (strIp == szbuf)
		{
			m_lstIp.DeleteString(i);
			break;
		}
	}
	return 0;
}
LRESULT CChatClientDlg::OnlineRq(WPARAM w, LPARAM l)
{

	int a = l;
	char szbuf[4];
	LPCSTR text;
	sprintf(szbuf, "%d", a);
	text = LPCSTR(szbuf);
	m_lstIp.AddString(text);
	return 0;
}
 LRESULT CChatClientDlg::OnlineRs(WPARAM w,LPARAM l)
{
	int a = l;
	char szbuf[4];
	LPCSTR text;
	sprintf(szbuf, "%d", a);
	text = LPCSTR(szbuf);
	m_lstIp.AddString(text);
	return 0;
 }
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//������Ϣ ������������ͬʱ��ʾ���Լ�������
void CChatClientDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strIp;
	//GetWindowText(strIp);

	UpdateData();
	STRU_DATAINFO sd;
	sd.m_nType = _DEF_PROTOCOL_DATAINFO_RQ;
	sd.type = 1;//Ⱥ��
	memcpy(sd.m_szBuf, m_content, _DEF_BUFFERSIZE);
	sd.m_nNum = 0;
	sd.fromid = 0;
	sd.toid = 0;
	if (theApp.GetMediator()->SendData(0, (char*)&sd, sizeof(sd)))//Ⱥ�ľʹ�0
	{
		m_listContent.AddString("I say:");
		m_listContent.AddString(m_content);
		m_content = "";

		UpdateData(FALSE);
	}
}
void CChatClientDlg::OnClose()
{
	if (theApp.GetMediator())
	{
		theApp.GetMediator()->Close();
	}
	/*if (g_hKeyBoard)
	{
		UnhookWindowsHookEx(g_hKeyBoard);
		g_hKeyBoard = NULL;
	}
	*/
	auto ite = m_mapIpToDlg.begin();
	while (ite != m_mapIpToDlg.end())
	{
		if (ite->second)
		{
			delete ite->second;
			ite->second = NULL;
		}
		ite++;
	}
	CDialogEx::OnClose();
}


void CChatClientDlg::OnLbnDblclkList2()
{
	int nindex = m_lstIp.GetCurSel();
	if (nindex == -1)return;

	CString strip;
	m_lstIp.GetText(nindex, strip);
	GetDlg(strip);	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
DlgChat* CChatClientDlg::GetDlg(CString strip)
{

	DlgChat *pdlg = m_mapIpToDlg[strip];
	if (!pdlg)
	{
		pdlg = new DlgChat;
		pdlg->Create(IDD_DIALOG1);

		pdlg->SetWindowText(strip);

		m_mapIpToDlg[strip] = pdlg;
	}


	pdlg->ShowWindow(SW_SHOW);

	return pdlg;
}
