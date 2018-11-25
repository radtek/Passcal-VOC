// ImgProc.cpp : 实现文件
//

#include "stdafx.h"
#include "ImgProc.h"

#define APP_FONT	_T("新宋体")

AFX_INLINE BOOL IsEqual(YX_HSV hsv1, YX_HSV hsv2)
{
	if (hsv1.H != hsv2.H)
	{
		return FALSE;
	}

	if (hsv1.S != hsv2.S)
	{
		return FALSE;
	}

	if (hsv1.V != hsv2.V)
	{
		return FALSE;
	}

	return TRUE;
}


AFX_INLINE BOOL IsEqual(YX_RGB rgb1, YX_RGB rgb2)
{
	if (rgb1.R != rgb2.R)
	{
		return FALSE;
	}

	if (rgb1.G != rgb2.G)
	{
		return FALSE;
	}

	if (rgb1.B != rgb2.B)
	{
		return FALSE;
	}

	return TRUE;
}


AFX_INLINE BOOL IsEqual(Rect2i rc1, CRect rc2)
{
	return (rc1.x == rc2.left && rc1.y == rc2.top &&
		rc1.width == rc2.Width() && rc1.height == rc2.Height());
}


AFX_INLINE BOOL IsEqual(CRect rc1, Rect2i rc2)
{
	return IsEqual(rc2, rc1);
}


AFX_INLINE BOOL IsEqual(Rect2i rc1, Rect2i rc2)
{
	return (rc1.x == rc2.x && rc1.y == rc2.y &&
		rc1.width == rc2.width && rc1.height == rc2.height);
}


// 设置RGB颜色
AFX_INLINE YX_RGB SetRGB(BYTE R, BYTE G, BYTE B)
{
	YX_RGB rgb = {B, G, R};
	return rgb;
}


// 设置RGB颜色
AFX_INLINE YX_RGB SetRGB(COLORREF rgbColor)
{
	YX_RGB rgb;

	rgb.R = GetRValue(rgbColor);
	rgb.G = GetGValue(rgbColor);
	rgb.B = GetBValue(rgbColor);

	return rgb;
}


// 设置HSV颜色
AFX_INLINE YX_HSV SetHSV(double H, double S, double V)
{
	YX_HSV hsv = {H, S, V};
	return hsv;
}


// 将图将Crect矩形转换成Rect2i矩形
AFX_INLINE Rect2i CRect2Rect2i(CRect rect)
{
	rect.NormalizeRect();

	return Rect2i(rect.left, rect.top, rect.Width(), rect.Height());
}


// Rect2i将图将矩形转换成Crect矩形
AFX_INLINE CRect Rect2i2CRect(Rect2i rect)
{
	return CRect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
}


// 将控件窗口矩形转换到客户区矩形
AFX_INLINE void Window2Client(CDialog * pDlg, DWORD dwID, CRect & rcCtrl)
{
	pDlg->GetDlgItem(dwID)->GetWindowRect(rcCtrl);
	pDlg->ScreenToClient(rcCtrl);
}


// 将客户区坐标转换为图像坐标
AFX_INLINE CPoint Client2Image(CSmartImage * pSrc, CRect rcClient, CPoint ptClient)
{
	CPoint ptImage;

	const double dScaleX = (double)(pSrc->Width() / (rcClient.Width() + (rcClient.Width() < ZERO? ZERO: 0)));
	const double dScaleY = (double)(pSrc->Height() / (rcClient.Height() + (rcClient.Height() < ZERO? ZERO: 0)));

	ptImage.x = cvRound(ptClient.x * dScaleX);
	ptImage.y = cvRound(ptClient.y * dScaleY);

	return ptImage;
}


// 将客户区坐标转换为图像坐标
AFX_INLINE Rect2i Client2Image(CSmartImage * pSrc, CRect rcClient, CRect rcDst)
{
	rcDst.NormalizeRect();

	const CPoint ptTL = Client2Image(pSrc, rcClient, rcDst.TopLeft());
	const CPoint ptBR = Client2Image(pSrc, rcClient, rcDst.BottomRight());

	return Rect2i(ptTL.x, ptTL.y, ptBR.x - ptTL.x, ptBR.y - ptTL.y);
}


// 将客户区坐标转换为图像坐标
AFX_INLINE CPoint Image2Client(CSmartImage * pSrc, CRect rcClient, CPoint ptImg)
{
	CPoint ptClient;

	const double dScaleX = (double)(rcClient.Width() / (pSrc->Width() + (pSrc->Width() < ZERO? ZERO: 0)));
	const double dScaleY = (double)(rcClient.Height() / (pSrc->Height() + (pSrc->Height() < ZERO? ZERO: 0)));

	ptClient.x = cvRound(ptImg.x * dScaleX);
	ptClient.y = cvRound(ptImg.y * dScaleY);

	return ptClient;
}


// 将客户区坐标转换为图像坐标
AFX_INLINE CRect Image2Client(CSmartImage * pSrc, CRect rcClient, CRect rcImg)
{
	const CPoint ptTL = Image2Client(pSrc, rcClient, rcImg.TopLeft());
	const CPoint ptBR = Image2Client(pSrc, rcClient, rcImg.BottomRight());

	return CRect(ptTL.x, ptTL.y, ptBR.x - ptTL.x, ptBR.y - ptTL.y);
}


// RGB转换成HSV
YX_HSV RGB2HSV(BYTE rgbR, BYTE rgbG, BYTE rgbB)
{
	YX_HSV hsvColor;
	BYTE MaxVector = max(rgbR, max(rgbG, rgbB));
	BYTE MinVector = min(rgbR, min(rgbG, rgbB));
	BYTE MaxValue = 0;

	if (rgbR == MaxVector)
	{
		MaxValue = 0;
	}
	else if (rgbG == MaxVector)
	{
		MaxValue = 2;
	}
	else
	{
		MaxValue = 4;
	}

	hsvColor.V = MaxVector / 255.0;

	if (0 == MaxVector)
	{
		hsvColor.S = 0;
	}
	else
	{
		hsvColor.S = ((double)(MaxVector - MinVector)) / MaxVector;
	}

	if (hsvColor.V * hsvColor.S)
	{
		switch (MaxValue)
		{
		case 0:
			hsvColor.H = 60 * (MaxValue + (double)(rgbG - rgbB) / (MaxVector - MinVector));
			break;

		case 2:
			hsvColor.H = 60 * (MaxValue + (double)(rgbB - rgbR) / (MaxVector - MinVector));
			break;

		case 4:
			hsvColor.H = 60 * (MaxValue + (double)(rgbR - rgbG) / (MaxVector - MinVector));
			break;
		default: 
			break;
		}
		if (hsvColor.H < 0)
		{
			hsvColor.H += 360.0;
		}
	}
	else
	{
		hsvColor.H = 0;
	}

	hsvColor.S *= 100.0;
	hsvColor.V *= 100.0;

	return hsvColor;
}


// RGB转换成HSV
AFX_INLINE YX_HSV RGB2HSV(YX_RGB rgbColor)
{
	return RGB2HSV(rgbColor.R, rgbColor.G, rgbColor.B);
}


// RGB转换成HSV
AFX_INLINE YX_HSV RGB2HSV(COLORREF rgbColor)
{
	return RGB2HSV(GetRValue(rgbColor), GetGValue(rgbColor), GetBValue(rgbColor));
}


// RGB转换成HSV
AFX_INLINE void RGB2HSV(YX_RGB * pRGB, YX_HSV * pHSV)
{
	BYTE MaxVector = max(pRGB->R, max(pRGB->G, pRGB->B));
	BYTE MinVector = min(pRGB->R, min(pRGB->G, pRGB->B));
	BYTE MaxValue = 0;

	if (pRGB->R == MaxVector)
	{
		MaxValue = 0;
	}
	else if (pRGB->G == MaxVector)
	{
		MaxValue = 2;
	}
	else
	{
		MaxValue = 4;
	}

	pHSV->V = MaxVector / 255.0;

	if (0 == MaxVector)
	{
		pHSV->S = 0;
	}
	else
	{
		pHSV->S = ((double)(MaxVector - MinVector)) / MaxVector;
	}

	if (pHSV->V * pHSV->S)
	{
		switch (MaxValue)
		{
		case 0:
			pHSV->H = 60 * (MaxValue + (double)(pRGB->G - pRGB->B) / (MaxVector - MinVector));
			break;

		case 2:
			pHSV->H = 60 * (MaxValue + (double)(pRGB->B - pRGB->R) / (MaxVector - MinVector));
			break;

		case 4:
			pHSV->H = 60 * (MaxValue + (double)(pRGB->R - pRGB->G) / (MaxVector - MinVector));
			break;
		default: 
			break;
		}

		if (pHSV->H < 0)
		{
			pHSV->H += 360.0;
		}
	}
	else
	{
		pHSV->H = 0;
	}

	pHSV->S *= 100.0;
	pHSV->V *= 100.0;
}


// RGB转换成HSV
AFX_INLINE void RGB2HSV(YX_RGB * pRGB, YX_HSV * pHSV, int nWidth, int nHeight)
{
	YX_RGB * pRGBTemp = pRGB;
	YX_HSV * pHSVTemp = pHSV;

	for (int nH = 0; nH < nHeight; nH++)
	{
		for (int nW = 0; nW < nWidth; nW++)
		{
			RGB2HSV(pRGBTemp, pHSVTemp);

			pRGBTemp++;
			pHSVTemp++;
		}
	}
}


