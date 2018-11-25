#pragma once

#include "SmartImage.h"
#include "KeyMatcher.h"
#include "Stitcher.h"

#define FLIP_X					0
#define FLIP_Y					1
#define FLIP_XY					-1

#ifndef ZERO					// 0
#define ZERO					(1E-6)
#endif

#define CHECK_SRC(strFuntion, pSrc)			if (nullptr == pSrc || pSrc->empty())\
											{\
												TRACE(strFuntion);\
												TRACE(_T(" strFuntion returned because Src is empty!\n"));\
												return;\
											}

#define CHECK_SAME(strFuntion, Src, Dst)	if (Src.data == Dst.data)\
											{\
												TRACE(strFuntion);\
												TRACE(_T(" returned because Src == Dst!\n"));\
												return;\
											}

typedef struct _tagRGB 
{
	BYTE B;									// ��ɫ����
	BYTE G;									// ��ɫ����
	BYTE R;									// ��ɫ����
}YX_RGB;

typedef struct _tagHSV
{
	double H;								// ɫ��
	double S;								// ���Ͷ�
	double V;								// ����
}YX_HSV;


typedef struct _tagSmooth
{
	int				nSmoothMode;			// �˲���ʽ
	int				nMaskWidth;				// ���ڿ��
	int				nMaskHeight;			// ���ڸ߶�
	double			dParam3;				// ��׼��
	double			dParam4;				// ����ֵ
}YX_SMOOTH_PARAM;

typedef struct _tagEnclosingCircle
{
	Point2d ptCenter;
	double dRadius;
	double dArea;
}YX_ENCLOSING_CIRCLE;

typedef struct _tagCircleParam
{
	double x;
	double y;
	double r;
	vector<Point2d> vpt;
}FIT_CIRCLE_PARAM;


#define IMG_DLL_EXPORT	_declspec(dllexport)

AFX_INLINE BOOL IMG_DLL_EXPORT IsEqual(YX_HSV hsv1, YX_HSV hsv2);
AFX_INLINE BOOL IMG_DLL_EXPORT IsEqual(YX_RGB rgb1, YX_RGB rgb2);
AFX_INLINE BOOL IMG_DLL_EXPORT IsEqual(Rect2i rc1, CRect rc2);
AFX_INLINE BOOL IMG_DLL_EXPORT IsEqual(CRect rc1, Rect2i rc2);
AFX_INLINE BOOL IMG_DLL_EXPORT IsEqual(Rect2i rc1, Rect2i rc2);

/*================================================================
����: ����RGB��ɫ
�������:
	1. R: ��ɫ����
	2. G: ��ɫ����
	3. B: ��ɫ����
����ֵ: YX_RGB�ṹ
================================================================*/
AFX_INLINE YX_RGB IMG_DLL_EXPORT SetRGB(BYTE R, BYTE G, BYTE B);
AFX_INLINE YX_RGB IMG_DLL_EXPORT SetRGB(COLORREF rgbColor);


/*================================================================
����: ����HSV��ɫ
�������:
	1. H: H����
	2. S: S����
	3. V: V����
����ֵ: YX_HSV�ṹ
================================================================*/
AFX_INLINE YX_HSV IMG_DLL_EXPORT SetHSV(double H, double S, double V);


/*================================================================
����: ��ͼ��Crect����ת����Rect2i����
�������:
	1. rect: CRect�����
����ֵ: Rect2i����
================================================================*/
AFX_INLINE Rect2i IMG_DLL_EXPORT CRect2Rect2i(CRect rect);


/*================================================================
����: ��ͼ��CvRct����ת����CRect����
�������:
	1. rect: Rect2i�����
����ֵ: CRect����
================================================================*/
AFX_INLINE CRect IMG_DLL_EXPORT Rect2i2CRect(Rect2i rect);


/*================================================================
����: ���ؼ����ھ���ת�����ͻ�������
�������:
	1. pDlg: �Ի���ָ��
	2. dwID: �ؼ�ID
	3. rcCtrl: �ؼ�����
����ֵ: ��
================================================================*/
AFX_INLINE void IMG_DLL_EXPORT Window2Client(CDialog * pDlg, DWORD dwID, CRect & rcCtrl);


/*================================================================
����: ���ͻ�������ת��Ϊͼ������
�������:
	1. Src: ͼ���������
	2. rcClinet: �ͻ�������
	3. ptClient: �ͻ�������
����ֵ: ͼ������
================================================================*/
AFX_INLINE CPoint IMG_DLL_EXPORT Client2Image(CSmartImage * pSrc, CRect rcClinet, CPoint ptClient);


/*================================================================
����: ���ͻ�������ת��Ϊͼ������
�������:
	1. Src: ͼ���������
	2. rcClinet: �ͻ�������
	3. rcDst: Ŀ�����
����ֵ: ��
================================================================*/
AFX_INLINE Rect2i IMG_DLL_EXPORT Client2Image(CSmartImage * pSrc, CRect rcClinet, CRect rcDst);


/*================================================================
����: ��ͼ������ת���ɴ�������
�������:
	1. Src: ͼ���������
	2. rcClinet: �ͻ�������
	3. ptImg: ͼ������
����ֵ: ��
================================================================*/
AFX_INLINE CPoint IMG_DLL_EXPORT Image2Client(CSmartImage * pSrc, CRect rcClinet, CPoint ptImg);


/*================================================================
����: ���ͻ�������ת��Ϊͼ������
�������:
	1. Src: ͼ���������
	2. rcClinet: �ͻ�������
	3. rcImg: Ŀ�����
����ֵ: ��
================================================================*/
AFX_INLINE CRect IMG_DLL_EXPORT Image2Client(CSmartImage * pSrc, CRect rcClinet, CRect rcImg);


/*================================================================
����: RGBת����HSV
�������:	
	1. rgbR: ��ɫ����
	2. rgbG: ��ɫ����
	3. rgbB: ��ɫ����
����ֵ: HSV��ɫ
================================================================*/
YX_HSV IMG_DLL_EXPORT RGB2HSV(BYTE rgbR, BYTE rgbG, BYTE rgbB);


