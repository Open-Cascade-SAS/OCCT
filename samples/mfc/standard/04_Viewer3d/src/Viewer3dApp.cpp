// Viewer3dApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Viewer3dApp.h"

#include <OCC_MainFrame.h>
#include <OCC_3dChildFrame.h>
#include "Viewer3dDoc.h"
#include "Viewer3dView.h"

/////////////////////////////////////////////////////////////////////////////
// CViewer3dApp

BEGIN_MESSAGE_MAP(CViewer3dApp, CWinApp)
	//{{AFX_MSG_MAP(CViewer3dApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewer3dApp construction

CViewer3dApp::CViewer3dApp() : OCC_App()
{
  SampleName = "Viewer3d"; //for about dialog
  SetSamplePath (L"..\\..\\03_Viewer3d");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CViewer3dApp object

CViewer3dApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CViewer3dApp initialization

BOOL CViewer3dApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_3DTYPE,
		RUNTIME_CLASS(CViewer3dDoc),
		RUNTIME_CLASS(OCC_3dChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CViewer3dView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	OCC_MainFrame* pMainFrame = new OCC_MainFrame(with_AIS_TB);
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CViewer3dApp commands