// 清除边缘点
void ClearOutline(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr*/, int nLines/* = 1*/, int nDirSave/* = 0*/,
				  BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("ClearOutline")*/)
{
	// TRACE(_T("ClearOutline start!\n"));
	CHECK_SRC(_T("ClearOutline"), pSrc);

	CSmartImage *pImgDst = pDst;

	if (pImgDst != pSrc && nullptr != pImgDst)
	{
		pImgDst->Clone(pSrc);
	}
	else
	{
		pImgDst = pSrc;
	}

	// 去掉边界点
	const int nWidth	= pImgDst->Width();
	const int nHieght	= pImgDst->Height();

	if (nWidth <= 1 || nHieght <= 1)
	{
		// TRACE(_T("ClearOutline retruned because pSrc' width or height is too small!\n"));
		return;
	}

	BYTE *pData	= nullptr;

	for (int i = 0; i < nLines; i++)
	{
		pData = pImgDst->ptr<BYTE>(i);

		if (!(nDirSave & DIR_SAVE_TOP))
		{
			for(int nW = 0; nW < nWidth; nW++)
			{
				if (pData[nW])
				{
					FloodFill(pImgDst, Point2i(nW, i),
						MAT_RGB(0, 0, 0), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		pData = pImgDst->ptr<BYTE>(nHieght - 1 - i);

		if (!(nDirSave & DIR_SAVE_BOTTOM))
		{
			for(int nW = 0; nW < nWidth; nW++)
			{
				if (pData[nW])
				{
					FloodFill(pImgDst, Point2i(nW, nHieght - 1 - i),
						MAT_RGB(0, 0, 0), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		if (!(nDirSave & DIR_SAVE_LEFT))
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pImgDst->ptr<BYTE>(nH);

				if (pData[i])
				{
					FloodFill(pImgDst, Point2i(i, nH),
						MAT_RGB(0, 0, 0), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		if (!(nDirSave & DIR_SAVE_RIGHT))
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pImgDst->ptr<BYTE>(nH);

				if (pData[nWidth - 1 - i])
				{
					FloodFill(pImgDst, Point2i(nWidth - 1 - i, nH),
						MAT_RGB(0, 0, 0), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("ClearOutline complete!\n"));
}


// 保留边缘点
void SaveOutline(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr*/, int nLines/* = 1*/, int nDirClear/* = 0*/,
				 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("SaveOutline")*/)
{
	// TRACE(_T("SaveOutline start!\n"));
	CHECK_SRC(_T("SaveOutline"), pSrc);

	CSmartImage *pImgDst = pDst;

	if (pImgDst != pSrc && nullptr != pImgDst)
	{
		pImgDst->Clone(pSrc);
	}
	else
	{
		pImgDst = pSrc;
	}

	// 去掉边界点
	const int nWidth	= pImgDst->Width();
	const int nHieght	= pImgDst->Height();

	if (nWidth <= 1 || nHieght <= 1)
	{
		// TRACE(_T("SaveOutline retruned because pSrc' width or height is too small!\n"));
		return;
	}

	BYTE *pData	= nullptr;

	for (int i = 0; i < nLines; i++)
	{
		pData = pImgDst->ptr<BYTE>(i);

		if (!(nDirClear & DIR_CLEAR_TOP))
		{
			for(int nW = 0; nW < nWidth; nW++)
			{
				if (255 == pData[nW])
				{
					FloodFill(pImgDst, Point2i(nW, i),
						MAT_RGB(254, 254, 254), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		pData = pImgDst->ptr<BYTE>(nHieght - 1 - i);

		if (!(nDirClear & DIR_CLEAR_BOTTOM))
		{
			for(int nW = 0; nW < nWidth; nW++)
			{
				if (255 == pData[nW])
				{
					FloodFill(pImgDst, Point2i(nW, nHieght - 1 - i),
						MAT_RGB(254, 254, 254), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		if (!(nDirClear & DIR_CLEAR_LEFT))
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pImgDst->ptr<BYTE>(nH);

				if (255 == pData[i])
				{
					FloodFill(pImgDst, Point2i(i, nH),
						MAT_RGB(254, 254, 254), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}

		if (!(nDirClear & DIR_CLEAR_RIGHT))
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pImgDst->ptr<BYTE>(nH);

				if (255 == pData[nWidth - 1 - i])
				{
					FloodFill(pImgDst, Point2i(nWidth - 1 - i, nH),
						MAT_RGB(254, 254, 254), Scalar::all(0), Scalar::all(0),
						4 | CV_FLOODFILL_FIXED_RANGE);
				}
			}
		}
	}

//	pImgDst->Show();

	CSmartImage ImgMask;
	Convert2Gray(pImgDst, &ImgMask, FALSE);

	CSmartImage Img255;
	Threshold(&ImgMask, &Img255, 254, 255, CV_THRESH_BINARY, FALSE, _T("ImgMask-CV_THRESH_BINARY"));
	ImgMask -= Img255;

	Threshold(&ImgMask, pImgDst, 253, 255, CV_THRESH_BINARY, bShow, strWnd);

	// TRACE(_T("SaveOutline complete!\n"));
}


// 取得各方向空间宽度
int GetClearWidth(CSmartImage * pSrc, int nDir)
{
	// TRACE(_T("GetClearWidth start!\n"));
	
	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("GetClearWidth returned because Src is empty!\n"));
		return 0;
	}

	// 去掉边界点
	int nWidth	= pSrc->Width();
	int nHieght	= pSrc->Height();

	BYTE *pData = nullptr;

	switch (nDir)
	{
	case DIR_LEFT:
		for (int nW = 0; nW < nWidth; nW++)
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pSrc->ptr<BYTE>(nH) + nW;

				if (*pData)
				{
					return nW;
				}
			}
		}
		break;

	case DIR_RIGHT:
		for (int nW = nWidth - 1; nW >= 0; nW--)
		{
			for(int nH = 0; nH < nHieght; nH++)
			{
				pData = pSrc->ptr<BYTE>(nH) + nW;

				if (*pData)
				{
					return nWidth - nW - 1;
				}
			}
		}
		break;

	case DIR_TOP:
		for(int nH = 0; nH < nHieght; nH++)
		{
			for (int nW = 0; nW < nWidth; nW++)
			{
				pData = pSrc->ptr<BYTE>(nH) + nW;

				if (*pData)
				{
					return nH;
				}
			}
		}
		break;

	case DIR_BOTTOM:
		for(int nH = nHieght - 1; nH >= 0; nH--)
		{
			for (int nW = 0; nW < nWidth; nW++)
			{
				pData = pSrc->ptr<BYTE>(nH) + nW;

				if (*pData)
				{
					return nHieght - nH - 1;
				}
			}
		}
		break;

	default:
		break;
	}

	return 0;

	// TRACE(_T("GetClearWidth complete!\n"));
}


//取得外框矩形
Rect2i BoundingRectEx(CSmartImage * pSrc)
{
	// TRACE(_T("BoundingRectEx start!\n"));

	Rect2i rcBound;

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("BoundingRectEx returned because Src is empty!\n"));
		return rcBound;
	}

	CSmartImage ImgTemp;
	ImgTemp.Clone(pSrc);

	CSmartImage ImgOutline;
	ImgOutline.Clone(&ImgTemp);

	cvContours vImgContour;
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	
	
	const int nSize = vImgContour.size();

	for (int i = 1; i < nSize; i++)
	{
		line(ImgTemp, vImgContour[0][0], vImgContour[i][0], Scalar::all(255), 2, 8);
	}

	ImgOutline.Clone(&ImgTemp);
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	if (vImgContour.size())
	{
		rcBound = boundingRect(vImgContour[0]);
	}

	// TRACE(_T("BoundingRectEx complete!\n"));

	return rcBound;
}


//取得外框矩形
RotatedRect AreaMinRectEx(CSmartImage * pSrc)
{
	// TRACE(_T("AreaMinRectEx start!\n"));

	RotatedRect rcRect;

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("AreaMinRectEx returned because Src is empty!\n"));
		return rcRect;
	}

	CSmartImage ImgTemp;
	ImgTemp.Clone(pSrc);

	CSmartImage ImgOutline;
	ImgOutline.Clone(&ImgTemp);

	cvContours vImgContour;
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	const int nSize = vImgContour.size();

	for (int i = 1; i < nSize; i++)
	{
		line(ImgTemp, vImgContour[0][0], vImgContour[i][0], Scalar::all(255), 2, 8);
	}

	ImgOutline.Clone(&ImgTemp);
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	if (vImgContour.size())
	{
		rcRect = minAreaRect(vImgContour[0]);
	}

	// TRACE(_T("AreaMinRectEx complete!\n"));

	return rcRect;
}


//取得外接圆
YX_ENCLOSING_CIRCLE MinEnclosingCircleEx(CSmartImage * pSrc)
{
	// TRACE(_T("BoundingRectEx start!\n"));

	YX_ENCLOSING_CIRCLE EnCircle = {0, 0, 0};

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("BoundingRectEx returned because Src is empty!\n"));
		return EnCircle;
	}

	CSmartImage ImgTemp;
	ImgTemp.Clone(pSrc);

	CSmartImage ImgOutline;
	ImgOutline.Clone(&ImgTemp);

	cvContours vImgContour;
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	const int nSize = vImgContour.size();

	for (int i = 1; i < nSize; i++)
	{
		line(ImgTemp, vImgContour[0][0], vImgContour[i][0], Scalar::all(255), 2, 8);
	}

	ImgOutline.Clone(&ImgTemp);
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	if (vImgContour.size())
	{
		Point2f ptCenter;
		float fRadius;
		minEnclosingCircle(vImgContour[0], ptCenter, fRadius);

		EnCircle.ptCenter.x = ptCenter.x;
		EnCircle.ptCenter.y = ptCenter.y;
		EnCircle.dRadius	= fRadius;
	}	

	// TRACE(_T("BoundingRectEx complete!\n"));

	EnCircle.dArea = EnCircle.dRadius * EnCircle.dRadius * CV_PI;

	return EnCircle;
}


// 分离通道
void SplitImage(CSmartImage * pSrc,
				CSmartImage * pRed/* = nullptr */,
				CSmartImage * pGreen/* = nullptr */,
				CSmartImage * pBlue/* = nullptr */,
				CSmartImage * pAlpha/* = nullptr */,
				BOOL bShow/* = FALSE */)
{
	// TRACE(_T("SplitImage start!\n"));
	CHECK_SRC(_T("SplitImage"), pSrc);

	if (1 == pSrc->channels())
	{
		// TRACE(_T("SplitImage returned because pSrc->channels == 1!\n"));
		return;
	}

	vector<Mat> ImgPlanes;

	split(*pSrc, ImgPlanes);

	if (nullptr != pBlue)
	{
		*pBlue = ImgPlanes[0];

		if (bShow)
		{
			pBlue->Show(_T("pBlue"));
		}
	}
	
	if (nullptr != pGreen)
	{
		*pGreen = ImgPlanes[1];

		if (bShow)
		{
			pGreen->Show(_T("pGreen"));
		}
	}

	if (nullptr != pRed)
	{
		*pRed = ImgPlanes[2];

		if (bShow)
		{
			pRed->Show(_T("pRed"));
		}
	}

	if (nullptr != pAlpha)
	{
		*pAlpha = ImgPlanes[3];

		if (bShow)
		{
			pAlpha->Show(_T("pAlpha"));
		}
	}

	// TRACE(_T("SplitImage complete!\n"));
}


void SplitImage(CSmartImage * pSrc, vector<Mat> * pPlanes)
{
	// TRACE(_T("SplitImage start!\n"));
	CHECK_SRC(_T("SplitImage"), pSrc);

	ASSERT(nullptr != pPlanes);

	if (1 == pSrc->channels())
	{
		// TRACE(_T("SplitImage returned because pSrc->channels == 1!\n"));
		return;
	}

	split(*pSrc, *pPlanes);

	// TRACE(_T("SplitImage complete!\n"));
}


void GetHSV(CSmartImage * pSrc,
			CSmartImage * pH/* = nullptr */,
			CSmartImage * pS/* = nullptr */,
			CSmartImage * pV/* = nullptr */,
			BOOL bShow/* = FALSE */)
{
	// TRACE(_T("GetHSV start!\n"));
	CHECK_SRC(_T("GetHSV"), pSrc);

	if (1 == pSrc->channels())
	{
		// TRACE(_T("GetHSV returned because pSrc->channels == 1!\n"));
		return;
	}

	CSmartImage ImgSrc;
	Convert2HSV(pSrc, &ImgSrc);

	vector<Mat> ImgHsv;
	split(ImgSrc, ImgHsv);

	if (nullptr != pH)
	{
		*pH = ImgHsv[0];

		if (bShow)
		{
			pH->Show(_T("pH"));
		}
	}

	if (nullptr != pS)
	{
		*pS = ImgHsv[1];

		if (bShow)
		{
			pS->Show(_T("pS"));
		}
	}

	if (nullptr != pV)
	{
		*pV = ImgHsv[2];

		if (bShow)
		{
			pV->Show(_T("pV"));
		}
	}

	// TRACE(_T("GetHSV complete!\n"));
}


void GetHSV(CSmartImage * pSrc, vector<Mat> * pHsv)
{
	// TRACE(_T("GetHSV start!\n"));
	CHECK_SRC(_T("GetHSV"), pSrc);

	ASSERT(nullptr != pHsv);

	if (1 == pSrc->channels())
	{
		// TRACE(_T("GetHSV returned because pSrc->channels == 1!\n"));
		return;
	}

	Convert2HSV(pSrc);
	split(*pSrc, *pHsv);

	// TRACE(_T("GetHSV complete!\n"));
}


// 通道合并
void Merge(CSmartImage * pRed,
		   CSmartImage * pGreen,
		   CSmartImage * pBlue,
		   CSmartImage * pDst,
		   BOOL bShow/* = FALSE */,
		   const CString & strWnd/* = _T("Merge")*/)
{
	// TRACE(_T("Merge start!\n"));

	ASSERT(nullptr != pDst);

	vector<Mat> Src(3);

	if (nullptr != pBlue)
	{
		Src[0] = *pBlue;
	}

	if (nullptr != pGreen)
	{
		Src[1] = *pGreen;
	}

	if (nullptr != pRed)
	{
		Src[2] = *pRed;
	}

	merge(Src, *pDst);

	if (bShow)
	{
		pDst->Show(strWnd);
	}

	// TRACE(_T("Merge complete!\n"));
}


void Merge(vector<Mat> * pPlanes, CSmartImage * pDst, BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("Merge")*/)
{
	// TRACE(_T("Merge start!\n"));

	ASSERT(nullptr != pPlanes && pPlanes->size());
	ASSERT(nullptr != pDst);

	merge(*pPlanes, *pDst);

	if (bShow)
	{
		pDst->Show(strWnd);
	}

	// TRACE(_T("Merge complete!\n"));
}


// 缩减图像颜色
void ColorReduce(CSmartImage * pSrc, CSmartImage * pDst, int nDiv/* = 32 */, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("ColorReduce start!\n"));
	CHECK_SRC(_T("ColorReduce"), pSrc);

	if (1 == pSrc->channels())
	{
		// TRACE(_T("ColorReduce returned because pSrc->channels() == 1!\n"));
		return;
	}

	CSmartImage ImgSrc;
	ImgSrc.Clone(pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	int nWidth	= ImgSrc.Width();
	int nHeight	= ImgSrc.Height();

	if (ImgSrc.isContinuous())
	{
		nWidth	= nWidth * nHeight;
		nHeight	= 1;
	}

	int nValue		= static_cast<int>(log(static_cast<double>(nDiv)) / log(2.0));
	BYTE byMask		= 0xFF << nValue;
	int nChannels	= ImgSrc.channels();

	for (int nH = 0; nH < nHeight; nH++)
	{
		BYTE *pData = ImgSrc.ptr<BYTE>(nH);
		for (int nW = 0; nW < nWidth; nW++)
		{
			for (int i = 0; i < nChannels; i++)
			{
				*pData++ = (*pData & byMask) + (nDiv >> 1);
			}
		}
	}

	pImgDst->Clone(&ImgSrc);

	if (bShow)
	{
		pImgDst->Show(_T("ColorReduce"));
	}

	// TRACE(_T("ColorReduce complete!\n"));
}

// 翻转图像
void Flip(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */, int nFlipMode/* = FLIP_X*/,
		  BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Flip")*/)
{
	// TRACE(_T("Flip start!\n"));
	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	flip(*pSrc, *pImgDst, nFlipMode);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}
	// TRACE(_T("Flip complete!\n"));
}


// 获取子图像
void GetSubImage(CSmartImage * pSrc, CSmartImage * pDst, Rect2i rcSub,
				 BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("GetSubImage")*/)
{
	// TRACE(_T("GetSubImage start!\n"));

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst, FALSE);

	if (rcSub.width <= 0 || rcSub.height <= 0)
	{
		return;
	}

	if (rcSub.x < 0)
	{
		const int nTemp = rcSub.x;

		rcSub.x = 0;
		rcSub.width += nTemp;

		if (rcSub.width <= 0)
		{
			return;
		}
	}

	if (rcSub.y < 0)
	{
		const int nTemp = rcSub.y;

		rcSub.y = 0;
		rcSub.height += nTemp;

		if (rcSub.height <= 0)
		{
			return;
		}
	}
	
	if (rcSub.x + rcSub.width > pSrc->Width())
	{
		rcSub.width = pSrc->Width() - rcSub.x - 1;
	}
	
	if (rcSub.y + rcSub.height > pSrc->Height())
	{
		rcSub.height = pSrc->Height() - rcSub.y - 1;
	}

	const IMG_ROI ROI = pSrc->GetROI();

	CSmartImage ImgSub;
	ImgSub = pSrc->SetROI(rcSub);
	pImgDst->Clone(&ImgSub);

	if (ROI.bRoiSetted)
	{
		pSrc->SetROI(ROI.rcROI);
	}
	else
	{
		pSrc->ClearROI();
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("GetSubImage complete!\n"));
}


// 获取子图像
void GetSubImage(CSmartImage * pSrc, CSmartImage * pDst, CRect rcSub,
				 BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("GetSubImage")*/)
{
	rcSub.NormalizeRect();
	GetSubImage(pSrc, pDst, CRect2Rect2i(rcSub), bShow, strWnd);
}


// 图像平滑
void Smooth(CSmartImage * pSrc, CSmartImage * pDst, YX_SMOOTH_PARAM sp,
			BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Smooth"*/)
{
	// TRACE(_T("Smooth start!\n"));

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	switch (sp.nSmoothMode)
	{
	case CV_BLUR_NO_SCALE:
		break;

	case CV_BLUR:
		blur(*pSrc, *pImgDst, Size2i(sp.nMaskWidth, sp.nMaskHeight));
		break;

	case CV_GAUSSIAN:
		GaussianBlur(*pSrc, *pImgDst, Size2i(sp.nMaskWidth, sp.nMaskHeight), sp.dParam3, sp.dParam4);
		break;

	case CV_MEDIAN:
		medianBlur(*pSrc, *pImgDst, sp.nMaskWidth);
		break;

	case CV_BILATERAL:
		if (pSrc == pImgDst)
		{
			CSmartImage ImgBilateral;
			bilateralFilter(*pSrc, ImgBilateral, sp.nMaskWidth, sp.dParam3, sp.dParam4);
			pImgDst->Clone(&ImgBilateral);
		}
		else
		{
			bilateralFilter(*pSrc, *pImgDst, sp.nMaskWidth, sp.dParam3, sp.dParam4);
		}
		break;

	default:
		break;
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Smooth complete!\n"));
}


// 图像锐化
void Sharpen(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */, int nNeighbourPoint/* = 4 */, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("Sharpen start!\n"));

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (4 == nNeighbourPoint)
	{
		Mat Kernel4 = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
		filter2D(*pSrc, *pImgDst, pSrc->depth(), Kernel4);
	}
	else
	{
		Mat Kernel8 = (Mat_<float>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
		filter2D(*pSrc, *pImgDst, pSrc->depth(), Kernel8);
	}

	if (bShow)
	{
		pImgDst->Show(_T("Sharpen"));
	}

	// TRACE(_T("Sharpen complete!\n"));
}


// 击中击不中变换
void HitMiss(CSmartImage * pSrc, CSmartImage *pDst,
						Mat * pStruct1, Mat * pStruct2,
						BOOL bShow/* = FALSE*/)
{
	CHECK_SRC(_T("HitMiss"), pSrc);

	CSmartImage ImgSrc;
	ImgSrc.Clone(pSrc);
	
	CSmartImage ImgInv;
	ImgSrc.Reverse(&ImgInv);

	Erode(&ImgSrc, nullptr, 1, pStruct1, Point2i(-1, -1), FALSE);
	Erode(&ImgInv, nullptr, 1, pStruct2, Point2i(-1, -1), FALSE);

	*pDst = ImgSrc & ImgInv;

	if (bShow)
	{
		pDst->Show(_T("HitMiss"));
	}
}


// 击中击不中变换
void HitMiss(CSmartImage * pSrc, CSmartImage *pDst,
						Mat * pTarget, BOOL bShow/* = FALSE*/)
{
	CHECK_SRC(_T("HitMiss"), pSrc);

	CSmartImage ImgSrc;
	ImgSrc.Clone(pSrc);

	CSmartImage ImgInv;
	ImgSrc.Reverse(&ImgInv);

	Erode(&ImgSrc, nullptr, 1, pTarget, Point2i(-1, -1), FALSE);

	threshold(*pTarget, *pTarget, 1, 255, CV_THRESH_BINARY);
	*pTarget = ~(*pTarget);

	Erode(&ImgInv, nullptr, 1, pTarget, Point2i(-1, -1), FALSE);

	*pDst = ImgSrc & ImgInv;

	if (bShow)
	{
		pDst->Show(_T("HitMiss"));
	}
}


// 转换成灰度图像
void Convert2Gray(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr*/,
				  BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Convert2Gray")*/)
{
	// TRACE(_T("Convert2Gray start!\n"));

	CHECK_SRC(_T("Convert2Gray"), pSrc);

	if (pSrc->channels() < 3)
	{
		if (nullptr != pDst)
		{
			pDst->Clone(pSrc);
		}

		// TRACE(_T("Convert2Gray returned because pSrc->channels < 3!\n"));

		if (bShow)
		{
			pSrc->Show(strWnd);
		}

		return;
	}

	if ((nullptr == pDst) || ((nullptr != pDst) && (*pSrc == *pDst)))
	{
		CSmartImage ImgGray;
		cvtColor(*pSrc, ImgGray, CV_BGR2GRAY);

		if (nullptr == pDst)
		{
			pDst = pSrc;
		}

		pDst->Clone(&ImgGray);
	}
	else
	{
		cvtColor(*pSrc, *pDst, CV_BGR2GRAY);
	}

	if (bShow)
	{
		pDst->Show(strWnd);
	}

	// TRACE(_T("Convert2Gray complete!\n"));
}


// 将RGB图像转换成HSV图像
void Convert2HSV(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr*/, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("Convert2HSV start!\n"));

	CHECK_SRC(_T("Convert2HSV"), pSrc);
	ASSERT(3 == pSrc->channels());

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	cvtColor(*pSrc, *pImgDst, CV_BGR2HSV);

	if (bShow)
	{
		pImgDst->Show(_T("Convert2HSV"));
	}

	// TRACE(_T("Convert2HSV complete!\n"));
}


//灰度拉伸
void Stretch(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
			 double dMin/* = 55.0 */, double dMax/* = 200.0 */,
			 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Stretch")*/)
{
	// TRACE(_T("Stretch start!\n"));
	CHECK_SRC(_T("Stretch"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	double dDelta	= dMax - dMin;
	dDelta			+= (dDelta < ZERO? ZERO: 0);

	const double dK	= 255.0 / dDelta;
	const double dB	= -255.0 * dMin / dDelta;
	const double dCross	= 255.0 * dMin / (255.0 - dDelta);

	Mat LookUp(1, 256, CV_8U, Scalar(0));
	BYTE *pData = LookUp.data;

	for (int i = 0; i < 256; i++)
	{
		if (i < dMin)
		{
			*pData = 0;
		}
		else if (i > dMax)
		{
			*pData = 255;
		}
		else
		{
			*pData = (BYTE)(dK * i + dB);
		}

		pData++;
	}

	if (pSrc == pImgDst)
	{
		CSmartImage ImgTemp;
		LUT(*pSrc, LookUp, ImgTemp);
		pImgDst->Clone(&ImgTemp);
	}
	else
	{
		LUT(*pSrc, LookUp, *pImgDst);
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Stretch complete!\n"));
}


// 阈值化
void Threshold(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
			   double dThreshold/* = 128.0 */, double dMaxValue/* = 255.0 */,
			   int nType/* = CV_THRESH_BINARY */,
			   BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Threshold")*/)
{
	// TRACE(_T("Threshold start!\n"));
	CHECK_SRC(_T("Threshold"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	threshold(*pSrc, *pImgDst, dThreshold, dMaxValue, nType);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Threshold complete!\n"));
}


// 形态学操作
void Morphology(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
				int nOperation/* = CV_MOP_OPEN */,
				int nIterations/* = 1 */,
				Mat * pKernel/* = nullptr */,
				Point2i ptAnchaor/* = Point(-1, -1) */,				
				 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Morphology")*/)
{
	// TRACE(_T("Morphology start!\n"));
	CHECK_SRC(_T("Morphology"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (nIterations)
	{
		if (YX_MOP_EDGE != nOperation)
		{
			morphologyEx(*pSrc, *pImgDst, nOperation,
				nullptr == pKernel? Mat(): *pKernel,
				ptAnchaor, nIterations);
		}
		else
		{
			CSmartImage ImgTemp;
			ImgTemp.Clone(pSrc);
			Erode(&ImgTemp, nullptr, nIterations, nullptr == pKernel? &Mat(): pKernel, ptAnchaor);
			*pImgDst = (*pSrc - ImgTemp);
		}
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Morphology complete!\n"));
}


// 腐蚀
void Erode(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
		   int nIterations/* = 1 */,
		   Mat * pKernel/* = nullptr */,
		   Point2i ptAnchaor/* = Point(-1, -1) */,		   
		   BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("Erode")*/)
{
	// TRACE(_T("Erode start!\n"));
	CHECK_SRC(_T("Erode"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	erode(*pSrc, *pImgDst, nullptr == pKernel? Mat(): *pKernel, ptAnchaor, nIterations);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Erode complete!\n"));
}


// 膨胀
void Dilate(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
			int nIterations/* = 1 */,
			Mat * pKernel/* = nullptr */,
			Point2i ptAnchaor/* = Point(-1, -1) */,			
			BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("Dilate")*/)
{
	// TRACE(_T("Dilate start!\n"));
	CHECK_SRC(_T("Dilate"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	dilate(*pSrc, *pImgDst, nullptr == pKernel? Mat(): *pKernel, ptAnchaor, nIterations);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Dilate complete!\n"));
}


// 条件膨胀
void LimitDilate(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
				 double dThreLow/* = 55.0 */, double dThresHigh/* = 200.0 */, BOOL bShow/* = FALSE*/)
{
	// TRACE(_T("LimitDilate start!\n"));
	CHECK_SRC(_T("LimitDilate"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	CSmartImage ImgTemp;
	CSmartImage ImgBack;
	CSmartImage ImgThresLow;
	CSmartImage ImgThresHigh;

	ImgTemp.Clone(pSrc);
	ImgBack.ReCreate(pSrc);

	Threshold(pSrc, &ImgThresLow, dThreLow);
	Threshold(pSrc, &ImgThresHigh, dThresHigh);

	while (TRUE)
	{
		Dilate(&ImgThresHigh, nullptr);
		ImgBack = ImgThresLow & ImgThresHigh;

		int nChannels	= ImgBack.channels();
		int nWidth		= ImgBack.Width();
		int nHeight		= ImgBack.Height();

		for (int nH = 0; nH < nHeight; nH++)
		{
			BYTE *pBack = ImgBack.ptr<BYTE>(nH);
			BYTE *pTemp = ImgTemp.ptr<BYTE>(nH);

			for (int nW = 0; nW < nWidth; nW++)
			{
				for (int i = 0; i < nChannels; i++)
				{
					if (*pBack != *pTemp)
					{
						goto LABELEND;
					}
				}
			}
		}

		pImgDst->Clone(&ImgBack);
		
		if (bShow)
		{
			pImgDst->Show(_T("LimitDilate"));
			// TRACE(_T("LimitDilate complete!\n"));
		}
		return;

LABELEND:
		ImgTemp.Clone(&ImgBack);
	}
}


// 标记指定颜色
void MarkSpecialColor(CSmartImage * pSrc, CSmartImage * pDst, YX_HSV hsvColor, YX_HSV hsvTolerance,
					  COLORREF rgbMark, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("MarkSpecialColor start!\n"));
	CHECK_SRC(_T("MarkSpecialColor"), pSrc);

	if (pSrc->channels() < 3)
	{
		// TRACE(_T("MarkSpecialColor returned because pSrc->channels < 3!\n"));
		return;
	}

	CSmartImage ImgDst(pSrc);

	CSmartImage Img_H;
	CSmartImage Img_S;
	CSmartImage Img_V;

	GetHSV(pSrc, &Img_H, &Img_S, &Img_V);

	int nWidth  = ImgDst.Width();
	int nHeight = ImgDst.Height();

	BYTE R = GetRValue(rgbMark);
	BYTE G = GetGValue(rgbMark);
	BYTE B = GetBValue(rgbMark);

	BYTE *pH = nullptr;
	BYTE *pS = nullptr;
	BYTE *pV = nullptr;

	BYTE *pMark = nullptr;
	BYTE *pData = nullptr;

	for(int nH = 0; nH < nHeight; nH++)
	{
		pH = Img_H.ptr<BYTE>(nH);
		pS = Img_S.ptr<BYTE>(nH);
		pV = Img_V.ptr<BYTE>(nH);

		pMark = ImgDst.ptr<BYTE>(nH);
		pData = pSrc->ptr<BYTE>(nH);

		for(int nW = 0; nW < nWidth; nW++)
		{			
			double dHue			= *pH * 2.0;
			double dSturation	= *pS * 0.3921568627;
			double dValue		= *pV * 0.3921568627;

			if (fabs(dHue		- hsvColor.H) > hsvTolerance.H ||
				fabs(dSturation - hsvColor.S) > hsvTolerance.S ||
				fabs(dValue		- hsvColor.V) > hsvTolerance.V)
			{
				pMark += 3;
				pData += 3;
			}
			else
			{
				if (nullptr == pDst || pDst == pSrc)
				{
					*pData++ = B;
					*pData++ = G;
					*pData++ = R;
				}
				else
				{
					*pMark++ = B;		// 设置标志
					*pMark++ = G;		// 设置标志
					*pMark++ = R;		// 设置标志
				}
			}

			pH++;
			pS++;
			pV++;
		}
	}

	if (nullptr != pDst && pDst != pSrc)
	{
		pDst->Clone(&ImgDst);
	}

	if (bShow)
	{
		if (nullptr == pDst || pDst == pSrc)
		{
			ImgDst = *pSrc;
		}

		ImgDst.Show(_T("MarkSpecialColor"));
	}

	// TRACE(_T("MarkSpecialColor complete!\n"));
}


// 标记指定颜色
void MarkSpecialColor(CSmartImage * pSrc, CSmartImage * pDst, YX_HSV hsvColor, YX_HSV hsvTolerLow, YX_HSV hsvTolerHigh,
					  COLORREF rgbMark, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("MarkSpecialColor start!\n"));
	CHECK_SRC(_T("MarkSpecialColor"), pSrc);

	if (pSrc->channels() < 3)
	{
		// TRACE(_T("MarkSpecialColor returned because pSrc->channels < 3!\n"));
		return;
	}

	CSmartImage ImgDst(pSrc);

	CSmartImage Img_H;
	CSmartImage Img_S;
	CSmartImage Img_V;

	GetHSV(pSrc, &Img_H, &Img_S, &Img_V);

	int nWidth  = ImgDst.Width();
	int nHeight = ImgDst.Height();

	BYTE R = GetRValue(rgbMark);
	BYTE G = GetGValue(rgbMark);
	BYTE B = GetBValue(rgbMark);

	BYTE *pH = nullptr;
	BYTE *pS = nullptr;
	BYTE *pV = nullptr;

	BYTE *pMark = nullptr;
	BYTE *pData = nullptr;

	for(int nH = 0; nH < nHeight; nH++)
	{
		pH = Img_H.ptr<BYTE>(nH);
		pS = Img_S.ptr<BYTE>(nH);
		pV = Img_V.ptr<BYTE>(nH);

		pMark = ImgDst.ptr<BYTE>(nH);
		pData = pSrc->ptr<BYTE>(nH);

		for(int nW = 0; nW < nWidth; nW++)
		{			
			double dHue			= *pH * 2.0;
			double dSturation	= *pS * 0.3921568627;
			double dValue		= *pV * 0.3921568627;

			if (dHue - hsvColor.H > hsvTolerHigh.H ||
				dHue - hsvColor.H < hsvTolerLow.H ||

				dSturation - hsvColor.S > hsvTolerHigh.S ||
				dSturation - hsvColor.S < hsvTolerLow.S ||

				dValue - hsvColor.V > hsvTolerHigh.V ||
				dValue - hsvColor.V < hsvTolerLow.V)	
			{
				pMark += 3;
				pData += 3;
			}
			else
			{
				if (nullptr == pDst || pDst == pSrc)
				{
					*pData++ = B;
					*pData++ = G;
					*pData++ = R;
				}
				else
				{
					*pMark++ = B;		// 设置标志
					*pMark++ = G;		// 设置标志
					*pMark++ = R;		// 设置标志
				}
			}

			pH++;
			pS++;
			pV++;
		}
	}

	if (nullptr != pDst && pDst != pSrc)
	{
		pDst->Clone(&ImgDst);
	}

	if (bShow)
	{
		if (nullptr == pDst || pDst == pSrc)
		{
			ImgDst = *pSrc;
		}

		ImgDst.Show(_T("MarkSpecialColor"));
	}

	// TRACE(_T("MarkSpecialColor complete!\n"));
}


// 图像缩放
void Resize(CSmartImage * pSrc, CSmartImage * pDst, Size2i ImgSize,
			int nInterpolation/* = CV_INTER_LINEAR */, BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("Resize")*/)
{
	// TRACE(_T("Resize start!\n"));
	CHECK_SRC(_T("Resize"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (pSrc == pImgDst)
	{
		CSmartImage ImgTemp;
		resize(*pSrc, ImgTemp, ImgSize, 0, 0, nInterpolation);
		pImgDst->Clone(&ImgTemp);
	}
	else
	{
		resize(*pSrc, *pImgDst, ImgSize, 0, 0, nInterpolation);
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("Resize complete!\n"));
}


// 图像缩放
void Resize(CSmartImage * pSrc, CSmartImage * pDst, int nWidth, int nHeight,
			int nInterpolation/* = CV_INTER_LINEAR */, BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("Resize")*/)
{
	Resize(pSrc, pDst, Size2i(nWidth, nHeight), nInterpolation, bShow, strWnd);
}


// Canny
void Canny(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */,
		   double dThresMin/* = 55.0 */, double dThresMax/* = 200.0 */,
		   int ApertureSize/* = 3 */, BOOL bL2gradient/* = FALSE */, BOOL bShow/* = FALSE */)
{
	// TRACE(_T("Canny start!\n"));
	CHECK_SRC(_T("Canny"), pSrc);

	ASSERT(1 == pSrc->channels());

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (pSrc == pImgDst)
	{
		CSmartImage ImgTemp;
		Canny(*pSrc, ImgTemp, dThresMin, dThresMax, ApertureSize, bL2gradient? true: false);
		pImgDst->Clone(&ImgTemp);
	}
	else
	{
		Canny(*pSrc, *pImgDst, dThresMin, dThresMax, ApertureSize, bL2gradient? true: false);
	}

	if (bShow)
	{
		pImgDst->Show(_T("Canny"));
	}

	// TRACE(_T("Canny complete!\n"));
}


// 消除杂点
void EraseNoise(CSmartImage * pSrc, CSmartImage * pDst, double dSize, BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("EraseNoise")*/)
{
	// TRACE(_T("EraseNoise start!\n"));
	CHECK_SRC(_T("EraseNoise"), pSrc);

#ifdef _DEBUG
	const double dStartTime = GetTickCount();
#endif // _DEBUG

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (dSize > 0)
	{
		if (pImgDst != pSrc)
		{
			pImgDst->Clone(pSrc);
		}

		CSmartImage ImgContours;
		ImgContours.Clone(pSrc);

		cvContours vOutline;
		findContours(ImgContours, vOutline, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

		const int nSize = vOutline.size();

		for (int i = 0; i < nSize; i++)
		{
			const double dArea = contourArea(vOutline[i]) + (vOutline[i].size() >> 1);

			if (dArea < dSize)
			{
				drawContours(*pImgDst, vOutline, i, Scalar::all(0), CV_FILLED);
			}
		}
	}

#ifdef _DEBUG
	const double dmS = GetTickCount() - dStartTime;
	// TRACE(_T("EraseNoise Process Time: %f mS\n"), dmS);
#endif

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("EraseNoise complete!\n"));
}


// 形状匹配
double MatchShape(CSmartImage * pSrc, CSmartImage * pDst)
{
	// TRACE(_T("MatchShape start!\n"));

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("MatchShape returned because pSrc is nullptr or empty!\n"));
		return 0;
	}

	if (nullptr == pDst || pDst->empty())
	{
		// TRACE(_T("MatchShape returned because pDst is nullptr or empty!\n"));
		return 0;
	}

	ASSERT(1 == pSrc->channels());
	ASSERT(1 == pDst->channels());
	ASSERT(pSrc->Width() == pDst->Width() && pSrc->Height() == pDst->Height());

	CSmartImage SrcTemp(*pSrc);
	CSmartImage DstTemp(*pDst);

	CSmartImage CannySrc(pSrc->GetImgSize(), pSrc->type());
	CSmartImage CannyDst(pDst->GetImgSize(), pSrc->type());

	vector<vector<Point2i>> ContoursSrc, ContoursDst;

	findContours(SrcTemp, ContoursSrc, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);		// 提取轮廓
	findContours(DstTemp, ContoursDst, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);		// 提取轮廓

	double dScore = 1.0 - matchShapes(*ContoursSrc.begin(), *ContoursDst.begin(), CV_CONTOURS_MATCH_I3, 0);

	// TRACE(_T("MatchShape complete!\n"));

	return dScore;
}


// 模板匹配
double MatchTemplate(CSmartImage * pSrc, CSmartImage * pTemplate,
					 int nMethod/* = CV_TM_CCORR_NORMED*/,
					 const Rect2i * prcLimit/* = nullptr*/,
					 vector<Point2i> * ptvTL/* = nullptr*/, vector<Point2i> * ptvBR/* = nullptr*/,
					 int nWanted/* = 0*/, double dThreshold/* = 0.7*/,
					 int nResize/* = 4*/,
					 BOOL bMarkPos/* = TRUE*/, Scalar scMarkColor/* = Scalar::all(255)*/,
					 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("MatchTemplate")*/)
{
	// TRACE(_T("MatchTemplate start!\n"));

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("MatchTemplate returned -1 because pSrc is nullptr or empty!\n"));
		return -1;
	}

	if (nullptr == pTemplate || pTemplate->empty())
	{
		// TRACE(_T("MatchTemplate returned because -2 pTemplate is nullptr or empty!\n"));
		return -2;
	}

#ifdef _DEBUG
	const double dStartTime = GetTickCount();	// 取得开始时间
#endif

	CSmartImage ImgSrc;							// 限制区域图像
	CSmartImage ImgDst;
	Point2i ptOffset(0, 0);						// 限制区域左上角离图像左上角偏移量

	ImgDst.Clone(pTemplate);

	// 如果有限制搜索区域
	if (nullptr != prcLimit && prcLimit->width > 0 && prcLimit->height > 0)
	{
		Rect2i rcSearch = *prcLimit;

		if (rcSearch.x < 0)
		{
			rcSearch.x = 0;
			rcSearch.width += prcLimit->x;

			if (rcSearch.width <= 0)
			{
				goto LABEL_NEXT;
			}
		}

		if (rcSearch.y < 0)
		{
			rcSearch.y = 0;
			rcSearch.height += prcLimit->y;

			if (rcSearch.height <= 0)
			{
				goto LABEL_NEXT;
			}
		}

		if (rcSearch.x + rcSearch.width >= pSrc->Width())
		{
			rcSearch.width = pSrc->Width() - rcSearch.x - 1;
		}

		if (rcSearch.y + rcSearch.height >= pSrc->Height())
		{
			rcSearch.height = pSrc->Height() - rcSearch.y - 1;
		}
		
		if (rcSearch.width <= ImgDst.Width())
		{
			rcSearch.width = ImgDst.Width() + 1;
		}

		if (rcSearch.height <= ImgDst.Height())
		{
			rcSearch.height = ImgDst.Height() + 1;
		}

		GetSubImage(pSrc, &ImgSrc, rcSearch, FALSE, _T("MatchTemplate-GetSubImage"));	// 取得子区域图像

		if (ImgSrc.empty() ||
			pTemplate->Width() > rcSearch.width - 4 ||
			pTemplate->Height() > rcSearch.height - 4)
		{
			ImgSrc.Clone(pSrc);
			goto LABEL_NEXT;
		}

		ptOffset.x = rcSearch.x;	// 保存偏移量
		ptOffset.y = rcSearch.y;
	}
	else
	{
		ImgSrc.Clone(pSrc);
	}

LABEL_NEXT:

	CSmartImage ImgBig;				// 大图像
	CSmartImage ImgSmall;			// 小图像

	// 如果通道数不一致
	if (ImgSrc.channels() > ImgDst.channels())
	{
		Convert2Gray(&ImgSrc, &ImgBig);
		ImgSmall = ImgDst;
	}
	else if (ImgSrc.channels() < ImgDst.channels())
	{
		Convert2Gray(&ImgDst, &ImgSmall);
		ImgBig = ImgSrc;
	}
	else
	{
		ImgBig = ImgSrc;
		ImgSmall = ImgDst;
	}

	const double dResize = nResize;

	if (nResize)
	{
		Resize(&ImgBig, nullptr,
			cvRound(ImgBig.Width() / dResize), cvRound(ImgBig.Height() / dResize),
			CV_INTER_CUBIC,
			FALSE, _T("ImgBig"));

		Resize(&ImgSmall, nullptr,
			cvRound(ImgSmall.Width() / dResize), cvRound(ImgSmall.Height() / dResize),
			CV_INTER_CUBIC,
			FALSE, _T("ImgSmall"));
	}

	CSmartImage ImgResult;

	matchTemplate(ImgBig, ImgSmall, ImgResult, nMethod);

	double dMatchValue = 0;
	vector<CRect> vrcFind;

	if (0 == nWanted)
	{
		nWanted = USHRT_MAX;
	}

	while (nWanted)
	{
		BOOL bMatched		= FALSE;
		double dValueMin	= 0;
		double dValueMax	= 0;			

		Point2i ptMin;
		Point2i ptMax;
		Point2i ptMatchLoc;

		Point2i ptTopLeft;
		Point2i ptBottomRight;

		minMaxLoc(ImgResult, &dValueMin, &dValueMax, &ptMin, &ptMax);

		if (CV_TM_SQDIFF == nMethod || CV_TM_SQDIFF_NORMED == nMethod)
		{
			ptMatchLoc	= ptMin;
			dMatchValue	= dValueMin;

			if (!bMatched)
			{
				bMatched = TRUE;
			}

			if (dMatchValue > 1.0 - dThreshold)
			{
				break;
			}
		}
		else
		{
			ptMatchLoc	= ptMax;
			dMatchValue	= dValueMax;

			if (!bMatched)
			{
				bMatched = TRUE;
			}

			if (dMatchValue < dThreshold)
			{
				break;
			}
		}

		ptTopLeft = ptMatchLoc;
		ptBottomRight = Point2i(ptTopLeft.x + ImgSmall.Width(), ptTopLeft.y + ImgSmall.Height());

		const CRect rcMatch = CRect(ptTopLeft.x, ptTopLeft.y, ptBottomRight.x, ptBottomRight.y);

		BOOL bStored = FALSE;
		const int nSize = vrcFind.size();

		for (int j = 0; j < nSize; j++)
		{
			const CRect &rcStore = vrcFind[j];

			if (rcStore.PtInRect(rcMatch.TopLeft()) ||
				rcStore.PtInRect(rcMatch.BottomRight()) ||
				rcStore.PtInRect(CPoint(rcMatch.left, rcMatch.bottom)) ||
				rcStore.PtInRect(CPoint(rcMatch.right, rcMatch.top)))
			{
				bStored = TRUE;

				rectangle(ImgResult,
					Point2i(ptTopLeft.x - 1, ptTopLeft.y - 1),
					Point2i(ptBottomRight.x - (rcMatch.Width() >> 1), ptBottomRight.y - (rcMatch.Height() >> 1)),
					Scalar::all((CV_TM_SQDIFF == nMethod) || (CV_TM_SQDIFF_NORMED == nMethod)),
					-1, 4, 0);

				break;
			}
		}

		if (!bStored)
		{
			nWanted--;

			vrcFind.push_back(rcMatch);

			rectangle(ImgResult,
				Point2i(ptTopLeft.x - 1, ptTopLeft.y - 1),
				Point2i(ptBottomRight.x - (rcMatch.Width() >> 1), ptBottomRight.y - (rcMatch.Height() >> 1)),
				Scalar::all((CV_TM_SQDIFF == nMethod) || (CV_TM_SQDIFF_NORMED == nMethod)),
				-1, 4, 0);

			if (nResize)
			{
				ptTopLeft.x = cvRound(ptTopLeft.x * dResize);
				ptTopLeft.y = cvRound(ptTopLeft.y * dResize);
			}

			ptTopLeft.x += ptOffset.x;
			ptTopLeft.y += ptOffset.y;

			ptBottomRight = Point2i(ptTopLeft.x + pTemplate->Width(), ptTopLeft.y + pTemplate->Height());

			if (bMarkPos)
			{
				rectangle(*pSrc, ptTopLeft, ptBottomRight, scMarkColor, 2, 4, 0);
			}

			if (nullptr != ptvTL)
			{
				ptvTL->push_back(ptTopLeft);			
			}

			if (nullptr != ptvBR)
			{
				ptvBR->push_back(ptBottomRight);	
			}
		}
	}

#ifdef _DEBUG
	const double dmS = GetTickCount() - dStartTime;
	// TRACE(_T("MatchTemplate Process Time: %f mS\n"), dmS);
#endif

	if (bShow)
	{
		pSrc->Show(strWnd);
	}

	// TRACE(_T("MatchShape complete and Return %f!\n"), dMatchValue);

	return dMatchValue;
}


// 二值图像匹配
double BinaryMatch(CSmartImage * pSrc, CSmartImage * pTemplate,
				   CRect * prcLimit/* = nullptr */,
				   int nNumber/* = 3 */,
				   double dThesTemp/* = 50.0 */, double dThreLow/* = 30.0 */, double dThresHigh/* = 100.0 */,
				   vector<double> * pThres/* = nullptr */,
				   double dStretchLow/* = 20.0 */, double dStretchHigh/* = 100.0 */,
				   int nMethod/* = CV_TM_CCORR_NORMED */,
				   vector<Point2i> * ptVectorTopLeft/* = nullptr */, vector<Point2i> * ptVectorBottomRight/* = nullptr */,
				   double dStopThres/* = 0.95 */,
				   BOOL bResize/* = TRUE */,
				   BOOL bMarkPos/* = TRUE */,
				   BOOL bShowResultWindow/* = FALSE */)
{
	using namespace std;
	using namespace cv;

	// TRACE(_T("BinaryMatch start!\n"));

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("BinaryMatch returned because pSrc is nullptr or empty!\n"));
		return 0;
	}

	if (nullptr == pTemplate || pTemplate->empty())
	{
		// TRACE(_T("BinaryMatch returned because pTemplate is nullptr or empty!\n"));
		return 0;
	}

	CSmartImage *pDst = pTemplate;

	CSmartImage SrcTemp;							// 原图像备份
	SrcTemp = *pSrc;

	int nResizeScale = 2;							// 缩放倍数

#ifdef _DEBUG
	double dStartTime = GetTickCount();				// 取得开始时间
#endif

	CSmartImage ImgLimit;							// 限制区域图像
	Point2i		ptOffset = cvPoint(0, 0);			// 限制区域左上角离图像左上角偏移量

	// 如果有限制搜索区域
	if (nullptr != prcLimit && !prcLimit->IsRectEmpty())
	{	
		prcLimit->NormalizeRect();
		CvRect rcSearch	= CRect2Rect2i(*prcLimit);

		if (rcSearch.x < 0)
		{
			rcSearch.x = 0;
		}

		if (rcSearch.x + rcSearch.width > SrcTemp.Width())
		{
			rcSearch.width = SrcTemp.Width() - rcSearch.x;
		}

		if (rcSearch.y < 0)
		{
			rcSearch.y = 0;
		}

		if (rcSearch.y + rcSearch.height > SrcTemp.Height())
		{
			rcSearch.height = SrcTemp.Height() - rcSearch.y;
		}

		GetSubImage(&SrcTemp, &ImgLimit, rcSearch);	// 取得子区域图像
		SrcTemp = ImgLimit;

		ptOffset.x = rcSearch.x;	// 保存偏移量
		ptOffset.y = rcSearch.y;
	}

	CSmartImage ImgBig;				// 大图像
	CSmartImage ImgBigTemp;			// 大图像备份
	CSmartImage ImgSmall;			// 小图像
	CSmartImage ImgSmallTemp;		// 小图像备份

	// 如果通道数不一致
	if (SrcTemp.channels() > 1)
	{
		Convert2Gray(&SrcTemp, &ImgBig);
		Stretch(&ImgBig, nullptr, dStretchLow, dStretchHigh);		// 灰度拉伸
	}
	else
	{
		ImgBig	= SrcTemp;
	}

	if (pDst->channels() > 1)
	{
		Convert2Gray(pDst, &ImgSmall);
		Stretch(&ImgSmall, nullptr, dStretchLow, dStretchHigh);

		double dThes = (dThesTemp < dThreLow || dThesTemp > dThresHigh)? (dThreLow + dThresHigh) * 0.5 : dThesTemp;

		Threshold(&ImgSmall, nullptr, dThes);
		Morphology(&ImgSmall);
	}
	else
	{
		ImgSmall = *pDst;
	}

	// 如果使用缩放, 目标图像的宽度和调试均大于24, 且源图像的宽度和调试均大于64
	if (bResize && ImgSmall.Width() > 24 && ImgSmall.Height() > 24 && ImgBig.Width() > 64 && ImgBig.Height() > 64)
	{
		nResizeScale = 2;

		if (bResize && ImgSmall.Width() > 64 && ImgSmall.Height() > 64 && ImgBig.Width() > 512 && ImgBig.Height() > 512)
		{
			nResizeScale = 3;
		}
	}
	else
	{
		bResize = FALSE;
		nResizeScale = 0;
	}

	if (bResize)
	{
		Resize(&ImgBig, &ImgBigTemp, ImgBig.Width() >> nResizeScale, ImgBig.Height() >> nResizeScale);
		ImgBig = ImgBigTemp;

		Resize(&ImgSmall, &ImgSmallTemp, ImgSmall.Width() >> nResizeScale, ImgSmall.Height() >> nResizeScale);
		ImgSmall = ImgSmallTemp;			
	}

	CSmartImage ImgResult;

	double dScoreMax = 0.0;					// 最大匹配分数
	Point2i ptScorMax = cvPoint(0, 0);

	CSmartImage ImgTemp;

	vector<Point2i> ptMarkTL, ptMarkBR;
	double dValueMin, dValueMax, dMatchValue;
	Point2i ptMin, ptMax, ptMatchLoc;
	double dThres = dThreLow;

	for (int i = 0; i < nNumber; i++)
	{
		dScoreMax = 0.0;

		for (; dThres < dThresHigh + i; dThres++)
		{
			ImgTemp.Clone(&ImgBig);

			Threshold(&ImgTemp, nullptr, dThres);
			Morphology(&ImgTemp);

			matchTemplate(ImgTemp, ImgSmall, ImgResult, nMethod);
			minMaxLoc(ImgResult, &dValueMin, &dValueMax, &ptMin, &ptMax);

			if(CV_TM_SQDIFF == nMethod || CV_TM_SQDIFF_NORMED == nMethod)
			{
				dMatchValue = 1.0 - dValueMin;
				ptMatchLoc = ptMin;
			}
			else
			{
				dMatchValue = dValueMax;
				ptMatchLoc = ptMax;
			}

			if (dScoreMax < dValueMax)
			{
				dScoreMax = dMatchValue;
				ptScorMax = ptMatchLoc;
			}

			if (dScoreMax >= dStopThres)
			{
				break;
			}
		}

		if (bShowResultWindow)
		{
			CString strWnd;
			strWnd.Format(_T("Wnd%d"), i);

			CString strScore;
			strScore.Format(_T("Thres = %d, Value = %5.3f"), dThres, dScoreMax);

			USES_CONVERSION;

			putText(ImgTemp, W2A(strScore), Point2i(10, 10), FONT_HERSHEY_COMPLEX, 1, MAT_RGB(0, 0, 0));

			ImgTemp.Show(strWnd);
		}

		Point2i ptBR;
		ptBR.x = ptScorMax.x + ImgSmall.Width();
		ptBR.y = ptScorMax.y + ImgSmall.Height();

		rectangle(ImgBig, ptScorMax, ptBR, MAT_RGB(255, 255, 255), -1, 4, 0);
		rectangle(ImgBig, ptScorMax, ptBR, MAT_RGB(0, 0, 0), 1, 4, 0);

		if (nullptr != pThres)
		{
			pThres->push_back(dThres);
		}

		if (bResize)
		{
			ptScorMax.x <<= nResizeScale;
			ptScorMax.y <<= nResizeScale;
		}

		ptScorMax.x += ptOffset.x;
		ptScorMax.y += ptOffset.y;
		ptMarkTL.push_back(ptScorMax);

		if (nullptr != ptVectorTopLeft)
		{			
			ptVectorTopLeft->push_back(ptScorMax);
		}

		ptBR.x = ptScorMax.x + pTemplate->Width();
		ptBR.y = ptScorMax.y + pTemplate->Height();
		ptMarkBR.push_back(ptBR);

		if (nullptr != ptVectorBottomRight)
		{			
			ptVectorBottomRight->push_back(ptBR);
		}
	}	

	if (bMarkPos)
	{
		for (int i = 0; i < nNumber; i++)
		{
			rectangle(*pSrc, ptMarkTL[i], ptMarkBR[i], MAT_RGB(0, 0, 0), 1, 4, 0);
		}
	}

	if (bShowResultWindow)
	{
		ImgBig.Show(_T("Big"));
	}

	ptMarkTL.clear();
	ptMarkBR.clear();

#ifdef _DEBUG
	double dEndTime = GetTickCount() - dStartTime;		// 取得开始时间
	// TRACE(_T("BinaryMatch process Time: %f mS\n"), dEndTime);
#endif

	// TRACE(_T("BinaryMatch complete and Return %f!\n"), dMatchValue);

	return dMatchValue;
}


// Surf
double Surf(CSmartImage * pObject, CSmartImage * pScene,
			double dHessianThreshold/* = 4 */, int nOctaves/* = 16 */, int nOctaveLayers/* = 8 */, double dFactor/* = 2.0 */,
			BOOL bShowResult/* = TRUE */)
{
	using namespace std;
	using namespace cv;

	// TRACE(_T("Surf start!\n"));

	if (nullptr == pObject || pObject->empty())
	{
		// TRACE(_T("MatchTemplate returned because pObject is nullptr or empty!\n"));
		return -1.0;
	}

	if (nullptr == pScene || pScene->empty())
	{
		// TRACE(_T("MatchTemplate returned because pScene is nullptr or empty!\n"));
		return -1.0;
	}

	SurfFeatureDetector		SurfDetector(dHessianThreshold, nOctaves, nOctaveLayers);
	vector<KeyPoint>		KeyPointsObject, KeyPointsScene;
	Mat						mObject = *pObject;
	Mat						mScene	= *pScene;

	SurfDetector.detect(mObject, KeyPointsObject);
	SurfDetector.detect(mScene, KeyPointsScene);

	//-- Step 2: Calculate descriptors (feature vectors)
	SiftDescriptorExtractor Extractor;

	Mat DescriptorsObject;
	Mat DescriptorsScene;

	Extractor.compute(mObject, KeyPointsObject, DescriptorsObject);
	Extractor.compute(mScene, KeyPointsScene, DescriptorsScene);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	BFMatcher		BfMatcher;
	vector<DMatch>	vDMatches;
	BfMatcher.match(DescriptorsObject, DescriptorsScene, vDMatches);

	double MaxDist = 0;
	double MinDist = INT_MAX;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < DescriptorsObject.rows; i++)
	{
		double dDist = vDMatches[i].distance;

		if (dDist < MinDist)
		{
			MinDist = dDist;
		}

		if (dDist > MaxDist)
		{
			MaxDist = dDist;
		}
	}

	// TRACE(_T("Min dist : %f \n"), MinDist);
	// TRACE(_T("Max dist : %f \n"), MaxDist);

	//-- Draw only "good" matches (i.e. whose distance is less than dFactor * Min_dist )
	vector<DMatch> GoodMatches;

	int nGoodPoint = 0;
	for (int i = 0; i < DescriptorsObject.rows; i++ )
	{
		if (vDMatches[i].distance < dFactor * MinDist)
		{
			GoodMatches.push_back(vDMatches[i]);
			nGoodPoint++;
		}
	}

	if (nGoodPoint < 4)
	{
		return -1.0;
	}

	Mat mMatch;

	if (bShowResult)
	{
		drawMatches(mObject, KeyPointsObject, mScene, KeyPointsScene,
			GoodMatches, mMatch, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	}

	//-- Localize the object from img_1 in img_2
	vector<Point2f> vObject;
	vector<Point2f> vScene;

	for (int i = 0; i < (int)GoodMatches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		vObject.push_back(KeyPointsObject[GoodMatches[i].queryIdx].pt);
		vScene.push_back(KeyPointsScene[GoodMatches[i].trainIdx ].pt);
	}

	Mat Homo = findHomography(vObject, vScene, CV_RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	vector<Point2f> vObjCorners(4);

	vObjCorners[0] = Point(0,0);
	vObjCorners[1] = Point(mObject.cols, 0);
	vObjCorners[2] = Point(mObject.cols, mObject.rows);
	vObjCorners[3] = Point(0, mObject.rows);

	vector<Point2f> vSceneCorners(4);

	perspectiveTransform(vObjCorners, vSceneCorners, Homo);

	Point2f ptOffset((float)mObject.cols, 0);

	if (bShowResult)
	{
		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(mMatch, vSceneCorners[0] + ptOffset, vSceneCorners[1] + ptOffset, Scalar(255, 0, 0), 1);
		line(mMatch, vSceneCorners[1] + ptOffset, vSceneCorners[2] + ptOffset, Scalar(0, 255, 0), 1);
		line(mMatch, vSceneCorners[2] + ptOffset, vSceneCorners[3] + ptOffset, Scalar(0, 0, 255), 1);
		line(mMatch, vSceneCorners[3] + ptOffset, vSceneCorners[0] + ptOffset, Scalar(255, 255, 0), 1);

		//-- Show detected matches
		imshow("Good Matches & Object detection", mMatch);
	}

	double dDelta_X = vSceneCorners[2].x - vSceneCorners[0].x;
	double dDelta_Y = vSceneCorners[2].y - vSceneCorners[0].y;

	if (fabs(dDelta_X) < mObject.cols * 0.5 || fabs(dDelta_Y) < mObject.rows * 0.5)
	{
		return  -1.0;
	}

	if (dDelta_X > 0 && dDelta_Y > 0)
	{
		return 0.0;
	}
	else if (dDelta_X < 0 && dDelta_Y > 0)
	{
		return 90.0;
	}
	else if (dDelta_X < 0 && dDelta_Y < 0)
	{
		return 180.0;
	}
	else if (dDelta_X > 0 && dDelta_Y < 0)
	{
		return 240.0;
	}

	return -1.0;
}


// Surf
double KeyMatch(CKeyMatcher * pObj, CSmartImage * pScene, BOOL bShowResult/* = TRUE */)
{
	using namespace std;
	using namespace cv;

	// TRACE(_T("KeyMatch start!\n"));

	if (nullptr == pObj)
	{
		// TRACE(_T("KeyMatch returned because pObj is nullptr or empty!\n"));
		return -1.0;
	}

	if (nullptr == pScene)
	{
		// TRACE(_T("KeyMatch returned because pScene is nullptr or empty!\n"));
		return -1.0;
	}

	if (nullptr == pObj->m_pDetector)
	{
		// TRACE(_T("KeyMatch returned because pObj->m_pDetector is nullptr or empty!\n"));
		return -1.0;
	}

	vector<KeyPoint>	KeyPointsScene;
	Mat					mScene = *pScene;

	pObj->m_pDetector->detect(mScene, KeyPointsScene);

	Mat DescriptorsScene;
	pObj->m_pExtractor->compute(mScene, KeyPointsScene, DescriptorsScene);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	vector<DMatch>	vDMatches;
	pObj->m_pMatcher->match(pObj->m_DescriptorsObject, DescriptorsScene, vDMatches);

	double MaxDist = 0;
	double MinDist = INT_MAX;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < pObj->m_DescriptorsObject.rows; i++)
	{
		double dDist = vDMatches[i].distance;

		if (dDist < MinDist)
		{
			MinDist = dDist;
		}

		if (dDist > MaxDist)
		{
			MaxDist = dDist;
		}
	}

	// TRACE(_T("Min dist : %f \n"), MinDist);
	// TRACE(_T("Max dist : %f \n"), MaxDist);

	//-- Draw only "good" matches (i.e. whose distance is less than dFactor * Min_dist )
	vector<DMatch> GoodMatches;

	int nGoodPoint = 0;
	for (int i = 0; i < pObj->m_DescriptorsObject.rows; i++ )
	{
		if (vDMatches[i].distance < pObj->m_dFactor * MinDist)
		{
			GoodMatches.push_back(vDMatches[i]);
			nGoodPoint++;
		}
	}

	if (nGoodPoint < 4)
	{
		return -1.0;
	}

	Mat mMatch;

	if (bShowResult)
	{
		drawMatches(pObj->m_mObject, pObj->m_KeyPointsObject, mScene, KeyPointsScene,
			GoodMatches, mMatch, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	}

	//-- Localize the object from img_1 in img_2
	vector<Point2f> vObject;
	vector<Point2f> vScene;

	for (int i = 0; i < (int)GoodMatches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		vObject.push_back(pObj->m_KeyPointsObject[GoodMatches[i].queryIdx].pt);
		vScene.push_back(KeyPointsScene[GoodMatches[i].trainIdx ].pt);
	}

	Mat Homo = findHomography(vObject, vScene, CV_RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	vector<Point2f> vObjCorners(4);

	vObjCorners[0] = Point(0,0);
	vObjCorners[1] = Point(pObj->m_mObject.cols, 0);
	vObjCorners[2] = Point(pObj->m_mObject.cols,pObj-> m_mObject.rows);
	vObjCorners[3] = Point(0, pObj->m_mObject.rows);

	vector<Point2f> vSceneCorners(4);

	perspectiveTransform(vObjCorners, vSceneCorners, Homo);

	Point2f ptOffset((float)pObj->m_mObject.cols, 0);

	if (bShowResult)
	{
		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(mMatch, vSceneCorners[0] + ptOffset, vSceneCorners[1] + ptOffset, Scalar(255, 0, 0), 1);
		line(mMatch, vSceneCorners[1] + ptOffset, vSceneCorners[2] + ptOffset, Scalar(0, 255, 0), 1);
		line(mMatch, vSceneCorners[2] + ptOffset, vSceneCorners[3] + ptOffset, Scalar(0, 0, 255), 1);
		line(mMatch, vSceneCorners[3] + ptOffset, vSceneCorners[0] + ptOffset, Scalar(255, 255, 0), 1);

		//-- Show detected matches
		imshow("Good Matches & Object detection", mMatch);
	}

	// TRACE(_T("KeyMatch complete!\n"));

	double dDelta_X = vSceneCorners[3].x - vSceneCorners[1].x;
	double dDelta_Y = vSceneCorners[3].y - vSceneCorners[1].y;

	if (fabs(dDelta_X) < pObj->m_mObject.cols * 0.5 || fabs(dDelta_Y) < pObj->m_mObject.rows * 0.5)
	{
		return -1.0;
	}

	dDelta_X = vSceneCorners[2].x - vSceneCorners[0].x;
	dDelta_Y = vSceneCorners[2].y - vSceneCorners[0].y;	

	if (fabs(dDelta_X) < pObj->m_mObject.cols * 0.5 || fabs(dDelta_Y) < pObj->m_mObject.rows * 0.5)
	{
		return -1.0;
	}

	if (dDelta_X > 0 && dDelta_Y > 0)
	{
		return 0.0;
	}
	else if (dDelta_X < 0 && dDelta_Y > 0)
	{
		return 90.0;
	}
	else if (dDelta_X < 0 && dDelta_Y < 0)
	{
		return 180.0;
	}
	else if (dDelta_X > 0 && dDelta_Y < 0)
	{
		return 240.0;
	}

	return -1.0;
}


// 漫水填充
void FloodFill(CSmartImage * pSrc, Point2i ptSeed, Scalar PixelValue,
			   Scalar LowDiff, Scalar UpDiff,
			   int nFlags)
{
	// TRACE(_T("FloodFill start!\n"));
	CHECK_SRC(_T("FloodFill"), pSrc);

	floodFill(*pSrc, ptSeed, PixelValue, nullptr, LowDiff, UpDiff, nFlags);

	// TRACE(_T("FloodFill complete!\n"));
}


// 旋转图像
void ImgRotation(CSmartImage * pSrc, CSmartImage * pDst, Point2d ptCenter,
				 double dDegree, double dScale/* = 1.0*/,
				 int nInterMode/* = CV_INTER_CUBIC | CV_WARP_FILL_OUTLIERS*/,
				 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("ImgRotation")*/)
{
	// TRACE(_T("Rotation start!\n"));

	CHECK_SRC(_T("Rotation"), pSrc);

	if ((fabs(dDegree) < 1E-6) && (fabs(dScale - 1.0) < 1E-6))
	{
		CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

		if (pImgDst != pSrc)
		{
			pImgDst->Clone(pSrc);
		}

		if (bShow)
		{
			pImgDst->Show(strWnd);
		}

		// TRACE(_T("Rotation return because of 0 == dDegree && 1 == dScale!\n"));
		return;
	}

#ifdef _DEBUG
	double dStartTime = GetTickCount();				// 取得开始时间
#endif

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (pSrc != pImgDst)
	{
		pImgDst->Set2Zero();
	}

	Mat RotateMat(2, 3, CV_32FC1);
	RotateMat = getRotationMatrix2D(ptCenter, dDegree, dScale);

	warpAffine(*pSrc, *pImgDst, RotateMat, pSrc->GetImgSize(), nInterMode);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

#ifdef _DEBUG
	double dEndTime = GetTickCount() - dStartTime;		// 取得开始时间
	// TRACE(_T("Rotation process Time: %f mS\n"), dEndTime);
#endif

	// TRACE(_T("Rotation complete!\n"));
}


void ImgRotationEx(CSmartImage * pSrc, CSmartImage * pDst, Point2d ptCenter,
				   double dDegree, double dScale/* = 1.0*/,
				   BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("ImgRotationEx")*/)
{
	// TRACE(_T("RotationEx start!\n"));
	CHECK_SRC(_T("RotationEx"), pSrc);

	if ((fabs(dDegree) < 1E-6) && (fabs(dScale - 1.0) < 1E-6))
	{
		CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

		if (pImgDst != pSrc)
		{
			pImgDst->Clone(pSrc);
		}

		if (bShow)
		{
			pImgDst->Show(_T("Rotation"));
		}

		// TRACE(_T("RotationEx return because of 0 == dDegree && 1 == dScale!\n"));
		return;
	}

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (bShow)
	{
		pImgDst->Show(_T("RotationEx"));
	}

	// TRACE(_T("RotationEx complete!\n"));
}


// 镜头校正
void LensCalibration(CSmartImage * pSrc, CSmartImage * pDst, double dCoeffs/* = 0.0*/)
{
	// TRACE(_T("LensCalibration start!\n"));

	if (nullptr == pSrc)
	{
		// TRACE(_T("LensCalibration return because pSrc is nullptr!\n"));
		return;
	}

	if (pSrc->empty())
	{
		// TRACE(_T("LensCalibration return because pSrc is empty!\n"));
		return;
	}

#ifdef _DEBUG
	double dStartTime = GetTickCount();				// 取得开始时间
#endif

	Mat CamMat = (Mat_<float>(3, 3) <<
		pSrc->Width(), 0, pSrc->Width() * 0.5f,
		0, pSrc->Height(), pSrc->Height() * 0.5f,
		0, 0, 1);

	double dCoeX = 0;
	double dCoeY = 0;

	if (pSrc->Width() >= pSrc->Height())
	{
		dCoeX = dCoeffs;
		dCoeY = dCoeffs * pSrc->Height() / pSrc->Width();
	}
	else
	{
		dCoeY = dCoeffs;
		dCoeX = dCoeffs * pSrc->Width() / pSrc->Height();
	}

	float Dist_Coef[4] = {(float)dCoeX, (float)dCoeY, 0, 0};

	Mat CoefMat(4, 1, CV_32F, Dist_Coef);
	
	CSmartImage ImgSrc;

	if (pSrc == pDst)
	{
		ImgSrc.Clone(pSrc);
		undistort(ImgSrc, *pDst, CamMat, CoefMat);
	}
	else
	{
		undistort(*pSrc, *pDst, CamMat, CoefMat);
	}

#ifdef _DEBUG
	double dEndTime = GetTickCount() - dStartTime;		// 取得开始时间
	// TRACE(_T("LensCalibration process Time: %f mS\n"), dEndTime);
#endif

	// TRACE(_T("LensCalibration complete!\n"));
}


// 取得重心坐标
Point2d GravityCenter(CSmartImage * pSrc, BOOL bDraw/* = FALSE */)
{
	// TRACE(_T("GravityCenter start!\n"));
	ASSERT(nullptr != pSrc && !pSrc->empty());
	ASSERT(1 == pSrc->channels());

	Moments Mom = moments(*pSrc);

	double M00 = (Mom.m00 < ZERO)? ZERO: Mom.m00;

	Point2d ptGravity(Mom.m10 / M00, Mom.m01 / M00);

	if (bDraw)
	{
		CSmartImage ImgTemp;
		ImgTemp.Clone(pSrc);

		circle(ImgTemp, ptGravity, 2, Scalar(128), 3);
		ImgTemp.Show(_T("GravityCenter"));
	}

	// TRACE(_T("GravityCenter complete!\n"));

	return ptGravity;
}


// 取得重心坐标
Point2d GravityCenter(const vector<Point2i> & vContour)
{
	// TRACE(_T("GravityCenter start!\n"));
	ASSERT(vContour.size());

	Moments Mom = moments(Mat(vContour));

	double M00 = (Mom.m00 < ZERO)? ZERO: Mom.m00;

	Point2d ptGravity(Mom.m10 / M00, Mom.m01 / M00);

	// TRACE(_T("GravityCenter complete!\n"));

	return ptGravity;
}


Point2d GravityCenterEx(CSmartImage * pSrc, BOOL bDraw /* = FALSE */)
{
	// TRACE(_T("GravityCenter start!\n"));
	ASSERT(nullptr != pSrc && !pSrc->empty());
	ASSERT(1 == pSrc->channels());

	CSmartImage ImgTemp;
	ImgTemp.Clone(pSrc);

	CSmartImage ImgOutline;
	ImgOutline.Clone(&ImgTemp);

	cvContours vImgContour;
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	const int nSize = vImgContour.size();

	for (int i = 1; i < nSize; i++)
	{
		line(ImgTemp, vImgContour[0][0], vImgContour[i][0], Scalar::all(255), 2, 8);
	}

	ImgOutline.Clone(&ImgTemp);
	findContours(ImgOutline, vImgContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	

	Point2d ptGravity;
	ptGravity.x = -1;
	ptGravity.y = -1;

	if (vImgContour.size())
	{
		ptGravity = GravityCenter(vImgContour[0]);
	}

	if (bDraw)
	{
		circle(ImgTemp, ptGravity, 2, Scalar(128), 3);
		ImgTemp.Show(_T("GravityCenterEx"));
	}

	// TRACE(_T("GravityCenter complete!\n"));

	return ptGravity;
}


// 平移图像
void Offset(CSmartImage * pSrc, CSmartImage * pDst, Point2i ptOffset,
			int nInterMode/* = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS */,
			BOOL bShow/* = FALSE */)
{
	// TRACE(_T("Offset start!\n"));
	CHECK_SRC(_T("Offset"), pSrc);

	if (0 == ptOffset.x && 0 == ptOffset.y)
	{
		CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

		if (pImgDst != pSrc)
		{
			pImgDst->Clone(pSrc);
		}

		if (bShow)
		{
			pImgDst->Show(_T("Offset"));
		}

		// TRACE(_T("Offset complete!\n"));

		return;
	}

	CSmartImage ImgSrc;
	ImgSrc.Clone(pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);
	pImgDst->Set2Zero();

	Rect2i rcSrc;
	Rect2i rcDst;

	if (ptOffset.x < 0)
	{
		rcSrc.x = -ptOffset.x;
		rcSrc.width = ImgSrc.Width() + ptOffset.x;
		rcDst.x = 0;
		rcDst.width = pImgDst->Width() + ptOffset.x;
	}
	else
	{
		rcSrc.x = 0;
		rcSrc.width = ImgSrc.Width() - ptOffset.x;
		rcDst.x = ptOffset.x;
		rcDst.width = pImgDst->Width() - ptOffset.x;
	}

	if (ptOffset.y < 0)
	{
		rcSrc.y = -ptOffset.y;
		rcSrc.height = ImgSrc.Height() + ptOffset.y;
		rcDst.y = 0;
		rcDst.height = pImgDst->Height() + ptOffset.y;
	}
	else
	{
		rcSrc.y = 0;
		rcSrc.height = ImgSrc.Height() - ptOffset.y;
		rcDst.y = ptOffset.y;
		rcDst.height = pImgDst->Height() - ptOffset.y;
	}

	ImgSrc.SetROI(rcSrc);
	pImgDst->SetROI(rcDst);

	pImgDst->Clone(&ImgSrc);
	pImgDst->ClearROI();

	if (bShow)
	{
		pImgDst->Show(_T("Offset"));
	}

	// TRACE(_T("Offset complete!\n"));
}


BOOL ConvexityDefects(vector<Point2i> vContour, double dDepth, double dLength, double dScale,
					  Point2i * pStart/* = nullptr*/, Point2i * pEnd/* = nullptr*/)
{
	// TRACE(_T("ConvexityDefects start!\n"));

	vector<int>		vHullInt;
	vector<Vec4i>	vDefects;

	convexHull(Mat(vContour), vHullInt);
	convexityDefects(Mat(vContour), vHullInt, vDefects);

	vector<Vec4i>::iterator pIt4i = vDefects.begin();

	BOOL bFind = FALSE;
	Point2i ptStart;
	Point2i ptEnd;
	Point2i ptFar;

	while (pIt4i != vDefects.end())
	{
		Vec4i &vData = *pIt4i;

		int nStart	= vData[0];
		int nEnd	= vData[1];
		int nFar	= vData[2];

		ptStart = vContour[nStart];
		ptEnd	= vContour[nEnd];
		ptFar	= vContour[nFar];

		double dDep = vData[3] / 256.0;
		double dX	= ptEnd.x - ptStart.x;
		double dY	= ptEnd.y - ptStart.y;

		double dDisLine = sqrt(dX * dX + dY * dY);
		double dDisContours = 0;

		dDisContours = nEnd? fabs((double)nEnd - nStart): vContour.size() - nStart;

		if (dDep > dDepth && dDisContours > dLength &&
			dDisContours > dDisLine * (dScale + 1))
		{
			bFind = TRUE;

			break;
		}

		pIt4i++;
	}

	if (nullptr != pStart)
	{
		*pStart = ptStart;
	}

	if (nullptr != pEnd)
	{
		*pEnd = ptEnd;
	}

	// TRACE(_T("ConvexityDefects complete!\n"));

	return bFind;
}



// 缺口填充
BOOL ConvexityFill(CSmartImage * pSrc, vector<Point2i> vContour, double dDepth, double dLength, double dScale, BOOL bShow/* = FALSE*/)
{
	// TRACE(_T("ConvexityFill start!\n"));

	BOOL bRetVal = FALSE;

	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("ConvexityFill complete!\n"));
		return bRetVal;
	}	

	Point2i ptStart;
	Point2i ptEnd;

	if (ConvexityDefects(vContour, dDepth, dLength, dScale, &ptStart, &ptEnd))
	{
		line(*pSrc, ptStart, ptEnd, Scalar::all(255), 2, 8);

		CSmartImage ImgTemp;
		ImgTemp.Clone(pSrc);
		
		cvContours vOutLine;
		findContours(ImgTemp, vOutLine, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);		// 提取轮廓

		drawContours(*pSrc, vOutLine, -1, Scalar::all(255), CV_FILLED);

		bRetVal = TRUE;
	}

	if (bShow)
	{
		pSrc->Show(_T("ConvexityFill"));
	}

	// TRACE(_T("ConvexityFill complete!\n"));

	return bRetVal;
}


void DrawNumber(CDC * pDC, int nNumber, int nSize, int nX, int nY,
				COLORREF rgbColor, CString strFont/* = APP_FONT */)
{
	CFont Font;

	if (nSize < 80)
	{
		nSize = 80;
	}

	Font.CreatePointFont(nSize,	strFont, nullptr);

	pDC->SelectObject(&Font);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(rgbColor);

	CString strNumber;
	strNumber.Format(_T("%d"), nNumber);

	pDC->TextOut(nX, nY, strNumber);
}


// 清晰度估计
double DefinitionEstimate(CSmartImage * pSrc, int nMode)
{
	// TRACE(_T("DefinitionEstimate start!\n"));
	
	if (nullptr == pSrc || pSrc->empty())
	{
		// TRACE(_T("DefinitionEstimate retrun 0 because pSrc is nullptr or empty!\n"));
		return 0;
	}

#ifdef _DEBUG
	const double dStartTime = GetTickCount();		// 取得开始时间
#endif

	CSmartImage ImgFocus;

	if (pSrc->channels() > 1)
	{
		Convert2Gray(pSrc, &ImgFocus);
		pSrc = &ImgFocus;
	}

	double dResult = 0;

	switch (nMode)
	{
	case DEFINIT_SMD:
		{
			int nNoneZero = 0;
			const int nWidth = pSrc->Width();
			const int nHeight = pSrc->Height();

			for(int nH = 0; nH < nHeight - 1; nH++)
			{
				BYTE *pData1 = pSrc->ptr<BYTE>(nH);
				BYTE *pData2 = pSrc->ptr<BYTE>(nH + 1);

				for(int nW = 0; nW < nWidth - 1; nW++)
				{
					if (*pData1 && *pData2)
					{
						nNoneZero++;

						double dVal = (*(pData1 + 1) - *pData1) * (*(pData1 + 1) - *pData1) +
							(*pData2 - *pData1) * (*pData2 - *pData1);

						if (dVal > 9)
						{
							dResult += dVal;
						}
					}
				}
			}

			dResult = dResult / nNoneZero;
		}
		break;

	case DEFINIT_EDGE:
		{
			CSmartImage ImgRes(pSrc->size(), CV_32FC1);

			Mat mKernel = (Mat_<BYTE>(3, 3) <<
				-1, -4, -1,
				-4, 20, -4,
				-1, -4, -1);

			filter2D(*pSrc, ImgRes, ImgRes.depth(), mKernel);
			abs(ImgRes);

			const Scalar Avg = mean(ImgRes);
			
			dResult = Avg[0] / pSrc->Width() / pSrc->Height();

			break;
		}
		
		break;

	case DEFINIT_REBLUR:
		break;

	default:
		break;
	}

#ifdef _DEBUG
	const double dmS = GetTickCount() - dStartTime;
	// TRACE(_T("DefinitionEstimate Process Time: %f mS\n"), dmS);
#endif

	// TRACE(_T("DefinitionEstimate complete!\n"));

	return dResult;
}


double MinDistance(const vector<Point2i> * pV1, const vector<Point2i> * pV2,
				   Point2i * ptDst1/* = nullptr*/, Point2i * ptDst2 /*= nullptr*/)
{
	double dMinDis = INT_MAX;

	const int nSize1 = pV1->size();
	const int nSize2 = pV2->size();

	for (int i = 0; i < nSize1; i++)
	{
		const Point2i *pt1 = &(*pV1)[i];

		for (int j = 0; j < nSize2; j++)
		{
			const Point2i *pt2 = &(*pV2)[j];

			const double dX = pt1->x - pt2->x;
			const double dY = pt1->y - pt2->y;
			const double dR = dX * dX + dY * dY;

			if (dR < dMinDis)
			{
				dMinDis = dR;

				if (nullptr != ptDst1)
				{
					*ptDst1 = *pt1;
				}

				if (nullptr != ptDst2)
				{
					*ptDst2 = *pt2;
				}
			}
		}
	}

	return sqrt(dMinDis);
}


// GrabCut
BOOL GrabCut(CSmartImage * pSrc, CRect rcGrab, CSmartImage * pResult)
{
	return TRUE;
}


/*================================================================
功能: 仿射变换
传入参数:	
	1. pSrc: 源图像
	2. pDst: 变换后图像
	3. ptSrcTri: 源图像变换点
	4. ptDstTri: 变换后对应点
	5. bShopw: 是否要显示变换后图像
	6. strWnd: 显示窗口名称
返回值: 最大轮廓序号
================================================================*/
void WarpAffine(CSmartImage * pSrc, CSmartImage * pDst,
				Point2f ptSrcTri[], Point2f ptDstTri[],
				int nInterpolation/* = CV_INTER_CUBIC */,
				BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("WarpAffine")*/)
{
	// TRACE(_T("WarpAffine start!\n"));
	CHECK_SRC(_T("WarpAffine"), pSrc);

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	Mat WarpMat(2, 3, CV_32FC1);
	WarpMat = getAffineTransform(ptSrcTri, ptDstTri);

	warpAffine(*pSrc, *pImgDst, WarpMat, pImgDst->size(), nInterpolation);

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	// TRACE(_T("WarpAffine complete!\n"));
}


// 傅里叶变换
void FourierTransform(CSmartImage * pSrc, CSmartImage *pSpectrum, CSmartImage * pDst,
					  BOOL bShow/* = FALSE */, const CString & strWnd/* = _T("FourierTransform")*/)
{
	// TRACE(_T("FourierTransform start!\n"));
	CHECK_SRC(_T("FourierTransform"), pSrc);

	ASSERT(pSrc->channels());
	ASSERT(nullptr != pSpectrum);

	CSmartImage ImgDft;

	copyMakeBorder(*pSrc, ImgDft,
		0, pSrc->rows, 0, pSrc->cols,
		BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = {Mat_<float>(ImgDft), Mat::zeros(ImgDft.size(), CV_32FC1)};

	const int nWidth = pSrc->cols;
	const int nHeight = pSrc->rows;

	for (int nH = 0; nH < nHeight; nH++)
	{
		float *pData = planes[0].ptr<float>(nH);

		for (int nW = 0; nW < nWidth; nW++)
		{
			if (IS_ODD(nW + nH))
			{
				pData[nW] *= -1;
			}
		}
	}

	CSmartImage ImgComplex;

	merge(planes, 2, ImgComplex);
	dft(ImgComplex, *pSpectrum);

	split(*pSpectrum, planes);	// planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	if (nullptr != pDst)
	{
		magnitude(planes[0], planes[1], *pDst);

		*pDst += Scalar::all(1);									// switch to logarithmic scale
		log(*pDst, *pDst);

		normalize(*pDst, *pDst, 0, 1, CV_MINMAX);
		pDst->convertTo(*pDst, CV_8UC1, 255);

		if (bShow)
		{
			pDst->Show(strWnd);
		}
	}
	else if (bShow)
	{
		CSmartImage ImgDst;

		magnitude(planes[0], planes[1], ImgDst);

		ImgDst += Scalar::all(1);
		log(ImgDst, ImgDst);

		normalize(ImgDst, ImgDst, 0, 1, CV_MINMAX);

		ImgDst.convertTo(ImgDst, CV_8UC1, 255);

		if (bShow)
		{
			ImgDst.Show(strWnd);
		}
	}

	// TRACE(_T("FourierTransform complete!\n"));
}


// 频谱中心化
void SpectrumCentralize(CSmartImage * pSpectrum, CSmartImage * pDst/* = nullptr*/,
						BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("SpectrumCentralize")*/)
{
	// TRACE(_T("SpectrumCentralize start!\n"));
	CHECK_SRC(_T("SpectrumCentralize"), pSpectrum);

	const int cx = pSpectrum->cols >> 1;
	const int cy = pSpectrum->rows >> 1;

	Mat Q0(*pSpectrum, Rect(0, 0, cx, cy));			// Top-Left - Create a ROI per quadrant 
	Mat Q1(*pSpectrum, Rect(cx, 0, cx, cy));		// Top-Right
	Mat Q2(*pSpectrum, Rect(0, cy, cx, cy));		// Bottom-Left
	Mat Q3(*pSpectrum, Rect(cx, cy, cx, cy));		// Bottom-Right

	// swap quadrants (Top-Left with Bottom-Right)
	// swap quadrant (Top-Right with Bottom-Left)

	Mat mTemp;
	Q0.copyTo(mTemp);
	Q3.copyTo(Q0);
	mTemp.copyTo(Q3);

	Q1.copyTo(mTemp);
	Q2.copyTo(Q1);
	mTemp.copyTo(Q2);

	if (nullptr != pDst)
	{
		if (pDst != pSpectrum)
		{
			pDst->Clone(pSpectrum);
		}
	}
	else
	{
		pDst = pSpectrum;
	}

	if (bShow)
	{
		pDst->Show(strWnd);
	}

	// TRACE(_T("SpectrumCentralize complete!\n"));
}


// 频域滤波
void FrequencyDomainFilter(CSmartImage * pSpectrum, CSmartImage * pDst, CSmartImage * pFilter, BOOL bCvt,
						   BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("FrequencyDomainFilter")*/)
{
	// TRACE(_T("FrequencyDomainFilter start!\n"));
	CHECK_SRC(_T("FrequencyDomainFilter"), pSpectrum);

	ASSERT(nullptr != pFilter);
	ASSERT(nullptr != pDst);
	ASSERT(pSpectrum != pDst);

	Mat planes[] = {*pFilter, *pFilter};

	CSmartImage ImgFilter2;
	merge(planes, 2, ImgFilter2);

	CSmartImage iDft;
	multiply(*pSpectrum, ImgFilter2, iDft);

	CSmartImage ImgResult;
	idft(iDft, ImgResult, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT ); // Applying IDFT

	const int nWidth = ImgResult.Width() >> 1;
	const int nHeight = ImgResult.Height() >> 1;

	for (int nH = 0; nH < nHeight; nH++)
	{
		float *pData = ImgResult.ptr<float>(nH);

		for (int nW = 0; nW < nWidth; nW++)
		{
			if (IS_ODD(nW + nH))
			{
				pData[nW] *= -1;
			}
		}
	}

	ImgResult.SetROI(Rect2i(0, 0, nWidth, nHeight));

	pDst->Clone(&ImgResult);

	if (bCvt)
	{
		pDst->convertTo(*pDst, CV_8UC1);
	}

	if (bShow)
	{
		pDst->Show(strWnd);
	}

	// TRACE(_T("FrequencyDomainFilter complete!\n"));
}


// 计算均值与方差
void MeanVariance(CSmartImage * pSrc, CSmartImage * pMean, CSmartImage * pVar, Size2i szKernel,
				  BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("MeanVar")*/)
{
	// TRACE(_T("MeanVariance start!\n"));
	CHECK_SRC(_T("MeanVariance"), pSrc);

	ASSERT(1 == pSrc->channels());

	if (IS_EVENT(szKernel.width))
	{
		szKernel.width++;
	}

	if (IS_EVENT(szKernel.height))
	{
		szKernel.height++;
	}

	const int nWidth		= pSrc->Width();
	const int nHeight		= pSrc->Height();
	const int nKernelSize	= szKernel.width * szKernel.height;
	const int nSubWidth		= szKernel.width - 1;
	const int nSubHeight	= szKernel.height - 1;

	CSmartImage ImgMean(nWidth, nHeight, CV_32FC1, Scalar::all(0));
	CSmartImage ImgVar(nWidth, nHeight, CV_32FC1, Scalar::all(0));

	CSmartImage ImgBig;

	copyMakeBorder(*pSrc,
		ImgBig, szKernel.height >> 1, szKernel.height >> 1, szKernel.width >> 1, szKernel.width >> 1,
		BORDER_REFLECT);

	BYTE **ppData = new BYTE *[szKernel.height];

	double *pLastRowSum = new double[szKernel.height];
	double *pLastColSum = new double[szKernel.width];
	double *pLastRowVar = new double[szKernel.height];
	double *pLastColVar = new double[szKernel.width];

	memset(pLastRowSum, 0, sizeof(double) * szKernel.height);
	memset(pLastColSum, 0, sizeof(double) * szKernel.width);
	memset(pLastRowVar, 0, sizeof(double) * szKernel.height);
	memset(pLastColVar, 0, sizeof(double) * szKernel.width);

	double dDelta = 0;

	for (int nH = 0; nH < nHeight; nH++)
	{
		for (int i = 0; i < szKernel.height; i++)
		{
			ppData[i] = ImgBig.ptr<BYTE>(nH + i);
		}

		float *pMean = ImgMean.ptr<float>(nH);
		float *pVar = ImgVar.ptr<float>(nH);

		for (int nW = 0; nW < nWidth; nW++)
		{
			float *pM = pMean + nW;
			float *pV = pVar + nW;

			if (nW)
			{
				(*pM) = (float)((*(pM - 1)) - pLastColSum[0]);

				memmove(pLastColSum, pLastColSum + 1, sizeof(double) * nSubWidth);
				pLastColSum[nSubWidth] = 0;

				for (int i = 0; i < szKernel.height; i++)
				{
					pLastColSum[nSubWidth] += ppData[i][nW + nSubWidth];
				}

				(*pM) += (float)pLastColSum[nSubWidth];

				//////////////////////////////////////////////////////////////////////////
				const double dMean = (*pM) / nKernelSize;

				(*pV) = (float)((*(pV - 1)) - pLastColVar[0]);

				memmove(pLastColVar, pLastColVar + 1, sizeof(double) * nSubWidth);
				pLastColVar[nSubWidth] = 0;

				for (int i = 0; i < szKernel.height; i++)
				{
					dDelta = ppData[i][nW + nSubWidth] - dMean;
					dDelta *= dDelta;

					pLastColVar[nSubWidth] += dDelta;
				}

				(*pV) += (float)pLastColVar[nSubWidth];
			}
			else if (nH)
			{
				(*pM) = (float)(*(ImgMean.ptr<float>(nH - 1)) - pLastRowSum[0]);

				memmove(pLastRowSum, pLastRowSum + 1, sizeof(double) * nSubHeight);
				memset(pLastColSum, 0, sizeof(double) * szKernel.width);

				pLastRowSum[nSubHeight] = 0;

				for (int i = 0; i < szKernel.width; i++)
				{
					pLastRowSum[nSubHeight] += ppData[nSubHeight][i];

					for (int j = 0; j < szKernel.height; j++)
					{
						pLastColSum[i] += ppData[j][i];
					}
				}

				(*pM) += (float)pLastRowSum[nSubHeight];

				//////////////////////////////////////////////////////////////////////////
				const double dMean = (*pM) / nKernelSize;

				(*pV) = (float)(*(ImgVar.ptr<float>(nH - 1)) - pLastRowVar[0]);

				memmove(pLastRowVar, pLastRowVar + 1, sizeof(double) * nSubHeight);
				memset(pLastColVar, 0, sizeof(double) * szKernel.width);

				pLastRowVar[nSubHeight] = 0;

				for (int i = 0; i < szKernel.width; i++)
				{
					dDelta = ppData[nSubHeight][i] - dMean;
					dDelta *= dDelta;

					pLastRowVar[nSubHeight] += dDelta;

					for (int j = 0; j < szKernel.height; j++)
					{
						dDelta = ppData[j][i] - dMean;
						dDelta *= dDelta;

						pLastColVar[i] += dDelta;
					}
				}

				(*pV) += (float)pLastRowVar[nSubHeight];
			}
			else
			{
				for (int i = 0; i < szKernel.height; i++)
				{
					for (int j = 0; j < szKernel.width; j++)
					{
						const double dData = ppData[i][j];

						pLastRowSum[i] += dData;
						pLastColSum[j] += dData;

						(*pM) += (float)dData;
					}
				}

				//////////////////////////////////////////////////////////////////////////
				const double dMean = (*pM) / nKernelSize;

				for (int i = 0; i < szKernel.height; i++)
				{
					for (int j = 0; j < szKernel.width; j++)
					{
						dDelta = ppData[i][j] - dMean;
						dDelta *= dDelta;

						pLastRowVar[i] += dDelta;
						pLastColVar[j] += dDelta;

						(*pV) += (float)dDelta;
					}
				}
			}
		}
	}

	delete []ppData;
	delete []pLastRowSum;
	delete []pLastColSum;
	delete []pLastRowVar;
	delete []pLastColVar;

	ImgMean /= nKernelSize;
	ImgVar /= nKernelSize;

	if (nullptr != pMean)
	{
		pMean->Clone(&ImgMean);
	}

	if (nullptr != pVar)
	{
		pVar->Clone(&ImgVar);
	}

	if (bShow)
	{
		ImgMean.convertTo(ImgMean, CV_8UC1, 1);
		ImgMean.Show(strWnd + _T("-Mean"));

		normalize(ImgVar, ImgVar, 0, 255, CV_MINMAX);
		ImgVar.convertTo(ImgVar, CV_8UC1, 1);
		ImgVar.Show(strWnd + _T("-Var"));
	}

	// TRACE(_T("MeanVariance complete!\n"));
}


// 自适应局部降噪滤波
void AdaptLocNoiseFilter(CSmartImage * pSrc, CSmartImage * pDst, Size2i szKernel, double dVarEst,
						 BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("AdaptLocNoiseFilter")*/)
{
	// TRACE(_T("AdaptLocNoiseFilter start!\n"));
	CHECK_SRC(_T("AdaptLocNoiseFilter"), pSrc);

	ASSERT(1 == pSrc->channels());

	CSmartImage ImgMean;
	CSmartImage ImgVar;

	MeanVariance(pSrc, &ImgMean, &ImgVar, szKernel);

	CSmartImage ImgDst(pSrc);

	const int nWidth = pSrc->Width();
	const int nHeight = pSrc->Height();

	for (int nH = 0; nH < nHeight; nH++)
	{
		const BYTE *pSrcData = pSrc->ptr<BYTE>(nH);
		const float *pMean = ImgMean.ptr<float>(nH);
		const float *pVar = ImgVar.ptr<float>(nH);

		BYTE *pDst = ImgDst.ptr<BYTE>(nH);

		for (int nW = 0; nW < nWidth; nW++)
		{
			const double dSrcMean = pSrcData[nW] - pMean[nW];
			pDst[nW] = (BYTE)(pSrcData[nW] - dVarEst * dVarEst * dSrcMean / (pVar[nW] + 0.000001));
		}
	}

	if (nullptr == pDst)
	{
		pSrc->Clone(&ImgDst);
	}
	else
	{
		pDst->Clone(&ImgDst);
	}

	if (bShow)
	{
		ImgDst.Show(strWnd);
	}

	// TRACE(_T("AdaptLocNoiseFilter complete!\n"));
}


//自适应中值滤波
void AdaptMedianFilter(CSmartImage * pSrc, CSmartImage * pDst, Size2i szMaxKernel,
					   BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("AdaptMeidanFilter")*/)
{
	// TRACE(_T("AdaptMeidanFilter start!\n"));
	CHECK_SRC(_T("AdaptMeidanFilter"), pSrc);

	ASSERT(1 == pSrc->channels());

	if (IS_EVENT(szMaxKernel.width))
	{
		szMaxKernel.width++;
	}

	if (IS_EVENT(szMaxKernel.height))
	{
		szMaxKernel.height++;
	}

	if (szMaxKernel.width > 9)
	{
		szMaxKernel.width = 9;
	}

	if (szMaxKernel.height > 9)
	{
		szMaxKernel.height = 9;
	}

	const int nMaxSize = max(szMaxKernel.width, szMaxKernel.height);
	const int nHalfSize = nMaxSize >> 1;

	CSmartImage ImgBig;

	copyMakeBorder(*pSrc, ImgBig, nHalfSize, nHalfSize, nHalfSize, nHalfSize, BORDER_REFLECT);

	CSmartImage ImgMed[4];

	medianBlur(ImgBig, ImgMed[0], 3);

	if (nMaxSize >= 5)
	{
		medianBlur(ImgBig, ImgMed[1], 5);
	}

	if (nMaxSize >= 7)
	{
		medianBlur(ImgBig, ImgMed[2], 7);
	}

	if (nMaxSize >= 9)
	{
		medianBlur(ImgBig, ImgMed[3], 9);
	}

	CSmartImage ImgDst(pSrc);

	double dA1 = 0;
	double dA2 = 0;
	double dB1 = 0;
	double dB2 = 0;
	int nMed = 0;
	int nSubSize = 3;

	double dMin = 0;
	double dMax = 0;

	const int nWidth = ImgBig.Width();
	const int nHeight = ImgBig.Height();

	for (int nH = nHalfSize; nH < nHeight - nHalfSize; nH++)
	{
		BYTE *pSrcData = pSrc->ptr<BYTE>(nH - nHalfSize);
		BYTE *pDstData = ImgDst.ptr<BYTE>(nH - nHalfSize);

		BYTE *pBigData = ImgBig.ptr<BYTE>(nH);

		for (int nW = nHalfSize; nW < nWidth - nHalfSize; nW++)
		{
LABEL_LOOP:

			const Mat mKernel = ImgBig(Rect2i(nW - (nSubSize >> 1), nH - (nSubSize >> 1), nSubSize, nSubSize));

			minMaxLoc(mKernel, &dMin, &dMax);

			switch (nSubSize)
			{
			case 3:
				nMed = (*(ImgMed[0].ptr<BYTE>(nH) + nW));
				break;

			case 5:
				nMed = (*(ImgMed[1].ptr<BYTE>(nH) + nW));
				break;

			case 7:
				nMed = (*(ImgMed[2].ptr<BYTE>(nH) + nW));
				break;

			case 9:
				nMed = (*(ImgMed[3].ptr<BYTE>(nH) + nW));
				break;

			default:
				break;
			}

			dA1 = nMed - dMin;
			dA2 = nMed - dMax;

			if (dA1 > 0 && dA2 < 0)
			{
				const BYTE nSrcData = pSrcData[nW - nHalfSize];

				dB1 = nSrcData - dMin;
				dB2 = nSrcData - dMax;

				if (dB1 > 0 && dB2 < 0)
				{
					pDstData[nW - nHalfSize] = nSrcData;
				}
				else
				{
					pDstData[nW - nHalfSize] = nMed;
				}

				nSubSize = 3;
			}
			else
			{
				nSubSize += 2;

				if (nSubSize > nMaxSize)
				{
					nSubSize = 3;

					pDstData[nW - nHalfSize] = nMed;
				}
				else
				{
					goto LABEL_LOOP;
				}
			}
		}
	}

	if (nullptr == pDst)
	{
		pSrc->Clone(&ImgDst);
	}
	else
	{
		pDst->Clone(&ImgDst);
	}

	if (bShow)
	{
		ImgDst.Show(strWnd);
	}

	// TRACE(_T("AdaptMeidanFilter complete!\n"));
}


// 图像细化
void Thin(CSmartImage * pSrc, CSmartImage *pDst/* = nullptr*/, BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("Thin")*/)
{
	// TRACE(_T("Thin start!\n"));
	CHECK_SRC(_T("Thin"), pSrc);

	ASSERT(1 == pSrc->channels());

	CSmartImage ImgBig;
	copyMakeBorder(*pSrc, ImgBig, 2, 2, 2, 2, BORDER_CONSTANT, Scalar::all(0));

	const int nWidth	= ImgBig.Width() - 2;
	const int nHeight	= ImgBig.Height() - 2;
	const int nStep		= ImgBig.step;

	int nMarks = 0;

	BYTE **ppMark = new BYTE *[nWidth * nHeight];
	memset(ppMark, 0, nWidth * nHeight);

	while (TRUE)
	{
		for (int nH = 1; nH < nHeight; nH++)
		{
			BYTE *pData = ImgBig.ptr<BYTE>(nH);

			for (int nW = 1; nW < nWidth; nW++)
			{
				// 如果满足四个条件，进行标记
				//  P9 P2 P3
				//  P8 P1 P4
				//  P7 P6 P5

				if (!pData[nW])
				{
					continue;
				}

				BYTE *pNei = pData - nStep + nW - 1;

				const BYTE P9 = pNei[0];
				const BYTE P2 = pNei[1];
				const BYTE P3 = pNei[2];

				pNei = pData + nW - 1;

				const BYTE P8 = pNei[0];
				const BYTE P4 = pNei[2];

				pNei = pData + nStep + nW - 1;

				const BYTE P7 = pNei[0];
				const BYTE P6 = pNei[1];
				const BYTE P5 = pNei[2];

				const int nSum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;

				if (nSum > 255 && nSum < 1785)
				{
					int A = 0;

					A += (P3 - P2 > 0);
					A += (P4 - P3 > 0);
					A += (P5 - P4 > 0);
					A += (P6 - P5 > 0);
					A += (P7 - P6 > 0);
					A += (P8 - P7 > 0);
					A += (P9 - P8 > 0);
					A += (P2 - P9 > 0);

					const int nP46 = P4 * P6;

					if ((1 == A) && 0 == P2 * nP46 && 0 == nP46 * P8)
					{
						// 标记
						ppMark[nMarks++] = pData + nW;
					}
				}
			}
		}

		if (nMarks)
		{
			for (int i = 0; i < nMarks; i++)
			{
				*ppMark[i] = 0;
			}

			nMarks = 0;
		}
		else
		{
			break;
		}

		for (int nH = 1; nH < nHeight; nH++)
		{
			BYTE *pData = ImgBig.ptr<BYTE>(nH);

			for (int nW = 1; nW < nWidth; nW++)
			{
				// 如果满足四个条件，进行标记
				//  P9 P2 P3
				//  P8 P1 P4
				//  P7 P6 P5

				if (!pData[nW])
				{
					continue;
				}

				BYTE *pNei = pData - nStep + nW - 1;

				const BYTE P9 = pNei[0];
				const BYTE P2 = pNei[1];
				const BYTE P3 = pNei[2];

				pNei = pData + nW - 1;

				const BYTE P8 = pNei[0];
				const BYTE P4 = pNei[2];

				pNei = pData + nStep + nW - 1;

				const BYTE P7 = pNei[0];
				const BYTE P6 = pNei[1];
				const BYTE P5 = pNei[2];

				const int nSum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;

				if (nSum > 255 && nSum < 1785)
				{
					int A = 0;

					A += (P3 - P2 > 0);
					A += (P4 - P3 > 0);
					A += (P5 - P4 > 0);
					A += (P6 - P5 > 0);
					A += (P7 - P6 > 0);
					A += (P8 - P7 > 0);
					A += (P9 - P8 > 0);
					A += (P2 - P9 > 0);

					const int nP28 = P2 * P8;

					if (1 == A && 0 == nP28 * P4 && 0 == nP28 * P6)
					{
						// 标记
						ppMark[nMarks++] = pData + nW;
					}
				}
			}
		}

		if (nMarks)
		{
			for (int i = 0; i < nMarks; i++)
			{
				*ppMark[i] = 0;
			}

			nMarks = 0;
		}
		else
		{
			break;
		}
	}

	delete []ppMark;

	CSmartImage mDst = ImgBig(Rect2i(2, 2, nWidth - 2, nHeight - 2));

	if (nullptr == pDst)
	{		
		pSrc->Clone(&mDst);
	}
	else
	{
		pDst->Clone(&mDst);
	}

	if (bShow)
	{
		mDst.Show(strWnd);
	}

	// TRACE(_T("Thin complete!\n"));
}


// 图像细化
void CentraAxis(CSmartImage * pSrc, CSmartImage *pDst/* = nullptr*/, BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("CentraAxis")*/)
{
	// TRACE(_T("CentraAxis start!\n"));
	CHECK_SRC(_T("CentraAxis"), pSrc);

	ASSERT(1 == pSrc->channels());

	CSmartImage ImgDis;
	distanceTransform(*pSrc, ImgDis, CV_DIST_L2, CV_DIST_MASK_3);

	CSmartImage ImgSign(&ImgDis);

	int nWidth	= ImgDis.Width() - 2;
	int nHeight	= ImgDis.Height() - 2;
	int nStep	= ImgDis.step;

	for (int nH = 1; nH < nHeight; nH++)
	{
		float *pDis = ImgDis.ptr<float>(nH);
		float *pSign = ImgSign.ptr<float>(nH);

		for (int nW = 1; nW < nWidth; nW++)
		{
			const float P3 = pDis[-nStep + nW + 1];
			const float P4 = pDis[nW + 1];
			const float P5 = pDis[nStep + nW + 1];
			const float P6 = pDis[nStep + nW];

			float fDir = P3 - pSign[nW];	

			if (fDir > 0)
			{
				pSign[nW] += 1;
			}
			else if (fDir < 0)
			{
				pSign[nW] -= 1;
			}

			fDir = P4 - pDis[nW];

			if (fDir > 0)
			{
				pSign[nW] += 1;
			}
			else if (fDir < 0)
			{
				pSign[nW] -= 1;
			}

			fDir = P5 - pDis[nW];

			if (fDir > 0)
			{
				pSign[nW] += 1;
			}
			else if (fDir < 0)
			{
				pSign[nW] -= 1;
			}

			fDir = P6 - pDis[nW];

			if (fDir > 0)
			{
				pSign[nW] += 1;
			}
			else if (fDir < 0)
			{
				pSign[nW] -= 1;
			}
		}
	}

	CSmartImage ImgMark(ImgSign.GetImgSize(), CV_8UC1, Scalar::all(0));
	
	nWidth	= ImgSign.Width() - 2;
	nHeight	= ImgSign.Height() - 2;
	nStep	= ImgSign.step;

	for (int nH = 2; nH < nHeight; nH++)
	{
		BYTE *pMark = ImgMark.ptr<BYTE>(nH);
		float *pSign = ImgSign.ptr<float>(nH);

		for (int nW = 2; nW < nWidth; nW++)
		{
			const float P2 = pSign[-nStep + nW];
			const float P3 = pSign[-nStep + nW + 1];
			const float P4 = pSign[nW + 1];
			const float P5 = pSign[nStep + nW + 1];
			const float P6 = pSign[nStep + nW];
			const float P7 = pSign[nStep + nW - 1];
			const float P8 = pSign[nW - 1];
			const float P9 = pSign[-nStep + nW - 1];

			if ((P8 > 0 && P4 < 0) ||
				(P8 > 0 && 0 == pSign[nW] && P4 < 0))
			{
				pMark[nW] = 255;
			}

// 			if ((pSign[nW] > 0 && P4 < 0) ||
// 				(pSign[nW] > 0 && P6 < 0) ||
// 				(pSign[nW] > 0 && P3 < 0) ||
// 				(pSign[nW] > 0 && P5 < 0))
// 			{
// 				pMark[nW] = 255;
// 			}
// 			if ((pSign[nW] < 0 && P4 > 0) ||
// 				(pSign[nW] < 0 && P6 > 0) ||
// 				(pSign[nW] < 0 && P3 > 0) ||
// 				(pSign[nW] < 0 && P5 > 0))
// 			{
// 				pMark[nW] = 255;
// 			}
// 			else if ((P2 > 0 && 0 == pSign[nW] && P6 < 0) ||
// 				(P9 > 0 && 0 == pSign[nW] && P5 < 0) ||
// 				(P8 > 0 && 0 == pSign[nW] && P4 < 0) ||
// 				(P7 > 0 && 0 == pSign[nW] && P3 < 0))
// 			{
// 				pMark[nW] = 255;
// 			}
// 			else if ((P2 < 0 && 0 == pSign[nW] && P6 > 0) ||
// 				(P9 < 0 && 0 == pSign[nW] && P5 > 0) ||
// 				(P8 < 0 && 0 == pSign[nW] && P4 > 0) ||
// 				(P7 < 0 && 0 == pSign[nW] && P3 > 0))
// 			{
// 				pMark[nW] = 255;
// 			}
// 			else if ((P2 > 0 && 0 == pSign[nW] && P6 < 0) ||
// 				(P9 > 0 && 0 == pSign[nW] && P5 < 0) ||
// 				(P8 > 0 && 0 == pSign[nW] && P4 < 0) ||
// 				(P7 > 0 && 0 == pSign[nW] && P3 < 0))
// 			{
// 				pMark[nW] = 255;
// 			}
// 			else if ((P2 < 0 && 0 == pSign[nW] && P6 > 0) ||
// 				(P9 < 0 && 0 == pSign[nW] && P5 > 0) ||
// 				(P8 < 0 && 0 == pSign[nW] && P4 > 0) ||
// 				(P7 < 0 && 0 == pSign[nW] && P3 > 0))
// 			{
// 				pMark[nW] = 255;
// 			}
		}
	}

	if (nullptr == pDst)
	{
		pSrc->Clone(&ImgMark);
	}
	else
	{
		pDst->Clone(&ImgMark);
	}

	if (bShow)
	{
		ImgMark.Show(strWnd);
	}

	// TRACE(_T("CentraAxis!\n"));
}


// 取得最大轮廓序号
int GetMaxSizeContour(const cvContours * pContours)
{
	const int nSize = pContours->size();

	if (!nSize)
	{
		return -1;
	}

	int nMaxIndex = -1;
	int nMaxSize = 0;

	for (int i = 0; i < nSize; i++)
	{
		const int nSubSize = pContours->at(i).size();

		if (nSubSize > nMaxSize)
		{
			nMaxSize = nSubSize;
			nMaxIndex = i;
		}
	}

	return nMaxIndex;
}


// 圆度计算
double RoundnessEstimate(const vector<Point2i> * pvContour)
{	
	ASSERT(nullptr != pvContour);

	double dPerimeter = pvContour->size();
	double dArea = contourArea(*pvContour) + dPerimeter * 0.5;

	return RoundnessEstimate(dArea, dPerimeter);
}


// 圆度计算
double RoundnessEstimate(double dArea, double dPerimeter)
{
	if (dPerimeter <= 0)
	{
		return 0;
	}

	return (4 * CV_PI * dArea) / (dPerimeter * dPerimeter);
}


//计算轮廓最小外接矩形的角度
double MinRectAngle(const RotatedRect & rc, BOOL bLongSize)
{
	Point2f pt[4];
	rc.points(pt);

	const double dOfst10X	= pt[1].x - pt[0].x;
	const double dOfst10Y	= pt[1].y - pt[0].y;
	const double dDis10		= sqrt(dOfst10X * dOfst10X + dOfst10Y * dOfst10Y);

	const double dOfst21X	= pt[2].x - pt[1].x;
	const double dOfst21Y	= pt[2].y - pt[1].y;
	const double dDis21		= sqrt(dOfst21X * dOfst21X + dOfst21Y * dOfst21Y);

	double dAngle = 0;

	if (dDis10 >= dDis21)
	{
		dAngle = -acos(fabs(dOfst10X) / dDis10) * 180.0 / CV_PI;
	}
	else
	{
		dAngle = acos(fabs(dOfst21X) / dDis21) * 180.0 / CV_PI;
	}

	if (FALSE == bLongSize)
	{
		if (dAngle > 45)
		{
			dAngle -= 90;
		}
		else if (dAngle < -45)
		{
			dAngle += 90;
		}
	}

	return dAngle;
}


// 三点确定圆
AFX_INLINE Point3d _FitCircle(vector<Point2d> * pvPos)
{
	double sum_x1 = 0;
	double sum_y1 = 0;
	double sum_x2 = 0;
	double sum_y2 = 0;
	double sum_x3 = 0;
	double sum_y3 = 0;
	double sum_x1y1 = 0;
	double sum_x1y2 = 0;
	double sum_x2y1 = 0;

	const int nFitSize = pvPos->size();

	for (int i = 0; i < nFitSize; i++)
	{
		const double x = pvPos->at(i).x;
		const double y = pvPos->at(i).y;
		const double x2 = x * x;
		const double y2 = y * y;

		sum_x1 += x;
		sum_y1 += y;
		sum_x2 += x2;
		sum_y2 += y2;
		sum_x3 += x2 * x;
		sum_y3 += y2 * y;
		sum_x1y1 += x * y;
		sum_x1y2 += x * y2;
		sum_x2y1 += x2 * y;
	}

	const double C = nFitSize * sum_x2 - sum_x1 * sum_x1;
	const double D = nFitSize * sum_x1y1 - sum_x1 * sum_y1;
	const double E = nFitSize * sum_x3 + nFitSize * sum_x1y2 - (sum_x2 + sum_y2) * sum_x1;
	const double G = nFitSize * sum_y2 - sum_y1 * sum_y1;
	const double H = nFitSize * sum_x2y1 + nFitSize * sum_y3 - (sum_x2 + sum_y2) * sum_y1;
	const double a = (H * D - E * G) / (C * G - D * D);
	const double b = (H * C - E * D) / (D * D - G * C);
	const double c = -(a * sum_x1 + b * sum_y1 + sum_x2 + sum_y2) / nFitSize;

	const double center_x = -a * 0.5;
	const double center_y = -b * 0.5;
	const double radius = sqrt(a * a + b * b - 4 * c) * 0.5;

	return Point3d(center_x, center_y, radius);
}


// 比较向量大小
bool _CompSizeB2S(const FIT_CIRCLE_PARAM & v1, const FIT_CIRCLE_PARAM & v2)
{
	return v1.vpt.size() > v2.vpt.size();
}


vector<Point3d> GetFitCircle(vector<Point2i> *ptSrc,
							 double dRadius, double dScaleMin, double dScaleMax,
							 double dMaxOfst, int nInters, double dScore, int nWant)
{
#ifdef _DEBUG
	const double dStart = (double)getTickCount();
#endif // DEBUG	

	// 设定极限参数, 都是平方
	const double dR2 = dRadius * dRadius;
	const double dRMin2 = dR2 * dScaleMin * dScaleMin;
	const double dRMax2 = dR2 * dScaleMax * dScaleMax;
	const double dDiameter2 = dRMax2 * 4.0;

	const double dLimit = CV_PI * dRadius * 2.0 * dScore;

	vector<FIT_CIRCLE_PARAM> vProbCirs;

	RNG rng(time(nullptr));

	Point2i _3pts[3];
	double d3Dist[3] = {0, 0, 0};

	const int nPtSum = ptSrc->size();
	const int nLoops = nPtSum * nInters;

	for (int i = 0; i < nLoops; i++)
	{
LABEL_RE_FIND:

		// 随机取3个点
		for (int j = 0; j < 3; j++)
		{
			const int nPos = rng.uniform(0, nPtSum - 1);

			_3pts[j] = ptSrc->at(nPos);
		}

		// 计算3个点这间的距离
		d3Dist[0] = (_3pts[0].x - _3pts[1].x) * (_3pts[0].x - _3pts[1].x) + (_3pts[0].y - _3pts[1].y) * (_3pts[0].y - _3pts[1].y);
		d3Dist[1] = (_3pts[1].x - _3pts[2].x) * (_3pts[1].x - _3pts[2].x) + (_3pts[1].y - _3pts[2].y) * (_3pts[1].y - _3pts[2].y);
		d3Dist[2] = (_3pts[2].x - _3pts[0].x) * (_3pts[2].x - _3pts[0].x) + (_3pts[2].y - _3pts[0].y) * (_3pts[2].y - _3pts[0].y);

		// 		d3Dist[0] = fabs((double)_3pts[0].x - _3pts[1].x) + fabs((double)_3pts[0].y - _3pts[1].y);
		// 		d3Dist[1] = fabs((double)_3pts[1].x - _3pts[2].x) + fabs((double)_3pts[1].y - _3pts[2].y);
		// 		d3Dist[2] = fabs((double)_3pts[2].x - _3pts[0].x) + fabs((double)_3pts[2].y - _3pts[0].y);

		// 如果其中两个点的距离 < 16, 则重新找3个点, 因为距离太近后, 计算的圆误差太大

		if (d3Dist[0] < 16 ||
			d3Dist[1] < 16 ||
			d3Dist[2] < 16)
		{
			goto LABEL_RE_FIND;
		}

		// 如果其中两个点的距离大于了最大直径, 则重新找3个点
		if (d3Dist[0] > dDiameter2 ||
			d3Dist[1] > dDiameter2 ||
			d3Dist[2] > dDiameter2)
		{
			goto LABEL_RE_FIND;
		}

		// 开始计算拟合圆所需参数
		const double A = _3pts[0].x - _3pts[1].x;
		const double B = _3pts[0].y - _3pts[1].y;
		const double C = _3pts[0].x - _3pts[2].x;
		const double D = _3pts[0].y - _3pts[2].y;

		const double dDet = B * C - A * D;

		if (abs(dDet) < 0.00001)
		{
			goto LABEL_RE_FIND;
		}

		const double dX02 = _3pts[0].x * _3pts[0].x;
		const double dY02 = _3pts[0].y * _3pts[0].y;

		const double E = (dX02 - _3pts[1].x * _3pts[1].x + dY02 - _3pts[1].y * _3pts[1].y) * 0.5;
		const double F = (dX02 - _3pts[2].x * _3pts[2].x + dY02 - _3pts[2].y * _3pts[2].y) * 0.5;

		const Point2d ptFind = Point2d((B * F - D * E) / dDet, (C * E - A * F) / dDet);

		const double dRFind2 = (_3pts[0].x - ptFind.x) * (_3pts[0].x - ptFind.x) +
			(_3pts[0].y - ptFind.y) * (_3pts[0].y - ptFind.y);

		if (dRFind2 < dRMin2 || dRFind2 > dRMax2)
		{
			goto LABEL_RE_FIND;
		}

		// 找出3个点中, 最小距离与最大距离
		double dMinDis = d3Dist[0];
		double dMaxDis = d3Dist[0];

		for (int j = 1; j < 3; j++)
		{
			if (dMinDis > d3Dist[j])
			{
				dMinDis = d3Dist[j];
			}

			if (dMaxDis < d3Dist[j])
			{
				dMaxDis = d3Dist[j];
			}
		}

		const double dRFind = sqrt(dRFind2);
		const double dRate = sqrt(dMinDis / dMaxDis);	

		BOOL bFnd = FALSE;
		const int nCircles = vProbCirs.size();

		for (int j = 0; j < nCircles; j++)
		{
			FIT_CIRCLE_PARAM *pCurCircle = &vProbCirs[j];

			const double dOfstX = fabs(pCurCircle->x - ptFind.x);
			const double dOfstY = fabs(pCurCircle->y - ptFind.y);
			const double dOfxtR = fabs(pCurCircle->r - dRFind);

			double dOriLimt = dRadius * 0.1;

			if (dOriLimt < dMaxOfst)
			{
				dOriLimt = dMaxOfst;
			}

			if (dOfstX + dOfstY + dOfxtR < dOriLimt)
			{
				bFnd = TRUE;

				const double dTemp = 1 - dRate;

				pCurCircle->x = pCurCircle->x * dTemp + ptFind.x * dRate;
				pCurCircle->y = pCurCircle->y * dTemp + ptFind.y * dRate;
				pCurCircle->r = pCurCircle->r * dTemp + dRFind * dRate;

				pCurCircle->vpt.push_back(Point2d(_3pts[0].x, _3pts[0].y));
				pCurCircle->vpt.push_back(Point2d(_3pts[1].x, _3pts[1].y));
				pCurCircle->vpt.push_back(Point2d(_3pts[2].x, _3pts[2].y));

				break;
			}
		}

		if (!bFnd)
		{
			vector<Point2d> vpt;

			vpt.push_back(Point2d(_3pts[0].x, _3pts[0].y));
			vpt.push_back(Point2d(_3pts[1].x, _3pts[1].y));
			vpt.push_back(Point2d(_3pts[2].x, _3pts[2].y));

			const FIT_CIRCLE_PARAM FitParam = {ptFind.x, ptFind.y, dRFind, vpt};

			vProbCirs.push_back(FitParam);
		}
	}

	// 从点大到小排序
	sort(vProbCirs.begin(), vProbCirs.end(), _CompSizeB2S);

	vector<Point3d> vRetCirles;

	int nReorderSize = vProbCirs.size();

	for (int i = 0; i < nReorderSize; i++)
	{
		if (vProbCirs[i].vpt.size() < dLimit * 0.1)
		{
			continue;
		}

		const Point3d ptFit = _FitCircle(&vProbCirs[i].vpt);

		BOOL bPush = TRUE;

		const int nValidSize = vRetCirles.size();

		for (int j = 0; j < nValidSize; j++)
		{
			double dX2 = vRetCirles[j].x - ptFit.x;
			dX2 *= dX2;

			double dY2 = vRetCirles[j].y - ptFit.y;
			dY2 *= dY2;

			if (dX2 + dY2 < dRadius * dRadius)
			{
				bPush = FALSE;
				break;
			}
		}

		if (!bPush)
		{
			continue;
		}

		bPush = FALSE;

		int nPtOnCircle = 0;

		for (int j = 0; j < nPtSum; j++)
		{
			double dOfstX = ptFit.x - ptSrc->at(j).x;
			dOfstX *= dOfstX;

			double dOfstY = ptFit.y - ptSrc->at(j).y;
			dOfstY *= dOfstY;

			const double dR2 = dOfstX + dOfstY;

			if (dR2 > dRMin2 && dR2 < dRMax2)
			{
				nPtOnCircle++;
			}

			if (nPtOnCircle > dLimit)
			{
				bPush = TRUE;
				break;
			}
		}

		if (bPush)
		{
			vRetCirles.push_back(ptFit);
		}

		if ((int)vRetCirles.size() >= nWant)
		{
			break;
		}
	}

#ifdef _DEBUG
	const double dEnd = ((double)getTickCount() - dStart) * 1000 / getTickFrequency();
#endif // DEBUG	

	return vRetCirles;
}


vector<Point3d> GetFitCircle(vector<Point2d> *ptSrc,
							 double dRadius, double dScaleMin, double dScaleMax,
							 double dMaxOfst, int nInters, double dScore, int nWant)
{
#ifdef _DEBUG
	const double dStart = (double)getTickCount();
#endif // DEBUG	

	// 设定极限参数, 都是平方
	const double dR2 = dRadius * dRadius;
	const double dRMin2 = dR2 * dScaleMin * dScaleMin;
	const double dRMax2 = dR2 * dScaleMax * dScaleMax;
	const double dDiameter2 = dRMax2 * 4.0;

	const double dLimit = CV_PI * dRadius * 2.0 * dScore;

	if (ptSrc->size() < dLimit)
	{
#ifdef _DEBUG
		const double dEnd = ((double)getTickCount() - dStart) * 1000 / getTickFrequency();
#endif // DEBUG	

		vector<Point3d> vFail;

		return vFail;
	}

	vector<FIT_CIRCLE_PARAM> vProbCirs;

	RNG rng(time(nullptr));

	Point2d _3pts[3];
	double d3Dist[3] = {0, 0, 0};

	const int nPtSum = ptSrc->size();
	const int nLoops = nPtSum * nInters;

	for (int i = 0; i < nLoops; i++)
	{
LABEL_RE_FIND:

		// 随机取3个点
		for (int j = 0; j < 3; j++)
		{
			const int nPos = rng.uniform(0, nPtSum - 1);

			_3pts[j] = ptSrc->at(nPos);
		}

		// 计算3个点这间的距离
		d3Dist[0] = (_3pts[0].x - _3pts[1].x) * (_3pts[0].x - _3pts[1].x) + (_3pts[0].y - _3pts[1].y) * (_3pts[0].y - _3pts[1].y);
		d3Dist[1] = (_3pts[1].x - _3pts[2].x) * (_3pts[1].x - _3pts[2].x) + (_3pts[1].y - _3pts[2].y) * (_3pts[1].y - _3pts[2].y);
		d3Dist[2] = (_3pts[2].x - _3pts[0].x) * (_3pts[2].x - _3pts[0].x) + (_3pts[2].y - _3pts[0].y) * (_3pts[2].y - _3pts[0].y);

// 		d3Dist[0] = fabs((double)_3pts[0].x - _3pts[1].x) + fabs((double)_3pts[0].y - _3pts[1].y);
// 		d3Dist[1] = fabs((double)_3pts[1].x - _3pts[2].x) + fabs((double)_3pts[1].y - _3pts[2].y);
// 		d3Dist[2] = fabs((double)_3pts[2].x - _3pts[0].x) + fabs((double)_3pts[2].y - _3pts[0].y);

		// 如果其中两个点的距离 < 16, 则重新找3个点, 因为距离太近后, 计算的圆误差太大

		if (d3Dist[0] < 16 ||
			d3Dist[1] < 16 ||
			d3Dist[2] < 16)
		{
			goto LABEL_RE_FIND;
		}

		// 如果其中两个点的距离大于了最大直径, 则重新找3个点
		if (d3Dist[0] > dDiameter2 ||
			d3Dist[1] > dDiameter2 ||
			d3Dist[2] > dDiameter2)
		{
			goto LABEL_RE_FIND;
		}

		// 开始计算拟合圆所需参数
		const double A = _3pts[0].x - _3pts[1].x;
		const double B = _3pts[0].y - _3pts[1].y;
		const double C = _3pts[0].x - _3pts[2].x;
		const double D = _3pts[0].y - _3pts[2].y;

		const double dDet = B * C - A * D;

		if (abs(dDet) < 0.0001)
		{
			goto LABEL_RE_FIND;
		}

		const double dX02 = _3pts[0].x * _3pts[0].x;
		const double dY02 = _3pts[0].y * _3pts[0].y;

		const double E = (dX02 - _3pts[1].x * _3pts[1].x + dY02 - _3pts[1].y * _3pts[1].y) * 0.5;
		const double F = (dX02 - _3pts[2].x * _3pts[2].x + dY02 - _3pts[2].y * _3pts[2].y) * 0.5;

		const Point2d ptFind = Point2d((B * F - D * E) / dDet, (C * E - A * F) / dDet);

		const double dRFind2 = (_3pts[0].x - ptFind.x) * (_3pts[0].x - ptFind.x) +
			(_3pts[0].y - ptFind.y) * (_3pts[0].y - ptFind.y);


		if (dRFind2 < dRMin2 || dRFind2 > dRMax2)
		{
// 			CString strPos;
// 			strPos.Format(_T("i = %d, R = %f"), i, dRFind2);
// 
// 			AfxMessageBox(strPos);

			goto LABEL_RE_FIND;
		}

		// 找出3个点中, 最小距离与最大距离
		double dMinDis = d3Dist[0];
		double dMaxDis = d3Dist[0];

		for (int j = 1; j < 3; j++)
		{
			if (dMinDis > d3Dist[j])
			{
				dMinDis = d3Dist[j];
			}

			if (dMaxDis < d3Dist[j])
			{
				dMaxDis = d3Dist[j];
			}
		}

		const double dRFind = sqrt(dRFind2);
		const double dRate = sqrt(dMinDis / dMaxDis);	

		BOOL bFnd = FALSE;
		const int nCircles = vProbCirs.size();

		for (int j = 0; j < nCircles; j++)
		{
			FIT_CIRCLE_PARAM *pCurCircle = &vProbCirs[j];

			const double dOfstX = fabs(pCurCircle->x - ptFind.x);
			const double dOfstY = fabs(pCurCircle->y - ptFind.y);
			const double dOfxtR = fabs(pCurCircle->r - dRFind);

			double dOriLimt = dRadius * 0.1;

			if (dOriLimt < dMaxOfst)
			{
				dOriLimt = dMaxOfst;
			}

			if (dOfstX + dOfstY + dOfxtR < dOriLimt)
			{
				bFnd = TRUE;

				const double dTemp = 1 - dRate;

				pCurCircle->x = pCurCircle->x * dTemp + ptFind.x * dRate;
				pCurCircle->y = pCurCircle->y * dTemp + ptFind.y * dRate;
				pCurCircle->r = pCurCircle->r * dTemp + dRFind * dRate;

				pCurCircle->vpt.push_back(Point2d(_3pts[0].x, _3pts[0].y));
				pCurCircle->vpt.push_back(Point2d(_3pts[1].x, _3pts[1].y));
				pCurCircle->vpt.push_back(Point2d(_3pts[2].x, _3pts[2].y));

				break;
			}
		}

		if (!bFnd)
		{
			vector<Point2d> vpt;

			vpt.push_back(Point2d(_3pts[0].x, _3pts[0].y));
			vpt.push_back(Point2d(_3pts[1].x, _3pts[1].y));
			vpt.push_back(Point2d(_3pts[2].x, _3pts[2].y));

			const FIT_CIRCLE_PARAM FitParam = {ptFind.x, ptFind.y, dRFind, vpt};

			vProbCirs.push_back(FitParam);
		}
	}

	// 从点大到小排序
	sort(vProbCirs.begin(), vProbCirs.end(), _CompSizeB2S);

	vector<Point3d> vRetCirles;

	int nReorderSize = vProbCirs.size();

	for (int i = 0; i < nReorderSize; i++)
	{
		if (vProbCirs[i].vpt.size() < dLimit * 0.1)
		{
			continue;
		}

		const Point3d ptFit = _FitCircle(&vProbCirs[i].vpt);

		BOOL bPush = TRUE;

		const int nValidSize = vRetCirles.size();

		for (int j = 0; j < nValidSize; j++)
		{
			double dX2 = vRetCirles[j].x - ptFit.x;
			dX2 *= dX2;

			double dY2 = vRetCirles[j].y - ptFit.y;
			dY2 *= dY2;

			if (dX2 + dY2 < dRadius * dRadius)
			{
				bPush = FALSE;
				break;
			}
		}

		if (!bPush)
		{
			continue;
		}

		bPush = FALSE;

		int nPtOnCircle = 0;

		for (int j = 0; j < nPtSum; j++)
		{
			double dOfstX = ptFit.x - ptSrc->at(j).x;
			dOfstX *= dOfstX;

			double dOfstY = ptFit.y - ptSrc->at(j).y;
			dOfstY *= dOfstY;

			const double dR2 = dOfstX + dOfstY;

			if (dR2 > dRMin2 && dR2 < dRMax2)
			{
				nPtOnCircle++;
			}

			if (nPtOnCircle > dLimit)
			{
				bPush = TRUE;
				break;
			}
		}

		if (bPush)
		{
			vRetCirles.push_back(ptFit);
		}

		if ((int)vRetCirles.size() >= nWant)
		{
			break;
		}
	}

#ifdef _DEBUG
	const double dEnd = ((double)getTickCount() - dStart) * 1000 / getTickFrequency();
#endif // DEBUG	

	return vRetCirles;
}


// 两直线交点坐标
Point2d Intersection(const FIT_LINE L1, const FIT_LINE L2)
{
	Point2d ptDst;

	if (fabs(L2.k - L1.k) < 1E-6)
	{
		ptDst.x = -MAXINT;
		ptDst.y = -MAXINT;
	}
	else
	{
		ptDst.x = (L1.b - L2.b) / (L2.k - L1.k);
		ptDst.y = L1.k * ptDst.x + L1.b;
	}

	return ptDst;
}


// 圆拟合
vector<Point3d> GetFitCircle(CSmartImage * pSrc, int nCannyThres,
							 double dRadius, double dScaleMin, double dScaleMax,
							 double dMaxOfst, int nInters, double dScore, int nWant)
{
	ASSERT(1 == pSrc->channels());

	// 设定极限参数, 都是平方
	const double dR2 = dRadius * dRadius;
	const double dRMin2 = dR2 * dScaleMin * dScaleMin;
	const double dRMax2 = dR2 * dScaleMax * dScaleMax;
	const double dDiameter2 = dRMax2 * 4.0;

	const double dLimit = CV_PI * dRadius * 2.0 * dScore;

	// 可能为圆上的点
	vector<Point2i> vptCircle;

	CSmartImage ImgCanny;
	Canny(pSrc, &ImgCanny, nCannyThres, nCannyThres << 1, 3, 0, FALSE);

	EraseNoise(&ImgCanny, nullptr, 16, FALSE);

	const int nWidth = ImgCanny.Width();
	const int nHeight = ImgCanny.Height();

	for (int nH = 0; nH < nHeight; nH++)
	{
		const BYTE *pData = ImgCanny.ptr<BYTE>(nH);

		for (int nW = 0; nW < nWidth; nW++)
		{
			if (*(pData + nW))
			{
				vptCircle.push_back(Point2i(nW, nH));
			}
		}
	}

	// 随机打乱顺序
	random_shuffle(vptCircle.begin(), vptCircle.end());

	return GetFitCircle(&vptCircle, dRadius, dScaleMin, dScaleMax, dMaxOfst, nInters, dScore, nWant);;
}


// 功能: 亚像素边缘
BOOL SubPixelEdge(CSmartImage * pSrc, CSmartImage * pDst, CSmartImage * pSubPixelEdge,
				  int nThres1, int nThres2, BOOL bShow/* = FALSE*/, const CString & strWnd/* = _T("SubPixelEdge")*/)
{
	CSmartImage ImgSrc;

	if (pSrc->channels() > 1)
	{
		Convert2Gray(pSrc, &ImgSrc);
		pSrc = &ImgSrc;
	}

	CSmartImage *pImgDst = _GetDstImg(pSrc, pDst);

	if (nThres1 > nThres2)
	{
		std::swap(nThres1, nThres2);
	}

	Canny(pSrc, pImgDst, nThres1, nThres2, 3, FALSE);

	CSmartImage ImgDirs(pImgDst);
	CSmartImage ImgGrad(pSrc->Width(), pSrc->Height(), CV_32SC1);

	const int nStart = 1;
	const int nWidth = pSrc->Width() - 1;
	const int nHeight = pSrc->Height() - 1;

	int nSub[9] = {0};

	// 边缘方向
	for (int h = nStart; h < nHeight; h++)
	{
		const BYTE *pEdge8 = pImgDst->ptr<BYTE>(h);

		const BYTE *pData_1 = pSrc->ptr<BYTE>(h - 1);
		const BYTE *pData_2 = pSrc->ptr<BYTE>(h);
		const BYTE *pData_3 = pSrc->ptr<BYTE>(h + 1);

		BYTE *pDir = ImgDirs.ptr<BYTE>(h);
		int *pGrad = ImgGrad.ptr<int>(h);

		for (int w = nStart; w < nWidth; w++)
		{
			if (pEdge8[w])
			{
				nSub[0] = pData_1[w - 1];
				nSub[1] = pData_1[w];
				nSub[2] = pData_1[w + 1];
				nSub[3] = pData_2[w - 1];
				nSub[4] = pData_2[w];
				nSub[5] = pData_2[w + 1];
				nSub[6] = pData_3[w - 1];
				nSub[7] = pData_3[w];
				nSub[8] = pData_3[w + 1];

				int nSum1 = nSub[0] + (nSub[3] << 1) + nSub[6];
				int nSum2 = nSub[2] + (nSub[5] << 1) + nSub[8];

				int nDir = 1;	// 与横向夹角为0度(横向方向为由左指向右)
				int nMax = abs(nSum2 - nSum1);

				nSum1 = (nSub[0] << 1) + nSub[1] + nSub[3];
				nSum2 = nSub[5] + nSub[7] + (nSub[8] << 1);

				int nTmp = abs(nSum2 - nSum1);

				if (nTmp > nMax)
				{
					nMax = nTmp;
					nDir = 2;	// 与横向夹角为－45度(横向方向为由左指向右，逆时针为正)
				}

				nSum1 = nSub[0] + (nSub[1] << 1) + nSub[2];
				nSum2 = nSub[6] + (nSub[7] << 1) + nSub[8];

				nTmp = abs(nSum2 - nSum1);

				if (nTmp > nMax)
				{
					nMax = nTmp;
					nDir = 3;	// 与横向夹角为－90度(即与纵轴同向)(横向方向为由左指向右，逆时针为正)
				}

				nSum1 = nSub[1] + (nSub[2] << 1) + nSub[5];
				nSum2 = nSub[3] + (nSub[6] << 1) + nSub[7];

				nTmp = abs(nSum2 - nSum1);

				if (nTmp > nMax)
				{
					nMax = nTmp;
					nDir = 4;	// 与横向夹角为－135度(即与纵轴同向)(横向方向为由左指向右，逆时针为正)
				}

				pDir[w] = nDir;
				pGrad[w] = nMax;
			}
		}
	}

	CSmartImage ImgGradTmp;
	ImgGradTmp.Clone(&ImgGrad);

	// 非极大值抑制
	for (int h = nStart; h < nHeight; h++)
	{
		const BYTE *pEdge8 = pImgDst->ptr<BYTE>(h);

		const int *pData_1 = ImgGradTmp.ptr<int>(h - 1);
		const int *pData_2 = ImgGradTmp.ptr<int>(h);
		const int *pData_3 = ImgGradTmp.ptr<int>(h + 1);

		const BYTE *pDir = ImgDirs.ptr<BYTE>(h);

		int *pWrite = ImgGrad.ptr<int>(h);

		for (int w = nStart; w < nWidth; w++)
		{
			if (pEdge8[w])
			{
				BOOL bMaxDir = FALSE;

				const int & nMax = pData_2[w];

				switch (pDir[w])	// 判断方向
				{
				case 1:
					bMaxDir = (nMax > pData_2[w - 1]) && (nMax > pData_2[w + 1]);
					break;

				case 2:
					bMaxDir = (nMax > pData_1[w - 1]) && (nMax > pData_3[w + 1]);
					break;

				case 3:
					bMaxDir = (nMax > pData_1[w]) && (nMax > pData_3[w]);
					break;

				case 4:
					bMaxDir = (nMax > pData_1[w + 1]) && (nMax > pData_3[w - 1]);
					break;

				default:
					break;
				}

				if (!bMaxDir)
				{
					pWrite[w] = 0;
				}
			}
		}
	}

	if (pSubPixelEdge->empty())
	{
		pSubPixelEdge->ReCreate(pSrc->Width(), pSrc->Height(), CV_64FC2);
	}

	for (int h = nStart; h < nHeight; h++)
	{
		const int *pGrad_1 = ImgGrad.ptr<int>(h - 1);
		const int *pGrad_2 = ImgGrad.ptr<int>(h);
		const int *pGrad_3 = ImgGrad.ptr<int>(h + 1);

		const BYTE *pDir = ImgDirs.ptr<BYTE>(h);
		const BYTE *pEdge8 = pImgDst->ptr<BYTE>(h);

		double *pEdge64 = pSubPixelEdge->ptr<double>(h) + (nStart << 1);

		for (int w = nStart; w < nWidth; w++)
		{
			double dEdgeH = h;
			double dEdgeW = w;

			if (pEdge8[w])
			{
				int nG0 = 0;
				int nG1 = 0;
				int nG2 = 0;

				double dUp = 0;
				double dDown = 1;

				switch (pDir[w])
				{
				case 1:
					nG0 = pGrad_2[w - 1];
					nG1 = pGrad_2[w];
					nG2 = pGrad_2[w + 1];

					dUp = 0.5 * (nG0 - nG2);
					dDown = nG0 + nG2 - (nG1 << 1);

					dEdgeW = w + dUp / dDown;
					dEdgeH = h;

					break;

				case 2:
					nG0 = pGrad_1[w - 1];
					nG1 = pGrad_2[w];
					nG2 = pGrad_3[w + 1];

					dUp = 0.5 * (nG0 - nG2);
					dDown = nG0 + nG2 - (nG1 << 1);

					dEdgeW = w + dUp / dDown;
					dEdgeH = h + dUp / dDown;

					break;

				case 3:
					nG0 = pGrad_1[w];
					nG1 = pGrad_2[w];
					nG2 = pGrad_3[w];

					dUp = 0.5 * (nG0 - nG2);
					dDown = nG0 + nG2 - (nG1 << 1);

					dEdgeW = w;
					dEdgeH = h + dUp / dDown;

					break;

				case 4:
					nG0 = pGrad_1[w + 1];
					nG1 = pGrad_2[w];
					nG2 = pGrad_3[w - 1];

					dUp = 0.5 * (nG0 - nG2);
					dDown = nG0 + nG2 - (nG1 << 1);

					dEdgeW = w + dUp / dDown;
					dEdgeH = h + dUp / dDown;

					break;

				default:
					break;
				}
			}

			*pEdge64 = dEdgeW;
			pEdge64++;
			*pEdge64 = dEdgeH;
			pEdge64++;
		}
	}

	if (bShow)
	{
		pImgDst->Show(strWnd);
	}

	return TRUE;
}

// 取得目标图像指针
AFX_INLINE CSmartImage * _GetDstImg(CSmartImage * pSrc, CSmartImage * pDst/* = nullptr */, BOOL bInPlace/* = TRUE */)
{
	ASSERT(nullptr != pSrc && !pSrc->empty());

	if (nullptr == pSrc || pSrc->empty())
	{
		return nullptr;
	}

	if (bInPlace && (nullptr == pDst))
	{
		// TRACE(_T("_GetDstImg return pSrc!\n"));

		return pSrc;
	}
	else if (nullptr == pDst)
	{
		return nullptr;
	}
	else
	{
		ASSERT(nullptr != pDst);
		pDst->ReCreate(pSrc);

		// TRACE(_T("_GetDstImg return pDst!\n"));

		return pDst;
	}
}
