#include "stdafx.h"
#include "Stitcher.h"

const CString g_StitchErrs[] =
{
	_T("拼接成功。"),
	_T("拼接图像少于1张。"),
	_T("错误的特征检测类型。"),
	_T("输入图像为空。"),
	_T("错误的 Bundle 调整参数。"),
	_T("错误的波形校正参数。"),
	_T("Wraper 创建失败。")
	_T("SeamFinder 创建失败。")
};

CStitcher::CStitcher(void)
	: m_bUseGPU(FALSE)
	, m_nFeatureType(STITCH_FEATURE_SURF)
	, m_dWorkMegaPix(0.6)
	, m_dSeamMegaPix(0.1)
	, m_dComposeMegaPix(-1)
	, m_dMatchConfidence(0.3)
	, m_BundleAdj(STITCH_BAJ_RAY)
	, m_WaveCorr(detail::WAVE_CORRECT_HORIZ)
	, m_strWrapType(_T("plane"))
	, m_nExposComptype(detail::ExposureCompensator::GAIN_BLOCKS)
	, m_nSeamType(STITCH_SEAM_GC_COLOR)
	, m_nBlendType(detail::Blender::MULTI_BAND)
	, m_dBlendStrenth(5)
{
}


CStitcher::~CStitcher(void)
{
}


