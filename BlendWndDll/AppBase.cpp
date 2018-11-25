// AppBase.cpp : 实现文件
//

#include "stdafx.h"
#include "AppBase.h"
#include "afxdialogex.h"

#include "Blender.h"
#include "MsgBox.h"
#include "PsdMgr.h"

std::vector<BASE_BLUR_INFO> g_vBaseInfo;
CSemaphore g_seBaseInfo;

#ifdef _DEBUG
	int g_nPsdLevel = PSD_LEVEL_MGR;
	BOOL g_bUiLocked = FALSE;
	BOOL g_bRegistered = TRUE;
#else
	int g_nPsdLevel = PSD_LEVEL_LOCK;
	BOOL g_bUiLocked = TRUE;
	BOOL g_bRegistered = FALSE;
#endif // _DEBUG

BOOL g_bFileDirty = FALSE;
int g_nFireMode = FIRE_MODE_232;

// CAppBase 对话框

IMPLEMENT_DYNAMIC(CAppBase, CDialogEx)

CAppBase::CAppBase(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAppBase::IDD, pParent)
	, m_bClickShow(FALSE)
	, m_pOwner(nullptr)
	, m_bUiLocked(FALSE)
	, m_bStatusLock(FALSE)
	, m_bChildWnd(FALSE)
	, m_bNoMenu(FALSE)
	, m_nMenuStart(0)
	, m_fTitleFontSize(13)
	, m_strTitleFont(_T("微软雅黑"))
	, m_rgbTitle(RGB_0_255_230)
	, m_rgbKey(RGB_TRANS_TOP_KEY)
	, m_nBlurCount(0)
	, m_nUdpMemSize(1024)
	, m_bNetAnswered(FALSE)
	, m_Socket(INVALID_SOCKET)
	, m_pNetThread(nullptr)
	, m_bAppMain(FALSE)
	, m_bCanMove(FALSE)
	, m_bAppQuit(FALSE)
	, m_nFlash(0)
	, m_nHoldDays(7)
{
	m_AutoSave.bPassAutoSave = TRUE;
	m_AutoSave.bFailAutoSave = TRUE;
}

CAppBase::CAppBase(UINT nIDTemplate, CWnd * pParent/* = NULL*/)
	: CDialogEx(nIDTemplate, pParent)
	, m_bClickShow(FALSE)
	, m_pOwner(nullptr)
	, m_bUiLocked(FALSE)
	, m_bStatusLock(FALSE)
	, m_bChildWnd(FALSE)
	, m_bNoMenu(FALSE)
	, m_nMenuStart(0)
	, m_fTitleFontSize(13)
	, m_strTitleFont(_T("微软雅黑"))
	, m_rgbTitle(RGB_0_255_230)
	, m_rgbKey(RGB_TRANS_TOP_KEY)
	, m_nBlurCount(0)
	, m_nUdpMemSize(1024)
	, m_bNetAnswered(FALSE)
	, m_Socket(INVALID_SOCKET)
	, m_pNetThread(nullptr)
	, m_bAppMain(FALSE)
	, m_bCanMove(FALSE)
	, m_bAppQuit(FALSE)
	, m_nFlash(0)
	, m_nHoldDays(7)
{
	m_AutoSave.bPassAutoSave = TRUE;
	m_AutoSave.bFailAutoSave = TRUE;
}

CAppBase::~CAppBase()
{
	if (INVALID_SOCKET != m_Socket)
	{
		shutdown(m_Socket, SD_BOTH);
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	_RemoveBlurInfo(this);
}

void CAppBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAppBase, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(gMsgSemiTransBlur, &CAppBase::OnGmsgsemitransblur)
	ON_REGISTERED_MESSAGE(gMsgAppBaseLockUi, &CAppBase::OnGmsgappbaselockui)
	ON_REGISTERED_MESSAGE(gMsgAppBaseUnlockUi, &CAppBase::OnGmsgappbaseunlockui)
	ON_REGISTERED_MESSAGE(gMsgAppBaseSetFocus, &CAppBase::OnGmsgappbasesetfocus)	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAppBase, CDialogEx)
	ON_EVENT(CAppBase, IDC_BT_BASE_CLOSE, 1, CAppBase::LBtClickedBtClose, VTS_I4)
	ON_EVENT(CAppBase, IDC_BT_BASE_SIZE, 1, CAppBase::LBtClickedBtSize, VTS_I4)
	ON_EVENT(CAppBase, IDC_BT_BASE_MIN, 1, CAppBase::LBtClickedBtMin, VTS_I4)
	ON_EVENT(CAppBase, IDC_SW_BASE_LOCK, 1, CAppBase::StatusChangedSwAppLock, VTS_BOOL)
	ON_EVENT(CAppBase, IDC_BT_BASE_OK, 1, CAppBase::LBtClickedBtOk, VTS_I4)
	ON_EVENT(CAppBase, IDC_BT_BASE_CANCEL, 1, CAppBase::LBtClickedBtCancel, VTS_I4)
	ON_EVENT(CAppBase, IDC_MENU_APP_MAIN, 1, CAppBase::PrePopShowMenuAppMain, VTS_BSTR)
	ON_EVENT(CAppBase, IDC_MENU_APP_MAIN, 2, CAppBase::ItemClickMenuAppMain, VTS_BSTR VTS_BSTR VTS_I2 VTS_BOOL VTS_I4)
END_EVENTSINK_MAP()

