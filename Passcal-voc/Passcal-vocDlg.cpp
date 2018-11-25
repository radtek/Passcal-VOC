
// Passcal-vocDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Passcal-voc.h"
#include "Passcal-vocDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasscalvocDlg 对话框



CPasscalvocDlg::CPasscalvocDlg(CWnd* pParent /*=NULL*/)
	: CAppBase(CPasscalvocDlg::IDD, pParent)
{
	m_bAppMain = TRUE;
	m_bNoMenu = TRUE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPasscalvocDlg::DoDataExchange(CDataExchange* pDX)
{
	CAppBase::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPasscalvocDlg, CAppBase)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPasscalvocDlg, CAppBase)
	ON_EVENT(CPasscalvocDlg, IDC_BT_BROWSE, 1, CPasscalvocDlg::LBtClickedBtBrowse, VTS_I4)
	ON_EVENT(CPasscalvocDlg, IDC_BT_RENAME, 1, CPasscalvocDlg::LBtClickedBtRename, VTS_I4)
	ON_EVENT(CPasscalvocDlg, IDC_BT_CHANGE_TYPE, 1, CPasscalvocDlg::LBtClickedBtChangeType, VTS_I4)
END_EVENTSINK_MAP()

// CPasscalvocDlg 消息处理程序

BOOL CPasscalvocDlg::OnInitDialog()
{
	CAppBase::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTitle(_T("Passcal-VOC"));
	m_Menu.ShowWindow(SW_HIDE);

	m_swBaseLock.ShowWindow(SW_HIDE);
	m_BtBaseOk.ShowWindow(SW_HIDE);
	m_BtBaseSize.ShowWindow(SW_HIDE);

	m_BtBaseCancel.SetCaption(_T("退出"));
	m_BtBaseCancel.SetBlendColor(RGB(64, 64, 64));
	m_BtBaseCancel.SetBackColor(RGB(64, 64, 64));

	_SetWndConer();

	const CString strPath = _GetAppIniPath();

	const CString strKey = _T("Opt");
	GetPrivateProfileString(strKey, _T("ImgPath"), _T(""),
		m_strImgPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH, strPath);

	m_strImgPath.ReleaseBuffer();
	((CBL_Edit *)(GetDlgItem(IDC_EDIT_IMG_PATH)))->SetValueText(m_strImgPath);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPasscalvocDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CAppBase::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPasscalvocDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CAppBase::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPasscalvocDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPasscalvocDlg::LBtClickedBtBrowse(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	CFolderPickerDialog Dlg;

	if (IDOK == Dlg.DoModal())
	{
		m_strImgPath = Dlg.GetFolderPath();

		((CBL_Edit *)(GetDlgItem(IDC_EDIT_IMG_PATH)))->SetValueText(m_strImgPath);

		const CString strPath = _GetAppIniPath();
		
		const CString strKey = _T("Opt");
		WritePrivateProfileString(strKey, _T("ImgPath"), m_strImgPath, strPath);
	}
}


void CPasscalvocDlg::LBtClickedBtRename(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	CBlender<CNameInfo> Ni(IDD_NAMEINFO, this);

	if (IDOK == Ni.CreateTopWnd(TRUE))
	{
		std::vector<FOLDER_INFO> vInfo;
		_TraversalFile(m_strImgPath, vInfo);

		const int nInfos = vInfo.size();

		USES_CONVERSION;

		for (int i = 0; i < nInfos; i++)
		{
			FOLDER_INFO &Info = vInfo[i];
			const int nDot = Info.strPath.ReverseFind('.');

			if (nDot > 0)
			{
				const CString strExt = Info.strPath.Right(Info.strPath.GetLength() - nDot - 1);

				CString strFomat;
				strFomat.Format(_T("%s\\%s%%0%dd.%s"), m_strImgPath, Ni->m_strPrefix, Ni->m_nNums, strExt);

				CString strIndex;
				strIndex.Format(strFomat, i + Ni->m_nStart);

				if (Info.bFolder || Info.bHidden || Info.bSysFile || Info.bTemporary)
				{
					continue;
				}

				rename(W2A(Info.strPath), W2A(strIndex));
			}
		}
	}
}


void CPasscalvocDlg::LBtClickedBtChangeType(long nFlags)
{
	// TODO: 在此处添加消息处理程序代码
	CBlender<CExtInfo> dlg(IDD_EXTINFO, this);

	dlg->m_strSavePath = m_strImgPath + _T("\\jpg_files");

	if (IDOK == dlg.CreateTopWnd(TRUE))
	{
		if (dlg->m_strDstType == dlg->m_strSrcType)
		{
			return;
		}

		std::vector<FOLDER_INFO> vInfo;
		_TraversalFile(m_strImgPath, vInfo);

		const int nInfos = vInfo.size();

		USES_CONVERSION;

		for (int i = 0; i < nInfos; i++)
		{
			FOLDER_INFO &Info = vInfo[i];

			
			const int nDot = Info.strPath.ReverseFind('.');

			if (nDot > 0)
			{
				if (Info.bFolder || Info.bHidden || Info.bSysFile || Info.bTemporary)
				{
					continue;
				}

				const CString strSrcExt = Info.strPath.Right(Info.strPath.GetLength() - nDot);

				CSmartImage Img;
				Img.Open(Info.strPath);

				Info.strPath.Replace(strSrcExt, dlg->m_strDstType);

				const int nPos = Info.strPath.ReverseFind('\\');

				const CString strPath = Info.strPath.Left(nPos);
				const CString strName = Info.strPath.Right(Info.strPath.GetLength() - nPos - 1);

// 				if (strSrcExt != dlg->m_strSrcType)
// 				{
// 					continue;
// 				}

				Img.Save(dlg->m_strSavePath, strName);
			}
		}
	}
}
