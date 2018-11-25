
// Passcal-vocDlg.h : 头文件
//

#pragma once

#include "ExtInfo.h"
#include "..\\ImgProcDll\\SmartImage.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\ImgProcDll.lib")
#else
#pragma comment(lib, "..\\Release\\ImgProcDll.lib")
#endif

// CPasscalvocDlg 对话框
class CPasscalvocDlg : public CAppBase
{
// 构造
public:
	CPasscalvocDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PASSCALVOC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
