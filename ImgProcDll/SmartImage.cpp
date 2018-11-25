#include "stdafx.h"
#include "SmartImage.h"


CSmartImage::CSmartImage(void)
	: Mat()
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::CSmartImage(Mat Img)
	: m_bRoiSetted(FALSE)
{
	*this = Img;
}


CSmartImage::CSmartImage(const CSmartImage * pSrc, Scalar InitVal/* = Scalar::all(0) */)
	: Mat(pSrc->rows, pSrc->cols, pSrc->type(), InitVal)
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::CSmartImage(Size2i ImgSize, int nType/* = CV_8UC3 */, Scalar InitVal/* = Scalar::all(0) */)
	: Mat(ImgSize, nType, InitVal)
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::CSmartImage(int nWidth, int nHeight, int nType/* = CV_8UC3 */, Scalar InitVal/* = Scalar::all(0) */)
	: Mat(nHeight, nWidth, nType, InitVal)
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::CSmartImage(Size2i ImgSize, int nType, void * pData)
	: Mat(ImgSize, nType, pData)
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::CSmartImage(int nWidth, int nHeight, int nType, void * pData)
	: Mat(nHeight, nWidth, nType, pData)
	, m_bRoiSetted(FALSE)
{
}


CSmartImage::~CSmartImage(void)
{
	release();
}


AFX_INLINE void CSmartImage::ReCreate(int nWidth, int nHeight, int nType/* = CV_8UC3 */)
{
	create(nHeight, nWidth, nType);
}


AFX_INLINE void CSmartImage::ReCreate(Size2i ImgSize, int nType/* = CV_8UC3 */)
{
	create(ImgSize, nType);
}


AFX_INLINE void CSmartImage::ReCreate(CSmartImage * pSrc)
{
	if (nullptr == pSrc || pSrc->empty())
	{
		return;
	}

	create(pSrc->rows, pSrc->cols, pSrc->type());
}


void CSmartImage::Serialize(CArchive & ar)
{ 
	// TRACE(_T("CSmartImage.Serialize started!\n"));

	if (ar.IsStoring())
	{	// storing code
		if (!empty())
		{
			ar << TRUE;					// 如果图像指针不为空, 则保存TRUE

			int nType		= type();
			int nWidth		= Width();
			int nHeight		= Height();

			ar << nType;
			ar << nWidth;
			ar << nHeight;

			nWidth = Width() * channels();

			if (isContinuous())
			{
				nWidth	= nWidth * nHeight;
				nHeight = 1;
			}

			for (int nH = 0; nH < nHeight; nH++)
			{
				BYTE *pData = ptr<BYTE>(nH);

				for(int nW = 0; nW < nWidth; nW++)
				{
					ar << pData[nW];
				}
			}
		}
		else
		{
			ar << FALSE;
		}
	}
	else
	{	// loading code
		BOOL bFind = FALSE;

		ar >> bFind;

		if (bFind)
		{
			int nType, nWidth, nHeight;

			ar >> nType;
			ar >> nWidth;
			ar >> nHeight;

			ReCreate(nWidth, nHeight, nType);

			nWidth = Width() * channels();

			if (isContinuous())
			{
				nWidth	= nWidth * nHeight;
				nHeight = 1;
			}

			for (int nH = 0; nH < nHeight; nH++)
			{
				BYTE *pData = ptr<BYTE>(nH);

				for(int nW = 0; nW < nWidth; nW++)
				{
					ar >> pData[nW];
				}
			}
		}
	}

	// TRACE(_T("CSmartImage.Serialize complete!\n"));
}


AFX_INLINE int CSmartImage::Width(void)
{
	return cols;
}


AFX_INLINE int CSmartImage::HalfWidth(void)
{
	return cols >> 1;
}


AFX_INLINE int CSmartImage::Height(void)
{
	return rows;
}


AFX_INLINE int CSmartImage::HalfHeight(void)
{
	return rows >> 1;
}


