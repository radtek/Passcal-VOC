// TrialPsd.cpp : 实现文件
//

#include "stdafx.h"
#include "TrialPsd.h"
#include "afxdialogex.h"

#define WM_SHOW_TRIAL_BOX	WM_USER + 123

// CTrialPsd 对话框

IMPLEMENT_DYNAMIC(CTrialPsd, CTpLayerWnd)

CTrialPsd::CTrialPsd(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CTrialPsd::IDD, pParent)
	, m_bTrialAgain(FALSE)
	, m_nMaxLimit(100000)
	, m_nDayLimit(30)
	, m_bThreadAlive(FALSE)
	, m_pThread(nullptr)
{
	m_TrialInfo.nTrial	 = 1;
	m_TrialInfo.nOutputs = m_nMaxLimit;
	m_TrialInfo.nRunTime = TST_LIMIT_HOURS;
}

CTrialPsd::CTrialPsd(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
	, m_bTrialAgain(FALSE)
	, m_nMaxLimit(100000)
	, m_nDayLimit(30)
	, m_bThreadAlive(FALSE)
	, m_pThread(nullptr)
{
	m_TrialInfo.nTrial	 = 1;
	m_TrialInfo.nOutputs = m_nMaxLimit;
	m_TrialInfo.nRunTime = TST_LIMIT_HOURS;
}

CTrialPsd::~CTrialPsd()
{
	m_bThreadAlive = FALSE;

	while (nullptr != m_pThread)
	{
		Sleep(10);
	}
}

void CTrialPsd::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTrialPsd, CTpLayerWnd)
	ON_MESSAGE(WM_SHOW_TRIAL_BOX, &CTrialPsd::OnShowTrialBox)
END_MESSAGE_MAP()


// CTrialPsd 消息处理程序


BOOL CTrialPsd::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRegister r;
	m_strId = r.ReadSoftwareId();

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_TRIAL_ID)))->SetValueText(m_strId);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTrialPsd::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CRegKey regKey;
	CString strKeyName;

	CAES Aes;
	const CString strApp = AfxGetAppName();
	const CString strPath = Aes.Cipher(_T("TrialInfo"));
	strKeyName.Format(_T("Software\\%s\\%s"), strApp, strPath);

	BOOL bKeyVal = (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName));

	if (!bKeyVal)
	{
		ShowMsgBox(_T("访问密码文件失败，请联系供应商解决。"), nullptr, MB_OK || MB_ICONERROR);
		return;
	}

	CString strQery;
	CString strValue;
	CString strRegKeyHex;
	DWORD dwChars = 1024;

	//////////////////////////////////////////////////////////////////////////
	const CString strInput = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_TRIAL_PSD)))->GetValueText();

	const CString strLeft = m_strId.Right(5);
	const CString strRight = m_strId.Left(5);
	const CString strKey = Aes.Cipher(strLeft + strRight);

	m_bTrialAgain = FALSE;

	if (strInput == strKey)
	{
		SendMessage(WM_SHOW_TRIAL_BOX, TRUE);
	}
	else
	{
		dwChars = 1024;
		strRegKeyHex = Aes.Cipher(_T("TrialCount"));
		bKeyVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
		strQery.ReleaseBuffer();

		if (bKeyVal)
		{
			strQery = Aes.Inv_Cipher(strQery);
			const int nPos = strQery.Find(_T("Tr"));

			if (0 == nPos)
			{
				strQery.Delete(0, 2);
				m_TrialInfo.nTrial = _ttoi(strQery);

				CString strTmp;
				strTmp.Format(_T("%s%s%d"), strApp.Left(2), strApp.Right(2), m_TrialInfo.nTrial);

				strTmp = Aes.Cipher(strTmp);

				if (strTmp == strInput)
				{
					m_bTrialAgain = TRUE;

					m_TrialInfo.nOutputs = (m_nMaxLimit >> 2) * m_TrialInfo.nTrial;
					m_TrialInfo.nRunTime = ((m_nDayLimit >> 2) * 24 * 60) * m_TrialInfo.nTrial;

					m_TrialInfo.nTrial++;
				}
			}

			regKey.Close();
		}

		if (!m_bTrialAgain)
		{
			SendMessage(WM_SHOW_TRIAL_BOX, FALSE);

			regKey.Close();
			return;
		}
	}

	if (m_bTrialAgain)
	{
		regKey.Close();
		regKey.Create(HKEY_LOCAL_MACHINE, strKeyName);

		strRegKeyHex = Aes.Cipher(_T("TrialCount"));
		strValue.Format(_T("Tr%d"), m_TrialInfo.nTrial);
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);

		strRegKeyHex = Aes.Cipher(_T("Output"));
		strValue.Format(_T("Op%d"), m_TrialInfo.nOutputs);
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);

		strRegKeyHex = Aes.Cipher(_T("RunTime"));
		strValue.Format(_T("Rt%d"), m_TrialInfo.nRunTime);
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);

		SYSTEMTIME st;
		GetLocalTime(&st);
		const CTimeSpan span(m_nDayLimit - (m_nDayLimit >> 2) * (m_TrialInfo.nTrial - 1), 0, 0, 0);

		CTime tEnd(st);
		tEnd += span;

		strRegKeyHex = Aes.Cipher(_T("EndDate"));
		strValue.Format(_T("%d%02d%02d"), tEnd.GetYear(), tEnd.GetMonth(), tEnd.GetDay());
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);
	}
	else
	{
		m_TrialInfo.nTrial = 0;

		strRegKeyHex = Aes.Cipher(_T("IsTrial"));
		strValue = Aes.Cipher(_T("AppChecked"));
		regKey.SetStringValue(strRegKeyHex, strValue);

		strRegKeyHex = Aes.Cipher(_T("TrialPsd"));
		regKey.SetStringValue(strRegKeyHex, strInput);
	}

	regKey.Close();

	CTpLayerWnd::OnOK();
}


