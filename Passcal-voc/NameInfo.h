#pragma once

#include "..\\BlendWndDll\\PathBrowse.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\BlendWndDll.lib")
#else
#pragma comment(lib, "..\\Release\\BlendWndDll.lib")
#endif

// CNameInfo �Ի���

class CNameInfo : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CNameInfo)

public:
	CNameInfo(CWnd* pParent = NULL);   // ��׼���캯��
	CNameInfo(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CNameInfo();

// �Ի�������
	enum { IDD = IDD_NAMEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPrefix;
	int m_nStart;
	int m_nNums;	

	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
