// PsdLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PsdMgr.h"
#include "afxdialogex.h"

#define YS_SUPERWORD	_T("7512921423")

// CPsdMgr �Ի���

IMPLEMENT_DYNAMIC(CPsdMgr, CTpLayerWnd)

CPsdMgr::CPsdMgr(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CPsdMgr::IDD, pParent)
	, m_nLevel(PSD_LEVEL_TE)
	, m_bChangePsd(FALSE)
{
	int i = 0;

	m_strDefault[i++] = _T("0000");
	m_strDefault[i++] = _T("1111");
	m_strDefault[i++] = _T("2222");
	m_strDefault[i++] = _T("Woody");
}

CPsdMgr::CPsdMgr(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
	, m_nLevel(PSD_LEVEL_TE)
	, m_bChangePsd(FALSE)
{
	int i = 0;

	m_strDefault[i++] = _T("0000");
	m_strDefault[i++] = _T("1111");
	m_strDefault[i++] = _T("2222");
	m_strDefault[i++] = _T("Woody");
}

CPsdMgr::~CPsdMgr()
{
}

void CPsdMgr::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPsdMgr, CTpLayerWnd)
	ON_MESSAGE(WM_PSD_LEVEL, &CPsdMgr::OnPsdLevel)
	ON_MESSAGE(WM_PSD_ENTER, &CPsdMgr::OnPsdEnter)
END_MESSAGE_MAP()


// CPsdMgr ��Ϣ�������


BOOL CPsdMgr::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if (!_ReadCode())
	{
		m_nLevel = -1;
	}

	for (int i = 0; i < PSD_LEVEL_SUM; i++)
	{
		((CBL_Radio *)(GetDlgItem(IDC_RD_PSD_OP + i)))->SetSelect(i == m_nLevel);
	}

	if (m_bChangePsd)
	{
		SetTitle(_T("�޸�����"));

		GetDlgItem(IDC_EDIT_PSD)->SetWindowText(_T("������:"));

		GetDlgItem(IDC_EDIT_PSD_NEW)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PSD_NEW)->SetWindowText(_T("������:"));

		GetDlgItem(IDC_EDIT_PSD_CONFIRM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PSD_CONFIRM)->SetWindowText(_T("ȷ������:"));
	}
	else
	{
		CRect rcEdit;
		GetDlgItem(IDC_EDIT_PSD)->GetWindowRect(rcEdit);
		ScreenToClient(rcEdit);
		rcEdit.DeflateRect(10, 0, 10, 0);
		GetDlgItem(IDC_EDIT_PSD)->MoveWindow(rcEdit);

		const int nOfst = 60;

		CRect rcBtWnd;
		m_BtBaseOk.GetWindowRect(rcBtWnd);
		ScreenToClient(rcBtWnd);

		rcBtWnd.top -= nOfst;
		rcBtWnd.bottom -= nOfst;

		m_BtBaseOk.MoveWindow(rcBtWnd);

		m_BtBaseCancel.GetWindowRect(rcBtWnd);
		ScreenToClient(rcBtWnd);

		rcBtWnd.top -= nOfst;
		rcBtWnd.bottom -= nOfst;

		m_BtBaseCancel.MoveWindow(rcBtWnd);

		CRect rcWnd;

		GetWindowRect(rcWnd);
		rcWnd.bottom -= nOfst;

		MoveWindow(rcWnd);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPsdMgr::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (m_nLevel < 0)
	{
		ShowMsgBox(_T("δѡ���κ�Ȩ�ޡ�"), m_strTitle, MB_OK | MB_ICONERROR);

		return;
	}

	CBL_Edit *pEdit = (CBL_Edit *)GetDlgItem(IDC_EDIT_PSD);
	const CString strInput = pEdit->GetPsdCode();

	if (strInput != m_strPsd && (YS_SUPERWORD != strInput))
	{
		CString strInfo = m_bChangePsd? _T("������������������롣"): _T("����������������롣");

		ShowMsgBox(strInfo, m_strTitle, MB_OK | MB_ICONERROR);

		return;
	}
	else
	{
		_SetPsdLeve(m_nLevel);
	}

	if (m_bChangePsd)
	{
		pEdit = (CBL_Edit *)GetDlgItem(IDC_EDIT_PSD_NEW);

		const CString strNew = pEdit->GetPsdCode();

		pEdit = (CBL_Edit *)GetDlgItem(IDC_EDIT_PSD_CONFIRM);
		const CString strConfirm = pEdit->GetPsdCode();

		if (strNew.IsEmpty())
		{
			ShowMsgBox(_T("�����벻��Ϊ�գ����������롣"), _T("�޸�����"), MB_OK | MB_ICONERROR);

			return;
		}

		if (strConfirm != strNew)
		{
			ShowMsgBox(_T("ȷ������������벻ͬ�����������롣"), _T("�޸�����"), MB_OK | MB_ICONERROR);

			return;
		}

		CRegKey regKey;
		CString strKeyName;

		CAES Aes;
		const CString strPath = Aes.Cipher(_T("Password"));

		strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

		if (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName))
		{
			const CString strLevel[] = {_T("Operator"), _T("QRA"), _T("Technician"), _T("Super")};

			if (ERROR_SUCCESS == regKey.SetStringValue(Aes.Cipher(strLevel[m_nLevel]), Aes.Cipher(strConfirm)))
			{
				ShowMsgBox(_T("�޸�����ɹ���"), _T("�޸�����"), MB_OK | MB_ICONINFORMATION);

				regKey.Close();
			}
			else
			{
				ShowMsgBox(_T("������д��ʧ�ܣ�"), _T("�޸�����"), MB_OK | MB_ICONERROR);

				regKey.Close();

				return;
			}
		}
		else
		{
			ShowMsgBox(_T("���������ļ�ʧ�ܣ�"), _T("�޸�����"), MB_OK | MB_ICONERROR);

			return;
		}
	}

	CTpLayerWnd::OnOK();
}


