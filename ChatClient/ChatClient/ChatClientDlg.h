
// ChatClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <map>
#include "DlgChat.h"

// CChatClientDlg �Ի���
class CChatClientDlg : public CDialogEx
{
// ����
public:
	CChatClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT DataInfoMsg(WPARAM, LPARAM);
	afx_msg LRESULT DataInfoMsg_O(WPARAM, LPARAM);
	afx_msg LRESULT OnlineRs(WPARAM,LPARAM);
	afx_msg LRESULT OnlineRq(WPARAM, LPARAM);
	afx_msg LRESULT OfflineRq(WPARAM, LPARAM);
//	afx_msg void OnClose();


	DECLARE_MESSAGE_MAP()
public:
	CString m_content;
	CListBox m_listContent;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();
	CListBox m_lstIp;

	std::map<CString,DlgChat*> m_mapIpToDlg;
public:
	DlgChat* GetDlg(CString strip);

	afx_msg void OnLbnDblclkList2();
};