BOOL CSmartImage::Open(int nSColor/* = CV_LOAD_IMAGE_UNCHANGED */)
{
	// TRACE(_T("CSmartImage::Open start!\n"));

	CFileDialog dlgFile(TRUE, _T("*.bmp"), nullptr, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("Bitmap Files(*.bmp)|*.bmp|JPEG Files(*.jpg)|*.jpg|Raw Files(*.raw)|*.raw|All Files(*.*)||"), nullptr);

	BOOL bSucceed = FALSE;

	if (IDOK == dlgFile.DoModal()) 
	{	
		USES_CONVERSION;
		CVT_MAT(this) = imread(W2A(dlgFile.GetPathName()), nSColor);

		bSucceed = TRUE;
	}

	// TRACE(_T("CSmartImage::Open complete!\n"));

	return bSucceed;
}


BOOL CSmartImage::Open(const CString & strPath, int nSColor/* = CV_LOAD_IMAGE_UNCHANGED */)
{
	// TRACE(_T("CSmartImage::Open start!\n"));

	BOOL bSucceed = FALSE;

	// 如果是UNICODE模式, 需要进行字符转换
	USES_CONVERSION;

	CVT_MAT(this) = imread(W2A(strPath), nSColor);

	bSucceed = TRUE;

	// TRACE(_T("CSmartImage::Open complete!\n"));

	return bSucceed;
}


