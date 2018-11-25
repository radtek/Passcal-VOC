#pragma once

#include "Resource.h"

#include <vector>
#include <algorithm>
#include <afxsock.h>
#pragma comment(lib, "ws2_32.lib")

#include "ValRst.h"
#include "NewPtr.h"
#include "SyncCtrl.h"
#include "TimerCtrl.h"
#include "TopBlur.h"
#include "GdiplusText.h"

#include "..\BlActiveXDll\BLCtrls.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\BlActiveXDll.lib")
#else
#pragma comment(lib, "..\\Release\\BlActiveXDll.lib")
#endif

#define APP_LOCK_AUTO			-1
#define TMR_TOP_LEFT_FLASH		32767

#define RGB_APP_MAIN_KEY		RGB(55, 56, 57)
#define RGB_TRANS_TOP_KEY		RGB(55, 56, 57)
#define RGB_BT_LAYER_BACK		RGB(45, 50, 55)
#define RGB_ALL_64				RGB(64, 64, 64)
#define RGB_0_255_230			RGB(0, 255, 230)
#define RGB_BT_HOVER_BACK		RGB(70, 70, 70)

#ifndef IS_ODD
#define IS_ODD(v)				((v) & 0x01)
#endif // !IS_ODD

#ifndef IS_EVENT
#define IS_EVENT(v)				(!IS_ODD(v))
#endif // !IS_ODD

#define RGB2ARGB(a, rgb)		(Color(a, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)))
#define GET_WORK_AREA(rcWork)	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0)

#define CRECT2RECTF(rect)		RectF((REAL)rect.left, (REAL)rect.top, (REAL)rect.Width(), (REAL)rect.Height())
#define SET_FILE_VERSION(n)		const int nVersion = n; ar << nVersion
#define GET_FILE_VERSION		int nVersion = 0; ar >> nVersion
#define LOCK_CTRL_AUTO			if (nullptr != GetSafeHwnd() && TRUE == IsWindowVisible()) LockCtrls(APP_LOCK_AUTO)
#define UPDATE_LOCK_CTRL_AUTO	if (nullptr != GetSafeHwnd() && TRUE == IsWindowVisible())\
								{\
									UpdateUi();\
									LockCtrls(APP_LOCK_AUTO);\
								}

#define FUN_ERR_INFO_LOAD(Info)	if (nullptr != pErr) *pErr = Info

// 格式化错误消息
#define FORMAT_ERR_INFO(pInfo)	LPVOID lpMsgBuf = nullptr;\
								if (nullptr != (pInfo))\
								{\
									const DWORD nErr = GetLastError();\
									FormatMessage(\
										FORMAT_MESSAGE_ALLOCATE_BUFFER |\
										FORMAT_MESSAGE_FROM_SYSTEM |\
										FORMAT_MESSAGE_IGNORE_INSERTS,\
										nullptr,\
										nErr,\
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
										(LPTSTR) &lpMsgBuf,\
										0,\
										nullptr);\
									(pInfo)->Format(_T("Err:%d(%s)"), nErr, lpMsgBuf);\
								}\
								LocalFree(lpMsgBuf)

enum
{
	FIRE_MODE_MENU = 0,
	FIRE_MODE_232,
	FIRE_MODE_COPYDATA,
	FIRE_MODE_SHARE_WND,
	FIRE_MODE_ETHERNET,
	FIRE_MODE_CLICK
};

enum
{
	IDC_BT_BASE_OK = 32761,
	IDC_BT_BASE_CANCEL,
	IDC_SW_BASE_LOCK,
	IDC_BT_BASE_MIN,
	IDC_BT_BASE_SIZE,
	IDC_BT_BASE_CLOSE,
	IDC_MENU_APP_MAIN
};

// 权限级别
enum
{
	PSD_LEVEL_LOCK = -1,
	PSD_LEVEL_OP = 0,
	PSD_LEVEL_QC,
	PSD_LEVEL_TE,
	PSD_LEVEL_MGR,
	PSD_LEVEL_SUM
};

// 顶层禁用窗口
typedef struct _tagBaseBlurInfo
{
	CWnd *pWnd;
	CString strInfo;
	int nShowCmd;
}BASE_BLUR_INFO;

