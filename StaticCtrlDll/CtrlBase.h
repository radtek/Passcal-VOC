#pragma once

#include "SmartDC.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// CCtrlBase

class AFX_EXT_CLASS CCtrlBase : public CStatic
{
#define RGB_STCTRL_BASE_BLEND	RGB(55, 56, 57)		// ͸��ɫ
#define RGB_STCTRL_BASE_BK		RGB(45, 50, 55)		// ����ɫ
#define RGB_STCTRL_BASE_BORDER	RGB(0, 255, 230)	// �߿���ɫ

	DECLARE_DYNAMIC(CCtrlBase)

public:
	CCtrlBase();
	virtual ~CCtrlBase();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowCross;				// �Ƿ�Ҫ�����ֱ��

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void CreateSubWnd(void);
	virtual void DrawElement(CDC * pDC, CRect rcDraw);	

	virtual void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	virtual void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);

	void SetBlendColor(COLORREF rgbBlend);
	COLORREF GetBlendColor(void);

	void SetBkColor(COLORREF rgbBack);
	COLORREF GetBkColor(void);

	void SetBorderColor(COLORREF rgbBorder);
	COLORREF GetBorderColor(void);

	void SetBorderWidth(int nWidth);
	int GetBorderWidth(void);

	void DrawCenterCross(CDC * pDC, CRect rcDraw, COLORREF rgb, int nSize);
	CRect GetScaledRect(int nWidth, int nHeight, CRect rcDraw, BOOL bTopLeft);	
protected:
	COLORREF m_rgbBlend;			// ͸����ɫ
	COLORREF m_rgbBack;				// ����ɫ
	COLORREF m_rgbBorder;			// �߿���ɫ
	int m_nBorderWidth;				// �߿���
public:
	afx_msg void OnPaint();
};


