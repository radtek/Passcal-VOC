// ImgBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImgBox.h"


// CImgBox

IMPLEMENT_DYNAMIC(CImgBox, CStatic)
	CImgBox::CImgBox()
	: m_nIndex(0)
	, m_strTitle(_T("ImgShow"))
	, m_rgbBack(RGB_IMG_BOX_BACK)
	, m_rgbBorder(RGB_IMG_BOX_BORDER)
	, m_nBorderWidth(1)
	, m_bKeepScale(TRUE)
	, m_bTopLeft(FALSE)
{
#ifdef _DEBUG
	m_seDraw.m_strName = _T("CImgBox.m_seDraw");
#endif // _DEBUG
}

CImgBox::~CImgBox()
{
}


BEGIN_MESSAGE_MAP(CImgBox, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CImgBox ��Ϣ�������


void CImgBox::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY);

	CStatic::PreSubclassWindow();
}


void CImgBox::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{

	// TODO:  ������Ĵ����Ի���ָ����
}


void CImgBox::DrawElement(CDC * pDC, CRect rcDraw)
{
	m_seDraw.Lock();

	if (!m_Image.empty())
	{
		m_rcScale = m_Image.Draw2DC(pDC, rcDraw, m_bKeepScale, m_bTopLeft);
	}

	m_seDraw.Unlock();

	const CPen penBorder(PS_SOLID, 1, m_rgbBorder);

	pDC->SelectObject(penBorder);
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(rcDraw);
}


void CImgBox::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint/* = TRUE*/)
{
	CStatic::MoveWindow(x, y, nWidth, nHeight, bRepaint);

	Invalidate(FALSE);
}


void CImgBox::MoveWindow(LPCRECT lpRect, BOOL bRepaint /* = TRUE */)
{
	CStatic::MoveWindow(lpRect, bRepaint);

	Invalidate(FALSE);
}


void CImgBox::SetBorderColor(COLORREF rgbBorder)
{
	m_rgbBorder = rgbBorder;

	Invalidate(FALSE);
}


COLORREF CImgBox::GetBorderColor(void)
{
	return m_rgbBorder;
}


void CImgBox::SetImage(CSmartImage * pImgSrc, CString * pTitle/* = nullptr*/)
{
	if (nullptr != pTitle)
	{
		m_strTitle = *pTitle;
	}

	m_seDraw.Lock();

	m_Image.Clone(pImgSrc);

	m_seDraw.Unlock();

	Invalidate(FALSE);
}


BOOL CImgBox::IsImgEmpty(void)
{
	return m_Image.empty();
}


void CImgBox::SetImgEmpty(void)
{
	m_seDraw.Lock();

	m_Image.release();

	m_seDraw.Unlock();	

	Invalidate(FALSE);
}


CSmartImage * CImgBox::GetImage(void)
{
	return &m_Image;
}


void CImgBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_seDraw.Lock();

	m_Image.Show(m_strTitle);

	m_seDraw.Unlock();

	CStatic::OnLButtonDblClk(nFlags, point);
}


void CImgBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	CRect rcClient;
	GetClientRect(rcClient);

	CSmartDC dcMem(&dc, rcClient, TRUE);

	//////////////////////////////////////////////////////////////////////////
	DrawElement(&dcMem, rcClient);
	//////////////////////////////////////////////////////////////////////////

	dcMem.ShowBitBit(&dc);

	const CWnd * pWnd = FindWindow(nullptr, m_strTitle);

	if (nullptr != pWnd)
	{
		m_seDraw.Lock();

		m_Image.Show(m_strTitle);

		m_seDraw.Unlock();
	}
}
