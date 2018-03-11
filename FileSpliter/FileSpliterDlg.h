
// FileSpliterDlg.h : header file
//

#pragma once


// CFileSpliterDlg dialog
class CFileSpliterDlg : public CDialogEx
{
// Construction
public:
	CFileSpliterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESPLITER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDrop);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeMfceditbrowse1();

private:
	void FormatNumberString(CString& str, ULARGE_INTEGER num);
	void UpdateInformation();
	void InsertLCString(CString& txt1, CString& txt2);
	void GetBlockSize();
	void SplitFiles();
	void MergeFiles();
	BOOL IsDotNumber(CString& str, CString& str2);
	
	LONG m_iCount;
	LONGLONG m_uiBlockSize;

	set<CString> m_tFileName;

	CMFCEditBrowseCtrl* m_pBC;
	CMFCButton* m_pAbout;
	CMFCButton* m_pMerge;
	CMFCButton* m_pSplit;
	CListCtrl* m_pLC;
	CComboBox* m_pBytesGroup;
	CComboBox* m_pBytes;
	CEdit* m_pDirPath;
	CEdit* m_pTotal;
	CEdit* m_pUsed;
	CEdit* m_pAva;
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedMfcbutton2();
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg void OnBnClickedMfcbutton3();
	afx_msg void OnBnClickedMfclink1();
};
