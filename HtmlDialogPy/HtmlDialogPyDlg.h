
// HtmlDialogPyDlg.h : ͷ�ļ�
//

#pragma once


// CHtmlDialogPyDlg �Ի���

class CHtmlDialogPyDlg : public CDHtmlDialog
{
// ����
public:
	CHtmlDialogPyDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HTMLDIALOGPY_DIALOG, IDH = IDR_HTML_HTMLDIALOGPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��
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