int CStitcher::Stitch(vector<CSmartImage *> * pvImgSrc, vector<Rect2i> * pvRoi, CSmartImage * pDst)
{
using namespace cv::detail;

#ifdef DEBUG
	const double dTickStart = (double)getTickCount();
#endif // DEBUG

	cv::setBreakOnError(true);

	int nPictures = pvImgSrc->size();

	if (nPictures < 2)
	{
		const int nErr = ST_ERR_MORE_IMAGE;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;
	}

	Ptr<FeaturesFinder> finder;

	switch (m_nFeatureType)
	{
	case STITCH_FEATURE_SURF:
#if defined(HAVE_OPENCV_NONFREE) && defined(HAVE_OPENCV_GPU)
		if (m_bUseGPU && gpu::getCudaEnabledDeviceCount() > 0)
		{
			finder = new SurfFeaturesFinderGpu();
		}
		else
		{
			finder = new SurfFeaturesFinder();
		}
#endif
		break;

	case STITCH_FEATURE_ORB:
		finder = new OrbFeaturesFinder();
		break;

	default:
		const int nErr = ST_ERR_UNKNOW_FEATURE;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;

		break;
	}

	double work_scale = 1, seam_scale = 1, compose_scale = 1;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	Mat full_img, img;
	vector<ImageFeatures> features(nPictures);
	vector<Mat> images(nPictures);
	vector<Size> full_img_sizes(nPictures);
	double seam_work_aspect = 1;

	for (int i = 0; i < nPictures; i++)
	{
		full_img = *pvImgSrc->at(i);
		full_img_sizes[i] = full_img.size();

		if (full_img.empty())
		{
			const int nErr = ST_ERR_IMAGE_EMPTY;
			TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

			return nErr;
		}

		if (m_dWorkMegaPix < 0)
		{
			img = full_img;
			work_scale = 1;
			is_work_scale_set = true;
		}
		else
		{
			if (!is_work_scale_set)
			{
				work_scale = min(1.0, sqrt(m_dWorkMegaPix * 1e6 / full_img.size().area()));
				is_work_scale_set = true;
			}

			resize(full_img, img, Size(), work_scale, work_scale);

// 			if (nullptr != pvRoi &&
// 				pvRoi->at(i).width > 0 &&
// 				pvRoi->at(i).height > 0)
// 			{
// 				pvRoi->at(i).x = cvRound(pvRoi->at(i).x * work_scale);
// 				pvRoi->at(i).y = cvRound(pvRoi->at(i).y * work_scale);
// 				pvRoi->at(i).width = cvRound(pvRoi->at(i).width * work_scale);
// 				pvRoi->at(i).height = cvRound(pvRoi->at(i).height * work_scale);
// 			}
		}

		if (!is_seam_scale_set)
		{
			seam_scale = min(1.0, sqrt(m_dSeamMegaPix * 1e6 / full_img.size().area()));
			seam_work_aspect = seam_scale / work_scale;
			is_seam_scale_set = true;
		}

// 		if (nullptr != pvRoi &&
// 			pvRoi->at(i).width > 0 &&
// 			pvRoi->at(i).height > 0)
// 		{
// 			vector<Rect2i> vroi;
// 			vroi.push_back(pvRoi->at(i));
// 			(*finder)(img, features[i], vroi);
// 		}
// 		else
		{
			(*finder)(img, features[i]);
		}

		features[i].img_idx = i;
		TRACE(_T("\nFeatures in image-%d: %d"), i + 1, features[i].keypoints.size());

		resize(full_img, img, Size(), seam_scale, seam_scale);
		images[i] = img.clone();
	}

	finder->collectGarbage();
	full_img.release();
	img.release();

#ifdef DEBUG
	const double dFeatureTime = (double)getTickCount();

	TRACE(_T("\nFeature detect time: %f"), (dFeatureTime - dTickStart) / getTickFrequency());
#endif // DEBUG

	vector<MatchesInfo> pairwise_matches;
	BestOf2NearestMatcher matcher(m_bUseGPU? true: false, (float)m_dMatchConfidence);
	matcher(features, pairwise_matches);
	matcher.collectGarbage();

	// Leave only images we are sure are from the same panorama
	const vector<int> indices = leaveBiggestComponent(features, pairwise_matches, 1.0f);
	const int nSize = indices.size();

	vector<Mat> img_subset;
	vector<string> img_names_subset;
	vector<Size> full_img_sizes_subset;

	for (int i = 0; i < nSize; i++)
	{
		img_subset.push_back(images[indices[i]]);
		full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
	}

	images = img_subset;
	full_img_sizes = full_img_sizes_subset;

	// Check if we still have enough images
	nPictures = static_cast<int>(images.size());

	if (nPictures < 2)
	{
		const int nErr = ST_ERR_MORE_IMAGE;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;
	}

#ifdef DEBUG
	const double dPairwiseMatchTime = (double)getTickCount();

	TRACE(_T("\nPairwise match time: %f"), (dPairwiseMatchTime - dFeatureTime) / getTickFrequency());
#endif // DEBUG

	HomographyBasedEstimator estimator;
	vector<CameraParams> cameras;
	estimator(features, pairwise_matches, cameras);

	for (size_t i = 0; i < cameras.size(); i++)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}

	Ptr<detail::BundleAdjusterBase> adjuster;
	if (STITCH_BAJ_REPROJ == m_BundleAdj)
	{
		adjuster = new detail::BundleAdjusterReproj();
	}
	else if (STITCH_BAJ_RAY == m_BundleAdj)
	{
		adjuster = new detail::BundleAdjusterRay();
	}
	else
	{
		const int nErr = ST_ERR_UNKNOW_BUNDLE_ADJ;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;
	}

	adjuster->setConfThresh(1.0f);

	Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
	refine_mask(0,0) = 1;
	refine_mask(0,1) = 1;
	refine_mask(0,2) = 1;
	refine_mask(1,1) = 1;
	refine_mask(1,2) = 1;

	adjuster->setRefinementMask(refine_mask);

	(*adjuster)(features, pairwise_matches, cameras);

	// Find median focal length

	vector<double> focals;

	const int nCameras = cameras.size();

	for (int i = 0; i < nCameras; i++)
	{
		focals.push_back(cameras[i].focal);
	}

	sort(focals.begin(), focals.end());

	float warped_image_scale = 0;

	if (IS_ODD(nCameras))
	{
		warped_image_scale = static_cast<float>(focals[nCameras >> 1]);
	}
	else
	{
		warped_image_scale = static_cast<float>(focals[(nCameras >> 1) - 1] + focals[nCameras >> 1]) * 0.5f;
	}

	vector<Mat> rmats;

	for (int i = 0; i < nCameras; i++)
	{
		rmats.push_back(cameras[i].R.clone());
	}

	waveCorrect(rmats, m_WaveCorr);

	for (int i = 0; i < nCameras; i++)
	{
		cameras[i].R = rmats[i];
	}

#ifdef DEBUG
	const double dEstimateTime = (double)getTickCount();

	TRACE(_T("\nEstimate time: %f"), (dEstimateTime - dPairwiseMatchTime) / getTickFrequency());
