
// HtmlDialogPyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HtmlDialogPy.h"
#include "HtmlDialogPyDlg.h"
#include "afxdialogex.h"
#include "python_support.h"

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
public:
	//	virtual BOOL PreTranslateMessage(MSG* pMsg);
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

// CHtmlDialogPyDlg 对话框

BEGIN_DHTML_EVENT_MAP(CHtmlDialogPyDlg)
	//DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	//DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


BEGIN_DISPATCH_MAP(CHtmlDialogPyDlg, CDHtmlDialog)
	DISP_FUNCTION(CHtmlDialogPyDlg, "ext_fun", ext_fun, VT_NULL, VTS_NONE)
END_DISPATCH_MAP()


CHtmlDialogPyDlg *gpHtmlDialogPyDlg = nullptr;

//functions export to python./////////////////////////////////////////////////////////////////////////////
void set_title(WCHAR *s) { if (gpHtmlDialogPyDlg)gpHtmlDialogPyDlg->SetWindowTextW(s); }

void set_size(int x, int y, int z, bool fixed)
{
	if (!gpHtmlDialogPyDlg)return;

	if (x==-1 || y==-1)
	{
		CRect rct;
		gpHtmlDialogPyDlg->GetWindowRect(&rct);
		x = rct.Width();
		y = rct.Height();
	}

	if (fixed)
	{
		gpHtmlDialogPyDlg->m_fixed_size = { x, y };
	}
	else
	{
		gpHtmlDialogPyDlg->m_fixed_size = { 0, 0 };
	}

	
	DWORD flag=SWP_NOMOVE;
	if (z == -1)flag |= SWP_NOZORDER;
	const CWnd *p_wnd_after = z ? &CWnd::wndTopMost : &CWnd::wndNoTopMost;
	gpHtmlDialogPyDlg->SetWindowPos(p_wnd_after, 0, 0,x, y,  flag);
}


UINT get_browser_hwnd()
{
	if (gpHtmlDialogPyDlg)
	{
		HWND ret = 0;
		for (HWND wnd = gpHtmlDialogPyDlg->m_hWnd; wnd; wnd = ::FindWindowEx(wnd, 0, 0, 0))
		{
			ret = wnd;
		}
		return UINT(ret);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHtmlDialogPyDlg::CHtmlDialogPyDlg(CWnd* pParent /*=NULL*/)
: CDHtmlDialog(CHtmlDialogPyDlg::IDD, CHtmlDialogPyDlg::IDH, pParent)
, m_fixed_size()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gpHtmlDialogPyDlg = this;
	EnableAutomation();
	SetExternalDispatch(GetIDispatch(TRUE));
}

void CHtmlDialogPyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHtmlDialogPyDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CHtmlDialogPyDlg 消息处理程序

BOOL CHtmlDialogPyDlg::OnInitDialog()
{

	CDHtmlDialog::OnInitDialog();

	// disable script error box.
	m_pBrowserApp->put_Silent(VARIANT_TRUE);

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	REG_EXE_FUN("maindlg", set_title, "#S", "void set_title(WCHAR *s);\nset window title")
		REG_EXE_FUN("maindlg", set_size, "#llll", "void set_size(int x, int y, int z, bool fixed);\nif x or y is -1,keep size. if z is -1 keep z-order.")
		REG_EXE_FUN("maindlg", get_browser_hwnd, "u", "UINT get_browser_hwnd();\nget the browser hwnd ,for get html document.")

	if (!PyExecA("import autorun"))
	{
		AfxMessageBox(PyGetStr());
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHtmlDialogPyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHtmlDialogPyDlg::OnPaint()
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
		CDHtmlDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHtmlDialogPyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHtmlDialogPyDlg::ext_fun()
{
	PyExecA("_js_fun()");
}

//disable safe warning.
BOOL CHtmlDialogPyDlg::CanAccessExternal()
{
	return TRUE;
}

//hook contex menu.
HRESULT STDMETHODCALLTYPE CHtmlDialogPyDlg::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	if (dwID == CONTEXT_MENU_CONTROL)return CDHtmlDialog::ShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
	return FALSE;
}

//hook accelerator.
HRESULT STDMETHODCALLTYPE CHtmlDialogPyDlg::TranslateAccelerator(LPMSG lpMsg,
	const GUID *pguidCmdGroup,
	DWORD nCmdID)
{
	if (lpMsg && lpMsg->message == WM_KEYDOWN)
	{
		bool bCtrl = (0x80 == (0x80 & GetKeyState(VK_CONTROL)));
		WPARAM wp = lpMsg->wParam;
		// prevent Ctrl+N,F
		if (bCtrl && (wp=='N' || wp=='F') )
		{
			return S_OK;
		}

		// prevent F5,escape,return,backspace.
		if (wp == VK_F5 || wp==VK_ESCAPE || wp==VK_RETURN || wp==VK_BACK)
		{
			return S_OK;
		}
	}
	return CDHtmlDialog::TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}



BOOL CHtmlDialogPyDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == 256 && pMsg->wParam == 123 && GetAsyncKeyState(0x11) & 0x8000)//Ctrl+F12 pressed.
	{
		InteractInConsole(m_hWnd, false);
	}
	if (pMsg->message == 1024)//disable retriving document by WM_HTML_GETOBJECT.
	{
		//	if (MessageBoxA(m_hWnd,"是否允许获取html文档？","收到WM_HTML_GETOBJECT",MB_YESNO|MB_ICONWARNING)==IDNO)
		{
			//return TRUE;
		}
	}
	return CDHtmlDialog::PreTranslateMessage(pMsg);
}

STDMETHODIMP CHtmlDialogPyDlg::GetHostInfo(DOCHOSTUIINFO *pInfo)
{	
//add scroll bar(DOCHOSTUIFLAG_SCROLL_NO,DOCHOSTUIFLAG_FLAT_SCROLLBAR),disable drag and drop.
	pInfo->dwFlags = DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DIALOG;
	return S_OK;
}

void CHtmlDialogPyDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_fixed_size.x!=0 && m_fixed_size.y!=0)
	{
		lpMMI->ptMaxTrackSize = m_fixed_size;
		lpMMI->ptMinTrackSize = m_fixed_size;
	}
	CDHtmlDialog::OnGetMinMaxInfo(lpMMI);
}
