
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



// CHtmlDialogPyDlg �Ի���

BEGIN_DHTML_EVENT_MAP(CHtmlDialogPyDlg)
	//DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	//DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


BEGIN_DISPATCH_MAP(CHtmlDialogPyDlg, CDHtmlDialog)
	DISP_FUNCTION(CHtmlDialogPyDlg, "ext_fun", ext_fun, VT_BSTR, VTS_BSTR)
END_DISPATCH_MAP()


CHtmlDialogPyDlg *gpHtmlDialogPyDlg = nullptr;


WCHAR *__call_js(WCHAR *para)
{
	if (!gpHtmlDialogPyDlg)return false;
	gpHtmlDialogPyDlg->m_str_tmp = para;
	gpHtmlDialogPyDlg->SendMessage(WM_CALL_JS);
	return gpHtmlDialogPyDlg->m_str_tmp.GetBuffer();
}

CHtmlDialogPyDlg::CHtmlDialogPyDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CHtmlDialogPyDlg::IDD, CHtmlDialogPyDlg::IDH, pParent)
	, m_str_tmp(_T(""))
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
END_MESSAGE_MAP()


// CHtmlDialogPyDlg ��Ϣ�������

BOOL CHtmlDialogPyDlg::OnInitDialog()
{
	//add scroll bar,disable drag and drop.
	SetHostFlags(DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DIALOG);

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
	PySetStrW(para,0);
	PyEvalA("_js_fun()");
	return CComBSTR (PyGetStr());
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
	if (pMsg->message==1024)//disable retriving document by WM_HTML_GETOBJECT.
	{
	//	if (MessageBoxA(m_hWnd,"�Ƿ������ȡhtml�ĵ���","�յ�WM_HTML_GETOBJECT",MB_YESNO|MB_ICONWARNING)==IDNO)
		{
			return TRUE;
		}
	}
	return CDHtmlDialog::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CHtmlDialogPyDlg::OnCallJs(WPARAM wParam, LPARAM lParam)
{
	IHTMLDocument2* pDoc2 = NULL;
	VARIANT varResult;
	CDHtmlDialog::GetDHtmlDocument(&pDoc2);
	IDispatch *pDispScript = NULL;
	HRESULT hResult;
	hResult = pDoc2->get_Script(&pDispScript);
	if (FAILED(hResult))
	{
		return 0;
	}

	DISPID   dispid;
	CComBSTR objbstrValue = _T("CallJs");
	BSTR bstrValue = objbstrValue.Copy();
	OLECHAR *pszFunct = bstrValue;
	hResult = pDispScript->GetIDsOfNames(IID_NULL,
		&pszFunct,
		1,
		LOCALE_SYSTEM_DEFAULT,
		&dispid);
	if (FAILED(hResult))
	{
		pDispScript->Release();
		return 0;
	}
	DISPPARAMS dispParams;
	VARIANTARG var;
	dispParams.cArgs = 1;
	dispParams.cNamedArgs = 0;
	dispParams.rgdispidNamedArgs = NULL;
	dispParams.rgvarg = &var;
	dispParams.rgvarg[0].vt = VT_BSTR;
	dispParams.rgvarg[0].bstrVal = CComBSTR(m_str_tmp.GetBuffer());
	varResult.vt = VT_VARIANT;
	hResult = pDispScript->Invoke(dispid,
		IID_NULL, LOCALE_USER_DEFAULT,
		DISPATCH_METHOD,
		&dispParams,
		&varResult,
		0,
		0);
	pDispScript->Release();
	m_str_tmp = varResult.bstrVal;
	return 1;
}
