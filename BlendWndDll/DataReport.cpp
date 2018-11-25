// StatisticWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DataReport.h"
#include "afxdialogex.h"

#define WM_DATA_CHANGEED		WM_USER + 1

// CDataReport 对话框

IMPLEMENT_DYNAMIC(CDataReport, CTpLayerWnd)

CDataReport::CDataReport(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CDataReport::IDD, pParent)
	, m_nPass(0)
	, m_nFail(0)
	, m_bUseNum(FALSE)
	, m_bUseRate(TRUE)
	, m_nNgLimit(10)
	, m_dNgRateLimit(5)
{

}

CDataReport::CDataReport(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
	, m_nPass(0)
	, m_nFail(0)
	, m_bUseNum(FALSE)
	, m_bUseRate(TRUE)
	, m_nNgLimit(10)
	, m_dNgRateLimit(5)
{
}

CDataReport::~CDataReport()
{
}

void CDataReport::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDataReport, CTpLayerWnd)
	ON_MESSAGE(WM_DATA_CHANGEED, &CDataReport::OnDataChangeed)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDataReport, CTpLayerWnd)
	ON_EVENT(CDataReport, IDC_REPORT_CHART, 1, CDataReport::RtDbClickStatisticChart, VTS_NONE)
	ON_EVENT(CDataReport, IDC_REPORT_CHART, 2, CDataReport::NgNumReachLimitStatisticChart, VTS_I4)
	ON_EVENT(CDataReport, IDC_REPORT_CHART, 3, CDataReport::NgRateReachLimitStatisticChart, VTS_R8)
	ON_EVENT(CDataReport, IDC_BT_REPORT_CLEAR, 1, CDataReport::LBtClickedBtrReportClear, VTS_I4)
	ON_EVENT(CDataReport, IDC_BT_REPORT_EXCEL, 1, CDataReport::LBtClickedBtReportExcel, VTS_I4)
	ON_EVENT(CDataReport, IDC_BT_REPORT_EXIT, 1, CDataReport::LBtClickedBtReportExit, VTS_I4)
END_EVENTSINK_MAP()


// CDataReport 消息处理程序


void CDataReport::LockCtrls(int nLock)
{
	const BOOL bLocked = _GetLockState(nLock, PSD_LEVEL_QC);
	const BOOL bEnable = !bLocked;

	GetDlgItem(IDC_BT_REPORT_CLEAR)->EnableWindow(bEnable);
}


void CDataReport::PassIncrease(int nInc)
{
	PostMessage(WM_DATA_CHANGEED, TRUE, nInc);
}


void CDataReport::FailIncrease(int nInc)
{
	PostMessage(WM_DATA_CHANGEED, FALSE, nInc);
}


void CDataReport::ClearData(void)
{
	((CBL_DataChart *)(GetDlgItem(IDC_REPORT_CHART)))->ClearData();

	SetDlgItemText(IDC_CT_REPORT_OKS, _T("良品数: 0"));
	SetDlgItemText(IDC_CT_REPORT_OK_RATE, _T("良品率: 0.00%"));
	SetDlgItemText(IDC_CT_REPORT_NGS, _T("不良数: 0"));
	SetDlgItemText(IDC_CT_REPORT_NG_RATE, _T("不良率: 0.00%"));
	SetDlgItemText(IDC_CT_REPORT_SUM, _T("总产出: 0"));
}


void CDataReport::LoadData(CString strFile)
{
	m_strFile = strFile;

	int nFind = strFile.ReverseFind('\\');

	if (nFind >= 0)
	{
		strFile.Delete(0, nFind + 1);
	}

	nFind = strFile.ReverseFind('.');

	if (nFind >= 0)
	{
		strFile = strFile.Left(nFind);
	}

	strFile.Insert(0, _T("DataReport-"));

	const CString strPath = _GetAppIniPath();

	const int nOK = GetPrivateProfileInt(strFile, _T("OK"), 0, strPath);
	const int nNG = GetPrivateProfileInt(strFile, _T("NG"), 0, strPath);

	((CBL_DataChart *)(GetDlgItem(IDC_REPORT_CHART)))->ClearData();

	SetDlgItemText(IDC_CT_REPORT_OKS, _T("良品数: 0"));
	SetDlgItemText(IDC_CT_REPORT_OK_RATE, _T("良品率: 0.00%"));
	SetDlgItemText(IDC_CT_REPORT_NGS, _T("不良数: 0"));
	SetDlgItemText(IDC_CT_REPORT_NG_RATE, _T("不良率: 0.00%"));
	SetDlgItemText(IDC_CT_REPORT_SUM, _T("总产出: 0"));

	PassIncrease(nOK);
	FailIncrease(nNG);
}


void CDataReport::SaveData(CString strFile)
{
	if (strFile.IsEmpty())
	{
		return;
	}

	m_strFile = strFile;

	int nFind = strFile.ReverseFind('\\');

	if (nFind >= 0)
	{
		strFile.Delete(0, nFind + 1);
	}

	nFind = strFile.ReverseFind('.');

	if (nFind >= 0)
	{
		strFile = strFile.Left(nFind);
	}

	strFile.Insert(0, _T("DataReport-"));

	CBL_DataChart *pChart = (CBL_DataChart *)GetDlgItem(IDC_REPORT_CHART);

	const CString strPath = _GetAppIniPath();

	CString strValue;
	strValue.Format(_T("%d"), pChart->GetPassNum());
	WritePrivateProfileString(strFile, _T("OK"), strValue, strPath);

	strValue.Format(_T("%d"), pChart->GetFailNum());
	WritePrivateProfileString(strFile, _T("NG"), strValue, strPath);
}


