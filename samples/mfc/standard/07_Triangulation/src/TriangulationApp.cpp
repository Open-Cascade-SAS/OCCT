// TriangulationApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "TriangulationApp.h"

#include "OCC_MainFrame.h"
#include "OCC_3dChildFrame.h"
#include "TriangulationDoc.h"
#include "OCC_3dView.h"

/////////////////////////////////////////////////////////////////////////////
// CTriangulationApp construction

CTriangulationApp::CTriangulationApp() : OCC_App()
{
  SampleName = "TopologyTriangulation"; //for about dialog
  SetSamplePath (L"..\\..\\07_Triangulation");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTriangulationApp object

CTriangulationApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTriangulationApp initialization

BOOL CTriangulationApp::InitInstance()
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
		RUNTIME_CLASS(CTriangulationDoc),
		RUNTIME_CLASS(OCC_3dChildFrame), // custom MDI child frame
		RUNTIME_CLASS(OCC_3dView));
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

