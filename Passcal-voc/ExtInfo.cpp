// ExtInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Passcal-voc.h"
#include "ExtInfo.h"
#include "afxdialogex.h"


// CExtInfo �Ի���

IMPLEMENT_DYNAMIC(CExtInfo, CTpLayerWnd)

CExtInfo::CExtInfo(CWnd* pParent /*=NULL*/)
	: CTpLayerWnd(CExtInfo::IDD, pParent)
{

}

CExtInfo::CExtInfo(UINT nIDTemplate, CWnd * pParent/* = nullptr*/)
	: CTpLayerWnd(nIDTemplate, pParent)
{
}

CExtInfo::~CExtInfo()
{
}

void CExtInfo::DoDataExchange(CDataExchange* pDX)
{
	CTpLayerWnd::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExtInfo, CTpLayerWnd)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CExtInfo, CTpLayerWnd)
	ON_EVENT(CExtInfo, IDC_BT_BROWSE_DST, 1, CExtInfo::LBtClickedBtBrowseDst, VTS_I4)
END_EVENTSINK_MAP()

// CExtInfo ��Ϣ�������


BOOL CExtInfo::OnInitDialog()
{
	CTpLayerWnd::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_DST_PATH)))->SetValueText(m_strSavePath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CExtInfo::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_strSrcType = ((CBL_DropDown *)(GetDlgItem(IDC_DN_TYPE_SRC)))->GetValueText();
	m_strDstType = ((CBL_DropDown *)(GetDlgItem(IDC_DN_TYPE_DST)))->GetValueText();

	CTpLayerWnd::OnOK();
}


void CExtInfo::LBtClickedBtBrowseDst(long nFlags)
{
	// TODO: �ڴ˴������Ϣ����������
	CFolderPickerDialog Dlg;

	if (IDOK == Dlg.DoModal())
	{
		m_strSavePath = Dlg.GetFolderPath();

		((CBL_Edit *)(GetDlgItem(IDC_EDIT_DST_PATH)))->SetValueText(m_strSavePath);
	}
}
