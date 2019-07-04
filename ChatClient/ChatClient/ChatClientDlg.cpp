
// ChatClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CChatClientDlg 对话框



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


// CChatClientDlg 消息处理程序

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if (!theApp.GetMediator()->Open())
	{
		MessageBox("连接服务器启动失败");
		return FALSE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//收到消息将消息显示到窗口
LRESULT CChatClientDlg::DataInfoMsg_O(WPARAM w, LPARAM l)
{
	char *szContent = (char*)w;
	//lparam --ip
	int a = l;
	char szbuf[20];
	LPCSTR text;
	sprintf(szbuf, "%d", a);
	text = LPCSTR(szbuf);

	//打开窗口
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
LRESULT CChatClientDlg::DataInfoMsg(WPARAM w, LPARAM l)//把收到的消息显示到窗口上
{
	//将内容显示到窗口上
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
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//发送消息 发给服务器，同时显示在自己窗口上
void CChatClientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strIp;
	//GetWindowText(strIp);

	UpdateData();
	STRU_DATAINFO sd;
	sd.m_nType = _DEF_PROTOCOL_DATAINFO_RQ;
	sd.type = 1;//群聊
	memcpy(sd.m_szBuf, m_content, _DEF_BUFFERSIZE);
	sd.m_nNum = 0;
	sd.fromid = 0;
	sd.toid = 0;
	if (theApp.GetMediator()->SendData(0, (char*)&sd, sizeof(sd)))//群聊就传0
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
	GetDlg(strip);	// TODO: 在此添加控件通知处理程序代码
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