// 保存图像
BOOL CSmartImage::Save(void)
{
	// TRACE(_T("CSmartImage::Save start!\n"));

	CString strFileName;

	SYSTEMTIME st;
	GetLocalTime(&st);

	strFileName.Format(_T("%d%02d%02d%02d%02d%02d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CFileDialog dlgFile(FALSE, _T("*.bmp"), strFileName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("Bitmap Files(*.bmp)|*.bmp|JPG Files(*.jpg)|*.jpg|Png Files(*.png)|*.png|Raw Files(*.raw)|*.raw|All Files(*.*)|*.*||"),
		nullptr);

	if (IDOK == dlgFile.DoModal())
	{	
		USES_CONVERSION;
		imwrite(W2A(dlgFile.GetPathName()), *this);
	}

	// TRACE(_T("CSmartImage::Save complete!\n"));

	return TRUE;
}


// 保存图像
BOOL CSmartImage::Save(CString strPath)
{
	// TRACE(_T("CSmartImage::Save start!\n"));

	CString strFileName;

	SYSTEMTIME st;
	GetLocalTime(&st);

	strFileName.Format(_T("\\%d%02d%02d%02d%02d%02d-%03d.bmp"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (!PathIsDirectory(strPath))
	{
		BOOL bSucceed = CreateDirectory(strPath, nullptr);
	}

	strPath.Append(strFileName);

	USES_CONVERSION;
	imwrite(W2A(strPath), *this);

	// TRACE(_T("CSmartImage::Save complete!\n"));

	return TRUE;
}


BOOL CSmartImage::Save(CString strPath, const CString & strName)
{
	// TRACE(_T("CSmartImage::Save start!\n"));

	if (!PathIsDirectory(strPath))
	{
		BOOL bSucceed = CreateDirectory(strPath, nullptr);
	}

	strPath = strPath + _T("\\") + strName;

	USES_CONVERSION;
	imwrite(W2A(strPath), *this);

	// TRACE(_T("CSmartImage::Save complete!\n"));

	return TRUE;
}


void CSmartImage::Show(const CString & strWindow, int nFlag/* = WINDOW_NORMAL */)
{
	if (strWindow.IsEmpty())
	{
		return;
	}

	if (empty())
	{
		return;
	}

	USES_CONVERSION;
	namedWindow(W2A(strWindow), nFlag);
	imshow(W2A(strWindow), *this);
}


CRect CSmartImage::Draw2DC(CDC * pDC, CRect rcDst, BOOL bKeepScale/* = TRUE */, BOOL bTopLeft/* = FALSE */) 
{
	if (nullptr == pDC || empty() || rcDst.IsRectEmpty())
	{
//		// TRACE(_T("Draw2DC returned because pDC is nullptr or image is empty or rcDst is empty!\n"));
		return rcDst;
	}

	if (bKeepScale)
	{
		rcDst = _SetScale(rcDst, bTopLeft);
	}

	CSmartImage ImgTemp;

	if (m_bRoiSetted)
	{
		ImgTemp.Clone(&m_ImgROI);
	}
	else
	{
		ImgTemp = *this;
	}

	if (ImgTemp.channels() < 3)
	{
		vector<Mat> Planes(3);
		Planes[0] = ImgTemp;
		Planes[1] = ImgTemp;
		Planes[2] = ImgTemp;

		merge(Planes, ImgTemp);

		Planes.clear();
	}

	if (ImgTemp.Width() > rcDst.Width() || ImgTemp.Height() > rcDst.Height())
	{
		// 缩小时会失真， 所以要设置成HALFTONE
		SetStretchBltMode(
			pDC->GetSafeHdc(),				 // handle to device context
			HALFTONE);
	}
	else
	{
		SetStretchBltMode(
			pDC->GetSafeHdc(),				 // handle to device context
			COLORONCOLOR);
	}

	BYTE Buffer[sizeof(BITMAPINFOHEADER) + 1024];
	BITMAPINFO *pBmi = (BITMAPINFO *)Buffer;
	CSmartImage ImgConvert;

	if (CV_8U != ImgTemp.depth())
	{
		ImgTemp.convertTo(ImgConvert, NORM_MINMAX, 255, 0);
		ImgTemp = ImgConvert;
	}

	if(ImgTemp.Width() % 4)
	{
		IplImage *pImg = cvCreateImage(ImgTemp.GetImgSize(), IPL_DEPTH_8U, ImgTemp.channels());
		ImgTemp.copyTo(Mat(pImg));

		_FillBitmapInfo(pBmi, pImg->width, pImg->height, ImgTemp.channels());

		::StretchDIBits(
			pDC->GetSafeHdc(),
			rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(),
			0, 0, pImg->width, pImg->height,
			pImg->imageData, pBmi, DIB_RGB_COLORS, SRCCOPY);

		cvReleaseImage(&pImg);
	}  
	else
	{
		_FillBitmapInfo(pBmi, ImgTemp.Width(), ImgTemp.Height(), ImgTemp.channels());

		::StretchDIBits(
			pDC->GetSafeHdc(),
			rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(),
			0, 0, ImgTemp.Width(), ImgTemp.Height(),
			ImgTemp.data, pBmi, DIB_RGB_COLORS, SRCCOPY);
	}

	return rcDst;
}


void CSmartImage::DrawImg2Ctrl(CDialog * pDlg, DWORD dwID,
							   BOOL bKeepScale/* = TRUE*/, BOOL bTopLeft/* = FALSE*/,
							   COLORREF rgbBack/* = RGB(64, 64, 64)*/, COLORREF rgbBorder/* = RGB(0, 255, 255)*/)
{
	CRect rcShow;
	pDlg->GetDlgItem(dwID)->GetClientRect(rcShow);

	CDC *pDC = pDlg->GetDlgItem(dwID)->GetDC();

	CSmartDC dcMem(pDC, rcShow, FALSE);

	//////////////////////////////////////////////////////////////////////////
	dcMem.FillSolidRect(rcShow, rgbBack);

	if (!empty())
	{
		Draw2DC(&dcMem, rcShow, bKeepScale, bTopLeft);
	}

	CPen penBorder(PS_SOLID, 1, rgbBorder);

	dcMem.SelectObject(penBorder);
	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.Rectangle(rcShow);

	dcMem.ShowBitBit(pDC);

	pDlg->ReleaseDC(pDC);
}


AFX_INLINE Size2i CSmartImage::GetImgSize(void)
{
	return Size2i(Width(), Height());
}


AFX_INLINE int CSmartImage::DataSize(void)
{
	return total() * elemSize();
}


Mat CSmartImage::SetROI(Rect2i rcROI)
{
	if (rcROI.x < 0)
	{
		rcROI.x = 0;
	}
	if (rcROI.x + rcROI.width >= Width())
	{
		rcROI.width = Width() - rcROI.x;
	}

	if (rcROI.y < 0)
	{
		rcROI.y = 0;
	}
	if (rcROI.y + rcROI.height >= Height())
	{
		rcROI.height = Height() - rcROI.y;
	}

	m_bRoiSetted	= TRUE;
	m_rcROI			= rcROI;
	m_ImgROI		= CVT_MAT(this)(m_rcROI);

	return m_ImgROI;
}


Mat CSmartImage::SetROI(CRect rcROI)
{
	return SetROI(rcROI.left, rcROI.top, rcROI.Width(), rcROI.Height());
}


Mat CSmartImage::SetROI(CSmartImage & ImgROI)
{
	Rect2i rcROI(0, 0, ImgROI.Width(), ImgROI.Height());

	m_bRoiSetted	= TRUE;
	m_rcROI			= rcROI;
	m_ImgROI		= CVT_MAT(this)(m_rcROI);

	return m_ImgROI;
}


Mat CSmartImage::SetROI(int nX, int nY, int nWidth, int nHeight)
{
	return SetROI(Rect2i(nX, nY, nWidth, nHeight));
}


IMG_ROI CSmartImage::GetROI(void)
{
	const IMG_ROI ROI = {m_rcROI, m_bRoiSetted};

	return ROI;
}


void CSmartImage::ClearROI(void)
{
	m_bRoiSetted = FALSE;
}


AFX_INLINE Point2d CSmartImage::CenterPoint2d(void)
{
	return Point2d(Width() * 0.5, Height() * 0.5);
}


AFX_INLINE Point2i CSmartImage::CenterPoint2i(void)
{
	return Point2i(Width() >> 1, Height() >> 1);
}


void CSmartImage::EraseOutside(Rect2i rcBox)
{
	Rect2i rcRoi = m_rcROI;

	if (rcBox.x < 0)
	{
		rcBox.x = 0;
	}

	if (rcBox.y < 0)
	{
		rcBox.y = 0;
	}

	if (rcBox.width + rcBox.x > Width())
	{
		rcBox.width = Width() - rcBox.x;
	}

	if (rcBox.height + rcBox.y > Height())
	{
		rcBox.height = Height() - rcBox.y;
	}

	if (Width() == rcBox.width && Height() == rcBox.height)
	{
		return;
	}

	SetROI(0, 0, Width(), rcBox.y);
	Set2Zero();

	SetROI(0, rcBox.y, rcBox.x, rcBox.height);
	Set2Zero();

	SetROI(rcBox.x + rcBox.width, rcBox.y, Width() - rcBox.width - rcBox.x, rcBox.height);
	Set2Zero();

	SetROI(0, rcBox.y + rcBox.height, Width(), Height() - rcBox.y - rcBox.height);
	Set2Zero();

	if (m_bRoiSetted)
	{
		SetROI(rcRoi);
	}
	else
	{
		ClearROI();
	}
}


AFX_INLINE void CSmartImage::Clone(Mat * pMat)
{
	pMat->copyTo(*this);
}


AFX_INLINE void CSmartImage::Clone(CSmartImage * pSrc)
{
	if (nullptr == pSrc)
	{
		// TRACE(_T("Clone is returned because pImg is nullptr"));
		return;
	}

	if (m_bRoiSetted && pSrc->m_bRoiSetted)
	{
		pSrc->m_ImgROI.copyTo(m_ImgROI);
	}
	else if (m_bRoiSetted && !pSrc->m_bRoiSetted)
	{
		pSrc->copyTo(m_ImgROI);
	}
	else if (!m_bRoiSetted && pSrc->m_bRoiSetted)
	{
		pSrc->m_ImgROI.copyTo(*this);
	}
	else
	{
		pSrc->copyTo(*this);
	}
}


AFX_INLINE void CSmartImage::Reverse(CSmartImage * pDst/* = nullptr*/)
{
	if (nullptr != pDst)
	{
		CVT_MAT(pDst) = ~CVT_MAT(this);
	}
	else
	{
		CVT_MAT(this) = ~CVT_MAT(this);
	}
}


AFX_INLINE void CSmartImage::Set2Zero(void)
{
	if (m_bRoiSetted)
	{
		m_ImgROI.setTo(Scalar(0, 0, 0, 0));
	}
	else
	{
		setTo(Scalar(0, 0, 0, 0));
	}
}


AFX_INLINE void CSmartImage::SetValue(Scalar dValue)
{
	if (m_bRoiSetted)
	{
		m_ImgROI.setTo(dValue);
	}
	else
	{
		setTo(dValue);
	}
}


AFX_INLINE void CSmartImage::operator = (Mat & Img)
{
	CVT_MAT(this) = Img;
}


AFX_INLINE void CSmartImage::operator = (CSmartImage & Img)
{
	CVT_MAT(this) = CVT_MAT(&Img);
}


AFX_INLINE BOOL CSmartImage::operator ==(CSmartImage & Img)
{
	return data == Img.data;
}


AFX_INLINE BOOL CSmartImage::operator !=(CSmartImage & Img)
{
	return data != Img.data;
}


AFX_INLINE Mat CSmartImage::operator + (CSmartImage & Img)
{
	return (CVT_MAT(this) + CVT_MAT(&Img));
}


AFX_INLINE void CSmartImage::operator +=(CSmartImage & Img)
{
	CVT_MAT(this) += CVT_MAT(&Img);
}


AFX_INLINE Mat CSmartImage::operator - (CSmartImage & Img)
{
	return (CVT_MAT(this) - CVT_MAT(&Img));
}


AFX_INLINE void CSmartImage::operator -=(CSmartImage & Img)
{
	CVT_MAT(this) -= CVT_MAT(&Img);
}


AFX_INLINE Mat CSmartImage::operator & (CSmartImage & Img)
{
	return (CVT_MAT(this) & CVT_MAT(&Img));
}


AFX_INLINE void CSmartImage::operator &= (CSmartImage & Img)
{
	CVT_MAT(this) = CVT_MAT(this) & CVT_MAT(&Img);
}


AFX_INLINE CRect CSmartImage::_SetScale(CRect rcDst, BOOL bTopLeft/* = FALSE */)
{
	CRect rcScale;

	const double dWidth	= Width();
	const double dHeight = Height();

	if (dWidth / dHeight <= (double)rcDst.Width() / rcDst.Height())
	{
		const double dScale = dHeight / rcDst.Height();
		const int nWidth	= cvRound(dWidth / dScale);
		const int nSpace	= (rcDst.Width() - nWidth) >> 1;
		rcScale = CRect(rcDst.left + nSpace, rcDst.top, rcDst.right - nSpace, rcDst.bottom);

		if (bTopLeft)
		{
			rcScale.OffsetRect(CPoint(-nSpace, 0));
		}
	}
	else
	{
		const double dScale = dWidth / rcDst.Width();
		const int nHeight	= cvRound(dHeight / dScale);
		const int nSpace	= (rcDst.Height() - nHeight) >> 1;
		rcScale = CRect(rcDst.left, rcDst.top + nSpace, rcDst.right, rcDst.bottom - nSpace);

		if (bTopLeft)
		{
			rcScale.OffsetRect(CPoint(0, -nSpace));
		}
	}

	return rcScale;
}


// 初始化显示信息
AFX_INLINE void CSmartImage::_FillBitmapInfo(BITMAPINFO * pBmi, int nWidth, int nHeight, int nChannels/* = 3 */)
{
//	// TRACE(_T("CSmartImage::_FillBitmapInfo start!\n"));

	ASSERT(nullptr != pBmi);
	ASSERT(nWidth >= 0 && nHeight >= 0);

	const int nPixelBits = IPL_DEPTH_8U * nChannels;

	BITMAPINFOHEADER *pBmiheader = &(pBmi->bmiHeader);

	memset(pBmiheader, 0, sizeof(*pBmiheader));

	pBmiheader->biSize			= sizeof(BITMAPINFOHEADER);
	pBmiheader->biWidth			= nWidth;
	pBmiheader->biHeight			= -abs(nHeight);
	pBmiheader->biPlanes			= 1;
	pBmiheader->biBitCount		= nPixelBits;
	pBmiheader->biCompression	= BI_RGB;

	if(8 == nPixelBits)
	{
		RGBQUAD *palette = pBmi->bmiColors;

		for(int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}

//	// TRACE(_T("CSmartImage::_FillBitmapInfo complete!\n"));
}