/*================================================================
����: RGBת����HSV
�������:	
	1. rgbColor: RGB��ɫ
����ֵ: HSV��ɫ
================================================================*/
AFX_INLINE YX_HSV IMG_DLL_EXPORT RGB2HSV(YX_RGB rgbColor);


/*================================================================
����: RGBת����HSV
�������:	
	1. rgbColor: RGB��ɫ
����ֵ: HSV��ɫ
================================================================*/
AFX_INLINE YX_HSV IMG_DLL_EXPORT RGB2HSV(COLORREF rgbColor);


/*================================================================
����: RGBת����HSV
�������:	
	1. pRGB: RGB��ɫָ��
	2. pHSV: HSV��ɫָ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT RGB2HSV(YX_RGB * pRGB, YX_HSV * pHSV);


	/*================================================================
����: RGBת����HSV
�������:	
	1. pRGB: RGB��ɫָ��
	2. pHSV: HSV��ɫָ��
	3. nWidth: ͼ����
	4. nHeight: ͼ�����
����ֵ: ��
================================================================*/
AFX_INLINE void IMG_DLL_EXPORT RGB2HSV(YX_RGB * pRGB, YX_HSV * pHSV, int nWidth, int nHeight);


/*================================================================
����: �����Ե��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nLines: ������
	4. nDirSave: ��������
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
#define DIR_SAVE_TOP		1
#define DIR_SAVE_BOTTOM		2
#define DIR_SAVE_LEFT		4
#define DIR_SAVE_RIGHT		8

void IMG_DLL_EXPORT ClearOutline(CSmartImage * pSrc, CSmartImage * pDst = nullptr, int nLines = 1, int nDirSave = 0,
								 BOOL bShow = FALSE, const CString & strWnd = _T("ClearOutline"));


/*================================================================
����: ������Ե��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nLines: �������
	4. nDirClear: ȥ������
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
#define DIR_CLEAR_TOP		1
#define DIR_CLEAR_BOTTOM	2
#define DIR_CLEAR_LEFT		4
#define DIR_CLEAR_RIGHT		8

void IMG_DLL_EXPORT SaveOutline(CSmartImage * pSrc, CSmartImage * pDst = nullptr, int nLines = 1, int nDirClear = 0,
								BOOL bShow = FALSE, const CString & strWnd = _T("SaveOutline"));



/*================================================================
����: ȡ�ø�����ռ���
�������:	
	1. pSrc: Դͼ��ָ��
	2. nDir: ����
����ֵ: �ռ���
================================================================*/
#define DIR_TOP			1
#define DIR_BOTTOM		2
#define DIR_LEFT		4
#define DIR_RIGHT		8
int IMG_DLL_EXPORT GetClearWidth(CSmartImage * pSrc, int nDir);


/*================================================================
����: ȡ����С������
�������:	
	1. pSrc: Դͼ��ָ��
����ֵ: ������
================================================================*/
Rect2i IMG_DLL_EXPORT BoundingRectEx(CSmartImage * pSrc);


/*================================================================
����: ȡ��������
�������:	
	1. pSrc: Դͼ��ָ��
����ֵ: ��С������
================================================================*/
RotatedRect IMG_DLL_EXPORT AreaMinRectEx(CSmartImage * pSrc);


/*================================================================
����: ȡ�����Բ
�������:	
	1. pSrc: Դͼ��ָ��

����ֵ: Բ������뾶
================================================================*/
YX_ENCLOSING_CIRCLE IMG_DLL_EXPORT MinEnclosingCircleEx(CSmartImage * pSrc);


/*================================================================
����: ����ͨ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pRed: ��ɫͨ��ͼ��ָ��
	3. pGreen: ��ɫͨ��ͼ��ָ��
	4. pBlue: ��ɫͨ��ͼ��ָ��
	5. pAlpha: Alphaͨ��ͼ��ָ��
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: �������ڴ���ͼ���ָ��
================================================================*/
void IMG_DLL_EXPORT SplitImage(CSmartImage * pSrc,
							   CSmartImage * pRed = nullptr,
							   CSmartImage * pGreen = nullptr,
							   CSmartImage * pBlue = nullptr,
							   CSmartImage * pAlpha = nullptr,
							   BOOL bShow = FALSE);
						   
void IMG_DLL_EXPORT SplitImage(CSmartImage * pSrc, vector<Mat> * pPlanes);


/*================================================================
����: ȡ��HSVͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pH: Hueͼ��ָ��
	3. pS: Saturationͼ��ָ��
	4. pV: Valueͼ��ָ��
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT GetHSV(CSmartImage * pSrc,
						   CSmartImage * pH = nullptr,
						   CSmartImage * pS = nullptr,
						   CSmartImage * pV = nullptr,
						   BOOL bShow = FALSE);
					  
void IMG_DLL_EXPORT GetHSV(CSmartImage * pSrc, vector<Mat> * pHsv);


/*================================================================
����: ͨ���ϲ�
�������:	
	1. pSrc1: Դͼ1��ָ��
	2. pSrc2: Դͼ2��ָ��
	3. pSrc3: Դͼ3��ָ��
	5. pDst: Ŀ��ͼ��ָ��
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	7. strWnd: ��ʾ��������
����ֵ: �������ڴ���ͼ���ָ��
================================================================*/
void IMG_DLL_EXPORT Merge(CSmartImage * pRed,
						  CSmartImage * pGreen,
						  CSmartImage * pBlue,
						  CSmartImage * pDst,
						  BOOL bShow = FALSE,
						  const CString & strWnd = _T("Merge"));
					 
void IMG_DLL_EXPORT Merge(vector<Mat> * pPlanes, CSmartImage * pDst, BOOL bShow = FALSE, const CString & strWnd = _T("Merge"));