// 客户区画线
typedef struct _tagTpUiLine
{
	CString strName;
	int nNum;
	CPoint pt1;
	CPoint pt2;
	COLORREF rgbLine;
	int nWidth;
	int nStytle;
}TP_UI_LINE;

typedef struct _tagFolderInfo
{
	CString strPath;

	BOOL bFolder;
	BOOL bSysFile;
	BOOL bHidden;
	BOOL bReadOnly;
	BOOL bTemporary;
}FOLDER_INFO;


static const UINT gMsgAppBaseLockUi		= RegisterWindowMessage(_T("gMsgAppBaseLockUi"));
static const UINT gMsgAppBaseUnlockUi	= RegisterWindowMessage(_T("gMsgAppBaseUnlockUi"));
static const UINT gMsgAppBaseSetFocus	= RegisterWindowMessage(_T("gMsgAppBaseSetFocus"));
static const UINT gMsgAppBaseLoadFile	= RegisterWindowMessage(_T("gMsgAppBaseLoadFile"));

// CAppBase 对话框

class AFX_EXT_CLASS CAppBase : public CDialogEx
{
	DECLARE_DYNAMIC(CAppBase)

public:
	CAppBase(CWnd* pParent = NULL);   // 标准构造函数
	CAppBase(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CAppBase();

	// 对话框数据
	enum { IDD = IDD_APPBASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	BOOL m_bClickShow;									// 点击激活窗口
	CString m_strLastFile;								// 上一次调用档案
	CString m_strFilePath;								// 档案保存路径

	virtual BOOL OnInitDialog();
	virtual void DrawTitle(CDC * pDC, CRect rcDraw);	// 绘制标题
	virtual void DrawElement(CDC * pDC, CRect rcDraw);	// 自定义绘图
	virtual void DrawLine(CDC * pDC, CRect rcDraw);		// 画线
	virtual void LockCtrls(int nLock);					// 锁定控件
	virtual void PreWndDestroy(void);					// 在窗口销毁前调用函数

	virtual BOOL ServerMsgPorc(CSocket * pUser, CNewPtr<char> * pGets, CNewPtr<char> * pAns);	// 服务器消息处理函数
	virtual BOOL ClientMsgPorc(CAppBase * pUser, CNewPtr<char> * pGets);	// 客户端消息处理函数

	BOOL ShowWindow(int nCmdShow);						// 显示窗口

	CWnd * GetOwner(void);								// 取得父窗口指针
	void SetOwner(CWnd * pOwner);						// 设置父窗口指针

	CString GetTitle(void);								// 取得窗口标题
	void SetTitle(const CString & strTitle);			// 设置窗口标题

	COLORREF GetTitleColor(void);						// 取得标题字符颜色
	void SetTitleColor(COLORREF rgbTitle);				// 设置标题字符颜色

	CString GetTitleFont(void);							// 取得标题字体
	REAL GetTitleFontSize(void);						// 取得标题字体尺寸
	void SetTitleFont(CString strFont, REAL fSize);		// 设置窗口标题字体

	void LockStatus(BOOL bLock);						// 设置窗口控件锁定
	void AdjustMenuPos(void);							// 调整菜单位置

	void DrawTarget(Graphics * pGraph, Pen * pen, Brush * pBrush, GraphicsPath * pPath);	// 按路径绘图
	BOOL AddLine(const CString & strName, int nIndex, CPoint pt1, CPoint pt2, COLORREF rgbLine, int nWidth, int nStyle);	// 在窗口上画一条线
	BOOL DeleteLine(const CString & strName);			// 删除在窗口上画的线
	BOOL DeleteLine(int nLine);							// 删除在窗口上画的线

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		char * pSend, int & nLength, char * pAns = nullptr, int nWaitTime = 3000);			// 发送UDP消息

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		CNewPtr<char> * pSend, CNewPtr<char> * pAns = nullptr, int nWaitTime = 3000);		// 发送UDP消息

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		const CString & strSend, CString * pAns = nullptr, int nWaitTime = 3000);			// 发送UDP消息