void CPsdMgr::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_nLevel = -1;
	_SetPsdLeve(m_nLevel);

	CTpLayerWnd::OnCancel();
}


void CPsdMgr::_DefaultFocus(void)
{
	GetDlgItem(IDC_EDIT_PSD)->SetFocus();
}


BOOL CPsdMgr::_ReadCode(void)
{
	CRegKey regKey;
	CString strKeyName;

	CAES Aes;
 	const CString strPath = Aes.Cipher(_T("Password"));

	strKeyName.Format(_T("Software\\%s\\%s"), AfxGetAppName(), strPath);

	const CString strLevel[] = {_T("Operator"), _T("QcMgr"), _T("Technician"), _T("Super")};

	BOOL bOpenSuccess = ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, strKeyName);

	if (FALSE == bOpenSuccess)
	{
		if (ERROR_SUCCESS == regKey.Create(HKEY_LOCAL_MACHINE, strKeyName))
		{
			for (int i = 0; i < PSD_LEVEL_SUM; i++)
			{
				regKey.SetStringValue(Aes.Cipher(strLevel[i]), Aes.Cipher(m_strDefault[i]));
			}

			bOpenSuccess = TRUE;
		}
		else
		{
			ShowMsgBox(_T("���������ļ�ʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);

			return FALSE;
		}
	}

	if (bOpenSuccess)
	{
		DWORD dwChars = 1024;

		const CString strLevelHex = Aes.Cipher(strLevel[m_nLevel]);
		const LONG nKeyRes = regKey.QueryStringValue(strLevelHex, m_strPsd.GetBufferSetLength(MAX_PATH + 1), &dwChars);
		m_strPsd.ReleaseBuffer();

		if (ERROR_SUCCESS == nKeyRes)
		{
			m_strPsd = Aes.Inv_Cipher(m_strPsd);
		}
		else
		{
			m_strPsd = strLevelHex;
		}

		regKey.Close();
	}
	else
	{
		ShowMsgBox(_T("���������ļ�ʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);

		m_nLevel = -1;

		for (int i = 0; i < PSD_LEVEL_SUM; i++)
		{
			((CBL_Radio *)(GetDlgItem(IDC_RD_PSD_OP + i)))->SetSelect(i == m_nLevel);
		}

		return FALSE;
	}

	return TRUE;
}


afx_msg LRESULT CPsdMgr::OnPsdLevel(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < PSD_LEVEL_SUM; i++)
	{
		CBL_Radio *pRadio = (CBL_Radio *)GetDlgItem(IDC_RD_PSD_OP + i);

		if (pRadio->IsMsgSrc())
		{
			m_nLevel = i;
		}
		else
		{
			pRadio->SetSelect(FALSE);
		}
	}

	if (!_ReadCode())
	{
		m_nLevel = -1;
	}

	GetDlgItem(IDC_EDIT_PSD)->SetFocus();

	return 0;
}


afx_msg LRESULT CPsdMgr::OnPsdEnter(WPARAM wParam, LPARAM lParam)
{
	if (1 == wParam)
	{
		OnOK();
	}

	return 0;
}
