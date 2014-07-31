// Viewer2dApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Viewer2dApp.h"

#include "OCC_MainFrame.h"
#include "OCC_2dChildFrame.h"
#include "Viewer2dDoc.h"
#include "OCC_2dView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewer2dApp construction

CViewer2dApp::CViewer2dApp() : OCC_App()
{
  SampleName = "Viewer2d"; //for about dialog
  SetSamplePath (L"..\\..\\03_Viewer2d");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CViewer2dApp object

CViewer2dApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CViewer2dApp initialization

BOOL CViewer2dApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_2DTYPE,
		RUNTIME_CLASS(CViewer2dDoc),
		RUNTIME_CLASS(OCC_2dChildFrame),
		RUNTIME_CLASS(OCC_2dView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	OCC_MainFrame* pMainFrame = new OCC_MainFrame;
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
