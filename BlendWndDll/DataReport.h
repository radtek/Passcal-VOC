#pragma once

#include "NgWarnLimit.h"

static const UINT gMsgStatisticReport = RegisterWindowMessage(_T("gMsgStatisticReport"));
static const UINT gMsgStatisticBeyongLimit = RegisterWindowMessage(_T("gMsgStatisticBeyongLimit"));

// CDataReport 对话框

class AFX_EXT_CLASS CDataReport : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CDataReport)

public:
	CDataReport(CWnd* pParent = NULL);   // 标准构造函数
	CDataReport(UINT nIDTemplate, CWnd * pParent = nullptr);

	virtual ~CDataReport();

	// 对话框数据
	enum { IDD = IDD_DATAREPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	int m_nPass;
	int m_nFail;

	virtual void LockCtrls(int nLock);

	void PassIncrease(int nInc);
	void FailIncrease(int nInc);
	void ClearData(void);

	void LoadData(CString strFile);
	void SaveData(CString strFile);
protected:
	CString m_strFile;

	BOOL m_bUseNum;
	BOOL m_bUseRate;

	int m_nNgLimit;
	double m_dNgRateLimit;

	virtual void _PreShowWnd(void);
	virtual void _DefaultFocus(void);
public:	
	void RtDbClickStatisticChart();
	void NgNumReachLimitStatisticChart(long nNgNum);
	void NgRateReachLimitStatisticChart(double dNgRate);
	void LBtClickedBtrReportClear(long nFlags);
	void LBtClickedBtReportExcel(long nFlags);
	void LBtClickedBtReportExit(long nFlags);
protected:
	afx_msg LRESULT OnDataChangeed(WPARAM wParam, LPARAM lParam);
};
