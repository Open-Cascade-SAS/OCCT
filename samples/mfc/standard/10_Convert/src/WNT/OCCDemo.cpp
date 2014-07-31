// OCCDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OCCDemo.h"

#include "MainFrm.h"
#include "OCCDemoDoc.h"
#include "OCCDemoView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COCCDemoApp

BEGIN_MESSAGE_MAP(COCCDemoApp, CWinApp)
	//{{AFX_MSG_MAP(COCCDemoApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COCCDemoApp construction

COCCDemoApp::COCCDemoApp() : OCC_App()
{
  SampleName = "";
  try
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
    myGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
  }
  catch (Standard_Failure)
  {
    AfxMessageBox (L"Fatal Error During Graphic Initialisation");
  }
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COCCDemoApp object

COCCDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COCCDemoApp initialization

BOOL COCCDemoApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COCCDemoDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(COCCDemoView));
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	
  // Create result dialog
  AfxInitRichEdit();
  CMainFrame* cFrame = (CMainFrame*) GetMainWnd();
  COCCDemoDoc* aDoc = (COCCDemoDoc*) cFrame->GetActiveDocument();

  // pass cFrame->GetDesktopWindow() as parent to have an independent dialog
  // pass cFrame as parent to have the result dialog always above the main window
  if (!aDoc->GetResultDialog()->Create(CResultDialog::IDD, cFrame->GetDesktopWindow()))
    TRACE0("Failed to create result dialog\n");

  aDoc->GetResultDialog()->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	aDoc->GetResultDialog()->ShowWindow(SW_HIDE);
  aDoc->GetResultDialog()->Initialize();

  // resize the result dialog, so no clipping occures when user
  // resizes the dialog manually
  // position the result dialog to the left bottom corner of the view
  CRect aDlgRect, aViewRect;
  aDoc->GetResultDialog()->GetWindowRect(&aDlgRect);
  cFrame->GetActiveView()->GetWindowRect(&aViewRect);

  int x = aViewRect.left;
  int y = aViewRect.bottom - aDlgRect.Size().cy;
  int cx = aDlgRect.Size().cx+1;
  int cy = aDlgRect.Size().cy+1;
  aDoc->GetResultDialog()->SetWindowPos(NULL, x, y, cx, cy, SWP_NOREDRAW | SWP_NOZORDER);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
  m_pMainWnd->SetFocus();
  aDoc->Start();

	return TRUE;
}

/*
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void COCCDemoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// COCCDemoApp message handlers

*/