void CDataReport::_PreShowWnd(void)
{
	LockCtrls(APP_LOCK_AUTO);

	m_BtBaseOk.ShowWindow(SW_HIDE);
	m_BtBaseCancel.ShowWindow(SW_HIDE);
}


void CDataReport::_DefaultFocus(void)
{
	GetDlgItem(IDC_BT_REPORT_EXIT)->SetFocus();
}


void CDataReport::RtDbClickStatisticChart()
{
	// TODO: 在此处添加消息处理程序代码
	CBlender<CNgWarnLimit> Lmt(IDD_NGWARNLIMIT, this);

	Lmt->m_bUseNum = m_bUseNum;
	Lmt->m_bUseRate = m_bUseRate;

	Lmt->m_nNgLimit = m_nNgLimit;
	Lmt->m_dNgRateLimit = m_dNgRateLimit;

	if (IDOK == Lmt.CreateTopWnd(TRUE))
	{
		BOOL bDirty = FALSE;

		if (m_bUseNum != Lmt->m_bUseNum)
		{
			bDirty = TRUE;
			m_bUseNum = Lmt->m_bUseNum;
		}

		if (m_bUseRate != Lmt->m_bUseRate)
		{
			bDirty = TRUE;
			m_bUseRate = Lmt->m_bUseRate;
		}

		if (m_nNgLimit != Lmt->m_nNgLimit)
		{
			bDirty = TRUE;
			m_nNgLimit = Lmt->m_nNgLimit;
		}

		if (m_dNgRateLimit != Lmt->m_dNgRateLimit)
		{
			bDirty = TRUE;
			m_dNgRateLimit = Lmt->m_dNgRateLimit;
		}

		_FileDirty(bDirty);

		((CBL_DataChart *)(GetDlgItem(IDC_REPORT_CHART)))->SetNgLImit(m_nNgLimit);
		((CBL_DataChart *)(GetDlgItem(IDC_REPORT_CHART)))->SetNgRateLimit(m_dNgRateLimit);
	}
}


void CDataReport::NgNumReachLimitStatisticChart(long nNgNum)
{
	// TODO: 在此处添加消息处理程序代码
	if (nullptr != m_pOwner && m_bUseNum)
	{
		m_pOwner->PostMessage(gMsgStatisticBeyongLimit, TRUE);
	}
}


void CDataReport::NgRateReachLimitStatisticChart(double dNgRate)
{
	// TODO: 在此处添加消息处理程序代码
	if (nullptr != m_pOwner && m_bUseRate)
	{
		m_pOwner->PostMessage(gMsgStatisticBeyongLimit, FALSE);
	}
}


void CDataReport::LBtClickedBtrReportClear(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	CMsgBox MsgBox(this);

	if (IDYES == MsgBox.ShowMsg(_T("清除后不可恢复，确认要清除数据吗？"), _T("数据统计"), MB_YESNO | MB_ICONQUESTION))
	{
		ClearData();
		SaveData(m_strFile);
	}
}


void CDataReport::LBtClickedBtReportExcel(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	if (nullptr != m_pOwner)
	{
		m_pOwner->PostMessage(gMsgStatisticReport);
	}
}


void CDataReport::LBtClickedBtReportExit(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	OnCancel();
}


afx_msg LRESULT CDataReport::OnDataChangeed(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		CBL_DataChart *pChart = (CBL_DataChart *)GetDlgItem(IDC_REPORT_CHART);
		pChart->PassIncrease(lParam);

		m_nPass = pChart->GetPassNum();
		m_nFail = pChart->GetFailNum();

		CString strPass = _GetKilobitStr(m_nPass);
		strPass.Insert(0, _T("良品数: "));
		SetDlgItemText(IDC_CT_REPORT_OKS, strPass);

		double dRate = 0;

		const int nSum = m_nPass + m_nFail;

		if (nSum)
		{
			dRate = (double)m_nPass / nSum * 100.0;
		}

		CString strRate;
		strRate.Format(_T("良品率: %.2f%%"), dRate);
		SetDlgItemText(IDC_CT_REPORT_OK_RATE, strRate);

		strRate.Format(_T("不良率: %.2f%%"), nSum? 100 - dRate: 0);
		SetDlgItemText(IDC_CT_REPORT_NG_RATE, strRate);

		CString strSum = _GetKilobitStr(m_nPass + m_nFail);
		strSum.Insert(0, _T("总产出: "));
		SetDlgItemText(IDC_CT_REPORT_SUM, strSum);
	}
	else
	{
		CBL_DataChart *pChart = (CBL_DataChart *)GetDlgItem(IDC_REPORT_CHART);
		pChart->FailIncrease(lParam);

		m_nPass = pChart->GetPassNum();
		m_nFail = pChart->GetFailNum();

		CString strFail = _GetKilobitStr(m_nFail);
		strFail.Insert(0, _T("不良数: "));
		SetDlgItemText(IDC_CT_REPORT_NGS, strFail);

		double dRate = 0;

		const int nSum = m_nPass + m_nFail;

		if (nSum)
		{
			dRate = (double)m_nPass / nSum * 100.0;
		}

		CString strRate;
		strRate.Format(_T("良品率: %.2f%%"), dRate);
		SetDlgItemText(IDC_CT_REPORT_OK_RATE, strRate);

		strRate.Format(_T("不良率: %.2f%%"), nSum? 100 - dRate: 0);
		SetDlgItemText(IDC_CT_REPORT_NG_RATE, strRate);

		CString strSum = _GetKilobitStr(m_nPass + m_nFail);
		strSum.Insert(0, _T("总产出: "));
		SetDlgItemText(IDC_CT_REPORT_SUM, strSum);
	}

	SaveData(m_strFile);

	return 0;
}