protected:
	CWnd *m_pOwner;						// 父窗口
	BOOL m_bUiLocked;					// UI锁定
	BOOL m_bStatusLock;					// 状态锁定
	BOOL m_bChildWnd;					// 子窗口标记
	BOOL m_bNoMenu;						// 不需要菜单

	int m_nMenuStart;					// 菜单开始坐标

	COLORREF m_rgbTitle;				// 标题字符颜色
	COLORREF m_rgbKey;					// 透明颜色

	REAL m_fTitleFontSize;				// 标题字体大小
	CString m_strTitle;					// 窗口标题
	CString m_strTitleFont;				// 标题字体

	int m_nBlurCount;					// 顶层禁用窗口引用计数
	CTopBlur m_TopBlur;					// 顶怪禁用窗口
	std::vector<TP_UI_LINE> m_vUiLines;	// UI线条

	CBL_Menu m_Menu;					// 菜单
	CBL_Button m_BtBaseClose;			// 关闭按钮
	CBL_Button m_BtBaseSize;			// 还原按钮
	CBL_Button m_BtBaseMin;				// 最小化按钮
	CBL_Switch m_swBaseLock;			// 锁定按钮
	CBL_Button m_BtBaseOk;				// 确定按钮
	CBL_Button m_BtBaseCancel;			// 取消按钮

	int m_nUdpMemSize;					// 接收临时UDP消息缓冲区大小
	BOOL m_bNetAnswered;				// 网络消息是否被回复
	SOCKET m_Socket;					// 客户Socket
	SOCKADDR_IN m_SockAddr;				// 通信地址
	CNewPtr<char> m_NetBuffer;			// 接收缓冲区
	CWinThread *m_pNetThread;			// 接收线程

	struct
	{
		BOOL bPassAutoSave;				// 保存OK图像
		BOOL bFailAutoSave;				// 保存NG图像
		CString strSavePath;			// 图像保存路径
	}m_AutoSave;

	BOOL m_bAppMain;					// 是否是程序主窗口
	BOOL m_bCanMove;					// 是否可以移动窗口
	BOOL m_bAppQuit;					// 主程序退出标记

	virtual void _PreShowWnd(void);		// 显示窗口示调用此函数
	virtual BOOL _LoadFileEx(void);		// 需要在加载档案是可以用此函数增加其他操作
	virtual void _DefaultFocus(void);	// 默认拥有焦点的控件

	void _SetWndConer(void);			// 设置窗口边角透明
	void _SetRegister(BOOL bState);		// 设置注册状态
	BOOL _IsRegistered(void);			// 查询软件是否已经注册

	BOOL _IsCtrlDown(void);				// Ctrl键是否被按下
	BOOL _IsShfitDown(void);			// Shift键是否被按下

	void _SetFireMode(int nMode);		// 设置程序启动方式
	int _GetFireMode(void);				// 取得程序启动方式

	int _Rand(int nMin, int nMax);		// 产生一个随机整数
	void _DelayUs(long nTime);			// 延时单位为uS的函数
	void _DelayMs(long nTime);			// 延时单位为mS的函数

	CRect _GetScaledRect(int nWidth, int nHeight, CRect rcDraw, BOOL bTopLeft);		// 取得一个等比例绽放的矩形

	BOOL _GetLockState(int nLock, int nLevel);			// 取得当前权限是否被锁定
	BOOL _IsUiLocked(void);								// 查询窗口锁定状态
	void _LockAllUis(BOOL bLock);						// 锁定全部窗口

	int _GetPsdLeve(void);								// 取得当前权限
	void _SetPsdLeve(int nLevel);						// 设置当前权限
	BOOL _IsPsdLevel(int nLevel);						// 是否是某个权限

	void _AddModifyLog(const CString & strLogInfo);		// 增加修改日志
	void _AutoDeleteTstImages(int nHoldDays);			// 删除检测图像
	void _DeleteOldestImages(void);						// 删除检测图像
	int _GetDiskFreeSpace(const CString & strPath);		// 获取磁盘可用空间单位:MB

	void _SendBlurMsg(BOOL bShow);
	void _AddBlurInfo(CWnd * pDstWnd, const CString & strWnd);
	void _RemoveBlurInfo(CWnd * pDstWnd);

	std::vector<CString> _GetSegInfo(const CString & strInfo, TCHAR nMark);			// 取得分割字符串
	std::vector<CString> _GetSegInfoEx(const CString & strInfo, int nMarks, ...);	// 取得分割字符串
	std::vector<int> _GetIndex(CString strText, TCHAR nMark);						// 取得整数序号
	CString _GetKilobitStr(int nData);												// 取得用千分位分隔的字符串

	int _FindNonnumeric(const CString & strData, int nIncludes, ...);				// 找出非数字字符
	void _DeleteSpace(CString & strData);				// 删除字符串口空格
	void _DeleteZeros(CString & strData);				// 去掉小数点后多余的0
	
	int _HexStr2int(CString strHex);					// 将十六进制字符串转换成整数
	int _IsHexMode(const CString & strData);			// 返回值: -1. 错误的数据 0. 十进制数据 1. 带0x开始的十六进制 2. 不带0x开始的十六进制

	BOOL _SaveOkNgPath(void);							// 设置检测OK保存路径
	BOOL _LoadOkNgPath(void);							// 设置检测NG保存路径
	CString _GetTstImgSavePath(BOOL bResult, const CString & strPass, const CString & strFail);	// 到得测试图像保存路径

	BOOL _SaveFile(const CString & strType, const CString & strExt, BOOL bSaveAs);
	BOOL _LoadFile(const CString & strType, const CString & strExt, BOOL bRecent);
	BOOL _LoadFile(const CString & strType, const CString & strExt, const CString & strFile);

	BOOL _IsFileDirty(void);					// 查询档案是否被修改过
	void _FileDirty(BOOL bDirty);				// 设置极少数修改标记
	void _SetTitleFromPath(void);				// 将档案名设置为主程序窗口标题
	void _SetNoTitle(void);						// 设置为 "无标题"
	CString _GetFileName(void);					// 取得档案名

	BOOL _InstallFont(const CString & strFile);	// 安装字体
	BOOL _RemoveFont(CString strFont);			// 删除字体

	BOOL _CreateFileDir(CString strPath);		// 创建文件夹
	BOOL _ClearFolder(CString strPath);			// 清空文件夹
	BOOL _DeleteFolder(CString strPath);		// 删除文件夹
	void _TraversalFile(const CString & strPath, std::vector<FOLDER_INFO> & vFiles, int nOrder = 0);	// 遍历文件夹中的文件

	CString _GetCurTime(void);					// 取得当前系统时间
	CString _GetAppExePath(void);				// 取得应用程序路径
	CString _GetAppIniPath(void);				// 取得应用程序ini文件路径(应用程序路径中)
	CString _GetSysFontPath(void);				// 取得系统字体路径

	BOOL _RegisterActivex(BOOL bRegister);		// 注册ActiveX控件
	BOOL _LoadImageFromResource(ATL::CImage *pImage, UINT nResID, LPCTSTR lpTyp);	// 从资源文件中加载图片

	static UINT _ReceiveUdpAnsThread(LPVOID pParam);	// 接收Upd服务器消息线程
	UINT _ReceiveUdpAnsThreadKernel(void);				// 接收Upd服务器消息线程

