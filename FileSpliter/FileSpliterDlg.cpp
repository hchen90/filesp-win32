
// FileSpliterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileSpliter.h"
#include "FileSpliterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileSpliterDlg dialog



CFileSpliterDlg::CFileSpliterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILESPLITER_DIALOG, pParent), m_iCount(0), m_uiBlockSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileSpliterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileSpliterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CFileSpliterDlg::OnEnChangeMfceditbrowse1)
	ON_EN_CHANGE(IDC_EDIT1, &CFileSpliterDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_MFCBUTTON2, &CFileSpliterDlg::OnBnClickedMfcbutton2)
	ON_BN_CLICKED(IDC_MFCBUTTON1, &CFileSpliterDlg::OnBnClickedMfcbutton1)
	ON_BN_CLICKED(IDC_MFCBUTTON3, &CFileSpliterDlg::OnBnClickedMfcbutton3)
	ON_BN_CLICKED(IDC_MFCLINK1, &CFileSpliterDlg::OnBnClickedMfclink1)
END_MESSAGE_MAP()


// CFileSpliterDlg message handlers

void CFileSpliterDlg::FormatNumberString(CString& str, ULARGE_INTEGER num)
{
	char buffer[MAX_PATH];

	if (num.QuadPart >= 1024 * 1024 * 1024) {
		/*GB*/
		if (snprintf(buffer, sizeof(buffer), "%.2fGB", (double)num.QuadPart / 1024 / 1024 / 1024)) {
			str = buffer;
		}
	}
	else if (num.QuadPart >= 1024 * 1024 && num.QuadPart < 1024 * 1024 * 1024) {
		/*MB*/
		if (snprintf(buffer, sizeof(buffer), "%.2fMB", (double)num.QuadPart / 1024 / 1024)) {
			str = buffer;
		}
	}
	else if (num.QuadPart >= 1024 && num.QuadPart < 1024 * 1024) {
		/*KB*/
		if (snprintf(buffer, sizeof(buffer), "%.2fKB", (double)num.QuadPart / 1024)) {
			str = buffer;
		}
	}
	else {
		str.Format(_T("%uB"), num.QuadPart);
	}
}

void CFileSpliterDlg::UpdateInformation()
{
	ULARGE_INTEGER ava_cal, tot, ava;

	if (m_pTotal && m_pUsed && m_pAva) {
		CString str;

		m_pDirPath->GetWindowText(str);

		if (GetDiskFreeSpaceEx(str, &ava_cal, &tot, &ava)) {
			FormatNumberString(str, tot);
			m_pTotal->SetWindowText(str);
			tot.QuadPart -= ava.QuadPart;
			FormatNumberString(str, tot);
			m_pUsed->SetWindowTextW(str);
			FormatNumberString(str, ava_cal);
			m_pAva->SetWindowTextW(str);
			m_pDirPath->GetWindowText(str);
			if (str.GetLength() > 2) str.SetAt(2, 0);
			GetDlgItem(IDC_MFCLINK1)->SetWindowText(str);
		}
	}
}

void CFileSpliterDlg::InsertLCString(CString& txt1, CString& txt2)
{
	if (m_pLC) {
		m_pLC->InsertItem(m_iCount, txt1);
		m_pLC->SetItemText(m_iCount, 1, txt2);
		m_iCount++;
	}
}

void CFileSpliterDlg::GetBlockSize()
{
	if (m_pBytes != NULL && m_pBytesGroup != NULL) {
		CString str, str2;

		m_pBytesGroup->GetWindowText(str);
		m_pBytes->GetWindowText(str2);

#ifdef UNICODE
		char buffer[MAX_PATH] = {0};

		if (WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str2.GetString(), str2.GetLength(), buffer, sizeof(buffer), NULL, NULL)) {
			m_uiBlockSize = atoll(buffer);
		}

#else
		m_uiBlockSize = atoll(str2.GetString());
#endif
		
		if (str == CString(_T("Bytes"))) {
			m_uiBlockSize *= 1;
		}
		else if (str == CString(_T("KB"))) {
			m_uiBlockSize *= 1024;
		}
		else if (str == CString(_T("MB"))) {
			m_uiBlockSize *= 1024 * 1024;
		}
		else if (str == CString(_T("GB"))) {
			m_uiBlockSize *= 1024 * 1024 * 1024;
		}
	}
}

