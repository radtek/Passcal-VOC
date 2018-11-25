
// Passcal-vocDlg.h : ͷ�ļ�
//

#pragma once

#include "ExtInfo.h"
#include "..\\ImgProcDll\\SmartImage.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\ImgProcDll.lib")
#else
#pragma comment(lib, "..\\Release\\ImgProcDll.lib")
#endif

// CPasscalvocDlg �Ի���
class CPasscalvocDlg : public CAppBase
{
// ����
public:
	CPasscalvocDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PASSCALVOC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:	
	CString m_strImgPath;
	void LBtClickedBtBrowse(long nFlags);
	void LBtClickedBtRename(long nFlags);
	void LBtClickedBtChangeType(long nFlags);
};