#endif // DEBUG

	vector<Point> corners(nPictures);
	vector<Mat> masks_warped(nPictures);
	vector<Mat> images_warped(nPictures);
	vector<Size> sizes(nPictures);
	vector<Mat> masks(nPictures);

	// Preapre images masks
	for (int i = 0; i < nPictures; i++)
	{
		masks[i].create(images[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	// Warp images and their masks

	Ptr<WarperCreator> warper_creator;
#if defined(HAVE_OPENCV_GPU)
	if (m_bUseGPU && gpu::getCudaEnabledDeviceCount() > 0)
	{
		if (m_strWrapType == _T("plane")) warper_creator = new cv::PlaneWarperGpu();
		else if (m_strWrapType == _T("cylindrical")) warper_creator = new cv::CylindricalWarperGpu();
		else if (m_strWrapType == _T("spherical")) warper_creator = new cv::SphericalWarperGpu();
	}
	else
#endif
	{
		if (m_strWrapType == _T("plane")) warper_creator = new cv::PlaneWarper();
		else if (m_strWrapType == _T("cylindrical")) warper_creator = new cv::CylindricalWarper();
		else if (m_strWrapType == _T("spherical")) warper_creator = new cv::SphericalWarper();
		else if (m_strWrapType == _T("fisheye")) warper_creator = new cv::FisheyeWarper();
		else if (m_strWrapType == _T("stereographic")) warper_creator = new cv::StereographicWarper();
		else if (m_strWrapType == _T("compressedPlaneA2B1")) warper_creator = new cv::CompressedRectilinearWarper(2, 1);
		else if (m_strWrapType == _T("compressedPlaneA1.5B1")) warper_creator = new cv::CompressedRectilinearWarper(1.5, 1);
		else if (m_strWrapType == _T("compressedPlanePortraitA2B1")) warper_creator = new cv::CompressedRectilinearPortraitWarper(2, 1);
		else if (m_strWrapType == _T("compressedPlanePortraitA1.5B1")) warper_creator = new cv::CompressedRectilinearPortraitWarper(1.5, 1);
		else if (m_strWrapType == _T("paniniA2B1")) warper_creator = new cv::PaniniWarper(2, 1);
		else if (m_strWrapType == _T("paniniA1.5B1")) warper_creator = new cv::PaniniWarper(1.5, 1);
		else if (m_strWrapType == _T("paniniPortraitA2B1")) warper_creator = new cv::PaniniPortraitWarper(2, 1);
		else if (m_strWrapType == _T("paniniPortraitA1.5B1")) warper_creator = new cv::PaniniPortraitWarper(1.5, 1);
		else if (m_strWrapType == _T("mercator")) warper_creator = new cv::MercatorWarper();
		else if (m_strWrapType == _T("transverseMercator")) warper_creator = new cv::TransverseMercatorWarper();
	}

	if (warper_creator.empty())
	{
		const int nErr = ST_ERR_WARPER_EMPTY;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;
	}

	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));

	for (int i = 0; i < nPictures; i++)
	{
		Mat_<float> K;

		cameras[i].K().convertTo(K, CV_32F);

		const float swa = (float)seam_work_aspect;

		K(0,0) *= swa;
		K(0,2) *= swa;
		K(1,1) *= swa;
		K(1,2) *= swa;

		corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
	}

	vector<Mat> images_warped_f(nPictures);

	for (int i = 0; i < nPictures; i++)
	{
		images_warped[i].convertTo(images_warped_f[i], CV_32F);
	}

	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(m_nExposComptype);
	compensator->feed(corners, images_warped, masks_warped);

	Ptr<SeamFinder> seam_finder;

	switch (m_nSeamType)
	{
	case STITCH_SEAM_NO:
		seam_finder = new detail::NoSeamFinder();
		break;

	case STITCH_SEAM_VORONOI:
		seam_finder = new detail::VoronoiSeamFinder();
		break;

	case STITCH_SEAM_GC_COLOR:
#if defined(HAVE_OPENCV_GPU)
		if (m_bUseGPU && gpu::getCudaEnabledDeviceCount() > 0)
		{
			seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR);
		}
		else
		{
			seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR);
		}
#endif
		break;

	case STITCH_SEAM_GC_COLORGRAD:
#if defined(HAVE_OPENCV_GPU)
		if (m_bUseGPU && gpu::getCudaEnabledDeviceCount() > 0)
		{
			seam_finder = new detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR_GRAD);
		}
		else
		{
			seam_finder = new detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR_GRAD);
		}
#endif
		break;

	case STITCH_SEAM_DP_COLOR:
		seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR);
		break;

	case STITCH_SEAM_DP_COLORGRAD:
		seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR_GRAD);
		break;

	default:
		break;
	}
		
	if (seam_finder.empty())
	{
		const int nErr = ST_ERR_SEAM_FINDER;
		TRACE(_T("\nStitch Err: %d, %s"), nErr, g_StitchErrs[nErr]);

		return nErr;
	}

	seam_finder->find(images_warped_f, corners, masks_warped);

	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();

#ifdef DEBUG
	const double dWrapTime = (double)getTickCount();

	TRACE(_T("\nWrap time: %f"), (dWrapTime - dEstimateTime) / getTickFrequency());
