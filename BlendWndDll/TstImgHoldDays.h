#pragma once

#include "MsgBox.h"

// CTstImgHoldDays �Ի���

class AFX_EXT_CLASS CTstImgHoldDays : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CTstImgHoldDays)

public:
	CTstImgHoldDays(CWnd* pParent = NULL);   // ��׼���캯��
	CTstImgHoldDays(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CTstImgHoldDays();

// �Ի�������
	enum { IDD = IDD_TSTIMGHOLDDAYS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	int m_nHoldDays;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void LockCtrls(int nLock);

	void LoadHoldDays(void);
public:
	void LBtClickedBtImgDelNow(long nFlags);
};
