// NameInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Passcal-voc.h"
#include "NameInfo.h"
#include "afxdialogex.h"


// CNameInfo �Ի���

IMPLEMENT_DYNAMIC(CNameInfo, CTpLayerWnd)

CNameInfo::CNameInfo(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CNameInfo::IDD, pParent)
	, m_nStart(1)
	, m_nNums(6)
{

}

CNameInfo::CNameInfo(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
	, m_nStart(1)
	, m_nNums(6)
{
}

CNameInfo::~CNameInfo()
{
}

void CNameInfo::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNameInfo, CTpLayerWnd)
END_MESSAGE_MAP()


// CNameInfo ��Ϣ�������


BOOL CNameInfo::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	const CString strPath = _GetAppIniPath();

	const CString strKey = _T("Opt");

	GetPrivateProfileString(strKey, _T("Prefix"), _T(""),
		m_strPrefix.GetBufferSetLength(MAX_PATH + 1), MAX_PATH, strPath);
	m_strPrefix.ReleaseBuffer();

	m_nStart = GetPrivateProfileInt(strKey, _T("Start"), 1, strPath);
	m_nNums = GetPrivateProfileInt(strKey, _T("Nums"), 6, strPath);

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_PREFIX)))->SetValueText(m_strPrefix);
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_NUM_START)))->SetValue(m_nStart);
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_NUM_BITS)))->SetValue(m_nNums);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CNameInfo::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_strPrefix = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_PREFIX)))->GetValueText();
	m_nStart = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_NUM_START)))->GetIntValue();
	m_nNums = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_NUM_BITS)))->GetIntValue();

	const CString strPath = _GetAppIniPath();

	const CString strKey = _T("Opt");

	WritePrivateProfileString(strKey, _T("Prefix"), m_strPrefix, strPath);

	CString strValue;
	strValue.Format(_T("%d"),  m_nStart);
	WritePrivateProfileString(strKey, _T("Start"), strValue, strPath);

	strValue.Format(_T("%d"),  m_nNums);
	WritePrivateProfileString(strKey, _T("Nums"), strValue, strPath);

	CTpLayerWnd::OnOK();
}
