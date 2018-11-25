#pragma once

#include "SmartImage.h"

#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\opencv_modules.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\stitching\detail\autocalib.hpp>
#include <opencv2\stitching\detail\blenders.hpp>
#include <opencv2\stitching\detail\camera.hpp>
#include <opencv2\stitching\detail\exposure_compensate.hpp>
#include <opencv2\stitching\detail\matchers.hpp>
#include <opencv2\stitching\detail\motion_estimators.hpp>
#include <opencv2\stitching\detail\seam_finders.hpp>
#include <opencv2\stitching\detail\util.hpp>
#include <opencv2\stitching\detail\warpers.hpp>
#include <opencv2\stitching\warpers.hpp>


enum
{
	STITCH_FEATURE_SURF = 0,
	STITCH_FEATURE_ORB,
};

enum
{
	STITCH_BAJ_REPROJ = 0,
	STITCH_BAJ_RAY,
};

enum 
{
	STITCH_SEAM_NO = 0,
	STITCH_SEAM_VORONOI,
	STITCH_SEAM_GC_COLOR,
	STITCH_SEAM_GC_COLORGRAD,
	STITCH_SEAM_DP_COLOR,
	STITCH_SEAM_DP_COLORGRAD,
	STITCH_SEAM_TYPES
};

enum
{
	ST_ERR_OK = 0,
	ST_ERR_MORE_IMAGE,
	ST_ERR_UNKNOW_FEATURE,
	ST_ERR_IMAGE_EMPTY,
	ST_ERR_UNKNOW_BUNDLE_ADJ,
	ST_ERR_UNKNOW_WAVE_CORR,
	ST_ERR_WARPER_EMPTY,
	ST_ERR_SEAM_FINDER,
	ST_ERR_SUM
};

class AFX_EXT_CLASS CStitcher
{
public:
	CStitcher(void);
	~CStitcher(void);

public:
	BOOL m_bUseGPU;
	int m_nFeatureType;
	double m_dWorkMegaPix;
	double m_dSeamMegaPix;
	double m_dComposeMegaPix;
	double m_dMatchConfidence;

	int m_BundleAdj;
	detail::WaveCorrectKind m_WaveCorr;
	int m_nExposComptype;
	CString m_strWrapType;
	
	int m_nSeamType;
	int m_nBlendType;
	double m_dBlendStrenth;

	//////////////////////////////////////////////////////////////////////////
	int Stitch(vector<CSmartImage *> * pvImgSrc, vector<Rect2i> * pvRoi, CSmartImage * pDst);
	int Stitch(CSmartImage * pSrc1, Rect2i roi1, CSmartImage * pSrc2, Rect2i roi2, CSmartImage * pDst);

	CString GetErrInfo(int nErr);
};

