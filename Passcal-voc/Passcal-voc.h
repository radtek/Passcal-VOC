
// Passcal-voc.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPasscalvocApp:
// �йش����ʵ�֣������ Passcal-voc.cpp
//

class CPasscalvocApp : public CWinApp
{
public:
	CPasscalvocApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPasscalvocApp theApp;