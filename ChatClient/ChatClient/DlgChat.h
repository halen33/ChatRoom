#pragma once
#include "afxwin.h"


// DlgChat 对话框

class DlgChat : public CDialogEx
{
	DECLARE_DYNAMIC(DlgChat)

public:
	DlgChat(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgChat();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox c_lstcontent;
	CString c_mcontent;
	afx_msg void OnBnClickedButton1();
};
