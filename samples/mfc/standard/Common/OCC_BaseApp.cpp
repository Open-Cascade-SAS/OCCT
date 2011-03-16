// OCC_BaseApp.cpp: implementation of the OCC_BaseApp class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_BaseApp.h"

#include <res\OCC_Resource.h>

#include <Standard_Version.hxx>

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseApp

BEGIN_MESSAGE_MAP(OCC_BaseApp, CWinApp)
	//{{AFX_MSG_MAP(OCC_BaseApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseApp construction

OCC_BaseApp::OCC_BaseApp()
{
	SampleName = "";
	SetSamplePath(NULL);
}


void OCC_BaseApp::SetSamplePath(LPCTSTR aPath)
{
	char AbsoluteExecutableFileName[MAX_PATH+1];
	HMODULE hModule = GetModuleHandle(NULL);
	GetModuleFileName(hModule, AbsoluteExecutableFileName, MAX_PATH);

	SamplePath = CString(AbsoluteExecutableFileName);
	int index = SamplePath.ReverseFind('\\');
	SamplePath.Delete(index+1, SamplePath.GetLength() - index - 1);
	if (aPath == NULL)
		SamplePath += "..";
	else{
		CString aCInitialDir(aPath);
		SamplePath += "..\\" + aCInitialDir;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlgStd dialog used for App About

class CAboutDlgStd : public CDialog
{
public:
	CAboutDlgStd();
	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CAboutDlgStd)
	enum { IDD = IDD_OCC_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlgStd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	//{{AFX_MSG(CAboutDlgStd)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlgStd::CAboutDlgStd() : CDialog(CAboutDlgStd::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlgStd)
	//}}AFX_DATA_INIT
}

void CAboutDlgStd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlgStd)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlgStd, CDialog)
	//{{AFX_MSG_MAP(CAboutDlgStd)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlgStd::OnInitDialog(){
	CWnd* Title = GetDlgItem(IDC_ABOUTBOX_TITLE);

	CString About = "About ";
	CString Sample = "Sample ";
	CString SampleName = ((OCC_BaseApp*)AfxGetApp())->GetSampleName();
	CString Cascade = ", Open CASCADE Technology ";
	CString Version = OCC_VERSION_STRING;
	
	CString strTitle = Sample + SampleName + Cascade + Version;
	CString dlgTitle = About + SampleName;

	Title->SetWindowText(strTitle);
	SetWindowText(dlgTitle);

	CenterWindow();
	
	return TRUE;
}

// App command to run the dialog
void OCC_BaseApp::OnAppAbout()
{
	CAboutDlgStd aboutDlg;
	aboutDlg.DoModal();
}

LPCTSTR OCC_BaseApp::GetSampleName()
{
	return SampleName;
}

LPCTSTR OCC_BaseApp::GetInitDataDir()
{
	return (LPCTSTR) SamplePath;
}

void OCC_BaseApp::SetSampleName(LPCTSTR Name)
{
	SampleName = Name;
}
