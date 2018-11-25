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

// ��ʽ��������Ϣ
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

// Ȩ�޼���
enum
{
	PSD_LEVEL_LOCK = -1,
	PSD_LEVEL_OP = 0,
	PSD_LEVEL_QC,
	PSD_LEVEL_TE,
	PSD_LEVEL_MGR,
	PSD_LEVEL_SUM
};

// ������ô���
typedef struct _tagBaseBlurInfo
{
	CWnd *pWnd;
	CString strInfo;
	int nShowCmd;
}BASE_BLUR_INFO;

// �ͻ�������
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

// CAppBase �Ի���

class AFX_EXT_CLASS CAppBase : public CDialogEx
{
	DECLARE_DYNAMIC(CAppBase)

public:
	CAppBase(CWnd* pParent = NULL);   // ��׼���캯��
	CAppBase(UINT nIDTemplate, CWnd * pParent = nullptr);
	virtual ~CAppBase();

	// �Ի�������
	enum { IDD = IDD_APPBASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
public:
	BOOL m_bClickShow;									// ��������
	CString m_strLastFile;								// ��һ�ε��õ���
	CString m_strFilePath;								// ��������·��

	virtual BOOL OnInitDialog();
	virtual void DrawTitle(CDC * pDC, CRect rcDraw);	// ���Ʊ���
	virtual void DrawElement(CDC * pDC, CRect rcDraw);	// �Զ����ͼ
	virtual void DrawLine(CDC * pDC, CRect rcDraw);		// ����
	virtual void LockCtrls(int nLock);					// �����ؼ�
	virtual void PreWndDestroy(void);					// �ڴ�������ǰ���ú���

	virtual BOOL ServerMsgPorc(CSocket * pUser, CNewPtr<char> * pGets, CNewPtr<char> * pAns);	// ��������Ϣ������
	virtual BOOL ClientMsgPorc(CAppBase * pUser, CNewPtr<char> * pGets);	// �ͻ�����Ϣ������

	BOOL ShowWindow(int nCmdShow);						// ��ʾ����

	CWnd * GetOwner(void);								// ȡ�ø�����ָ��
	void SetOwner(CWnd * pOwner);						// ���ø�����ָ��

	CString GetTitle(void);								// ȡ�ô��ڱ���
	void SetTitle(const CString & strTitle);			// ���ô��ڱ���

	COLORREF GetTitleColor(void);						// ȡ�ñ����ַ���ɫ
	void SetTitleColor(COLORREF rgbTitle);				// ���ñ����ַ���ɫ

	CString GetTitleFont(void);							// ȡ�ñ�������
	REAL GetTitleFontSize(void);						// ȡ�ñ�������ߴ�
	void SetTitleFont(CString strFont, REAL fSize);		// ���ô��ڱ�������

	void LockStatus(BOOL bLock);						// ���ô��ڿؼ�����
	void AdjustMenuPos(void);							// �����˵�λ��

	void DrawTarget(Graphics * pGraph, Pen * pen, Brush * pBrush, GraphicsPath * pPath);	// ��·����ͼ
	BOOL AddLine(const CString & strName, int nIndex, CPoint pt1, CPoint pt2, COLORREF rgbLine, int nWidth, int nStyle);	// �ڴ����ϻ�һ����
	BOOL DeleteLine(const CString & strName);			// ɾ���ڴ����ϻ�����
	BOOL DeleteLine(int nLine);							// ɾ���ڴ����ϻ�����

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		char * pSend, int & nLength, char * pAns = nullptr, int nWaitTime = 3000);			// ����UDP��Ϣ

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		CNewPtr<char> * pSend, CNewPtr<char> * pAns = nullptr, int nWaitTime = 3000);		// ����UDP��Ϣ

	BOOL SendUdpMsg(const CString & strIp, USHORT nPort,
		const CString & strSend, CString * pAns = nullptr, int nWaitTime = 3000);			// ����UDP��Ϣ

