
// HtmlDialogPyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HtmlDialogPy.h"
#include "HtmlDialogPyDlg.h"
#include "afxdialogex.h"
#include "python_support.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


class CJsCaller
{
public:
	CJsCaller(CHtmlDialogPyDlg *pdlg)
	{
		pdlg->GetDHtmlDocument(&m_pdoc);
		m_pdoc->get_Script(&m_pDispScript);

		CComBSTR objbstrValue = _T("CallJs");
		BSTR bstrValue = objbstrValue.Copy();
		OLECHAR *pszFunct = bstrValue;

		m_pDispScript->GetIDsOfNames(IID_NULL,&pszFunct,1,LOCALE_SYSTEM_DEFAULT,&m_dispid);
		m_dispParams.cArgs = 1;
		m_dispParams.cNamedArgs = 0;
		m_dispParams.rgdispidNamedArgs = NULL;
		m_dispParams.rgvarg = &m_var;
		m_dispParams.rgvarg[0].vt = VT_BSTR;
		m_varResult.vt = VT_VARIANT;
	}
	WCHAR *CallJs(WCHAR *json_str)
	{
		m_dispParams.rgvarg[0].bstrVal = CComBSTR(json_str);
		m_pDispScript->Invoke(m_dispid,
			IID_NULL, LOCALE_USER_DEFAULT,
			DISPATCH_METHOD,
			&m_dispParams,
			&m_varResult,
			0,
			0);
		return m_varResult.bstrVal;
	}
	~CJsCaller()
	{
		m_pDispScript->Release();
	}

private:
	IHTMLDocument2* m_pdoc;
	VARIANT m_varResult;
	IDispatch *m_pDispScript;
	DISPID   m_dispid;
	DISPPARAMS m_dispParams;
	VARIANTARG m_var;
};

// CHtmlDialogPyDlg �Ի���

BEGIN_DHTML_EVENT_MAP(CHtmlDialogPyDlg)
	//DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	//DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


BEGIN_DISPATCH_MAP(CHtmlDialogPyDlg, CDHtmlDialog)
	DISP_FUNCTION(CHtmlDialogPyDlg, "ext_fun", ext_fun, VT_BSTR, VTS_BSTR)
END_DISPATCH_MAP()


CHtmlDialogPyDlg *gpHtmlDialogPyDlg = nullptr;

//functions export to python./////////////////////////////////////////////////////////////////////////////
void set_title(WCHAR *s) { if (gpHtmlDialogPyDlg)gpHtmlDialogPyDlg->SetWindowTextW(s); }
void set_size(int w, int h) { if (gpHtmlDialogPyDlg) gpHtmlDialogPyDlg->SetWindowPos(0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
void fixed_size(bool fixed) { if (gpHtmlDialogPyDlg) gpHtmlDialogPyDlg->m_fixed_size = fixed; }

WCHAR *__call_js(WCHAR *para)
{
	if (!gpHtmlDialogPyDlg)return false;
	gpHtmlDialogPyDlg->m_str_tmp = para;
	gpHtmlDialogPyDlg->SendMessage(WM_CALL_JS);
	return gpHtmlDialogPyDlg->m_str_tmp.GetBuffer();
}

UINT get_browser_hwnd()
{
	if (gpHtmlDialogPyDlg)
	{
		HWND wnd = gpHtmlDialogPyDlg->m_hWnd;
		wnd=::FindWindowEx(wnd, 0, 0, 0);
		wnd = ::FindWindowEx(wnd, 0, 0, 0);
		wnd = ::FindWindowEx(wnd, 0, 0, 0);
		return UINT(wnd);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHtmlDialogPyDlg::CHtmlDialogPyDlg(CWnd* pParent /*=NULL*/)
: CDHtmlDialog(CHtmlDialogPyDlg::IDD, CHtmlDialogPyDlg::IDH, pParent)
, m_str_tmp(_T(""))
, m_fixed_size(false)
, m_p_js_caller(nullptr)
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
	ON_MESSAGE(WM_CALL_JS, &CHtmlDialogPyDlg::OnCallJs)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CHtmlDialogPyDlg ��Ϣ�������

BOOL CHtmlDialogPyDlg::OnInitDialog()
{
	//add scroll bar(DOCHOSTUIFLAG_SCROLL_NO,DOCHOSTUIFLAG_FLAT_SCROLLBAR),disable drag and drop.
	SetHostFlags(DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DIALOG);

	CDHtmlDialog::OnInitDialog();

	// disable script error box.
	m_pBrowserApp->put_Silent(VARIANT_TRUE);

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	REG_EXE_FUN("", __call_js, "SS", "used for python call js,do not use it directly.")
		REG_EXE_FUN("maindlg", set_title, "#S", "set window title")
		REG_EXE_FUN("maindlg", set_size, "#ll", "set window size")
		REG_EXE_FUN("maindlg", fixed_size, "#l", "fixed window size")
		REG_EXE_FUN("maindlg", get_browser_hwnd, "u", "")


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHtmlDialogPyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHtmlDialogPyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


WCHAR* CHtmlDialogPyDlg::ext_fun(wchar_t* para)
{
	PySetStrW(para, 0);
	PyEvalA("_js_fun()");
	return CComBSTR(PyGetStr());
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

		// prevent Ctrl+N
		if (lpMsg->wParam == 'N' && bCtrl)
		{
			return S_OK;
		}

		// prevent Ctrl+F
		if (lpMsg->wParam == 'F' && bCtrl)
		{
			return S_OK;
		}

		// prevent F5
		if (lpMsg->wParam == VK_F5)
		{
			return S_OK;
		}

		// prevent ESC
		if (lpMsg->wParam == VK_ESCAPE)
		{
			return S_OK;
		}

		// prevent ENTER
		if (lpMsg->wParam == VK_RETURN)
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
		//	if (MessageBoxA(m_hWnd,"�Ƿ������ȡhtml�ĵ���","�յ�WM_HTML_GETOBJECT",MB_YESNO|MB_ICONWARNING)==IDNO)
		{
			//return TRUE;
		}
	}
	return CDHtmlDialog::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CHtmlDialogPyDlg::OnCallJs(WPARAM wParam, LPARAM lParam)
{
	if(!m_p_js_caller)m_p_js_caller = new CJsCaller(this);
	m_str_tmp = m_p_js_caller->CallJs(m_str_tmp.GetBuffer());
	return 1;
}


LRESULT CHtmlDialogPyDlg::OnNcHitTest(CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int ret = CDHtmlDialog::OnNcHitTest(point);

	//if����ǰ������������ֹ�ı��С�ģ����һ����������ֹ�ƶ���
	if ( m_fixed_size && (HTTOP == ret || HTBOTTOM == ret || HTLEFT == ret || HTRIGHT == ret
		                  || HTBOTTOMLEFT == ret || HTBOTTOMRIGHT == ret || HTTOPLEFT == ret
						  || HTTOPRIGHT == ret || HTCAPTION == ret)
       )
	return HTCLIENT;
	return ret;
}


LRESULT CHtmlDialogPyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (m_fixed_size && wParam == SC_MAXIMIZE) return 0;
	return CDHtmlDialog::WindowProc(message, wParam, lParam);
}
