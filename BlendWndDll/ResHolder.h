#pragma once

#include "Resource.h"
#include "TpLayerWnd.h"

enum
{
	RES_SHOW_HOLD = 0,
	RES_SHOW_SWITCH,
	RES_SHOW_MODE_SUM
};

enum
{
	RES_SWITCH_CLEAR = 0,
	RES_SWITCH_NG,
	RES_SWITCH_SUM
};

// CResHolder �Ի���

class AFX_EXT_CLASS CResHolder : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CResHolder)

public:
	CResHolder(CWnd* pParent = NULL);   // ��׼���캯��
	CResHolder(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CResHolder();

	// �Ի�������
	enum { IDD = IDD_RESHOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	int m_nHoldTime;
	int m_nShowMode;
	int m_nSwitchMode;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void LockCtrls(int nLock);

	void LoadDfaultSet(void);
	void SaveDfaultSet(void);

protected:
	int m_nHoldTimeBkup;
	int m_nShowModeBkup;
	int m_nSwitchModeBkup;
public:
	void StatusChangedRdResHold(BOOL bNewStatus);
	void StatusChangedRdResOkNg(BOOL bNewStatus);
	void ValueChangedEditResHoldTime(LPCTSTR strNew);
	void StatusChangedRdResClear(BOOL bNewStatus);
	void StatusChangedRdRes2Ng(BOOL bNewStatus);
};
