
// NerveBot.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNerveBotApp: 
// �йش����ʵ�֣������ NerveBot.cpp
//

class CNerveBotApp : public CWinApp
{
public:
	CNerveBotApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNerveBotApp theApp;