#pragma once

#include "Resource.h"
#include "TpLayerWnd.h"

// CStdRuler �Ի���

class AFX_EXT_CLASS CStdRuler : public CTpLayerWnd
{
	DECLARE_DYNAMIC(CStdRuler)

public:
	CStdRuler(CWnd* pParent = NULL);   // ��׼���캯��
	CStdRuler(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CStdRuler();

// �Ի�������
	enum { IDD = IDD_STDRULER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_dStdDist;
	double m_dStdRadius;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void Serialize(CArchive& ar);
	virtual void LockCtrls(int nLock);
protected:
	double m_dStdDistBkup;
	double m_dStdRadiusBkup;
};
