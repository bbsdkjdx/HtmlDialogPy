
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
void set_size(int w, int h) { if (gpHtmlDialogPyDlg) gpHtmlDialogPyDlg->SetWindowPos(0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
void fixed_size(bool fixed) { if (gpHtmlDialogPyDlg) gpHtmlDialogPyDlg->m_fixed_size = fixed; }


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
, m_fixed_size(false)
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
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()


// CHtmlDialogPyDlg 消息处理程序

BOOL CHtmlDialogPyDlg::OnInitDialog()
{
	//add scroll bar(DOCHOSTUIFLAG_SCROLL_NO,DOCHOSTUIFLAG_FLAT_SCROLLBAR),disable drag and drop.
	SetHostFlags(DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DIALOG);

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
		REG_EXE_FUN("maindlg", set_title, "#S", "set window title")
		REG_EXE_FUN("maindlg", set_size, "#ll", "set window size")
		REG_EXE_FUN("maindlg", fixed_size, "#l", "fixed window size")
		REG_EXE_FUN("maindlg", get_browser_hwnd, "u", "")

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

LRESULT CHtmlDialogPyDlg::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int ret = CDHtmlDialog::OnNcHitTest(point);

	//if语句的前两行是用来禁止改变大小的，最后一行是用来禁止移动的
	if ( m_fixed_size && (HTTOP == ret || HTBOTTOM == ret || HTLEFT == ret || HTRIGHT == ret
		                  || HTBOTTOMLEFT == ret || HTBOTTOMRIGHT == ret || HTTOPLEFT == ret
						  || HTTOPRIGHT == ret )
       )return HTCLIENT;
	return ret;
}


LRESULT CHtmlDialogPyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_fixed_size && wParam == SC_MAXIMIZE) return 0;
	return CDHtmlDialog::WindowProc(message, wParam, lParam);
}


void CHtmlDialogPyDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_fixed_size)return;
	CDHtmlDialog::OnNcLButtonDblClk(nHitTest, point);
}
