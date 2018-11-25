#pragma once

#include "NameInfo.h"

// CExtInfo �Ի���

class CExtInfo : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CExtInfo)

public:
	CExtInfo(CWnd* pParent = NULL);   // ��׼���캯��
	CExtInfo(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CExtInfo();

// �Ի�������
	enum { IDD = IDD_EXTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	CString m_strSrcType;
	CString m_strDstType;
	CString m_strSavePath;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
public:
	void LBtClickedBtBrowseDst(long nFlags);
};