protected:
	CWnd *m_pOwner;						// ������
	BOOL m_bUiLocked;					// UI����
	BOOL m_bStatusLock;					// ״̬����
	BOOL m_bChildWnd;					// �Ӵ��ڱ��
	BOOL m_bNoMenu;						// ����Ҫ�˵�

	int m_nMenuStart;					// �˵���ʼ����

	COLORREF m_rgbTitle;				// �����ַ���ɫ
	COLORREF m_rgbKey;					// ͸����ɫ

	REAL m_fTitleFontSize;				// ���������С
	CString m_strTitle;					// ���ڱ���
	CString m_strTitleFont;				// ��������

	int m_nBlurCount;					// ������ô������ü���
	CTopBlur m_TopBlur;					// ���ֽ��ô���
	std::vector<TP_UI_LINE> m_vUiLines;	// UI����

	CBL_Menu m_Menu;					// �˵�
	CBL_Button m_BtBaseClose;			// �رհ�ť
	CBL_Button m_BtBaseSize;			// ��ԭ��ť
	CBL_Button m_BtBaseMin;				// ��С����ť
	CBL_Switch m_swBaseLock;			// ������ť
	CBL_Button m_BtBaseOk;				// ȷ����ť
	CBL_Button m_BtBaseCancel;			// ȡ����ť

	int m_nUdpMemSize;					// ������ʱUDP��Ϣ��������С
	BOOL m_bNetAnswered;				// ������Ϣ�Ƿ񱻻ظ�
	SOCKET m_Socket;					// �ͻ�Socket
	SOCKADDR_IN m_SockAddr;				// ͨ�ŵ�ַ
	CNewPtr<char> m_NetBuffer;			// ���ջ�����
	CWinThread *m_pNetThread;			// �����߳�

	struct
	{
		BOOL bPassAutoSave;				// ����OKͼ��
		BOOL bFailAutoSave;				// ����NGͼ��
		CString strSavePath;			// ͼ�񱣴�·��
	}m_AutoSave;

	BOOL m_bAppMain;					// �Ƿ��ǳ���������
	BOOL m_bCanMove;					// �Ƿ�����ƶ�����
	BOOL m_bAppQuit;					// �������˳����

	virtual void _PreShowWnd(void);		// ��ʾ����ʾ���ô˺���
	virtual BOOL _LoadFileEx(void);		// ��Ҫ�ڼ��ص����ǿ����ô˺���������������
	virtual void _DefaultFocus(void);	// Ĭ��ӵ�н���Ŀؼ�

	void _SetWndConer(void);			// ���ô��ڱ߽�͸��
	void _SetRegister(BOOL bState);		// ����ע��״̬
	BOOL _IsRegistered(void);			// ��ѯ����Ƿ��Ѿ�ע��

	BOOL _IsCtrlDown(void);				// Ctrl���Ƿ񱻰���
	BOOL _IsShfitDown(void);			// Shift���Ƿ񱻰���

	void _SetFireMode(int nMode);		// ���ó���������ʽ
	int _GetFireMode(void);				// ȡ�ó���������ʽ

	int _Rand(int nMin, int nMax);		// ����һ���������
	void _DelayUs(long nTime);			// ��ʱ��λΪuS�ĺ���
	void _DelayMs(long nTime);			// ��ʱ��λΪmS�ĺ���

	CRect _GetScaledRect(int nWidth, int nHeight, CRect rcDraw, BOOL bTopLeft);		// ȡ��һ���ȱ������ŵľ���

	BOOL _GetLockState(int nLock, int nLevel);			// ȡ�õ�ǰȨ���Ƿ�����
	BOOL _IsUiLocked(void);								// ��ѯ��������״̬
	void _LockAllUis(BOOL bLock);						// ����ȫ������

	int _GetPsdLeve(void);								// ȡ�õ�ǰȨ��
	void _SetPsdLeve(int nLevel);						// ���õ�ǰȨ��
	BOOL _IsPsdLevel(int nLevel);						// �Ƿ���ĳ��Ȩ��

	void _AddModifyLog(const CString & strLogInfo);		// �����޸���־
	void _AutoDeleteTstImages(int nHoldDays);			// ɾ�����ͼ��
	void _DeleteOldestImages(void);						// ɾ�����ͼ��
	int _GetDiskFreeSpace(const CString & strPath);		// ��ȡ���̿��ÿռ䵥λ:MB

	void _SendBlurMsg(BOOL bShow);
	void _AddBlurInfo(CWnd * pDstWnd, const CString & strWnd);
	void _RemoveBlurInfo(CWnd * pDstWnd);

	std::vector<CString> _GetSegInfo(const CString & strInfo, TCHAR nMark);			// ȡ�÷ָ��ַ���
	std::vector<CString> _GetSegInfoEx(const CString & strInfo, int nMarks, ...);	// ȡ�÷ָ��ַ���
	std::vector<int> _GetIndex(CString strText, TCHAR nMark);						// ȡ���������
	CString _GetKilobitStr(int nData);												// ȡ����ǧ��λ�ָ����ַ���

	int _FindNonnumeric(const CString & strData, int nIncludes, ...);				// �ҳ��������ַ�
	void _DeleteSpace(CString & strData);				// ɾ���ַ����ڿո�
	void _DeleteZeros(CString & strData);				// ȥ��С���������0
	
	int _HexStr2int(CString strHex);					// ��ʮ�������ַ���ת��������
	int _IsHexMode(const CString & strData);			// ����ֵ: -1. ��������� 0. ʮ�������� 1. ��0x��ʼ��ʮ������ 2. ����0x��ʼ��ʮ������

	BOOL _SaveOkNgPath(void);							// ���ü��OK����·��
	BOOL _LoadOkNgPath(void);							// ���ü��NG����·��
	CString _GetTstImgSavePath(BOOL bResult, const CString & strPass, const CString & strFail);	// ���ò���ͼ�񱣴�·��

	BOOL _SaveFile(const CString & strType, const CString & strExt, BOOL bSaveAs);
	BOOL _LoadFile(const CString & strType, const CString & strExt, BOOL bRecent);
	BOOL _LoadFile(const CString & strType, const CString & strExt, const CString & strFile);

	BOOL _IsFileDirty(void);					// ��ѯ�����Ƿ��޸Ĺ�
	void _FileDirty(BOOL bDirty);				// ���ü������޸ı��
	void _SetTitleFromPath(void);				// ������������Ϊ�����򴰿ڱ���
	void _SetNoTitle(void);						// ����Ϊ "�ޱ���"
	CString _GetFileName(void);					// ȡ�õ�����

	BOOL _InstallFont(const CString & strFile);	// ��װ����
	BOOL _RemoveFont(CString strFont);			// ɾ������

	BOOL _CreateFileDir(CString strPath);		// �����ļ���
	BOOL _ClearFolder(CString strPath);			// ����ļ���
	BOOL _DeleteFolder(CString strPath);		// ɾ���ļ���
	void _TraversalFile(const CString & strPath, std::vector<FOLDER_INFO> & vFiles, int nOrder = 0);	// �����ļ����е��ļ�

	CString _GetCurTime(void);					// ȡ�õ�ǰϵͳʱ��
	CString _GetAppExePath(void);				// ȡ��Ӧ�ó���·��
	CString _GetAppIniPath(void);				// ȡ��Ӧ�ó���ini�ļ�·��(Ӧ�ó���·����)
	CString _GetSysFontPath(void);				// ȡ��ϵͳ����·��

	BOOL _RegisterActivex(BOOL bRegister);		// ע��ActiveX�ؼ�
	BOOL _LoadImageFromResource(ATL::CImage *pImage, UINT nResID, LPCTSTR lpTyp);	// ����Դ�ļ��м���ͼƬ

	static UINT _ReceiveUdpAnsThread(LPVOID pParam);	// ����Upd��������Ϣ�߳�
	UINT _ReceiveUdpAnsThreadKernel(void);				// ����Upd��������Ϣ�߳�

private:
	int m_nFlash;
	CTimerCtrl m_FlashTimer;
	CSemaphore m_seTitle;

	int m_nHoldDays;

	static UINT __DeleteTstImgThread(LPVOID pParam);		// ɾ�����ͼ���߳�
	UINT __DeleteTstImgThreadKernel(void);					// ɾ�����ͼ���߳�

	static UINT __DeleteOldestThread(LPVOID pParam);		// ɾ�����ͼ���߳�
	UINT __DeleteOldestThreadKernel(void);					// ɾ�����ͼ���߳�
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