/*================================================================
����: ����ͼ����ɫ
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nDiv: ��������
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT ColorReduce(CSmartImage * pSrc, CSmartImage * pDst, int nDiv = 32, BOOL bShow = FALSE);


/*================================================================
����: ��תͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. ��ת��ʽ
		1. FLIP_X: x��
		2. FLIP_Y: y��
		3. FLIP_XY: xy��
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Flip(CSmartImage * pSrc, CSmartImage * pDst, int nFlipMode = FLIP_X,
						 BOOL bShow = FALSE, const CString & strWnd = _T("Flip"));



/*================================================================
����: ��ȡ������ͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: ������ͼ��ָ��
	3. rcSub: ���������
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT GetSubImage(CSmartImage * pSrc, CSmartImage * pDst, Rect2i rcSub, BOOL
								bShow = FALSE, const CString & strWnd = _T("GetSubImage"));
void IMG_DLL_EXPORT GetSubImage(CSmartImage * pSrc, CSmartImage * pDst, CRect rcSub,
								BOOL bShow = FALSE, const CString & strWnd = _T("GetSubImage"));


/*================================================================
����: ͼ��ƽ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. sp: ƽ������
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Smooth(CSmartImage * pSrc, CSmartImage * pDst, YX_SMOOTH_PARAM sp,
						   BOOL bShow = FALSE, const CString & strWnd = _T("Smooth"));


/*================================================================
����: ͼ����
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nNeighbourPoint: �ڵ�������
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Sharpen(CSmartImage * pSrc, CSmartImage *pDst = nullptr, int nNeighbourPoint = 4, BOOL bShow = FALSE);


/*================================================================
����: ���л����б任
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. pStruct1: �ṹԪ��1
	4. pStruct2: �ṹԪ��2
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT HitMiss(CSmartImage * pSrc, CSmartImage *pDst,
							Mat * pStruct1, Mat * pStruct2,
							BOOL bShow = FALSE);
void IMG_DLL_EXPORT HitMiss(CSmartImage * pSrc, CSmartImage *pDst,
							Mat * pTarget, BOOL bShow = FALSE);


/*================================================================
����: ��HSVͼ��ת����RGBͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	4. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Convert2Gray(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
								 BOOL bShow = FALSE, const CString & strWnd = _T("Convert2Gray"));


/*================================================================
����: ��RGBͼ��ת����HSVͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Convert2HSV(CSmartImage * pSrc, CSmartImage * pDst = nullptr, BOOL bShow = FALSE);


/*================================================================
����: �Ҷ�����
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. dMin: �Ҷ���������ֵ
	4. dMax: �Ҷ���������ֵ
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Stretch(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
							double dMin = 55.0, double dMax = 200.0,
							BOOL bShow = FALSE, const CString & strWnd = _T("Stretch"));


/*================================================================
����: ��ֵ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. dThreshold: ��ֵ
	4. dMaxValue: ���Ҷ�ֵ
	5. nType: �任����
		1. CV_THRESH_BINARY      = 0	value = value > threshold ? max_value : 0
		2. CV_THRESH_BINARY_INV  = 1	value = value > threshold ? 0 : max_value
		3. CV_THRESH_TRUNC       = 2	value = value > threshold ? threshold : value
		4. CV_THRESH_TOZERO      = 3	value = value > threshold ? value : 0
		5. CV_THRESH_TOZERO_INV  = 4	value = value > threshold ? 0 : value
		6. CV_THRESH_MASK        = 7
		7. CV_THRESH_OTSU        = 8	use Otsu algorithm to choose the optimal threshold value;
										combine the flag with one of the above CV_THRESH_* values
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	7. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Threshold(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
							  double dThreshold = 128.0, double dMaxValue = 255.0,
							  int nType = CV_THRESH_BINARY,
							  BOOL bShow = FALSE, const CString & strWnd = _T("Threshold"));


/*================================================================
����: ��̬ѧ����
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nType: �任����
		1. MOP_ERODE	= 0
		2. MOP_DILATE	= 1
		3. MOP_OPEN		= 2
		4. MOP_CLOSE	= 3
		5. MOP_GRADIENT	= 4
		6. MOP_TOPHAT	= 5
		7. MOP_BLACKHAT	= 6
		8. MOP_EDGE		= 7
	4. nIterations: ��������
	5. pKernel: �ṹԪ��
	6. ptAnchaor: �ṹԪ��ê��
	7. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	8. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
#define YX_MOP_EDGE	7
void IMG_DLL_EXPORT Morphology(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
							   int nOperation = CV_MOP_OPEN,
							   int nIterations = 1,
							   Mat * pKernel = nullptr,
							   Point2i ptAnchaor = Point2i(-1, -1),
							   BOOL bShow = FALSE, const CString & strWnd = _T("Morphology"));


/*================================================================
����: ��ʴ
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nIterations: ��������
	4. pKernel: �ṹԪ��
	5. ptAnchaor: �ṹԪ��ê��
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	7. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Erode(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
						  int nIterations = 1,
						  Mat * pKernel = nullptr,
						  Point2i ptAnchaor = Point2i(-1, -1),
						  BOOL bShow = FALSE, const CString & strWnd = _T("Erode"));


/*================================================================
����: ����
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nIterations: ��������
	4. pKernel: �ṹԪ��
	5. ptAnchaor: �ṹԪ��ê��
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	7. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Dilate(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
						   int nIterations = 1,
						   Mat * pKernel = nullptr,
						   Point2i ptAnchaor = Point2i(-1, -1),
						   BOOL bShow = FALSE, const CString & strWnd = _T("Dilate"));


/*================================================================
����: ��������
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. nIterations: ��������
	4. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT LimitDilate(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
								double dThreLow = 55.0 , double dThresHigh = 200.0, BOOL bShow = FALSE);


/*================================================================
����: ���ָ����ɫ
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. hsvColor: ��Ҫ��ǵ���ɫ
	4. hsvTolerance: �����ɫ�ݲ�
	5. rgbMark: �����ɫ
	6. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT MarkSpecialColor(CSmartImage * pSrc, CSmartImage * pDst, YX_HSV hsvColor, YX_HSV hsvTolerance,
									 COLORREF rgbMark, BOOL bShow = FALSE);
void IMG_DLL_EXPORT MarkSpecialColor(CSmartImage * pSrc, CSmartImage * pDst, YX_HSV hsvColor, YX_HSV hsvTolerLow, YX_HSV hsvTolerHigh,
									 COLORREF rgbMark, BOOL bShow = FALSE);


/*================================================================
����: ͼ������
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. ImgSize: Ŀ��ͼ���С
	4. nInterpolation: �岹��ʽ
		1. CV_INTER_NN        = 0
		2. CV_INTER_LINEAR    = 1
		3. CV_INTER_CUBIC     = 2
		4. CV_INTER_AREA      = 3
		5. CV_INTER_LANCZOS4  = 4
	5. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Resize(CSmartImage * pSrc, CSmartImage * pDst, Size2i ImgSize,
						   int nInterpolation = CV_INTER_LINEAR, BOOL bShow = FALSE, const CString & strWnd = _T("Resize"));
void IMG_DLL_EXPORT Resize(CSmartImage * pSrc, CSmartImage * pDst, int nWidth, int nHeight,
						   int nInterpolation = CV_INTER_LINEAR, BOOL bShow = FALSE, const CString & strWnd = _T("Resize"));

/*================================================================
����: Canny��Ե���
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. dThresMin: ����ֵ
	4. dThresMax: ����ֵ
	5. ApertureSize:���ṹ��С
	6. L2gradient: �ݶȷ�ֵ�ı�ʶ
	7. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Canny(CSmartImage * pSrc, CSmartImage * pDst = nullptr,
						  double dThresMin = 55.0, double dThresMax = 200.0,
						  int ApertureSize = 3, BOOL bL2gradient = FALSE, BOOL bShow = FALSE);


/*================================================================
����: �����ӵ�
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. dSize: �������
	4. bShow: �Ƿ�Ҫ��ʾ������ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ���һ��ƥ�����
================================================================*/
void IMG_DLL_EXPORT EraseNoise(CSmartImage * pSrc, CSmartImage * pDst, double dSize, BOOL bShow = FALSE, const CString & strWnd = _T("EraseNoise"));