BOOL CAppBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ModifyStyle(WS_CAPTION, 0, 0);
	ModifyStyleEx(WS_EX_DLGMODALFRAME, 0, 0);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	if (m_strTitle.IsEmpty())
	{
		GetWindowText(m_strTitle);
	}

	BASE_BLUR_INFO Bi;

	Bi.pWnd = this;
	Bi.strInfo = m_strTitle;
	g_vBaseInfo.push_back(Bi);

	//////////////////////////////////////////////////////////////////////////
	CRect rcBox;
	GetClientRect(rcBox);

	rcBox.left = rcBox.right - 62;
	rcBox.right -= 12;
	rcBox.top += 8;
	rcBox.bottom = rcBox.top + 24;

	CBL_Button *pBt[] = {&m_BtBaseClose, &m_BtBaseSize, &m_BtBaseMin};
	CString strCaptions[] = {_T("关闭"), _T("还原"), _T("最小化")};
	const CString strPngName[] = {_T("Close_"), _T("Resize_"), _T("Min_")};

	for (int i = 0; i < 3; i++)
	{
		pBt[i]->Create(strCaptions[i], WS_CHILD | WS_VISIBLE, rcBox, this, IDC_BT_BASE_CLOSE - i);
		pBt[i]->SetShowBorder(FALSE);
		pBt[i]->SetShowBorder(FALSE);
		pBt[i]->SetBackColor(m_bAppMain? RGB_ALL_64: m_rgbKey);
		pBt[i]->SetBlendColor(m_bAppMain? RGB_ALL_64: m_rgbKey);
		pBt[i]->SetForeColor(RGB_0_255_230);
		pBt[i]->SetTipBackColor(RGB_ALL_64);
		pBt[i]->SetShowTip(TRUE);
		pBt[i]->SetPngName(strPngName[i]);
		pBt[i]->SetPngImage(TRUE);

		if (!m_bAppMain)
		{
			break;
		}
	}
	
	if (m_bAppMain)
	{
		m_swBaseLock.Create(_T("锁定"), WS_CHILD | WS_VISIBLE, rcBox, this, IDC_SW_BASE_LOCK);
		m_swBaseLock.SetBackColor(RGB(45, 50, 55));
		m_swBaseLock.SetForeColor(RGB_0_255_230);
		m_swBaseLock.SetStatus(g_bUiLocked);

		COleFont ft = m_swBaseLock.GetFont();
		CY sz = ft.GetSize();
		sz.int64 = (LONGLONG)(sz.int64 * 1.1);
		sz.Lo = (unsigned long)(sz.Lo * 1.1);
		ft.SetSize(sz);
		m_swBaseLock.SetFont(ft);

		m_Menu.Create(_T("AppMain"), WS_CHILD | WS_VISIBLE, rcBox, this, IDC_MENU_APP_MAIN);
		m_Menu.SetBackColor(RGB(40, 50, 55));
		m_Menu.SetForeColor(RGB_0_255_230);
		m_Menu.SetBorderColor2(RGB(0, 192, 192));
		m_Menu.SetPopUpBackColor(RGB(28, 87, 92));
		m_Menu.SetHotKeyFont(_T("LcdD,14pt"));
	}

	if (m_bChildWnd)
	{
		m_BtBaseClose.ShowWindow(SW_HIDE);
	}
	else
	{
		m_FlashTimer.SetTimer(this, TMR_TOP_LEFT_FLASH, 300);

		pBt[0] = &m_BtBaseOk;
		pBt[1] = &m_BtBaseCancel;

		strCaptions[0] = _T("确定");
		strCaptions[1] = _T("取消");

		for (int i = 0; i < 2; i++)
		{
			GetDlgItem(i + 1)->GetWindowRect(rcBox);
			ScreenToClient(rcBox);

			pBt[i]->Create(strCaptions[i], WS_CHILD | WS_VISIBLE, rcBox, this, IDC_BT_BASE_OK + i);
			pBt[i]->SetShowBorder(TRUE);
			pBt[i]->SetBackColor(m_rgbKey);
			pBt[i]->SetBlendColor(m_rgbKey);
			pBt[i]->SetForeColor(RGB_0_255_230);
			pBt[i]->SetBorderColor(RGB_0_255_230);
			pBt[i]->SetHoverBkColor(RGB_BT_HOVER_BACK);
			pBt[i]->SetTipBackColor(RGB_ALL_64);
			pBt[i]->SetShowTip(FALSE);
			pBt[i]->SetPngImage(FALSE);

			COleFont ft = pBt[i]->GetFont();
			ft.SetName(_T("微软雅黑"));
			CY sz = ft.GetSize();
			sz.int64 = (LONGLONG)(sz.int64 * 1.15);
			sz.Lo = (unsigned long)(sz.Lo * 1.15);
			ft.SetSize(sz);

			pBt[i]->SetFont(ft);
		}

		if (nullptr == m_TopBlur.GetSafeHwnd())
		{
			_AddBlurInfo(this, m_strTitle);

			m_TopBlur.m_strCaption = m_strTitle;
			m_TopBlur.m_strCaption.Insert(0, _T("TopBlur-"));
			m_TopBlur.Create(IDD_TOPBLUR, this);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CAppBase::DrawTitle(CDC * pDC, CRect rcDraw)
{
	if (m_bChildWnd)
	{
		return;
	}

	Graphics graph(pDC->GetSafeHdc());
	graph.SetCompositingQuality(CompositingQualityHighQuality);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);

	Pen pen(RGB2ARGB(255, RGB_0_255_230), 1);
	SolidBrush brush(RGB2ARGB(255, RGB_BT_LAYER_BACK));

	CString strWnd;

	if (m_strTitle.IsEmpty())
	{
		GetWindowText(strWnd);
	}
	else
	{
		strWnd = m_strTitle;
	}

	// 绘图路径
	GraphicsPath path;

	CGdiplusText GdiText;
	RectF rcText = GdiText.MeasureString(pDC, strWnd, m_strTitleFont, m_fTitleFontSize);

	if (rcText.Width < 164)
	{
		rcText.Width = 164;
	}

	// 保存绘图路径

	const int nLeftSpace = 32;
	const int nRightSpace = 140 + m_bAppMain * 64;
	const int nHeight = 35;
	const int nTop = 3;

	m_nMenuStart = nLeftSpace + nHeight;

	if (FALSE == m_bNoMenu)
	{
		path.AddLine((int)rcText.Width + nLeftSpace, nTop, (int)rcText.Width + nLeftSpace + nHeight, nHeight + nTop);
		path.AddLine((int)rcText.Width + nLeftSpace + nHeight, nHeight + nTop, rcDraw.right - nRightSpace - nHeight, nHeight + nTop);
		path.AddLine(rcDraw.right - nRightSpace - nHeight, nHeight + nTop, rcDraw.right - nRightSpace, nTop);

		DrawTarget(&graph, &pen, &brush, &path);
	}	

	GdiText.DrawText(pDC, CPoint(12, 9), strWnd, m_strTitleFont, m_fTitleFontSize, RGB2ARGB(255, m_rgbTitle));
}


void CAppBase::DrawElement(CDC * pDC, CRect rcDraw)
{
	return;
}


void CAppBase::DrawLine(CDC * pDC, CRect rcDraw)
{
	const int nLines = m_vUiLines.size();

	if (nLines)
	{
		Graphics graph(pDC->GetSafeHdc());
		graph.SetCompositingQuality(CompositingQualityHighQuality);
		graph.SetSmoothingMode(SmoothingModeAntiAlias);

		for (int i = 0; i < nLines; i++)
		{
			const Pen p(RGB2ARGB(255, m_vUiLines[i].rgbLine), (REAL)m_vUiLines[i].nWidth);

			graph.DrawLine(&p,
				Point(m_vUiLines[i].pt1.x, m_vUiLines[i].pt1.y),
				Point(m_vUiLines[i].pt2.x, m_vUiLines[i].pt2.y));
		}
	}
}


void CAppBase::LockCtrls(int nLock)
{
	return;
}


void CAppBase::PreWndDestroy(void)
{
	return;
}


BOOL CAppBase::ServerMsgPorc(CSocket * pUser, CNewPtr<char> * pGets, CNewPtr<char> * pAns)
{
	TRACE(_T("\nServer get message."));
	return TRUE;
}


BOOL CAppBase::ClientMsgPorc(CAppBase * pUser, CNewPtr<char> * pGets)
{
	TRACE(_T("\nClient get message."));
	return TRUE;
}


BOOL CAppBase::ShowWindow(int nCmdShow)
{
	if (nCmdShow)
	{
		m_bClickShow = TRUE;
	}

	const BOOL bRetVal = CDialogEx::ShowWindow(nCmdShow);

	if (nCmdShow)
	{
		BringWindowToTop();
	}

	return bRetVal;
}


CWnd * CAppBase::GetOwner(void)
{
	return m_pOwner;
}


void CAppBase::SetOwner(CWnd * pOwner)
{
	m_pOwner = pOwner;
}


CString CAppBase::GetTitle(void)
{
	return m_strTitle;
}


void CAppBase::SetTitle(const CString & strTitle)
{
	m_strTitle = strTitle;

	if (m_bChildWnd)
	{
		return;
	}

	if (nullptr != GetSafeHwnd())
	{
		CSyncCtrl s(&m_seTitle);

		SetWindowText(strTitle);

		CRect rcClient;
		GetClientRect(rcClient);

		CClientDC dc(this);

		CGdiplusText GdiText;
		RectF rcText = GdiText.MeasureString(&dc, m_strTitle, m_strTitleFont, m_fTitleFontSize);

		rcText.Width = (REAL)(rcClient.Width() - 80);

		if (rcText.Width < 164)
		{
			rcText.Width = 164;
		}

		if (rcText.Height < 30)
		{
			rcText.Height = 30;
		}

		InvalidateRect(CRect(0, 0, (LONG)rcText.Width + 16, (LONG)rcText.Height + 16), FALSE);
	}
}


COLORREF CAppBase::GetTitleColor(void)
{
	return m_rgbTitle;
}


void CAppBase::SetTitleColor(COLORREF rgbTitle)
{
	m_rgbTitle = rgbTitle;
}


CString CAppBase::GetTitleFont(void)
{
	return m_strTitleFont;
}


REAL CAppBase::GetTitleFontSize(void)
{
	return m_fTitleFontSize;
}


void CAppBase::SetTitleFont(CString strFont, REAL fSize)
{
	m_strTitleFont = strFont;
	m_fTitleFontSize = fSize;
}


void CAppBase::LockStatus(BOOL bLock)
{
#ifdef _DEBUG
	m_bStatusLock = FALSE;
#else
	m_bStatusLock = bLock;
#endif // _DEBUG

	if (nullptr != GetSafeHwnd())
	{
		PostMessage(gMsgAppBaseLockUi, APP_LOCK_AUTO);
	}
}


void CAppBase::AdjustMenuPos(void)
{
	CString strText;
	GetWindowText(strText);

	CRect rcWnd;
	GetClientRect(rcWnd);

	CClientDC dc(this);
	CGdiplusText GdiText;
	const RectF rcText = GdiText.MeasureString(&dc, strText, GetTitleFont(), GetTitleFontSize());

	const int nStart = max(164, (int)rcText.Width) + 80;
	const int nEnd = rcWnd.Width() - nStart - 320 - m_bAppMain * 64;

	m_Menu.MoveWindow(nStart, 4, nEnd, 30, TRUE);

	InvalidateRect(CRect(0, 0, nStart + 240, 48), TRUE);
}


void CAppBase::DrawTarget(Graphics * pGraph, Pen * pen, Brush * pBrush, GraphicsPath * pPath)
{
	if (nullptr != pen)
	{
		pGraph->DrawPath(pen, pPath);
	}

	if (nullptr != pBrush)
	{
		pGraph->FillPath(pBrush, pPath);
	}
}


BOOL CAppBase::AddLine(const CString & strName, int nIndex, CPoint pt1, CPoint pt2, COLORREF rgbLine, int nWidth, int nStyle)
{
	if (strName.IsEmpty())
	{
		return FALSE;
	}

	TP_UI_LINE line;
	line.strName = strName;
	line.nNum = nIndex;
	line.nStytle = nStyle;
	line.pt1 = pt1;
	line.pt2 = pt2;
	line.nWidth = nWidth;
	line.rgbLine = rgbLine;

	const int nLines = m_vUiLines.size();

	for (int i = nLines - 1; i >= 0; i--)
	{
		if (m_vUiLines[i].strName == strName)
		{
			return FALSE;
		}

		if (m_vUiLines[i].nNum == nIndex)
		{
			return FALSE;
		}
	}

	m_vUiLines.push_back(line);

	return TRUE;
}


BOOL CAppBase::DeleteLine(const CString & strName)
{
	const int nLines = m_vUiLines.size();

	for (int i = nLines - 1; i >= 0; i--)
	{
		if (m_vUiLines[i].strName == strName)
		{
			m_vUiLines.erase(m_vUiLines.begin() + i);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CAppBase::DeleteLine(int nLine)
{
	const int nLines = m_vUiLines.size();

	for (int i = nLines - 1; i >= 0; i--)
	{
		if (m_vUiLines[i].nNum == nLine)
		{
			m_vUiLines.erase(m_vUiLines.begin() + i);

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CAppBase::SendUdpMsg(const CString & strIp, USHORT nPort,
				char * pSend, int & nLength, char * pAns/* = nullptr*/, int nWaitTime/* = 3000*/)
{
	if (INVALID_SOCKET == m_Socket)
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 2);
		WSAStartup(wVersionRequested, &wsaData);

		m_Socket = socket(AF_INET, SOCK_DGRAM, 0);

		USES_CONVERSION;
		m_SockAddr.sin_addr.S_un.S_addr = inet_addr(W2A(strIp));
		m_SockAddr.sin_family = AF_INET;
		m_SockAddr.sin_port = htons(nPort);
	}

	if (nullptr != pAns)
	{
		m_bNetAnswered = FALSE;
		m_NetBuffer.ReNew(m_nUdpMemSize, 0);
	}

	const int nBytes = sendto(m_Socket, pSend, nLength, 0, (SOCKADDR*)&m_SockAddr, sizeof(SOCKADDR));
	//////////////////////////////////////////////////////////////////////////

	if (SOCKET_ERROR == nBytes)
	{
#ifdef _DEBUG
		CString strInfo;
		FORMAT_ERR_INFO(&strInfo);
#endif // _DEBUG

		return FALSE;
	}

	if (nullptr != pAns)
	{
		m_pNetThread = AfxBeginThread(_ReceiveUdpAnsThread, this);

		int i = 0;

		nWaitTime /= 10;

		while (!m_bNetAnswered)
		{
			if (i > nWaitTime)
			{
				return FALSE;
			}

			Sleep(10);

			i++;
		}

		nLength = m_NetBuffer.m_nSize;
		memcpy(pAns, m_NetBuffer.m_pNew, nLength);
	}

	return TRUE;
}


BOOL CAppBase::SendUdpMsg(const CString & strIp, USHORT nPort, CNewPtr<char> * pSend,
						  CNewPtr<char> * pAns/* = nullptr*/, int nWaitTime/* = 3000*/)
{
	char *pTmp = nullptr;

	if (nullptr != pAns)
	{
		pAns->ReNew(m_nUdpMemSize, 0);
		pTmp = pAns->m_pNew;
	}

	int nLength = pSend->m_nSize;

	const BOOL bRetVal = SendUdpMsg(strIp, nPort, pSend->m_pNew, nLength, pTmp, nWaitTime);

	if (bRetVal)
	{
		pAns->m_nSize = nLength;
	}

	return bRetVal;
}


BOOL CAppBase::SendUdpMsg(const CString & strIp, USHORT nPort, const CString & strSend,
						  CString * pAns/* = nullptr*/, int nWaitTime/* = 3000*/)
{
	USES_CONVERSION;

	BOOL bRetVal = FALSE;

	char *pSend = W2A(strSend);
	int nLength = strlen(pSend);

	char *pTmp = nullptr;

	if (nullptr == pAns)
	{
		bRetVal = SendUdpMsg(strIp, nPort, pSend, nLength, nullptr, nWaitTime);
	}
	else
	{
		CNewPtr<char> NewPtr(m_nUdpMemSize, 0);

		bRetVal = SendUdpMsg(strIp, nPort, pSend, nLength, NewPtr.m_pNew, nWaitTime);

		if (bRetVal)
		{
			*pAns = NewPtr.m_pNew;
		}
	}

	return bRetVal;
}


void CAppBase::_PreShowWnd(void)
{
	return;
}


BOOL CAppBase::_LoadFileEx(void)
{
	return TRUE;
}


void CAppBase::_DefaultFocus(void)
{
	if ((FALSE == m_bChildWnd) && IsKindOf(RUNTIME_CLASS(CTpLayerWnd)))
	{
		if (m_BtBaseCancel.IsWindowVisible())
		{
			m_BtBaseCancel.SetFocus();
		}
	}
}


void CAppBase::_SetWndConer(void)
{
	CRgn rgn;
	m_bCanMove = TRUE;

	CRect rcWnd;
	GetWindowRect(rcWnd);
	rcWnd.MoveToXY(0, 0);

	CPoint pt[] = {CPoint(rcWnd.left + 7, rcWnd.top), CPoint(rcWnd.right - 7, rcWnd.top),
		CPoint(rcWnd.right, rcWnd.top + 7), CPoint(rcWnd.right, rcWnd.bottom - 7),
		CPoint(rcWnd.right - 7, rcWnd.bottom), CPoint(rcWnd.left + 7, rcWnd.bottom),
		CPoint(rcWnd.left, rcWnd.bottom - 7), CPoint(rcWnd.left, rcWnd.top + 7),
		CPoint(rcWnd.left + 7, rcWnd.top)};

	rgn.CreatePolygonRgn(pt, 9, WINDING);
	SetWindowRgn(rgn, TRUE);
}


void CAppBase::_SetRegister(BOOL bState)
{
	g_bRegistered = bState;
}


BOOL CAppBase::_IsRegistered(void)
{
#ifdef DEBUG
	return TRUE;
#else
	return TRUE;
#endif // DEBUG
}


BOOL CAppBase::_IsCtrlDown(void)
{
	const BOOL bCtrlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;

	return bCtrlDown;
}


BOOL CAppBase::_IsShfitDown(void)
{
	const BOOL bCtrlDown = GetAsyncKeyState(VK_SHIFT) & 0x8000;

	return bCtrlDown;
}


void CAppBase::_SetFireMode(int nMode)
{
	g_nFireMode = nMode;
}


int CAppBase::_GetFireMode(void)
{
	return g_nFireMode;
}


int CAppBase::_Rand(int nMin, int nMax)
{
	if (nMin > nMax)
	{
		std::swap(nMin, nMax);
	}

	srand((unsigned)time(nullptr) + rand());

	const int nRand = rand() % (nMax - nMin + 1) + nMin;

	return nRand;
}


void CAppBase::_DelayUs(long nTime)
{
	LARGE_INTEGER lTemp;

	LONGLONG QStart = 0;
	LONGLONG QEnd = 0;
	double dFreq = 0;
	double dTime = 0;

	QueryPerformanceFrequency(&lTemp);
	dFreq = (double)lTemp.QuadPart;

	QueryPerformanceCounter(&lTemp);
	QStart = lTemp.QuadPart;

	do
	{
		QueryPerformanceCounter(&lTemp);
		QEnd = lTemp.QuadPart;
		dTime = (double)(QEnd - QStart) / dFreq;
		dTime *= 1000000;
	} while (dTime < nTime);
}


void CAppBase::_DelayMs(long nTime)
{
	LARGE_INTEGER lTemp;

	LONGLONG QStart = 0;
	LONGLONG QEnd = 0;
	double dFreq = 0;
	double dTime = 0;

	QueryPerformanceFrequency(&lTemp);
	dFreq = (double)lTemp.QuadPart;

	QueryPerformanceCounter(&lTemp);
	QStart = lTemp.QuadPart;

	do
	{
		QueryPerformanceCounter(&lTemp);
		QEnd = lTemp.QuadPart;
		dTime = (double)(QEnd - QStart) / dFreq;
		dTime *= 1000;
	} while (dTime < nTime);
}


CRect CAppBase::_GetScaledRect(int nWidth, int nHeight, CRect rcDraw, BOOL bTopLeft)
{
	CRect rcScale;

	const double dWidth	= (double)nWidth;
	const double dHeight = (double)nHeight;

	if (dWidth / dHeight <= (double)rcDraw.Width() / rcDraw.Height())
	{
		const double dScale = dHeight / rcDraw.Height();
		const int nWidth	= (int)(dWidth / dScale + 0.5);
		const int nSpace	= (int)((rcDraw.Width() - nWidth) * 0.5 + 0.5);
		rcScale = CRect(rcDraw.left + nSpace, rcDraw.top, rcDraw.right - nSpace, rcDraw.bottom);

		if (bTopLeft)
		{
			rcScale.OffsetRect(CPoint(-nSpace, 0));
		}
	}
	else
	{
		const double dScale = dWidth / rcDraw.Width();
		const int nHeight	= (int)(dHeight / dScale + 0.5);
		const int nSpace	= (int)((rcDraw.Height() - nHeight) * 0.5 + 0.5);
		rcScale = CRect(rcDraw.left, rcDraw.top + nSpace, rcDraw.right, rcDraw.bottom - nSpace);

		if (bTopLeft)
		{
			rcScale.OffsetRect(CPoint(0, -nSpace));
		}
	}

	return rcScale;
}


BOOL CAppBase::_GetLockState(int nLock, int nLevel)
{
	BOOL bUiLocked = FALSE;

	if (nLock < 0)
	{
		bUiLocked = _IsUiLocked() || (_GetPsdLeve() < nLevel);
	}
	else
	{
		bUiLocked = (nLock > 0) || (_GetPsdLeve() < nLevel);
	}

	return bUiLocked;
}


BOOL CAppBase::_IsUiLocked(void)
{
#ifdef _DEBUG
	return FALSE;
#else
	return g_bUiLocked || m_bStatusLock;
 #endif // _DEBUG	
}


void CAppBase::_LockAllUis(BOOL bLock)
{
	g_bUiLocked = bLock;

	const int nSize = g_vBaseInfo.size();

	for (int i = 0; i < nSize; i++)
	{
		const BASE_BLUR_INFO *pInfo = &g_vBaseInfo[i];
		CWnd *pWnd = pInfo->pWnd;

		if ((nullptr != pWnd) && (nullptr != pWnd->GetSafeHwnd()))
		{
			pWnd->PostMessage(gMsgAppBaseLockUi, bLock, (LPARAM)this);
		}
	}
}


int CAppBase::_GetPsdLeve(void)
{
#ifdef _DEBUG
	return PSD_LEVEL_MGR;
#else
	return g_nPsdLevel;
#endif // _DEBUG	
}


void CAppBase::_SetPsdLeve(int nLevel)
{
	g_nPsdLevel = nLevel;
}


BOOL CAppBase::_IsPsdLevel(int nLevel)
{
	return (g_nPsdLevel >= nLevel);
}


void CAppBase::_AddModifyLog(const CString & strLogInfo)
{
}


void CAppBase::_AutoDeleteTstImages(int nHoldDays)
{
	m_nHoldDays = nHoldDays;

	AfxBeginThread(__DeleteTstImgThread, this);
}


void CAppBase::_DeleteOldestImages(void)
{
	AfxBeginThread(__DeleteOldestThread, this);
}


int CAppBase::_GetDiskFreeSpace(const CString & strPath)
{
	const int nPos = strPath.Find(':');
	
	if (nPos < 1)
	{
		return 0;
	}

	const CString strRoot = strPath.Left(nPos + 1);

	int nSpace = 0;

	WCHAR Drive[MAX_PATH] = {0};				// 初始化数组用以存储盘符信息
	GetLogicalDriveStrings(MAX_PATH, Drive);	// 获得本地所有盘符存在Drive数组中

	CString strDrive;

	int i = 0;
	while ('\0' != Drive[i - 1] || '\0' != Drive[i])	// 搜索数组尝试得到盘符信息，如果两者皆否，则数组搜索到尽头
	{
		strDrive = L"";
		strDrive = Drive[i];

		strDrive = strDrive + L":";	// 将盘符信息的格式调整为像“C:”这种格式，以作为GetDiskFreeSpaceEx函数的第一个参数
		i += 4;

		if (strRoot != strDrive)
		{
			continue;
		}

		ULARGE_INTEGER nFreeBytesAvailable;
		ULARGE_INTEGER nTotalNumberOfBytes;
		ULARGE_INTEGER nTotalNumberOfFreeBytes;

		// 利用GetDiskFreeSpaceEx函数获得指定磁盘信息
		if (GetDiskFreeSpaceEx(strDrive,
			&nFreeBytesAvailable,
			&nTotalNumberOfBytes,
			&nTotalNumberOfFreeBytes))
		{
			const long nWholespace = (long)(nTotalNumberOfBytes.QuadPart / 1024 / 1024);	// 得到磁盘总空间，单位为MB
			const long nFreespace = (long)(nTotalNumberOfFreeBytes.QuadPart / 1024 / 1024);

			nSpace = (int)nFreespace;
		}
	}

	return nSpace;
}


void CAppBase::_SendBlurMsg(BOOL bShow)
{
	CSyncCtrl s(&g_seBaseInfo);

	const int nSize = g_vBaseInfo.size();

	for (int i = 0; i < nSize; i++)
	{
		const BASE_BLUR_INFO *pInfo = &g_vBaseInfo[i];
		CWnd *pWnd = pInfo->pWnd;

		if (pWnd == this)
		{
			continue;
		}
		
		if ((nullptr != pWnd) && (nullptr != pWnd->GetSafeHwnd()))
		{
			pWnd->PostMessage(gMsgSemiTransBlur, bShow, (LPARAM)this);
		}
	}
}


void CAppBase::_AddBlurInfo(CWnd * pDstWnd, const CString & strWnd)
{
	CSyncCtrl s(&g_seBaseInfo);

	BOOL bFind = FALSE;

	const int nSize = g_vBaseInfo.size();

	for (int i = 0; i < nSize; i++)
	{
		const BASE_BLUR_INFO *pInfo = &g_vBaseInfo[i];

		if (pInfo->pWnd == this)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		BASE_BLUR_INFO Bi;

		Bi.pWnd = this;
		Bi.strInfo = strWnd;
		Bi.nShowCmd = SW_SHOWNORMAL;

		g_vBaseInfo.push_back(Bi);
	}
}


void CAppBase::_RemoveBlurInfo(CWnd * pDstWnd)
{
	CSyncCtrl s(&g_seBaseInfo);

	const int nSize = g_vBaseInfo.size();

	for (int i = 0; i < nSize; i++)
	{
		const BASE_BLUR_INFO *pInfo = &g_vBaseInfo[i];

		if (pInfo->pWnd == this)
		{
			g_vBaseInfo.erase(g_vBaseInfo.begin() + i);

			break;
		}
	}
}


std::vector<CString> CAppBase::_GetSegInfo(const CString & strInfo, TCHAR nMark)
{
	CString strSrc = strInfo;

	std::vector<CString> vRetVal;
	int nPos = -1;

LABEL_RESTART:
	nPos = strSrc.Find(nMark);

	if (nPos >= 0)
	{
		const CString strTmp = strSrc.Left(nPos);
		vRetVal.push_back(strTmp);

		strSrc.Delete(0, nPos + 1);

		goto LABEL_RESTART;
	}
	else if (!strSrc.IsEmpty())
	{
		vRetVal.push_back(strSrc);
	}

	return vRetVal;
}


std::vector<CString> CAppBase::_GetSegInfoEx(const CString & strInfo, int nMarks, ...)
{
	va_list ap = nullptr;
	va_start(ap, nMarks);

	CNewPtr<char> NewPtr(nMarks);

	for (int i = 0; i < nMarks; i++)
	{
		NewPtr[i] = va_arg(ap, char);
	}

	va_end(ap);

	CString strSrc = strInfo;

	std::vector<CString> vRetVal;

	const int nLength = strInfo.GetLength();

	int k = 0;

	for (int i = 0; i < nLength; i++)
	{
		BOOL bFind = FALSE;

		for (int j = 0; j < nMarks; j++)
		{
			if (strInfo[i] == NewPtr[j])
			{
				bFind = TRUE;
				break;
			}
		}

		if (bFind)
		{
			const CString strTmp = strInfo.Mid(k, i - k);

			if (FALSE == strTmp.IsEmpty())
			{
				vRetVal.push_back(strTmp);
			}

			k = i + 1;
		}
	}

	if (k < nLength)
	{
		const CString strTmp = strInfo.Mid(k, nLength - k);

		if (FALSE == strTmp.IsEmpty())
		{
			vRetVal.push_back(strTmp);
		}
	}

	return vRetVal;
}


std::vector<int> CAppBase::_GetIndex(CString strText, TCHAR nMark)
{
	_DeleteSpace(strText);

	std::vector<int> vRetVal;

	std::vector<CString> vValues = _GetSegInfo(strText, nMark);

	const int nValues = vValues.size();

	for (int i = 0; i < nValues; i++)
	{
		const int nLine = vValues[i].Find('-');

		if (nLine >= 0)
		{
			const CString strStart = vValues[i].Left(nLine);

			const int nStart = _ttoi(strStart);

			vValues[i].Delete(0, nLine + 1);
			const CString &strEnd = vValues[i];

			const int nEnd = _ttoi(strEnd);

			for (int j = nStart; j <= nEnd; j++)
			{
				vRetVal.push_back(j);
			}
		}
		else
		{
			const int nValue = _ttoi(vValues[i]);

			vRetVal.push_back(nValue);
		}
	}

	return vRetVal;
}


CString CAppBase::_GetKilobitStr(int nData)
{
	CString strRetVal;
	strRetVal.Format(_T("%d"), nData);

	strRetVal.MakeReverse();

	const int nLength = strRetVal.GetLength();

	for (int i = nLength - (nLength % 3); i >= 3; i -= 3)
	{
		if (nLength > 3)
		{
			strRetVal.Insert(i, ',');
		}
	}

	strRetVal.MakeReverse();

	if (',' == strRetVal[0])
	{
		strRetVal.Delete(0, 1);
	}

	return strRetVal;
}


int CAppBase::_FindNonnumeric(const CString & strData, int nIncludes, ...)
{
	va_list ap = nullptr;
	va_start(ap, nIncludes);

	std::vector<int> vInclude;

	for (int i = 0; i < nIncludes; i++)
	{
		const int nTmp = va_arg(ap, int);
		vInclude.push_back(nTmp);
	}

	va_end(ap);

	int nPlus = 0;
	int nMinus = 0;
	int nDots = 0;

	const int nLength = strData.GetLength();

	for (int i = 0; i < nLength; i++)
	{
		if (strData[i] < '0' || strData[i] > '9')
		{
			BOOL bFind = FALSE;

			for (int j = 0; j < nIncludes; j++)
			{
				if (strData[i] == vInclude[j])
				{
					bFind = TRUE;

					if ('+' == strData[i])
					{
						nPlus++;

						if (i > 0)
						{
							return strData[i];
						}
					}
					else if ('-' == strData[i])
					{
						nMinus++;

						if (i > 0)
						{
							return strData[i];
						}
					}
					else if ('.' == strData[i])
					{
						nDots++;

						if (0 == i)
						{
							return strData[i];
						}
						else if (nMinus || nPlus)
						{
							if (i == 1)
							{
								return strData[i];
							}
						}
						else if (nDots >= 2)
						{
							return strData[i];
						}
					}

					break;
				}
			}

			if (!bFind)
			{
				return strData[i];
			}
		}
	}

	return 0;
}


void CAppBase::_DeleteSpace(CString & strData)
{
	const int nLength = strData.GetLength();

	for (int i = nLength - 1; i >= 0; i--)
	{
		if (' ' == strData[i])
		{
			strData.Delete(i);
		}
	}
}


void CAppBase::_DeleteZeros(CString & strData)
{
	const int nLength = strData.GetLength();

	for (int i = nLength - 1; i >= 0; i--)
	{
		if ('0' == strData[i])
		{
			strData.Delete(i, 1);
		}
		else
		{
			if ('.' == strData[i])
			{
				strData.Delete(i, 1);
			}

			break;
		}
	}
}


int CAppBase::_HexStr2int(CString strHex)
{
	_DeleteSpace(strHex);

	strHex.MakeLower();

	const CString strHead = strHex.Left(2);

	int nOfst = 0;

	if (_T("0x") == strHead)
	{
		nOfst = 2;
	}

	int nRetVal = 0;

	const int nLength = strHex.GetLength();

	for (int i = nOfst; i < nLength; i++)
	{
		int nTmp = 0;

		if (strHex[i] >= 'a' && strHex[i] <= 'f')
		{
			nTmp = strHex[i] - 'a' + 10;
		}
		else if (strHex[i] >= '0' && strHex[i] <= '9')
		{
			nTmp = strHex[i] - '0';
		}
		else
		{
			continue;
		}

		nRetVal <<= 4;
		nRetVal += nTmp;
	}

	return nRetVal;
}


int CAppBase::_IsHexMode(const CString & strData)
{
	int nHexMode = 0;

	if ('0' == strData[0] && ((strData[0] | 32) == 'x'))
	{
		nHexMode = 1;
	}
	else
	{
		const int nLenTmp = strData.GetLength();

		for (int i = 0; i < nLenTmp; i++)
		{
			if (strData[i] < '0' || strData[i] > '9')
			{
				const char nLower = (strData[i] | 32);

				if (nLower < 'a' || nLower > 'f')
				{
					return -1;
				}
				else
				{
					nHexMode = 2;
				}					
			}
		}
	}

	return nHexMode;
}


BOOL CAppBase::_SaveOkNgPath(void)
{
	const CString strPath = _GetAppIniPath();
	const CString strItem = _T("Img_Auto_Save");

	WritePrivateProfileString(strItem, _T("Save_Path"), m_AutoSave.strSavePath, strPath);
	WritePrivateProfileString(strItem, _T("Save_OK"), m_AutoSave.bPassAutoSave? _T("1"): _T("0"), strPath);
	WritePrivateProfileString(strItem, _T("Save_NG"), m_AutoSave.bFailAutoSave? _T("1"): _T("0"), strPath);

	return TRUE;
}


BOOL CAppBase::_LoadOkNgPath(void)
{
	const CString strPath = _GetAppIniPath();
	const CString strItem = _T("Img_Auto_Save");

	GetPrivateProfileString(strItem, _T("Save_Path"), m_Menu.m_strDefaultPath,
		m_AutoSave.strSavePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH, strPath);
	m_AutoSave.strSavePath.ReleaseBuffer();

	m_AutoSave.bPassAutoSave = GetPrivateProfileInt(strItem, _T("Save_OK"), 1, strPath);
	m_AutoSave.bFailAutoSave = GetPrivateProfileInt(strItem, _T("Save_NG"), 1, strPath);

	return TRUE;
}


CString CAppBase::_GetTstImgSavePath(BOOL bResult, const CString & strPass, const CString & strFail)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strDate;
	strDate.Format(_T("%d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);

	const CString strFileName = _GetFileName();

	CString strPath;
	strPath.Format(_T("%s\\%s\\%s"), m_AutoSave.strSavePath, strFileName, strDate);

	_CreateFileDir(strPath);
	
	if (bResult && m_AutoSave.bPassAutoSave)
	{
		strPath.Append(_T("\\"));
		strPath.Append(strPass);
	}
	else if ((!bResult) && m_AutoSave.bFailAutoSave)
	{
		strPath.Append(_T("\\"));
		strPath.Append(strFail);
	}

	return strPath;
}


BOOL CAppBase::_SaveFile(const CString & strType, const CString & strExt, BOOL bSaveAs)
{
	if (bSaveAs || m_strFilePath.IsEmpty())
	{
		CString strDefault = strExt;
		strDefault.Insert(0, _T("*."));

		CString strFilter;
		strFilter.Format(_T("%s Files(*.%s)|*.%s|All Files(*.*)|*.*||"), strType, strExt, strExt);

		CFileDialog dlgFile(FALSE, strDefault, _T("无标题"), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, strFilter, this);

		dlgFile.m_pOFN->lpstrTitle = _T("文件另存为");

		if (IDOK == dlgFile.DoModal())
		{
			m_strFilePath = dlgFile.GetPathName();
		}
		else
		{
			return TRUE;
		}
	}

	CFile file;
	CFileException FileEx;

	if (!file.Open(m_strFilePath, CFile::modeWrite | CFile::modeCreate, &FileEx))
	{
		FileEx.ReportError();
		return FALSE;
	}

	CArchive ar(&file, CArchive::store);

	Serialize(ar);

	ar.Flush();
	ar.Close();
	file.Close();

	_SetTitleFromPath();

	m_Menu.AddRecentFile(m_strFilePath);
	m_Menu.SaveRecentFile();

	_FileDirty(FALSE);

	ShowMsgBox(_T("保存成功!"), _T("保存"), MB_OK | MB_ICONINFORMATION);

	return TRUE;
}


BOOL CAppBase::_LoadFile(const CString & strType, const CString & strExt, BOOL bRecent)
{
	if (_IsFileDirty())
	{
		CMsgBox MsgBox(this);

		if (IDYES == MsgBox.ShowMsg(_T("是否保存当前文档?"), _T("打开"), MB_YESNO | MB_ICONQUESTION))
		{
			if (!_SaveFile(strType, strExt, FALSE))
			{
				return FALSE;
			}
		}
	}

	if (!bRecent)
	{
		CString strDefault = strExt;
		strDefault.Insert(0, _T("*."));

		CString strFilter;
		strFilter.Format(_T("%s Files(*.%s)|*.%s|All Files(*.*)|*.*||"), strType, strExt, strExt);

		CFileDialog dlgFile(TRUE, strDefault, nullptr, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, strFilter, this);

		dlgFile.m_pOFN->lpstrTitle = _T("打开文件");

		if (IDOK == dlgFile.DoModal())
		{
			CString strPath = dlgFile.GetPathName();

			if (m_strFilePath == strPath)
			{
				return TRUE;
			}
			else
			{
				m_strFilePath = strPath;
			}
		}
		else
		{
			return TRUE;
		}
	}

	if (m_strLastFile == m_strFilePath)
	{
		return TRUE;
	}

	m_strLastFile = m_strFilePath;

	CFile file;
	CFileException FileEx;

	if (!file.Open(m_strFilePath, CFile::modeRead, &FileEx))
	{
		FileEx.ReportError();

		m_Menu.RemoveRecentFile(m_strFilePath);
		return FALSE;
	}

	CArchive ar(&file, CArchive::load);

	BeginWaitCursor();

	Serialize(ar);

	ar.Flush();
	ar.Close();
	file.Close();

	EndWaitCursor();

	_SetTitleFromPath();

	m_Menu.AddRecentFile(m_strFilePath);
	m_Menu.SaveRecentFile();

	_FileDirty(FALSE);

#ifdef _DEBUG
	_LockAllUis(FALSE);
	m_swBaseLock.SetStatus(FALSE);
#else
	_LockAllUis(TRUE);
	m_swBaseLock.SetStatus(TRUE);
#endif // _DEBUG

	return TRUE;
}


BOOL CAppBase::_LoadFile(const CString & strType, const CString & strExt, const CString & strFile)
{
	if (_IsFileDirty())
	{
		CMsgBox MsgBox(this);

		if (IDYES == MsgBox.ShowMsg(_T("是否保存当前文档?"), _T("打开"), MB_YESNO | MB_ICONQUESTION))
		{
			if (!_SaveFile(strType, strExt, FALSE))
			{
				return FALSE;
			}
		}
	}

	CFile file;
	CFileException FileEx;

	CString strFullPath = strFile;
	strFullPath.Insert(0, _T("\\"));
	strFullPath.Append(_T(".") + strExt);

	m_strFilePath = m_Menu.m_strDefaultPath + strFullPath;

	if (m_strLastFile == m_strFilePath)
	{
		return TRUE;
	}

	m_strLastFile = m_strFilePath;

	if (!file.Open(m_strFilePath, CFile::modeRead, &FileEx))
	{
		FileEx.ReportError();

		m_Menu.RemoveRecentFile(m_strFilePath);
		return FALSE;
	}

	CArchive ar(&file, CArchive::load);

	BeginWaitCursor();

	Serialize(ar);

	ar.Flush();
	ar.Close();
	file.Close();

	EndWaitCursor();

	_SetTitleFromPath();

	m_Menu.AddRecentFile(m_Menu.m_strDefaultPath + strFile);
	m_Menu.SaveRecentFile();

	_FileDirty(FALSE);

#ifdef _DEBUG
	_LockAllUis(FALSE);
	m_swBaseLock.SetStatus(FALSE);
#else
	_LockAllUis(TRUE);
	m_swBaseLock.SetStatus(TRUE);
#endif // _DEBUG

	_LoadFileEx();

	return TRUE;
}


BOOL CAppBase::_IsFileDirty(void)
{
#ifdef _DEBUG
	return FALSE;
#endif // _DEBUG

	return g_bFileDirty;
}


void CAppBase::_FileDirty(BOOL bDirty)
{
	g_bFileDirty = bDirty;
}


void CAppBase::_SetTitleFromPath(void)
{
	int nFind = m_strFilePath.ReverseFind('\\');

	CString strTitle = m_strFilePath;
	strTitle.Delete(0, nFind + 1);

	nFind = strTitle.ReverseFind('.');
	strTitle = strTitle.Left(nFind);

	strTitle.Append(_T(" - "));
	strTitle.Append(AfxGetAppName());
	SetTitle(strTitle);

	AdjustMenuPos();
}


void CAppBase::_SetNoTitle(void)
{
	CString strTitle = _T("无标题 - ");
	strTitle.Append(AfxGetAppName());
	SetTitle(strTitle);

	AdjustMenuPos();
}


CString CAppBase::_GetFileName(void)
{
	const int nDash = m_strTitle.ReverseFind('-');
	const CString strFileName = m_strTitle.Left(nDash - 1);

	return strFileName;
}


BOOL CAppBase::_InstallFont(const CString & strFile)
{
#ifdef _DEBUG
	return TRUE;
#else
	//安装字体
	const CString strSysPath = _GetSysFontPath() + _T("\\");
	const CString strSysFile = strSysPath + strFile;

    if (PathFileExists(strSysFile))
    {
        return TRUE;
    }

	CString strCurPath = _GetAppExePath();
	strCurPath.AppendChar('\\');
	strCurPath.Append(strFile);

	if (!CopyFile(strCurPath, strSysFile, FALSE))
	{
		return FALSE;
	}

	//添加字体到系统字体表
	const int nFontNum = AddFontResource(strSysFile);

	if (nFontNum <= 0)
	{
		return FALSE;
	}

	CRegKey FontKey;

	const CString strKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	LONG nRes = FontKey.Open(HKEY_LOCAL_MACHINE, strKey);

	if (ERROR_SUCCESS != nRes)
	{
		nRes = FontKey.Create(HKEY_LOCAL_MACHINE, strKey);

		if (ERROR_SUCCESS != nRes)
		{
			return FALSE;
		}
	}

	CString strFont = strFile;

	const int nDot = strFont.ReverseFind('.');
	strFont = strFont.Left(nDot);
	strFont.Append(_T(" (TrueType)"));

	nRes = FontKey.SetStringValue(strFont, strFile);

	if (ERROR_SUCCESS != nRes)
	{
		return FALSE;
	}

	nRes = FontKey.Close();

	if (ERROR_SUCCESS != nRes)
	{
		return FALSE;
	}

	::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

#endif

	return TRUE;
}


BOOL CAppBase::_RemoveFont(CString strFont)
{
 #ifdef _DEBUG
 	return TRUE;
 #else
	//卸载字体
	const CString strSysPath = _GetSysFontPath() + _T("\\");
	const CString strSysFile = strSysPath + strFont;

	if (!PathFileExists(strSysFile))
	{
		return TRUE;
	}

	//删除注册表
	CRegKey FontKey;

	const CString strKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	LONG nRes = FontKey.Open(HKEY_LOCAL_MACHINE, strKey);

	if (ERROR_SUCCESS != nRes)
	{
		return FALSE;
	}

	const int nDot = strFont.ReverseFind('.');
	strFont = strFont.Left(nDot);
	strFont.Append(_T(" (TrueType)"));

	nRes = FontKey.DeleteValue(strFont);

	if (ERROR_SUCCESS != nRes)
	{
		return FALSE;
	}

	nRes = FontKey.Close();
	
	if (ERROR_SUCCESS != nRes)
	{
		return FALSE;
	}

	//从系统字体表移除字体
	if (!RemoveFontResource(strSysFile))
	{
		return FALSE;
	}

	if (!DeleteFile(strSysFile))
	{
		return FALSE;
	}

	::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

#endif

	return TRUE;
}


BOOL CAppBase::_CreateFileDir(CString strPath)
{
	const std::vector<CString> vDir = _GetSegInfo(strPath, '\\');

	const int nDirs = vDir.size();

	strPath.Empty();

	for (int i = 0; i < nDirs; i++)
	{
		strPath.Append(vDir[i]);

		if (!PathIsDirectory(strPath))
		{
			if (!CreateDirectory(strPath, nullptr))
			{
				return FALSE;
			}
		}

		if (i < nDirs - 1)
		{
			strPath.AppendChar('\\');
		}
	}

	return TRUE;
}


BOOL CAppBase::_ClearFolder(CString strPath)
{
	CFileFind Finder;

	BOOL bFind = Finder.FindFile(strPath + _T("\\*.*"));

	while (bFind)
	{
		bFind = Finder.FindNextFile();

		if (Finder.IsDirectory())	// 处理文件夹中的文件
		{
			if (Finder.IsDots())
			{
				continue;
			}

			const CString strFile = Finder.GetFilePath();

			_ClearFolder(Finder.GetFilePath());		// 递归删除文件夹

			if (!RemoveDirectory(strFile))
			{
				return FALSE;
			}
		}
		else // 删除文件
		{
			const CString strFile = Finder.GetFilePath();

			if (!DeleteFile(strFile))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CAppBase::_DeleteFolder(CString strPath)
{
	if (!_ClearFolder(strPath))
	{
		return FALSE;
	}

	if (!RemoveDirectory(strPath))
	{
		return FALSE;
	}
	
	return TRUE;
}

bool __TraversalOrderL2H(const FOLDER_INFO & v1, const FOLDER_INFO & v2)
{
	return v1.strPath < v2.strPath;
}
bool __TraversalOrderH2L(const FOLDER_INFO & v1, const FOLDER_INFO & v2)
{
	return v1.strPath > v2.strPath;
}

void CAppBase::_TraversalFile(const CString & strPath, std::vector<FOLDER_INFO> & vFiles, int nOrder/* = 0*/)
{
	FOLDER_INFO Info;

	CFileFind Finder;
	BOOL bNotEmpty = Finder.FindFile(strPath + _T("\\*.*"));	// 总文件夹, 开始遍历

	while (bNotEmpty)
	{ 
		bNotEmpty = Finder.FindNextFile();	// 查找文件
		const CString strFileName = Finder.GetFilePath();	// 获取文件的路径，可能是文件夹，可能是文件

		if (Finder.IsDirectory())
		{
			// 递归遍历用户文件夹
			if (!Finder.IsDots())
			{
				Info.strPath	= strFileName;

				Info.bFolder	= TRUE;
				Info.bSysFile	= Finder.IsSystem();
				Info.bHidden	= Finder.IsHidden();
				Info.bReadOnly	= Finder.IsReadOnly();
				Info.bTemporary	= Finder.IsTemporary();

				vFiles.push_back(Info);

				_TraversalFile(strFileName + _T("\\"), vFiles);
			}
		}
		else
		{
			// 如果是文件则加入文件列表
			Info.strPath	= strFileName;

			Info.bFolder	= FALSE;
			Info.bSysFile	= Finder.IsSystem();
			Info.bHidden	= Finder.IsHidden();
			Info.bReadOnly	= Finder.IsReadOnly();
			Info.bTemporary	= Finder.IsTemporary();

			vFiles.push_back(Info);
		}
	}

	if (nOrder < 0)
	{
		sort(vFiles.begin(), vFiles.end(), __TraversalOrderL2H);
	}
	else if (nOrder > 0)
	{
		sort(vFiles.begin(), vFiles.end(), __TraversalOrderH2L);
	}
}


CString CAppBase::_GetCurTime(void)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strRetVal;
	strRetVal.Format(_T("%d-%02d-%02d %02d:%02:d%0:2d.%03d"), st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return strRetVal;
}


CString CAppBase::_GetAppExePath(void)
{
	CString strPath;
	GetModuleFileName(nullptr, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); 
	strPath.ReleaseBuffer();

	const int nPos = strPath.ReverseFind('\\');

	strPath = strPath.Left(nPos);

	return strPath;
}


CString CAppBase::_GetAppIniPath(void)
{
	CString strPath;
	GetModuleFileName(nullptr, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); 
	strPath.ReleaseBuffer();
	strPath.Replace(_T(".exe"), _T(".ini"));

	return strPath;
}


CString CAppBase::_GetSysFontPath(void)
{
	TCHAR sWinDir[MAX_PATH];

	GetWindowsDirectory(sWinDir,MAX_PATH);

	CString sFontDir(sWinDir);
	sFontDir += _T("\\Fonts");

	return sFontDir;
}


BOOL CAppBase::_RegisterActivex(BOOL bRegister)
{
#ifdef _DEBUG
	return TRUE;
#else
	std::vector<CString> vstrCtrls = _GetSegInfo(g_strBL_Regs, ',');

	const int nCtrls = vstrCtrls.size();

	for (int i = 0; i < nCtrls; i++)
	{
		const CString &strDll = vstrCtrls[i];

		HINSTANCE hLib = LoadLibrary(strDll);

		if (hLib < (HINSTANCE)HINSTANCE_ERROR)
		{
			CString strInfo = _T("载入") + strDll;
			strInfo += _T(" 失败！\n请检测当前目录是否在在该文件。");

			ShowMsgBox(strInfo, nullptr, MB_OK | MB_ICONERROR);

			return FALSE;
		}

		// 获取注册函数DllRegisterServer地址
		FARPROC lpDllEntryPoint;
		lpDllEntryPoint = GetProcAddress(hLib, bRegister? "DllRegisterServer": "DllUnregisterServer");

		// 调用注册函数DllRegisterServer
		if(nullptr != lpDllEntryPoint)
		{ 
			if (FAILED((*lpDllEntryPoint)()))
			{
				CString strInfo = _T("调用") + strDll;

				if (bRegister)
				{
					strInfo += _T(" DllRegisterServer 失败！\n请确认是否为管理员权限。");
				}
				else
				{
					strInfo += _T(" DllUnregisterServer 失败！\n请确认是否为管理员权限。");
				}

				FreeLibrary(hLib);

				ShowMsgBox(strInfo, nullptr, MB_OK | MB_ICONERROR);

				return FALSE;
			};
		}
		else
		{
			CString strInfo = _T("调用") + strDll;

			if (bRegister)
			{
				strInfo += _T(" DllRegisterServer 失败！\n请确认是否为管理员权限。");
			}
			else
			{
				strInfo += _T(" DllUnregisterServer 失败！\n请确认是否为管理员权限。");
			}

			FreeLibrary(hLib);

			ShowMsgBox(strInfo, nullptr, MB_OK | MB_ICONERROR);			

			return FALSE;
		}
	}
#endif // _DEBUG

	return TRUE;
}


BOOL CAppBase::_LoadImageFromResource(ATL::CImage *pImage, UINT nResID, LPCTSTR lpTyp)
{
	if (nullptr == pImage)
	{
		return FALSE;
	}

	pImage->Destroy();

	const HINSTANCE hInst = AfxGetResourceHandle();

	// 查找资源
	const HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nResID), lpTyp);

	if (nullptr == hRsrc)
	{
		return FALSE;
	}

	// 加载资源
	HGLOBAL hImgData = ::LoadResource(hInst, hRsrc);

	if (nullptr == hImgData)
	{
		::FreeResource(hImgData);

		return FALSE;
	}

	// 锁定内存中的指定资源
	LPVOID lpVoid = ::LockResource(hImgData);

	LPSTREAM pStream = nullptr;

	const DWORD dwSize = ::SizeofResource(hInst, hRsrc);

	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);

	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);

	::memcpy(lpByte, lpVoid, dwSize);

	// 解除内存中的指定资源
	::GlobalUnlock(hNew);

	// 从指定内存创建流对象
	const HRESULT hResult = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);

	if (S_OK != hResult)
	{
		GlobalFree(hNew);
	}
	else
	{
		// 加载图片
		pImage->Load(pStream);
		GlobalFree(hNew);
	}

	// 释放资源
	::FreeResource(hImgData);

	return TRUE;
}


UINT CAppBase::_ReceiveUdpAnsThread(LPVOID pParam)
{
	CAppBase *pBase = (CAppBase *)pParam;
	UINT nRetVul = pBase->_ReceiveUdpAnsThreadKernel();

	return nRetVul;
}


UINT CAppBase::_ReceiveUdpAnsThreadKernel(void)
{
	SOCKADDR_IN sockSvr;  
	int nLength = sizeof(SOCKADDR);

	const int nBytesRead = recvfrom(m_Socket, m_NetBuffer.m_pNew, m_NetBuffer.m_nSize, 0, (SOCKADDR*)&sockSvr, &nLength);

	if (SOCKET_ERROR == nBytesRead)
	{
#ifdef _DEBUG
		CString strInfo;
		FORMAT_ERR_INFO(&strInfo);
#else
		;
#endif // _DEBUG
	}
	else
	{
		m_NetBuffer.m_nSize = nBytesRead;
		m_bNetAnswered = TRUE;
	}

	return (UINT)nBytesRead;
}


UINT CAppBase::__DeleteTstImgThread(LPVOID pParam)
{
	CAppBase *pBase = (CAppBase *)pParam;
	UINT nRetVul = pBase->__DeleteTstImgThreadKernel();

	return nRetVul;
}


UINT CAppBase::__DeleteTstImgThreadKernel(void)
{
	if (m_nHoldDays < 0)
	{
		return 1;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	const CTime t(st);
	const CString strFileName = _GetFileName();

	const CTimeSpan tspan(m_nHoldDays, 0, 0, 0);

	const CTime day = t - tspan;

	CString strFolder;
	strFolder.Format(_T("%s\\%s\\%d-%02d-%02d"),
		m_AutoSave.strSavePath, strFileName, day.GetYear(), day.GetMonth(), day.GetDay());

	const int nPathLenth = strFolder.GetLength();

	CString strPath;
	strPath.Format(_T("%s\\%s"), m_AutoSave.strSavePath, strFileName);

	std::vector<FOLDER_INFO> vFti;
	_TraversalFile(strPath, vFti);

	const int nSize = vFti.size();

	for (int i = 0; i < nSize; i++)
	{
		FOLDER_INFO f = vFti[i];

		if (TRUE == f.bFolder && f.strPath.GetLength() == nPathLenth)
		{
			if (f.strPath <= strFolder)
			{
				_DeleteFolder(f.strPath);
			}
		}
	}

	return 0;
}


UINT CAppBase::__DeleteOldestThread(LPVOID pParam)
{
	CAppBase *pBase = (CAppBase *)pParam;
	UINT nRetVul = pBase->__DeleteOldestThreadKernel();

	return nRetVul;
}


UINT CAppBase::__DeleteOldestThreadKernel(void)
{
	CString strPath;
	strPath.Format(_T("%s\\%s"), m_AutoSave.strSavePath, _GetFileName());

	std::vector<FOLDER_INFO> vFti;

	_TraversalFile(strPath, vFti, -1);

	const int nSize = vFti.size();

	for (int i = 0; i < nSize; i++)
	{
		const FOLDER_INFO & f = vFti[i];

		if (f.bSysFile || f.bHidden)
		{
			continue;
		}

		if (TRUE == f.bFolder)
		{
			_DeleteFolder(f.strPath);

			break;
		}
	}

	return 0;
}


afx_msg LRESULT CAppBase::OnGmsgsemitransblur(WPARAM wParam, LPARAM lParam)
{
	if (m_bChildWnd)
	{
		return 0;
	}

	if (IsWindowVisible())
	{
		if (wParam)
		{
			m_nBlurCount++;
		}
		else if (m_nBlurCount)
		{
			m_nBlurCount--;
		}

		if ((1 == m_nBlurCount) && wParam)
		{
 			m_TopBlur.EnableWindow(TRUE);
 			m_TopBlur.ShowWindow(wParam);
		}
		else if ((0 == m_nBlurCount) && (!wParam))
		{
			m_TopBlur.ShowWindow(wParam);
		}
	}

	return 0;
}


afx_msg LRESULT CAppBase::OnGmsgappbaselockui(WPARAM wParam, LPARAM lParam)
{
	if (nullptr == GetSafeHwnd() || FALSE == IsWindowVisible())
	{
		return 0;
	}

	LockCtrls((int)wParam);

	return 0;
}


afx_msg LRESULT CAppBase::OnGmsgappbaseunlockui(WPARAM wParam, LPARAM lParam)
{
	CBlender<CPsdMgr> Psd(IDD_PSDMGR, this);
	Psd->m_bChangePsd = FALSE;
	Psd->SetTitle(_T("密码"));

	if (IDOK == Psd.CreateTopWnd(TRUE))
	{
		_LockAllUis(FALSE);
	}
	else
	{
		g_nPsdLevel = PSD_LEVEL_LOCK;
		m_swBaseLock.SetStatus(TRUE);
	}

	return 0;
}


afx_msg LRESULT CAppBase::OnGmsgappbasesetfocus(WPARAM wParam, LPARAM lParam)
{
	_DefaultFocus();

	return 0;
}


void CAppBase::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect(rcClient);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	dcMem.SelectObject(bmp);

	dcMem.FillSolidRect(rcClient, m_bAppMain? RGB_ALL_64: RGB_TRANS_TOP_KEY);

	DrawElement(&dcMem, rcClient);
	DrawLine(&dcMem, rcClient);

	CSyncCtrl s(&m_seTitle);

	DrawTitle(&dcMem, rcClient);

	s.Unlock();

	if (!m_bChildWnd)
	{
		Graphics graph(dcMem.GetSafeHdc());
		graph.SetCompositingQuality(CompositingQualityHighQuality);
		graph.SetSmoothingMode(SmoothingModeAntiAlias);

		//////////////////////////////////////////////////////////////////////////
		CString strIniPath;
		GetModuleFileName(nullptr, strIniPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		strIniPath.ReleaseBuffer();

		CString strOld = AfxGetAppName();
		CString strFlash;
		strFlash.Format(_T("TopLeft_%d.png"), m_nFlash);
		strIniPath.Replace(strOld + _T(".exe"), strFlash);

		TCHAR lpImageFile[MAX_PATH];
		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		Image *pImgTopLeft = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImgTopLeft && (Status::Ok == pImgTopLeft->GetLastStatus()))
		{
			graph.DrawImage(pImgTopLeft, RectF((REAL)0, (REAL)0,
				(REAL)pImgTopLeft->GetWidth(), (REAL)pImgTopLeft->GetHeight()));
		}
		//////////////////////////////////////////////////////////////////////////

		strIniPath.Replace(strFlash, _T("TopRight.png"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		Image *pImgTopRight = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImgTopRight && (Status::Ok == pImgTopRight->GetLastStatus()))
		{
			graph.DrawImage(pImgTopRight,
				RectF((REAL)(rcClient.right - pImgTopRight->GetWidth()), (REAL)0,
				(REAL)pImgTopRight->GetWidth(), (REAL)pImgTopRight->GetHeight()));
		}
		//////////////////////////////////////////////////////////////////////////

		strIniPath.Replace(_T("TopRight"), _T("BottomRight"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		Image *pImgBottomRight = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImgBottomRight && (Status::Ok == pImgBottomRight->GetLastStatus()))
		{
			graph.DrawImage(pImgBottomRight,
				RectF((REAL)(rcClient.right - pImgBottomRight->GetWidth()), (REAL)(rcClient.bottom - pImgBottomRight->GetHeight()),
				(REAL)pImgBottomRight->GetWidth(), (REAL)pImgBottomRight->GetHeight()));
		}
		//////////////////////////////////////////////////////////////////////////

		strIniPath.Replace(_T("BottomRight"), _T("BottomLeft"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		Image *pImgBottomLeft = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImgBottomLeft && (Status::Ok == pImgBottomLeft->GetLastStatus()))
		{
			graph.DrawImage(pImgBottomLeft,
				RectF((REAL)0, (REAL)(rcClient.bottom - pImgBottomLeft->GetHeight()),
				(REAL)pImgBottomLeft->GetWidth(), (REAL)pImgBottomLeft->GetHeight()));
		}

		//////////////////////////////////////////////////////////////////////////

		strIniPath.Replace(_T("BottomLeft"), _T("Top"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		Image *pImage = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImage && (Status::Ok == pImage->GetLastStatus()))
		{
			graph.DrawImage(pImage,
				RectF((REAL)pImgTopLeft->GetWidth(), (REAL)0,
				(REAL)(rcClient.right - pImgTopLeft->GetWidth() - pImgTopRight->GetWidth() + 2), (REAL)pImage->GetHeight()));

			delete pImage;
			pImage = nullptr;
		}

		//////////////////////////////////////////////////////////////////////////
		strIniPath.Replace(_T("Top"), _T("Right"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		pImage = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImage && (Status::Ok == pImage->GetLastStatus()))
		{
			graph.DrawImage(pImage,
				RectF((REAL)(rcClient.right - pImage->GetWidth()), (REAL)pImgTopRight->GetHeight(),
				(REAL)pImage->GetWidth(), (REAL)(rcClient.bottom - pImgTopRight->GetHeight() - pImgBottomRight->GetHeight() + 2)));

			delete pImage;
			pImage = nullptr;
		}

		//////////////////////////////////////////////////////////////////////////
		strIniPath.Replace(_T("Right"), _T("Bottom"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		pImage = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImage && (Status::Ok == pImage->GetLastStatus()))
		{
			graph.DrawImage(pImage,
				RectF((REAL)pImgBottomLeft->GetWidth(), (REAL)(rcClient.bottom - pImage->GetHeight()),
				(REAL)(rcClient.right - pImgBottomLeft->GetWidth() - pImgBottomRight->GetWidth() + 2), (REAL)pImage->GetHeight()));

			delete pImage;
			pImage = nullptr;
		}

		//////////////////////////////////////////////////////////////////////////
		strIniPath.Replace(_T("Bottom"), _T("Left"));

		memset(lpImageFile, 0, MAX_PATH);
		swprintf_s(lpImageFile, strIniPath);

		pImage = Image::FromFile(lpImageFile, TRUE);
		if (nullptr != pImage && (Status::Ok == pImage->GetLastStatus()))
		{
			graph.DrawImage(pImage,
				RectF((REAL)0, (REAL)pImgTopLeft->GetHeight(),
				(REAL)pImage->GetWidth(), (REAL)(rcClient.bottom - pImgTopLeft->GetHeight() - pImgBottomLeft->GetHeight() + 2)));

			delete pImage;
			pImage = nullptr;
		}

		delete pImgTopLeft;
		delete pImgTopRight;
		delete pImgBottomRight;
		delete pImgBottomLeft;
	}

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	bmp.DeleteObject();
	dcMem.DeleteDC();
}


void CAppBase::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TMR_TOP_LEFT_FLASH == nIDEvent)
	{
		if (::GetActiveWindow() == GetSafeHwnd())
		{
			CRect rc;
			GetClientRect(rc);
			rc.right = rc.left + 164;
			rc.bottom = rc.top + 8;

			m_nFlash++;

			if (m_nFlash >= 5)
			{
				m_nFlash = 0;
			}

			InvalidateRect(rc, FALSE);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CAppBase::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}


void CAppBase::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (nullptr != m_BtBaseClose.GetSafeHwnd())
	{
		CRect rcCtrlPos;
		GetClientRect(rcCtrlPos);

		rcCtrlPos.left = rcCtrlPos.right - 62;
		rcCtrlPos.right -= 12;
		rcCtrlPos.top += 8;
		rcCtrlPos.bottom = rcCtrlPos.top + 24;

		m_BtBaseClose.MoveWindow(rcCtrlPos);

		if (m_bAppMain)
		{
			rcCtrlPos.OffsetRect(-55, 0);
			m_BtBaseSize.MoveWindow(rcCtrlPos);

			rcCtrlPos.OffsetRect(-55, 0);
			m_BtBaseMin.MoveWindow(rcCtrlPos);

			rcCtrlPos.OffsetRect(-128, 0);
			rcCtrlPos.InflateRect(50, 0, 0, 0);
			m_swBaseLock.MoveWindow(rcCtrlPos);

			if (nullptr != m_Menu.GetSafeHwnd())
			{
				AdjustMenuPos();
			}
		}
	}
}


void CAppBase::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
	if (WA_ACTIVE == nState)
	{
		CSyncCtrl s(&g_seBaseInfo);

		BASE_BLUR_INFO Info;

		BOOL bFind = FALSE;

		const int nSize = g_vBaseInfo.size();

		for (int i = 0; i < nSize; i++)
		{
			if (g_vBaseInfo[i].pWnd == this)
			{
				bFind = TRUE;
				Info = g_vBaseInfo[i];

				g_vBaseInfo.erase(g_vBaseInfo.begin() + i);
				break;
			}
		}

		if (bFind)
		{
			g_vBaseInfo.push_back(Info);
		}
	}
}


LRESULT CAppBase::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bCanMove)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		ClientToScreen(rcClient);

		if (rcClient.PtInRect(point))
		{
			return HTCAPTION;
		}
	}

	return CDialogEx::OnNcHitTest(point);
}


void CAppBase::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	m_FlashTimer.KillTimer();
}


void CAppBase::LBtClickedBtClose(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	PostMessage(WM_CLOSE);
}


void CAppBase::LBtClickedBtMin(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	ShowWindow(SW_MINIMIZE);
}


void CAppBase::LBtClickedBtSize(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	CRect rcWork;
	GET_WORK_AREA(rcWork);

	CRect rcWnd;
	GetWindowRect(rcWnd);
	
	CRgn rgn;

	if (rcWnd.Width() < rcWork.Width())
	{
		rgn.CreateRectRgn(rcWork.left, rcWork.top, rcWork.right, rcWork.bottom);

		m_bCanMove = FALSE;

		m_BtBaseSize.SetCaption(_T("还原"));
		m_BtBaseSize.SetPngName(_T("Resize_"));
	}
	else
	{
		m_bCanMove = TRUE;
		rcWork.DeflateRect(240, 120, 240, 120);

		rcWnd = rcWork;
		rcWnd.MoveToXY(0, 0);

		CPoint pt[] = {CPoint(rcWnd.left + 7, rcWnd.top), CPoint(rcWnd.right - 7, rcWnd.top),
			CPoint(rcWnd.right, rcWnd.top + 7), CPoint(rcWnd.right, rcWnd.bottom - 7),
			CPoint(rcWnd.right - 7, rcWnd.bottom), CPoint(rcWnd.left + 7, rcWnd.bottom),
			CPoint(rcWnd.left, rcWnd.bottom - 7), CPoint(rcWnd.left, rcWnd.top + 7),
			CPoint(rcWnd.left + 7, rcWnd.top)};

		rgn.CreatePolygonRgn(pt, 9, WINDING);

		m_BtBaseSize.SetCaption(_T("最大化"));
		m_BtBaseSize.SetPngName(_T("Max_"));
	}

	MoveWindow(rcWork, FALSE);
	SetWindowRgn(rgn, TRUE);

	Invalidate(TRUE);
}


void CAppBase::StatusChangedSwAppLock(BOOL bStatus)
{
	if (bStatus)
	{
		_LockAllUis(bStatus);
	}
	else
	{
		PostMessage(gMsgAppBaseUnlockUi);
	}
}


void CAppBase::LBtClickedBtOk(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	OnOK();
}


void CAppBase::LBtClickedBtCancel(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	OnCancel();
}


void CAppBase::PrePopShowMenuAppMain(LPCTSTR strMenu)
{
	// TODO: 在此处添加消息处理程序代码
	_PreMenuPopupShow(strMenu);
}


void CAppBase::ItemClickMenuAppMain(LPCTSTR strMenu, LPCTSTR strItem, short nItemPos, BOOL bChecked, long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	_ClickMenuItem(strMenu, strItem, nItemPos, bChecked, nFlags);
}


void CAppBase::_PreMenuPopupShow(LPCTSTR strMenu)
{
	return;
}


void CAppBase::_ClickMenuItem(LPCTSTR strMenu, LPCTSTR strItem, short nItemPos, BOOL bChecked, long nFlags)
{
	const CString strMenuSrc = strMenu;
	const CString strItemSrc = strItem;

	if (_T("文件") == strMenuSrc)
	{
		switch (nItemPos)
		{
		case 5:	// 退出
			PostMessage(WM_CLOSE);
			break;

		default:
			break;
		}
	}
}
