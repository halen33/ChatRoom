#pragma once
#include "afxwin.h"


// DlgChat �Ի���

class DlgChat : public CDialogEx
{
	DECLARE_DYNAMIC(DlgChat)

public:
	DlgChat(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgChat();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox c_lstcontent;
	CString c_mcontent;
	afx_msg void OnBnClickedButton1();
};
