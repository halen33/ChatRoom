
// ChatClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include"TCPMediator.h"

// CChatClientApp: 
// �йش����ʵ�֣������ ChatClient.cpp
//

class CChatClientApp : public CWinApp
{
public:
	CChatClientApp();

// ��д
public:
	virtual BOOL InitInstance();
private:
	IMediator *m_pMediator;
public:
	IMediator *GetMediator()
	{
		return m_pMediator;

	}

// ʵ��
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CChatClientApp theApp;