void CTrialPsd::GetTrialInfo(int nLimit, int nDays)
{
	m_nMaxLimit = nLimit;
	m_nDayLimit = nDays;

	m_TrialInfo.nTrial	 = 1;
	m_TrialInfo.nOutputs = m_nMaxLimit;
	m_TrialInfo.nRunTime = TST_LIMIT_HOURS;

	CRegister r;
	m_strId = r.ReadSoftwareId();

	//////////////////////////////////////////////////////////////////////////
	CRegKey regKey;
	CString strKeyName;

	CAES Aes;
	const CString strPath = Aes.Cipher(_T("TrialInfo"));
	strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

	BOOL bRetVal = (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName));

	if (bRetVal)
	{
		CString strQery;
		CString strRegKeyHex;

		DWORD dwChars = 1024;
		strRegKeyHex = Aes.Cipher(_T("IsTrial"));
		bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
		strQery.ReleaseBuffer();

		if (bRetVal)
		{
			strQery = Aes.Inv_Cipher(strQery);
			m_TrialInfo.nTrial = (_T("AppChecked") != strQery);
		}

		if (m_TrialInfo.nTrial)
		{
			dwChars = 1024;
			strRegKeyHex = Aes.Cipher(_T("TrialCount"));
			bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
			strQery.ReleaseBuffer();

			if (bRetVal)
			{
				strQery = Aes.Inv_Cipher(strQery);
				const int nPos = strQery.Find(_T("Tr"));

				if (0 == nPos)
				{
					strQery.Delete(0, 2);
					m_TrialInfo.nTrial = _ttoi(strQery);
				}
				else
				{
					m_TrialInfo.nTrial = -1;
				}
			}
		}

		dwChars = 1024;
		strRegKeyHex = Aes.Cipher(_T("Output"));
		bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
		strQery.ReleaseBuffer();

		if (bRetVal)
		{
			strQery = Aes.Inv_Cipher(strQery);
			const int nPos = strQery.Find(_T("Op"));

			if (0 == nPos)
			{
				strQery.Delete(0, 2);
				m_TrialInfo.nOutputs = _ttoi(strQery);
			}
		}

		dwChars = 1024;
		strRegKeyHex = Aes.Cipher(_T("RunTime"));
		bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
		strQery.ReleaseBuffer();

		if (bRetVal)
		{
			strQery = Aes.Inv_Cipher(strQery);
			const int nPos = strQery.Find(_T("Rt"));

			if (0 == nPos)
			{
				strQery.Delete(0, 2);
				m_TrialInfo.nRunTime = _ttoi(strQery);
			}
		}

		dwChars = 1024;
		strRegKeyHex = Aes.Cipher(_T("EndDate"));
		bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
		strQery.ReleaseBuffer();

		if (bRetVal)
		{
			strQery = Aes.Inv_Cipher(strQery);

			SYSTEMTIME et = {0};

			et.wYear = _ttoi(strQery.Left(4));
			et.wMonth = _ttoi(strQery.Mid(4, 2));
			et.wDay = _ttoi(strQery.Mid(6, 2));

			SYSTEMTIME ct;
			GetLocalTime(&ct);

			const CTime tEnd(et);
			const CTime tNow(ct);

			if (tNow > tEnd)
			{
				m_TrialInfo.nRunTime = TST_LIMIT_HOURS;
			}
		}

		if (m_TrialInfo.nTrial >= 0)
		{
			dwChars = 1024;
			strRegKeyHex = Aes.Cipher(_T("TrialPsd"));
			bRetVal = (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, strQery.GetBufferSetLength(MAX_PATH + 1), &dwChars));
			strQery.ReleaseBuffer();

			if (bRetVal)
			{
				strQery = Aes.Inv_Cipher(strQery);
				const CString strMixt = m_strId.Right(5) + m_strId.Left(5);

				if (strMixt == strQery)
				{
					m_TrialInfo.nTrial = 0;
				}
			}
		}

		regKey.Close();
	}
}