private:
	int m_nFlash;
	CTimerCtrl m_FlashTimer;
	CSemaphore m_seTitle;

	int m_nHoldDays;

	static UINT __DeleteTstImgThread(LPVOID pParam);		// 删除检测图像线程
	UINT __DeleteTstImgThreadKernel(void);					// 删除检测图像线程

	static UINT __DeleteOldestThread(LPVOID pParam);		// 删除检测图像线程
	UINT __DeleteOldestThreadKernel(void);					// 删除检测图像线程
protected:
	afx_msg LRESULT OnGmsgsemitransblur(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGmsgappbaselockui(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGmsgappbaseunlockui(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGmsgappbasesetfocus(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
public:
	void LBtClickedBtClose(long nFlags);
	void LBtClickedBtSize(long nFlags);
	void LBtClickedBtMin(long nFlags);
	void StatusChangedSwAppLock(BOOL bStatus);
	void LBtClickedBtOk(long nFlags);
	void LBtClickedBtCancel(long nFlags);
	void PrePopShowMenuAppMain(LPCTSTR strMenu);
	void ItemClickMenuAppMain(LPCTSTR strMenu, LPCTSTR strItem, short nItemPos, BOOL bChecked, long nFlags);
protected:
	virtual void _PreMenuPopupShow(LPCTSTR strMenu);
	virtual void _ClickMenuItem(LPCTSTR strMenu, LPCTSTR strItem, short nItemPos, BOOL bChecked, long nFlags);
};