/*================================================================
����: ��״ƥ��
�������:	 ��
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
����ֵ: ƥ������ƶȷ���, ���ֵΪ1
================================================================*/
double IMG_DLL_EXPORT MatchShape(CSmartImage * pSrc, CSmartImage * pDst);


/*================================================================
����: ģ��ƥ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pTemplate: ģ��ͼ��ָ��
	3. nMethod:��ƥ�䷽ʽ
		1. CV_TM_SQDIFF			=��0
		2. CV_TM_SQDIFF_NORMED	=��1
		3. CV_TM_CCORR			=��2
		4. CV_TM_CCORR_NORMED	=��3
		5. CV_TM_CCOEFF			=��4
		6. CV_TM_CCOEFF_NORMED	=��5
	4. prcLimit:��������Ѱ�������ָ��
	5. ptVectorTopLeft: ƥ���������Ͻ���������ָ��
	5. ptVectorBottomRight: ƥ���������Ͻ���������ָ��
	6. nWanted: ƥ������
	7. dThreshold: ƥ����ֵ
	8. bResize: �Ƿ���Ҫ����
	9. bMarkPos: �Ƿ�Ҫ��ԭͼ���ϱ��ƥ���
	10. bShow: �Ƿ�Ҫ��ʾ������ͼ��
	11. strWnd: ��ʾ��������
����ֵ: ���һ��ƥ�����
================================================================*/
double IMG_DLL_EXPORT MatchTemplate(CSmartImage * pSrc, CSmartImage * pTemplate,
									int nMethod = CV_TM_CCORR_NORMED,
									const Rect2i * prcLimit = nullptr,
									vector<Point2i> * ptvTL = nullptr, vector<Point2i> * ptvBR = nullptr,
									int nWanted = 0, double dThreshold = 0.7,
									int nResize = 4,
									BOOL bMarkPos = TRUE, Scalar scMarkColor = Scalar::all(255),
									BOOL bShow = FALSE, const CString & strWnd = _T("MatchTemplate"));


/*================================================================
����: ��ֵģ��ƥ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pTemplate: ģ��ͼ��ָ��
	3. prcLimit:��������Ѱ�������ָ��
	4. nNumber: ƥ������
	5. nThesTemp: ģ����ֵ�任��ֵ
	6. nThreLow: ģ����ֵ�任��ֵ����
	7. nThresHigh: ģ����ֵ�任��ֵ����
	8. pThres: ƥ���������ָ��
	9. nStretchLow: �Ҷ���������ֵ
	10. nStretchHigh: �Ҷ���������ֵ
	11. nMethod:��ƥ�䷽ʽ
		1. CV_TM_SQDIFF			=��0
		2. CV_TM_SQDIFF_NORMED	=��1
		3. CV_TM_CCORR			=��2
		4. CV_TM_CCORR_NORMED	=��3
		5. CV_TM_CCOEFF			=��4
		6. CV_TM_CCOEFF_NORMED	=��5
	
	12. ptVectorTopLeft: ƥ���������Ͻ���������ָ��
	13. ptVectorBottomRight: ƥ���������Ͻ���������ָ��

	14. dStopThres: ƥ�������ֵ
	15. bResize: �Ƿ���Ҫ����
	16. bMarkPos: �Ƿ�Ҫ��ԭͼ���ϱ��ƥ���
	17. bShowResultWindow: �Ƿ�Ҫ���´�������ʾƥ����ͼ��
����ֵ: ���һ��ƥ�����
================================================================*/
double IMG_DLL_EXPORT BinaryMatch(CSmartImage * pSrc, CSmartImage * pTemplate,
								  CRect * prcLimit = nullptr,
								  int nNumber = 3,
								  double dThesTemp = 50.0, double dThreLow = 40.0, double dThresHigh = 60.0,
								  vector<double> * pThres = nullptr,
								  double dStretchLow = 20.0, double dStretchHigh = 100.0,
								  int nMethod = CV_TM_CCORR_NORMED,
								  vector<Point2i> * ptVectorTopLeft = nullptr, vector<Point2i> * ptVectorBottomRight = nullptr,
								  double dStopThres = 0.95,
								  BOOL bResize = TRUE,
								  BOOL bMarkPos = TRUE,
								  BOOL bShowResultWindow = FALSE);


