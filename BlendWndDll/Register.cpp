// RegLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Register.h"
#include "afxdialogex.h"


// CRegister �Ի���

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

// CRegister ��Ϣ��������


BOOL CRegister::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	m_strId = ReadSoftwareId();

	_ReadKeyWord();

	CAES Aes;

	const CString strLeft = m_strId.Left(5);
	const CString strRight = m_strId.Right(5);
	const CString strId = Aes.Cipher(strLeft + strRight);

	if (strId == m_strPsd)
	{
		((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->SetValueText(_T("��������ע�ᣡ"));

		m_BtBaseOk.ShowWindow(SW_HIDE);
		m_BtBaseCancel.SetWindowText(_T("�˳�"));
	}

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_ID)))->SetValueText(m_strId);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CRegister::OnOK()
{
	// TODO: �ڴ�����ר�ô����/����û���
	const CString strPsd = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->GetValueText();

	if (strPsd.IsEmpty())
	{
		ShowMsgBox(_T("��Կ����Ϊ�գ�"), _T("ע��"), MB_OK | MB_ICONERROR);
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
			ShowMsgBox(_T("����ע���ļ�ʧ�ܣ�"), _T("ע��"), MB_OK | MB_ICONERROR);
			return;
		}

		regKey.Close();

		ShowMsgBox(_T("ע��ɹ�"), _T("ע��"), MB_OK | MB_ICONINFORMATION);

		((CBL_Edit *)(GetDlgItem(IDC_EDIT_REGWND_PSD)))->SetValueText(_T("��������ע�ᣡ"));

		m_BtBaseOk.ShowWindow(SW_HIDE);
		m_BtBaseCancel.SetWindowText(_T("�˳�"));

		return;
	}
	else
	{		
		ShowMsgBox(_T("��Կ����ע��ʧ�ܣ�"), _T("ע��"), MB_OK | MB_ICONINFORMATION);

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
	// TODO: �ڴ˴�������Ϣ�����������
	OnOK();
}