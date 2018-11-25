#pragma once

#include "SmartImage.h"
#include <afxmt.h>

// CImgBox

#define RGB_IMG_BOX_BACK		RGB(64, 64, 64)		// ����ɫ
#define RGB_IMG_BOX_BORDER		RGB(0, 190, 190)	// �߿���ɫ
#define RGB_IMG_BOX_GRAY		RGB(128, 128, 128)	// �߿���ɫ

class AFX_EXT_CLASS CImgBox : public CStatic
{
DECLARE_DYNAMIC(CImgBox)

public:
	CImgBox();
	virtual ~CImgBox();

protected:
	DECLARE_MESSAGE_MAP()

public:
	int m_nIndex;
	CString m_strTitle;
	COLORREF m_rgbBack;		// ����ɫ
	COLORREF m_rgbBorder;	// �߿���ɫ
	int m_nBorderWidth;		// �߿��߿�
	BOOL m_bKeepScale;		// �Ƿ񱣳ֱ���
	BOOL m_bTopLeft;		// �Ƿ�����Ͻ���ʾ

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void DrawElement(CDC * pDC, CRect rcDraw);

	virtual void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	virtual void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);

	void SetBorderColor(COLORREF rgbBorder);
	COLORREF GetBorderColor(void);

	void SetImage(CSmartImage * pImgSrc, CString * pTitle = nullptr);

	BOOL IsImgEmpty(void);
	void SetImgEmpty(void);

	CSmartImage * GetImage(void);
protected:
	CRect m_rcScale;
	CSmartImage m_Image;
	CSemaphore m_seDraw;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();	
};


