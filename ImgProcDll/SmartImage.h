#pragma once

#include "..\StaticCtrlDll\SmartDC.h"

#include <opencv2\opencv.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\features2d.hpp>


#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\StaticCtrlDll.lib")
#else
#pragma comment(lib, "..\\Release\\StaticCtrlDll.lib")
#endif


// using namespace std;
using namespace cv;

typedef cv::Rect				Rect2i;
typedef vector<vector<Point2i>> cvContours;

#define CV_CONT_IT				cvContours::const_iterator

#define CVT_MAT(Img)			(*(static_cast<Mat *>(Img)))
#define MAT_RGB(R, G, B)		Scalar(B, G, R)

#ifndef IS_ODD
#define IS_ODD(v)				((v) & 0x01)
#endif // !IS_ODD

#ifndef IS_EVENT
#define IS_EVENT(v)				(!IS_ODD(v))
#endif // !IS_ODD

typedef struct _tagROI
{
	Rect2i	rcROI;
	BOOL	bRoiSetted;
}IMG_ROI;

class AFX_EXT_CLASS CSmartImage : public Mat
{
public:
	CSmartImage(void);
	CSmartImage(Mat Img);
	CSmartImage(const CSmartImage * pSrc, Scalar InitVal = Scalar::all(0));
	CSmartImage(Size2i ImgSize, int nType = CV_8UC3, Scalar InitVal = Scalar::all(0));
	CSmartImage(int nWidth, int nHeight, int nType = CV_8UC3, Scalar InitVal = Scalar::all(0));
	CSmartImage(Size2i ImgSize, int nType, void * pData);
	CSmartImage(int nWidth, int nHeight, int nType, void * pData);
	~CSmartImage(void);

	AFX_INLINE void ReCreate(int nWidth, int nHeight, int nType = CV_8UC3);
	AFX_INLINE void ReCreate(Size2i ImgSize, int nType = CV_8UC3);
	AFX_INLINE void ReCreate(CSmartImage * pSrc);

	void Serialize(CArchive & ar);

	AFX_INLINE int Width(void);
	AFX_INLINE int HalfWidth(void);

	AFX_INLINE int Height(void);
	AFX_INLINE int HalfHeight(void);

	BOOL Open(int nSColor = CV_LOAD_IMAGE_UNCHANGED);
	BOOL Open(const CString & strPath, int nSColor = CV_LOAD_IMAGE_UNCHANGED);
	BOOL Save(void);
	BOOL Save(CString strPath);
	BOOL Save(CString strPath, const CString & strName);

	//	1. WINDOW_NORMAL	= 0x00000000	the user can resize the window (no constraint)  / also use to switch a fullscreen window to a normal size
	//	2. WINDOW_AUTOSIZE	= 0x00000001	the user cannot resize the window, the size is constrainted by the image displayed
	//	3. WINDOW_OPENGL    = 0x00001000	window with opengl support
	void Show(const CString & strWindow = _T("ShowImage"), int nFlag = WINDOW_NORMAL);

	CRect Draw2DC(CDC * pDC, CRect DstRect, BOOL bKeepScale = TRUE, BOOL bTopLeft = FALSE);
	void DrawImg2Ctrl(CDialog * pDlg, DWORD dwID,
		BOOL bKeepScale = TRUE, BOOL bTopLeft = FALSE,
		COLORREF rgbBack = RGB(64, 64, 64), COLORREF rgbBorder = RGB(0, 255, 255));

	AFX_INLINE Size2i GetImgSize(void);
	AFX_INLINE int DataSize(void);

	Mat SetROI(Rect2i rcROI);
	Mat SetROI(CRect rcROI);
	Mat SetROI(CSmartImage & ImgROI);
	Mat SetROI(int nX, int nY, int nWidth, int nHeight);
	IMG_ROI GetROI(void);
	void ClearROI(void);

	AFX_INLINE Point2d CenterPoint2d(void);
	AFX_INLINE Point2i CenterPoint2i(void);

	void EraseOutside(Rect2i rcBox);

	AFX_INLINE void Clone(Mat * pMat);
	AFX_INLINE void Clone(CSmartImage * pSrc);

	AFX_INLINE void Reverse(CSmartImage * pDst = nullptr);
	AFX_INLINE void Set2Zero(void);
	AFX_INLINE void SetValue(Scalar dValue);

	AFX_INLINE void operator = (Mat & Img);
	AFX_INLINE void operator = (CSmartImage & Img);
	AFX_INLINE BOOL operator ==(CSmartImage & Img);
	AFX_INLINE BOOL operator !=(CSmartImage & Img);
	AFX_INLINE Mat  operator + (CSmartImage & Img);
	AFX_INLINE void operator +=(CSmartImage & Img);
	AFX_INLINE Mat  operator - (CSmartImage & Img);
	AFX_INLINE void operator -=(CSmartImage & Img);

	AFX_INLINE Mat operator &(CSmartImage & Img);
	AFX_INLINE void operator &=(CSmartImage & Img);

protected:
	Rect2i	m_rcROI;
	BOOL	m_bRoiSetted;
	Mat		m_ImgROI;

	AFX_INLINE CRect _SetScale(CRect rcDst, BOOL bTopLeft = FALSE);
	AFX_INLINE void _FillBitmapInfo(BITMAPINFO * pBmi, int nWidth, int nHeight, int nChannels = 3);
};