#endif // DEBUG

	Mat img_warped, img_warped_s;
	Mat dilated_mask, seam_mask, mask, mask_warped;
	Ptr<Blender> blender;
	//double compose_seam_aspect = 1;
	double compose_work_aspect = 1;

	for (int img_idx = 0; img_idx < nPictures; img_idx++)
	{
		// Read image and resize it if necessary

		full_img = *pvImgSrc->at(indices[img_idx]);

		if (!is_compose_scale_set)
		{
			if (m_dComposeMegaPix > 0)
			{
				compose_scale = min(1.0, sqrt(m_dComposeMegaPix * 1e6 / full_img.size().area()));
			}

			is_compose_scale_set = true;

			// Compute relative scales
			//compose_seam_aspect = compose_scale / seam_scale;
			compose_work_aspect = compose_scale / work_scale;

			// Update warped image scale
			warped_image_scale *= static_cast<float>(compose_work_aspect);
			warper = warper_creator->create(warped_image_scale);

			// Update corners and sizes
			for (int i = 0; i < nPictures; i++)
			{
				// Update intrinsics
				cameras[i].focal *= compose_work_aspect;
				cameras[i].ppx *= compose_work_aspect;
				cameras[i].ppy *= compose_work_aspect;

				// Update corner and size
				Size sz = full_img_sizes[i];
				if (std::abs(compose_scale - 1) > 1e-1)
				{
					sz.width = cvRound(full_img_sizes[i].width * compose_scale);
					sz.height = cvRound(full_img_sizes[i].height * compose_scale);
				}

				Mat K;
				cameras[i].K().convertTo(K, CV_32F);
				Rect roi = warper->warpRoi(sz, K, cameras[i].R);
				corners[i] = roi.tl();
				sizes[i] = roi.size();
			}
		}
		if (abs(compose_scale - 1) > 1e-1)
		{
			resize(full_img, img, Size(), compose_scale, compose_scale);
		}
		else
		{
			img = full_img;
		}

		full_img.release();
		const Size img_size = img.size();

		Mat K;
		cameras[img_idx].K().convertTo(K, CV_32F);

		// Warp the current image
		warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);

		// Warp the current image mask
		mask.create(img_size, CV_8U);
		mask.setTo(Scalar::all(255));
		warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

		// Compensate exposure
		compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

		img_warped.convertTo(img_warped_s, CV_16S);
		img_warped.release();
		img.release();
		mask.release();

		dilate(masks_warped[img_idx], dilated_mask, Mat());
		resize(dilated_mask, seam_mask, mask_warped.size());
		mask_warped = seam_mask & mask_warped;

		if (blender.empty())
		{
			blender = Blender::createDefault(m_nBlendType, m_bUseGPU? true: false);

			const Size dst_sz = resultRoi(corners, sizes).size();
			const double blend_width = sqrt((double)dst_sz.area()) * m_dBlendStrenth / 100;

			if (blend_width < 1.f)
			{
				blender = Blender::createDefault(Blender::NO, m_bUseGPU? true: false);
			}
			else if (Blender::MULTI_BAND == m_nBlendType)
			{
				MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(static_cast<Blender*>(blender));
				mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.0)) - 1.));
			}
			else if (Blender::FEATHER == m_nBlendType)
			{
				FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
				fb->setSharpness((float)(1 / blend_width));
			}

			blender->prepare(corners, sizes);
		}

		// Blend the current image
		blender->feed(img_warped_s, mask_warped, corners[img_idx]);
	}

	Mat result, result_mask;
	blender->blend(result, result_mask);

	result.convertTo(result,CV_8UC1);

// 	namedWindow("Stitch", 0);
// 	imshow("Stitch", result);

	*pDst = result.clone();

#ifdef _DEBUG
	const double dTickEnd = (double)getTickCount();
	TRACE(_T("\nStitch time: %f"), (dTickEnd - dTickStart) / getTickFrequency());
#endif // _DEBUG

	return ST_ERR_OK;
}


int CStitcher::Stitch(CSmartImage * pSrc1, Rect2i roi1, CSmartImage * pSrc2, Rect2i roi2, CSmartImage * pDst)
{
	vector<CSmartImage *> vImgSrc;
	vector<Rect2i> vRois;

	vImgSrc.push_back(pSrc1);
	vImgSrc.push_back(pSrc2);

	vRois.push_back(roi1);
	vRois.push_back(roi2);

	const int nRetVal = Stitch(&vImgSrc, &vRois, pDst);

	return nRetVal;
}


CString CStitcher::GetErrInfo(int nErr)
{
	ASSERT(nErr >= 0 && nErr <= ST_ERR_SUM);

	CString strValue;

	if (nErr < 1 || nErr > ST_ERR_SUM)
	{
		strValue.Format(_T("错误代(%d)码超范围。"), nErr);
	}
	else
	{
		strValue.Format(_T("%s(%d)。"), g_StitchErrs[nErr], nErr);
	}

	return strValue;
}
