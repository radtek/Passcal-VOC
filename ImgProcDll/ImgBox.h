#pragma once

#include "SmartImage.h"
#include <afxmt.h>

// CImgBox

#define RGB_IMG_BOX_BACK		RGB(64, 64, 64)		// 背景色
#define RGB_IMG_BOX_BORDER		RGB(0, 190, 190)	// 边框颜色
#define RGB_IMG_BOX_GRAY		RGB(128, 128, 128)	// 边框颜色

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
	COLORREF m_rgbBack;		// 背景色
	COLORREF m_rgbBorder;	// 边框颜色
	int m_nBorderWidth;		// 边框线宽
	BOOL m_bKeepScale;		// 是否保持比例
	BOOL m_bTopLeft;		// 是否从左上角显示

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


