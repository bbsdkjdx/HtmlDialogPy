
// HtmlDialogPyDlg.h : 头文件
//

#pragma once


// CHtmlDialogPyDlg 对话框

class CHtmlDialogPyDlg : public CDHtmlDialog
{
// 构造
public:
	CHtmlDialogPyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HTMLDIALOGPY_DIALOG, IDH = IDR_HTML_HTMLDIALOGPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_DISPATCH_MAP()
public:
	void ext_fun();
	virtual BOOL CanAccessExternal();
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
public:
	POINT m_fixed_size;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual void OnOK();
};