/*================================================================
����: KeyMatch
�������:	
	1. pObject: Ŀ��ͼ��ָ��
	2. pScene: ����ͼ��ָ��
	3. dHessianThreshold: Hessian��ֵ
	4. nOctaves: ����������
	5. nOctaveLayers: ÿ��Layers��
	6. bShowResult: �Ƿ���ʾ���
����ֵ: ����ƫת�Ƕ�: û��ƥ�� = -1;
================================================================*/
double IMG_DLL_EXPORT Surf(CSmartImage * pObject, CSmartImage * pScene,
						   double dHessianThreshold = 4, int nOctaves = 16, int nOctaveLayers = 8, double dFactor = 2,
						   BOOL bShowResult = TRUE);

double IMG_DLL_EXPORT KeyMatch(CKeyMatcher * pObj, CSmartImage * pScene, BOOL bShowResult = TRUE);


/*================================================================
����: ��ˮ���
�������:	
	1. pSrc: Դͼ��ָ��
	2. ptSeed: ����λ��
	3. PixelValue: ������ɫ
	4. LowDiff: �ݲ�����
	5. UpDiff: �ݲ�����
	6. nFlags: �����
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT FloodFill(CSmartImage * pImage, Point2i ptSeed, Scalar PixelValue,
							  Scalar LowDiff, Scalar UpDiff,
							  int nFlags);


/*================================================================
����: ��תͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. ptCenter: ��ת��������
	4. dDegree: ��ת�Ƕ�, ��λΪ��
	5. dScale: ���ű���
	6. nInterMode: ��ֵ��ʽ
	7. bShow: �Ƿ���ʾ�����ͼ��
	8. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT ImgRotation(CSmartImage * pSrc, CSmartImage * pDst, Point2d ptCenter,
								double dDegree, double dScale = 1.0,
								int nInterMode = CV_INTER_CUBIC | CV_WARP_FILL_OUTLIERS,
								BOOL bShow = FALSE, const CString & strWnd = _T("ImgRotation"));
void IMG_DLL_EXPORT ImgRotationEx(CSmartImage * pSrc, CSmartImage * pDst, Point2d ptCenter,
								  double dDegree, double dScale = 1.0,
								  BOOL bShow = FALSE, const CString & strWnd = _T("ImgRotationEx"));


/*================================================================
����: ��ͷУ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. dCoeffs: У��ϵ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT LensCalibration(CSmartImage * pSrc, CSmartImage * pDst, double dCoeffs = 0.0);


/*================================================================
����: ȡ����������
�������:	
	1. pSrc: Դͼ��ָ��
	2. bDraw: �Ƿ�Ҫ��ǳ�����
����ֵ: ������������
================================================================*/
Point2d IMG_DLL_EXPORT GravityCenter(CSmartImage * pSrc, BOOL bDraw = FALSE);
Point2d IMG_DLL_EXPORT GravityCenter(const vector<Point2i> & vContour);
Point2d IMG_DLL_EXPORT GravityCenterEx(CSmartImage * pSrc, BOOL bDraw = FALSE);


/*================================================================
����: ƽ��ͼ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. ptOffset: ƫ����
	4. nInterMode: ��ֵ��ʽ
	5. bShow: �Ƿ���ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Offset(CSmartImage * pSrc, CSmartImage * pDst, Point2i ptOffset,
						   int nInterMode = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS,
						   BOOL bShow = FALSE);


/*================================================================
����: ȱ�ڼ��
�������:	
	1. vContour: ����
	2. dDepth: ���
	3. nLength: ������ֵ
	4. dScale: ���ȱ���
����ֵ: �����ȱ��, ����TRUE, ���򷵻�FALSE
================================================================*/
BOOL IMG_DLL_EXPORT ConvexityDefects(vector<Point2i> vContour, double dDepth, double dLength, double dScale,
									 Point2i * pStart = nullptr, Point2i * pEnd = nullptr);


/*================================================================
����: ȱ�����
�������:	
	1. pSrc: Դͼ��ָ��
	2. vContour: ����
	3. dDepth: ���
	4. nLength: ������ֵ
	5. dScale: ���ȱ���
	6. bShow: �Ƿ���ʾ�����ͼ��
����ֵ: �����ȱ��, ����TRUE, ���򷵻�FALSE
================================================================*/
BOOL IMG_DLL_EXPORT ConvexityFill(CSmartImage * pSrc, vector<Point2i> vContour,
								  double dDepth, double dLength, double dScale,
								  BOOL bShow = FALSE);


/*================================================================
����: �ڶԻ���ؼ��ϻ�ͼ
�������:	
	1. pDlg: �Ի���ָ��
	2. dwID: �ؼ�ID
	3. pSrc: ͼ��ָ��
	4. bRepain: �ػ���
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT DrawNumber(CDC * pDC, int nNumber, int nSize, int nX, int nY,
						   COLORREF rgbColor, CString strFont = _T("������"));


#define DEFINIT_SMD		0
#define DEFINIT_EDGE	1
#define DEFINIT_REBLUR	2
/*================================================================
����: �����ȹ���
�������:	
	1. pSrc: ԭͼ��ָ��
	2. nMode: ��������
		1. DEFINIT_SMD: �����ݶ�
		2. DEFINIT_REBLUR: ����ģ��
����ֵ: �����ȹ�����
================================================================*/
double IMG_DLL_EXPORT DefinitionEstimate(CSmartImage * pSrc, int nMode);


