#pragma once

#include "NameInfo.h"

// CExtInfo 对话框

class CExtInfo : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CExtInfo)

public:
	CExtInfo(CWnd* pParent = NULL);   // 标准构造函数
	CExtInfo(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CExtInfo();

// 对话框数据
	enum { IDD = IDD_EXTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
