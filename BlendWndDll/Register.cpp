// RegLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "Register.h"
#include "afxdialogex.h"


// CRegister 对话框

IMPLEMENT_DYNAMIC(CRegister, CTpLayerWnd)

CRegister::CRegister(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CRegister::IDD, pParent)
{

}

CRegister::CRegister(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
{
}

CRegister::~CRegister()
{
}

void CRegister::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegister, CTpLayerWnd)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CRegister, CTpLayerWnd)
	ON_EVENT(CRegister, IDC_EDIT_REGWND_PSD, 2, CRegister::EnterPressedEditRegwndpsd, VTS_BSTR)
END_EVENTSINK_MAP()

// CRegister 消息处理程序


BOOL CRegister::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_strId = ReadSoftwareId();

	_ReadKeyWord();

	CAES Aes;

	const CString strLeft = m_strId.Left(5);
	const CString strRight = m_strId.Right(5);
	const CString strId = Aes.Cipher(strLeft + strRight);

	if (strId == m_strPsd)
	{
		((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->SetValueText(_T("此软件已注册！"));

		m_BtBaseOk.ShowWindow(SW_HIDE);
		m_BtBaseCancel.SetWindowText(_T("退出"));
	}

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_ID)))->SetValueText(m_strId);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CRegister::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	const CString strPsd = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->GetValueText();

	if (strPsd.IsEmpty())
	{
		ShowMsgBox(_T("密钥不能为空！"), _T("注册"), MB_OK | MB_ICONERROR);
		return;
	}

	CAES Aes;
	
	const CString strLeft = m_strId.Left(5);
	const CString strRight = m_strId.Right(5);
	const CString strId = Aes.Cipher(strLeft + strRight);

	if (strId == strPsd)
	{
		CRegKey regKey;
		CString strKeyName;

		const CString strPath = Aes.Cipher(_T("Register"));

		strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

		const LONG nRes = regKey.Open(HKEY_LOCAL_MACHINE, strKeyName);

		const CString strRegKeyHex = Aes.Cipher(_T("SerialNumber"));

		if (ERROR_SUCCESS == nRes)
		{	
			regKey.SetStringValue(strRegKeyHex, strPsd);
		}
		else if (ERROR_SUCCESS == regKey.Create(HKEY_LOCAL_MACHINE, strKeyName))
		{
			regKey.SetStringValue(strRegKeyHex, strPsd);
		}
		else
		{
			ShowMsgBox(_T("访问注册文件失败！"), _T("注册"), MB_OK | MB_ICONERROR);
			return;
		}

		regKey.Close();

		ShowMsgBox(_T("注册成功"), _T("注册"), MB_OK | MB_ICONINFORMATION);

		((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->SetValueText(_T("此软件已注册！"));

		m_BtBaseOk.ShowWindow(SW_HIDE);
		m_BtBaseCancel.SetWindowText(_T("退出"));

		return;
	}
	else
	{		
		ShowMsgBox(_T("密钥错误，注册失败！"), _T("注册"), MB_OK | MB_ICONINFORMATION);

		return;
	}

	CTpLayerWnd::OnOK();
}


BOOL CRegister::IsRegistred(void)
{
	m_strId = ReadSoftwareId();

	_ReadKeyWord();

	CAES Aes;
	const CString strLeft = m_strId.Left(5);
	const CString strRight = m_strId.Right(5);
	const CString strId = Aes.Cipher(strLeft + strRight);

	return (strId == m_strPsd);
}


CString CRegister::ReadSoftwareId(void)
{
	CHardwareInfo Devinfo;
	DRIVEINFO DiskInfo;
	CString strId;

	BOOL bRes = Devinfo.GetDriveInfo(0, &DiskInfo);

	if (bRes)
	{
		strId = DiskInfo.sSerialNumber;

		if (strId.GetLength() < 8)
		{
			bRes = FALSE;
		}
	}

	if (FALSE == bRes)
	{
		std::vector<CString> vMacAdd;
		Devinfo.GetMacAdd(&vMacAdd);

		strId = vMacAdd[0];

		const int nLen = strId.GetLength();

		for (int i = nLen - 1; i >= 0; i--)
		{
			if ('-' == strId[i])
			{
				strId.Delete(i, 1);
			}
		}
	}

	CAES Aes;
	strId = Aes.Cipher(strId);

	if (strId.GetLength() > 32)
	{
		const CString strLeft = strId.Left(16);
		const CString strRight = strId.Right(16);

		strId = strLeft + strRight;
	}

	return strId;
}


void CRegister::_ReadKeyWord(void)
{
	CRegKey regKey;
	CString strKeyName;

	CAES Aes;
	const CString strPath = Aes.Cipher(_T("Register"));

	strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

	BOOL bOpenSuccess = ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName);

	if (bOpenSuccess)
	{
		DWORD dwChars = 1024;

		const CString strRegKeyHex = Aes.Cipher(_T("SerialNumber"));

		if (ERROR_SUCCESS == regKey.QueryStringValue(strRegKeyHex, m_strPsd.GetBufferSetLength(MAX_PATH + 1), &dwChars))
		{
			m_strPsd.ReleaseBuffer();
		}
		else
		{
			m_strPsd = strRegKeyHex;
		}

		regKey.Close();
	}
}


void CRegister::EnterPressedEditRegwndpsd(LPCTSTR strValue)
{
	// TODO: 在此处添加消息处理程序代码
	OnOK();
}