/*================================================================
����: ��������֮�����С����
�������:	
	1. pV1: ����1
	2. pV2: ����2
	3. ��������1
	4. ��������2
����ֵ: ��С����
================================================================*/
double IMG_DLL_EXPORT MinDistance(const vector<Point2i> * pV1, const vector<Point2i> * pV2,
								  Point2i * ptDst1 = nullptr, Point2i * ptDst2 = nullptr);

template <typename T1, typename T2>
void IMG_DLL_EXPORT MinMaxDistance(T1 ptDst, T2 * pvPos,
							   double * pdMinDis = nullptr, double * pdMaxDis = nullptr,
							   T1 * ptMin = nullptr, T1 * ptMax = nullptr)
{
	double dMinDis = INT_MAX;
	double dMaxDis = 0;

	const int nSize = pvPos->size();

	for (int i = 0; i < nSize; i++)
	{
		const T1 *ptOutline = &(*pvPos)[i];

		const double dX = ptDst.x - ptOutline->x;
		const double dY = ptDst.y - ptOutline->y;
		const double dR = dX * dX + dY * dY;

		if (dR < dMinDis)
		{
			dMinDis = dR;

			if (nullptr != ptMin)
			{
				*ptMin = *ptOutline;
			}
		}
		
		if (dR > dMaxDis)
		{
			dMaxDis = dR;

			if (nullptr != ptMax)
			{
				*ptMax = *ptOutline;
			}
		}
	}

	if (nullptr != pdMinDis)
	{
		*pdMinDis = sqrt(dMinDis);
	}

	if (nullptr != pdMaxDis)
	{
		*pdMaxDis = sqrt(dMaxDis);
	}
}


/*================================================================
����: GrabCut
�������:	
	1. pSrc: Դͼ��ָ��
	2. rcGrag: ��Ч��Χ
	3. pResult: �ָ���
����ֵ: ��
================================================================*/
BOOL IMG_DLL_EXPORT GrabCut(CSmartImage * pSrc, CRect rcGrab, CSmartImage * pResult);


/*================================================================
����: ����任
�������:	
	1. pSrc: Դͼ��
	2. pDst: �任��ͼ��
	3. ptSrcTri: Դͼ��任��
	4. ptDstTri: �任���Ӧ��
	5. bShopw: �Ƿ�Ҫ��ʾ�任��ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ����������
================================================================*/
void IMG_DLL_EXPORT WarpAffine(CSmartImage * pSrc, CSmartImage * pDst,
							   Point2f ptSrcTri[], Point2f ptDstTri[],
							   int nInterpolation = CV_INTER_CUBIC,
							   BOOL bShow = FALSE, const CString & strWnd = _T("WarpAffine"));


/*================================================================
����: ����Ҷ�任
�������:	
	1. pSrc: Դͼ��ָ��
	2. pSpectrum: Ƶ��ͼ��
	3. pDst: Ŀ��ͼ��ָ��
	4. bShow: �Ƿ���ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT FourierTransform(CSmartImage * pSrc, CSmartImage *pSpectrum, CSmartImage * pDst,
									 BOOL bShow = FALSE, const CString & strWnd = _T("FourierTransform"));


/*================================================================
����: Ƶ�����Ļ�
�������:	
	1. pSpectrum: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. bShow: �Ƿ���ʾ�����ͼ��
	4. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT SpectrumCentralize(CSmartImage * pSpectrum, CSmartImage * pDst = nullptr,
									   BOOL bShow = FALSE, const CString & strWnd = _T("SpectrumCentralize"));


/*================================================================
����: Ƶ���˲�
�������:	
	1. pSpectrum: Դͼ��ָ��
	3. pDst: Ŀ��ͼ��ָ��
	3. pFilter: �Ƿ���ʾ�����ͼ��
	4. bShow: �Ƿ���ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT FrequencyDomainFilter(CSmartImage * pSpectrum, CSmartImage * pDst, CSmartImage * pFilter, BOOL bCvt,
										  BOOL bShow = FALSE, const CString & strWnd = _T("SpectrumCentralize"));


/*================================================================
����: �����ֵ�뷽��
�������:	
	1. pSrc: Դͼ��ָ��
	3. pMean: Ŀ��ͼ��ָ��
	3. pVar: �Ƿ���ʾ�����ͼ��
	4. szKernel: ����ߴ�
	5. bShow: �Ƿ���ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT MeanVariance(CSmartImage * pSrc, CSmartImage * pMean, CSmartImage * pVar, Size2i szKernel,
								 BOOL bShow = FALSE, const CString & strWnd = _T("MeanVar"));


/*================================================================
����: ����Ӧ�ֲ������˲�
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. szKernel: ����ߴ�
	4. dVarEst: �������
	5. bShow: �Ƿ���ʾ�����ͼ��
	6. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT AdaptLocNoiseFilter(CSmartImage * pSrc, CSmartImage * pDst, Size2i szKernel, double dVarEst,
										BOOL bShow = FALSE, const CString & strWnd = _T("AdaptLocNoiseFilter"));


/*================================================================
����: ����Ӧ��ֵ�˲�
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. szKernel: ����ߴ�
	4. bShow: �Ƿ���ʾ�����ͼ��
	5. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT AdaptMedianFilter(CSmartImage * pSrc, CSmartImage * pDst, Size2i szMaxKernel,
									  BOOL bShow = FALSE, const CString & strWnd = _T("AdaptMeidanFilter"));;


/*================================================================
����: ͼ��ϸ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. bShow: �Ƿ���ʾ�����ͼ��
	4. strWnd: ��ʾ��������
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT Thin(CSmartImage * pSrc, CSmartImage *pDst = nullptr, BOOL bShow = FALSE, const CString & strWnd = _T("Thin"));


/*================================================================
����: ������
�������:	
	1. pSrc: Դͼ��ָ��
	2. pDst: Ŀ��ͼ��ָ��
	3. bShow: �Ƿ�Ҫ��ʾ�����ͼ��
����ֵ: ��
================================================================*/
void IMG_DLL_EXPORT CentraAxis(CSmartImage * pSrc, CSmartImage *pDst = nullptr, BOOL bShow = FALSE, const CString & strWnd = _T("CentraAxis"));