void CFileSpliterDlg::SplitFiles()
{
	if (m_tFileName.size() > 0) {
		// TODO
		CString str;
		LONG nCount;

		m_pDirPath->GetWindowText(str);

		if (GetFileAttributes(str) & FILE_ATTRIBUTE_DIRECTORY) {
			str += CString("\\");

			GetBlockSize();

			set<CString>::iterator i;

			for (i = m_tFileName.begin(); i != m_tFileName.end(); i++) {
				HANDLE hFile = CreateFile(*i, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				LARGE_INTEGER iFileSize;

				if (hFile != INVALID_HANDLE_VALUE && GetFileSizeEx(hFile, &iFileSize)) {
					for (nCount = 0; iFileSize.QuadPart > 0; nCount++) {
						CString file(str);
						CString iname;
						CString count;
						CString oname;

						TCHAR buf[MAX_PATH];

						count.Format(_T(".%d"), nCount);

						GetFileTitle(*i, buf, sizeof(buf));

						file += (iname = CString(buf));
						file += count;

						GetFileTitle(file, buf, sizeof(buf));

						oname = CString(buf);

						InsertLCString(CString(_T("Splitting ") + iname), oname);

						HANDLE hOut = CreateFile(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

						if (hOut != INVALID_HANDLE_VALUE) {
							if (iFileSize.QuadPart > m_uiBlockSize) {
								iFileSize.QuadPart -= m_uiBlockSize;
								
								HGLOBAL hGol = GlobalAlloc(GPTR, m_uiBlockSize);
								LPVOID pMem = (HGLOBAL)hGol;

								if (hGol != NULL) {
									DWORD dwRD;
									if (ReadFile(hFile, pMem, m_uiBlockSize, &dwRD, NULL)) {
										WriteFile(hOut, pMem, m_uiBlockSize, &dwRD, NULL);
									}
									GlobalFree(hGol);
								}
							}
							else {
								HGLOBAL hGol = GlobalAlloc(GPTR, iFileSize.QuadPart);
								LPVOID pMem = (LPVOID)hGol;

								if (hGol != NULL) {
									DWORD dwRD, dwWT;
									if (ReadFile(hFile, pMem, iFileSize.QuadPart, &dwRD, NULL)) {
										WriteFile(hOut, pMem, dwRD, &dwWT, NULL);
									}
									GlobalFree(hGol);
								}

								iFileSize.QuadPart = 0;
							}

							CloseHandle(hOut);
						}
					}
				}

				if (hFile != INVALID_HANDLE_VALUE) {
					CloseHandle(hFile);
				}
			}
		}
		else {
			AfxMessageBox(CString(_T("Not a directory! (")) + str + CString(_T(")")));
		}
	}
}

BOOL CFileSpliterDlg::IsDotNumber(CString& str, CString& str2)
{
	int len = str.GetLength();

	for (len--; len > 0; len--) {
		if (str.GetAt(len) == '.') {
			break;
		}
	}

	if (str.GetAt(len) == '.' && len + 1 < str.GetLength()) {
		str2 = CString(str);

		str2.SetAt(len, 0);

		CString nstr = str.Mid(len + 1);
		
		len = nstr.GetLength();

		int i, num = 0;

		for (i = 0; i < len; i++) {
			if (nstr.GetAt(i) >= '0' && nstr.GetAt(i) <= '9') {
				num = 1;
			}
			else {
				num = 0;
				break;
			}
		}

		if (num) return TRUE;
	}

	return FALSE;
}

void CFileSpliterDlg::MergeFiles()
{
	if (m_tFileName.size() > 1) {
		CString str, last;
		HANDLE hFileOut = INVALID_HANDLE_VALUE;

		m_pDirPath->GetWindowText(str);

		if (GetFileAttributes(str) & FILE_ATTRIBUTE_DIRECTORY) {
			set<CString>::iterator i;
			
			for (i = m_tFileName.begin(); i != m_tFileName.end(); i++) {
				CString in_file(*i);
				CString out_file;

				if (IsDotNumber(in_file, out_file)) {
					if (hFileOut != INVALID_HANDLE_VALUE && out_file != last) {
						CloseHandle(hFileOut);
						hFileOut = INVALID_HANDLE_VALUE;
					}
					if (hFileOut == INVALID_HANDLE_VALUE) {
						hFileOut = CreateFile(out_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					}
					if (hFileOut != INVALID_HANDLE_VALUE) {
						last = out_file;

						HANDLE hFileIn = CreateFile(in_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						
						if (hFileIn != INVALID_HANDLE_VALUE) {
							DWORD rd, wt;
							DWORD size = GetFileSize(hFileIn, NULL);
							
							if (size > 0) {
								HGLOBAL hGol = GlobalAlloc(GPTR, size);
								
								if (hGol != NULL) {
									LPVOID pMem = (LPVOID)hGol;

									if (ReadFile(hFileIn, pMem, size, &rd, NULL)) {
										WriteFile(hFileOut, pMem, rd, &wt, NULL);
									}

									InsertLCString(CString(_T("Merging ")) + out_file, in_file);
									GlobalFree(hGol);
								}
							}

							CloseHandle(hFileIn);
						}
					}
				}
			}
		}
		else {
			AfxMessageBox(CString(_T("Not a directory! (") + str + CString(")")));
		}
		
		if (hFileOut != INVALID_HANDLE_VALUE) {
			CloseHandle(hFileOut);
		}
	}
}

void CFileSpliterDlg::OnClose()
{
	CDialogEx::OnOK();
}

BOOL CFileSpliterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	if (m_pBytesGroup = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO1))) {
		m_pBytesGroup->AddString(_T("Bytes"));
		m_pBytesGroup->AddString(_T("KB"));
		m_pBytesGroup->AddString(_T("MB"));
		m_pBytesGroup->AddString(_T("GB"));
		m_pBytesGroup->SetCurSel(3);
	}

	if (m_pBytes = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO2))) {
		m_pBytes->AddString(_T("1024"));
		m_pBytes->AddString(_T("512"));
		m_pBytes->SetCurSel(1);
	}

	if (m_pDirPath = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT1))) {
		TCHAR buffer[MAX_PATH];

		if (GetCurrentDirectory(sizeof(buffer), buffer)) {
			m_pDirPath->SetWindowText(buffer);
		}
	}

	if (m_pLC = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST1))) {
		m_pLC->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		
		RECT rc;
		
		m_pLC->GetWindow(0)->GetWindowRect(&rc);
		
		LVCOLUMN lvc = {0};

		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = 0.3 * (rc.right - rc.left);
		lvc.pszText = _T("Status");

		m_pLC->InsertColumn(0, &lvc);

		lvc.cx = 0.6 * (rc.right - rc.left);
		lvc.pszText = _T("Text");

		m_pLC->InsertColumn(1, &lvc);

		CString str;

		m_pDirPath->GetWindowText(str);
		
		InsertLCString(CString(_T("Output Directory")), str);
	}

	m_pTotal = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT2));
	m_pUsed = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT3));
	m_pAva = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT4));
	m_pBC = reinterpret_cast<CMFCEditBrowseCtrl*>(GetDlgItem(IDC_MFCEDITBROWSE1));
	
	UpdateInformation();

	if (m_pMerge = reinterpret_cast<CMFCButton*>(GetDlgItem(IDC_MFCBUTTON1))) {
		m_pMerge->SetTextColor(RGB(0, 0x80, 0));
		m_pMerge->EnableWindow(FALSE);
	}

	if (m_pSplit = reinterpret_cast<CMFCButton*>(GetDlgItem(IDC_MFCBUTTON2))) {
		m_pSplit->SetTextColor(RGB(0x80, 0, 0x80));
		m_pSplit->EnableWindow(FALSE);
	}

	if (m_pAbout = reinterpret_cast<CMFCButton*>(GetDlgItem(IDC_MFCBUTTON3))) {
		m_pAbout->SetTextColor(RGB(0x80, 0x80, 0));
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileSpliterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileSpliterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileSpliterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileSpliterDlg::OnDropFiles(HDROP hDrop)
{
	TCHAR buffer[MAX_PATH];

	UINT n = DragQueryFile(hDrop, -1, buffer, sizeof(buffer));

	if (n > 0) {
		UINT i;

		for (i = 0; i < n; i++) {
			if (DragQueryFile(hDrop, i, buffer, sizeof(buffer)) > 0) {
				m_tFileName.insert(CString(buffer));
				GetDlgItem(IDC_MFCEDITBROWSE1)->SetWindowText(buffer);
			}
		}
	}

	if (m_tFileName.size() > 0) {
		m_pSplit->EnableWindow(TRUE);
	}
	else {
		m_pSplit->EnableWindow(FALSE);
	}

	if (m_tFileName.size() > 1) {
		m_pMerge->EnableWindow(TRUE);
	}
	else {
		m_pMerge->EnableWindow(FALSE);
	}

	DragFinish(hDrop);
}

void CFileSpliterDlg::OnEnChangeMfceditbrowse1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (m_pBC != NULL) {
		CString str;

		m_pBC->GetWindowText(str);
		InsertLCString(CString(_T("Adding File")), str);

		m_tFileName.insert(str);

		if (m_tFileName.size() > 0) {
			m_pSplit->EnableWindow(TRUE);
		}
		else {
			m_pSplit->EnableWindow(FALSE);
		}

		if (m_tFileName.size() > 1) {
			m_pMerge->EnableWindow(TRUE);
		}
		else {
			m_pMerge->EnableWindow(FALSE);
		}
	}
}

void CFileSpliterDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	
	UpdateInformation();

	CString str;

	m_pDirPath->GetWindowText(str);

	if (GetFileAttributes(str) == FILE_ATTRIBUTE_DIRECTORY) {
		InsertLCString(CString(_T("Output Directory")), str);
	}
}


void CFileSpliterDlg::OnBnClickedMfcbutton2()
/*Split*/
{
	// TODO: Add your control notification handler code here
	SplitFiles();
}


void CFileSpliterDlg::OnBnClickedMfcbutton1()
/*Merge*/
{
	// TODO: Add your control notification handler code here
	MergeFiles();
}


void CFileSpliterDlg::OnBnClickedMfcbutton3()
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}


void CFileSpliterDlg::OnBnClickedMfclink1()
{
	// TODO: Add your control notification handler code here
	CString str;

	GetDlgItem(IDC_MFCLINK1)->GetWindowText(str);

	if (str.GetLength() > 0) {
		ShellExecute(this->m_hWnd, _T("explore"), str, NULL, NULL, SW_NORMAL);
	}
}
