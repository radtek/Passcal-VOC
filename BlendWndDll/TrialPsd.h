#pragma once

#include "Register.h"

typedef struct _tagTrialInfo
{
	int nTrial;
	int nOutputs;
	int nRunTime;
}APP_TRIAL_INFO;

#define TST_LIMIT_HOURS		(m_nDayLimit * 24 * 60)

// CTrialPsd �Ի���

class AFX_EXT_CLASS CTrialPsd : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CTrialPsd)

public:
	CTrialPsd(CWnd* pParent = NULL);   // ��׼���캯��
	CTrialPsd(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CTrialPsd();

// �Ի�������
	enum { IDD = IDD_TRIALPSD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:	
	APP_TRIAL_INFO m_TrialInfo;

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void GetTrialInfo(int nLimit, int nDays);
	void SetTrialInfo(void);
	void StartTickCount(void);

	BOOL UpdateInfo(int nInc);
	BOOL IsNeedUpdatePsd(void);
protected:
	BOOL m_bTrialAgain;

	int m_nMaxLimit;
	int m_nDayLimit;

	CString m_strId;
	BOOL m_bThreadAlive;
	CWinThread *m_pThread;
	CCriticalSection m_csRecord;

	virtual void _DefaultFocus(void);

	static UINT _TickCountThread(LPVOID pParam);
	UINT _TickCountThreadKernel(void);
protected:
	afx_msg LRESULT OnShowTrialBox(WPARAM wParam, LPARAM lParam);
};