/*================================================================
����: ȡ�������������
�������:	
	1. pContours: ����
����ֵ: ����������
================================================================*/
int IMG_DLL_EXPORT GetMaxSizeContour(const cvContours * pContours);


/*================================================================
����: Բ�ȼ���
�������:	
	1. vContour: ͼ������
����ֵ: Բ��
================================================================*/
double IMG_DLL_EXPORT RoundnessEstimate(const vector<Point2i> * pvContour);
double IMG_DLL_EXPORT RoundnessEstimate(double dArea, double dPerimeter);


/*================================================================
����: ����������С��Ӿ��εĽǶ�
�������:	
	1. rc: ��С��Ӿ���
	2, Longside: �Ƿ��Գ���Ϊ��׼
����ֵ: �Ƕ�
================================================================*/
double IMG_DLL_EXPORT MinRectAngle(const RotatedRect & rc, BOOL Longside);


/*================================================================
����: ����֮�����
�������:	
	1. pt1: ��1��
	2. pt2: ��2��
����ֵ: ����
================================================================*/

template <typename T1, typename T2>
double IMG_DLL_EXPORT PointDistance(T1 pt1, T2 pt2)
{
	double dDeltaX = pt1.x - pt2.x;
	double dDeltaY = pt1.y - pt2.y;
	
	return sqrt(dDeltaX * dDeltaX + dDeltaY * dDeltaY);
}


/*================================================================
����: ����һ��������һ����ΪԲ����ת�������
�������:	
	1. ptSrc: ��Ҫ��ת�ĵ�
	2. ptCenter: ��ת����
	3. ��ת�Ƕ�: ��ʱ��Ϊ��
����ֵ: ��ת�������
================================================================*/
template <typename T1, typename T2>
Point2d IMG_DLL_EXPORT PtRotate(T1 ptSrc, T2 ptCenter, double dAngle)
{
	const double dTheta = dAngle * CV_PI / 180.0;

	Point2d ptOfst;

	ptOfst.x = ptSrc.x - ptCenter.x;
	ptOfst.y = ptSrc.y - ptCenter.y;

	const double dDeltaX = ptOfst.x * cos(dTheta) - ptOfst.y * sin(dTheta);
	const double dDeltaY = ptOfst.x * sin(dTheta) + ptOfst.y * cos(dTheta);

	Point2d ptDst;

	ptDst.x = ptCenter.x + dDeltaX;
	ptDst.y = ptCenter.y + dDeltaY;

	return ptDst;
}

typedef struct _tagFitLine
{
	double k;
	double b;

	Point2d ptStart;
	Point2d ptEnd;
}FIT_LINE;

#define VERTICAL_LINE_K 987654321.12345

/*================================================================
����: ֱ�����
�������:	
	1. pvPts: Ŀ���
	2. dReps: ����ľ��Ȳ���
	3. dAeps: �ǶȾ��Ȳ���
����ֵ: ֱ�߲���
================================================================*/
template <typename T>
BOOL IMG_DLL_EXPORT FitLine(vector<T> * vPts, int nDist, double dReps, double dAeps, FIT_LINE * pL)
{
// 	CV_DIST_USER    = -1,  /* User defined distance */
//  CV_DIST_L1      = 1,   /* distance = |x1-x2| + |y1-y2| */
//  CV_DIST_L2      = 2,   /* the simple euclidean distance */
//  CV_DIST_C       = 3,   /* distance = max(|x1-x2|,|y1-y2|) */
//  CV_DIST_L12     = 4,   /* L1-L2 metric: distance = 2(sqrt(1+x*x/2) - 1)) */
//  CV_DIST_FAIR    = 5,   /* distance = c^2(|x|/c-log(1+|x|/c)), c = 1.3998 */
//  CV_DIST_WELSCH  = 6,   /* distance = c^2/2(1-exp(-(x/c)^2)), c = 2.9846 */
//  CV_DIST_HUBER   = 7    /* distance = |x|<c ? x^2/2 : c(|x|-c/2), c=1.345 */

	const int nPts = vPts->size();

	if (nPts < 7)
	{
		return FALSE;
	}

	Vec4f vL;
	fitLine(*vPts, vL, nDist, 0, dReps, dAeps);

	FIT_LINE fl;

	double k = VERTICAL_LINE_K;
	double b = 0;

	if (fabs(vL[0]) < 1E-6)
	{
		fl.k = VERTICAL_LINE_K;
		fl.b = 0;

		const int nPts = vPts->size();

		fl.ptStart = (Point2d(vPts->at(0).x, vPts->at(0).y) + Point2d(vPts->at(nPts - 1).x, vPts->at(nPts - 1).y)) * 0.5;
		fl.ptEnd = fl.ptStart;
	}
	else
	{
		k = vL[1] / vL[0];
		b = vL[3] - k * vL[2];

		fl.k = k;
		fl.b = b;

		fl.ptStart = Point2d(vPts->at(0).x, k * vPts->at(0).x + b);
		fl.ptEnd = Point2d(vPts->at(vPts->size() - 1).x, k * vPts->at(vPts->size() - 1).x + b);
	}

// 	Vec4d vRes;
// 	vRes[0] = vPts->at(0).x;
// 	vRes[1] = k * vRes[0] + b;
// 	vRes[2] = vPts->at(vPts->size() - 1).x;
// 	vRes[3] = k * vRes[2] + b;

	if (nullptr != pL)
	{
		*pL = fl;
	}

	return TRUE;
}

/*================================================================
����: ��ֱ�߽�������
�������:	
	1. L1: ֱ��1
	2, L2: ֱ��2
����ֵ: ��������
================================================================*/
Point2d IMG_DLL_EXPORT Intersection(const FIT_LINE L1, const FIT_LINE L2);