void CTrialPsd::SetTrialInfo(void)
{
	CSyncCtrl s(&m_csRecord);

	CAES Aes;
	const CString strPath = Aes.Cipher(_T("TrialInfo"));

	CRegKey regKey;
	CString strKeyName;
	strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

	BOOL bRetVal = (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName));

	if (bRetVal)
	{
		CString strValue;
		CString strRegKeyHex;
		
		strRegKeyHex = Aes.Cipher(_T("Output"));
		strValue.Format(_T("Op%d"), m_TrialInfo.nOutputs);
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);

		strRegKeyHex = Aes.Cipher(_T("RunTime"));
		strValue.Format(_T("Rt%d"), m_TrialInfo.nRunTime);
		strValue = Aes.Cipher(strValue);
		regKey.SetStringValue(strRegKeyHex, strValue);

		regKey.Close();
	}
}


void CTrialPsd::StartTickCount(void)
{
	if (m_TrialInfo.nTrial)
	{
		m_pThread = AfxBeginThread(_TickCountThread, this);
	}
}

// #define SHOW_OUTPUT_COUNT

BOOL CTrialPsd::UpdateInfo(int nInc)
{
	if (m_TrialInfo.nTrial)
	{
		m_TrialInfo.nOutputs += nInc;

#ifdef SHOW_OUTPUT_COUNT
		CString strBox;
		strBox.Format(_T("%d"), m_TrialInfo.nOutputs);
		ShowMsgBox(strBox, nullptr, MB_OK);
#endif // SHOW_OUTPUT_COUNT	

		if (m_TrialInfo.nOutputs > m_nMaxLimit ||
			m_TrialInfo.nRunTime > TST_LIMIT_HOURS)
		{
			CBlender<CTrialPsd> psd(IDD_TRIALPSD, this);

			psd->m_nMaxLimit = m_nMaxLimit;
			psd->m_nDayLimit = m_nDayLimit;

			if (IDOK == psd.CreateTopWnd(TRUE))
			{
				m_TrialInfo = psd->m_TrialInfo;
			}
		}

		SetTrialInfo();
	}

	return m_TrialInfo.nTrial;
}


BOOL CTrialPsd::IsNeedUpdatePsd(void)
{
	const BOOL bNeed = m_TrialInfo.nTrial &&
		(m_TrialInfo.nOutputs >= m_nMaxLimit || m_TrialInfo.nOutputs > TST_LIMIT_HOURS);

	return bNeed;
}


void CTrialPsd::_DefaultFocus(void)
{
	GetDlgItem(IDC_EDIT_TRIAL_PSD)->SetFocus();
}


UINT CTrialPsd::_TickCountThread(LPVOID pParam)
{
	CTrialPsd *pTrial = (CTrialPsd *)pParam;

	const UINT nRetVul = pTrial->_TickCountThreadKernel();

	return nRetVul;
}


UINT CTrialPsd::_TickCountThreadKernel(void)
{
	m_bThreadAlive = TRUE;

	while (m_bThreadAlive)
	{
		for (int i = 0; i < 6000; i++)
		{
			Sleep(10);

			if (!m_bThreadAlive)
			{
				goto LABEL_END;
			}
		}

		m_TrialInfo.nRunTime++;

		SetTrialInfo();
	}

LABEL_END:

	m_pThread = nullptr;

	return 0;
}


afx_msg LRESULT CTrialPsd::OnShowTrialBox(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		ShowMsgBox(_T("密码正确，软件已升级成正式版。"), nullptr, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		ShowMsgBox(_T("密码错误，请重新输入。"), nullptr, MB_OK | MB_ICONINFORMATION);
	}

	return 0;
}
