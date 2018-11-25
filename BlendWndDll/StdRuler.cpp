// StdRuler.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StdRuler.h"
#include "afxdialogex.h"


// CStdRuler �Ի���

IMPLEMENT_DYNAMIC(CStdRuler, CTpLayerWnd)

CStdRuler::CStdRuler(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CStdRuler::IDD, pParent)
	, m_dStdDist(15)
	, m_dStdRadius(10)
	, m_dStdDistBkup(15)
	, m_dStdRadiusBkup(10)
{

}

CStdRuler::CStdRuler(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
	, m_dStdDist(15)
	, m_dStdRadius(10)
	, m_dStdDistBkup(15)
	, m_dStdRadiusBkup(10)
{
}

CStdRuler::~CStdRuler()
{
}

void CStdRuler::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStdRuler, CTpLayerWnd)
END_MESSAGE_MAP()


// CStdRuler ��Ϣ�������


BOOL CStdRuler::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_dStdDistBkup = m_dStdDist;
	m_dStdRadiusBkup = m_dStdRadius;

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_DIST)))->SetValue(m_dStdDist);
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_RADIUS)))->SetValue(m_dStdRadius);

	LockCtrls(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CStdRuler::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_dStdDist = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_DIST)))->GetDoubleValue();
	m_dStdRadius = ((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_RADIUS)))->GetDoubleValue();

	if (m_dStdDistBkup != m_dStdDist ||
		m_dStdRadiusBkup != m_dStdRadius)
	{
		_FileDirty(TRUE);
	}

	CTpLayerWnd::OnOK();
}


void CStdRuler::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	m_dStdDist = m_dStdDistBkup;
	m_dStdRadius = m_dStdRadiusBkup;

	CTpLayerWnd::OnCancel();
}


void CStdRuler::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		SET_FILE_VERSION(1);

		ar << m_dStdDist;
		ar << m_dStdRadius;
	}
	else
	{	// loading code
		GET_FILE_VERSION;

		switch (nVersion)
		{
		case 1:
			ar >> m_dStdDist;
			ar >> m_dStdRadius;
			break;

		default:
			break;
		}
	}
}


void CStdRuler::LockCtrls(int nLock)
{
	const BOOL bLocked = _GetLockState(nLock, PSD_LEVEL_TE);
	const BOOL bEnable = !bLocked;

	((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_DIST)))->SetReadOnly(bLocked);
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_RULER_RADIUS)))->SetReadOnly(bLocked);

	m_BtBaseOk.EnableWindow(bEnable);
}