/*================================================================
����: �㵽�ߵľ���
�������:	
	1. pLine: ֱ��
	2. pvPts: Ŀ���
����ֵ: �㵽�߾���
================================================================*/
template <typename T>
double IMG_DLL_EXPORT Pt2LineDist(const FIT_LINE * pLine, T ptDst)
{
	if (fabs(pLine->ptStart.x - pLine->ptEnd.x) < 1E-6)
	{
		const double dDist = fabs(ptDst.x - (pLine->ptStart.x + pLine->ptEnd.x) * 0.5);

		return dDist;
	}
	else if (fabs(pLine->k - VERTICAL_LINE_K) < 1E-6)
	{
		const double dDist = fabs(ptDst.x - (pLine->ptStart.x + pLine->ptEnd.x) * 0.5);

		return dDist;
	}

	const double dDelta = sqrt(pLine->k * pLine->k + 1);
	const double dDist = fabs(pLine->k * ptDst.x - ptDst.y + pLine->b) / dDelta;

	return dDist;
}


/*================================================================
����: ��������ƫ��
�������:	
	1. ptOutline: ����ָ��
	2. ptOfst: ƫ����
����ֵ: ��
================================================================*/
template <typename T>
void IMG_DLL_EXPORT ContourOffset(cvContours * pvOutline, T ptOfst)
{
	const int nConts = pvOutline->size();

	for (int i = 0; i < nConts; i++)
	{
		const int nSize = pvOutline->at(i).size();

		for (int j = 0; j < nSize; j++)
		{
			pvOutline->at(i).at(j).x += ptOfst.x;
			pvOutline->at(i).at(j).y += ptOfst.y;
		}
	}
}

template <typename T1, typename T2>
void IMG_DLL_EXPORT ContourOffset(vector<T1> * vPts, T2 ptOfst)
{
	const int nSize = vPts->size();

	for (int i = 0; i < nSize; i++)
	{
		vPts->at(i).x += ptOfst.x;
		vPts->at(i).y += ptOfst.y;
	}
}

/*================================================================
����: ������Size����
================================================================*/
template <typename T>
bool IMG_DLL_EXPORT CompSizeH2L(const vector<T> & v1, const vector<T> & v2)
{
	return v1.size() > v2.size();
}

template <typename T>
bool IMG_DLL_EXPORT CompSizeL2H(const vector<T> & v1, const vector<T> & v2)
{
	return v1.size() < v2.size();
}

/*================================================================
����: ������X����
================================================================*/
template <typename T>
bool IMG_DLL_EXPORT Comp_X_L2H(const T pt1, const T pt2)
{
	return pt1.x < pt2.x;
}

template <typename T>
bool IMG_DLL_EXPORT Comp_X_H2L(const T pt1, const T pt2)
{
	return pt1.x > pt2.x;
}

/*================================================================
����: ������Y����
================================================================*/
template <typename T>
bool IMG_DLL_EXPORT Comp_Y_L2H(const T pt1, const T pt2)
{
	return pt1.y < pt2.y;
}

template <typename T>
bool IMG_DLL_EXPORT Comp_Y_H2L(const T pt1, const T pt2)
{
	return pt1.y > pt2.y;
}

/*================================================================
����: Բ���
�������:	
	1. pSrc: Դͼ��(�Ҷ�ͼ��)
	2. nCannyThres: Canny��Ե��ֵ
	3. dRadius: �뾶
	4. dScaleMin: ��С����
	5. dScaleMax: ������
	6. dMaxOfst: ���ƫ����
	7. dScore: ��Сƥ�����
	8. nWant: ��Ѱ����
����ֵ: ����Բ������ֱ������
================================================================*/
vector<Point3d> IMG_DLL_EXPORT GetFitCircle(vector<Point2i> *ptSrc,
											double dRadius, double dScaleMin, double dScaleMax,
											double dMaxOfst, int nInters, double dScore, int nWant);
vector<Point3d> IMG_DLL_EXPORT GetFitCircle(vector<Point2d> *ptSrc,
											double dRadius, double dScaleMin, double dScaleMax,
											double dMaxOfst, int nInters, double dScore, int nWant);
vector<Point3d> IMG_DLL_EXPORT GetFitCircle(CSmartImage * pSrc, int nCannyThres,
											double dRadius, double dScaleMin, double dScaleMax,
											double dMaxOfst, int nInters, double dScore, int nWant);


/*================================================================
����: �����ر�Ե
�������:	
	1. pSrc: Դͼ��(�Ҷ�ͼ��)
	2. pDst: Ŀ���Եͼ��
	3. pSubPixelEdge: �����ر�Ե
	4. nThres1: ��ֵ����ֵ1 nThres1 <= nThres2
	5. nThres1: ��ֵ����ֵ1 nThres1 <= nThres2
	6. bShow: �Ƿ���ʾ�����ͼ��
	7. strWnd: ��ʾ��������
����ֵ: �ɹ�����TRUE, ���򷵻�FALSE
================================================================*/
BOOL IMG_DLL_EXPORT SubPixelEdge(CSmartImage * pSrc, CSmartImage * pDst, CSmartImage * pSubPixelEdge,
								 int nThres1, int nThres2, BOOL bShow = FALSE, const CString & strWnd = _T("SubPixelEdge"));


/*================================================================
����: ȡ��Ŀ��ͼ��ָ��
�������:	
	1. pSrc: Դͼ��ָ��
	2. ppDst: Ŀ��ͼ��ָ��
	3. bInPlace: Դͼ���Ŀ��ͼ���Ƿ����Ϊͬһͼ��
����ֵ: Ŀ��ͼ���ָ��
	================================================================*/
AFX_INLINE CSmartImage * _GetDstImg(CSmartImage * pSrc, CSmartImage * pDst = nullptr, BOOL bInPlace = TRUE);
