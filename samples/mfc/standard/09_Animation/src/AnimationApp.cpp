// AnimationApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "AnimationApp.h"

#include <OCC_MainFrame.h>
#include <OCC_3dChildFrame.h>
#include "AnimationDoc.h"
#include "AnimationView3D.h"
#include "..\..\Common\res\OCC_Resource.h"

BEGIN_MESSAGE_MAP(CAnimationApp, OCC_App)
	//{{AFX_MSG_MAP(CAnimationApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationApp construction

CAnimationApp::CAnimationApp() : OCC_App()
{
  SampleName = "Animation"; //for about dialog
  SetSamplePath (L"..\\..\\09_Animation");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAnimationApp object

CAnimationApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAnimationApp initialization

BOOL CAnimationApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// Modified by CasCade :
	SetRegistryKey(_T("Local CasCade Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplateForView3d;
    pDocTemplateForView3d  = new CMultiDocTemplate(
		IDR_3DTYPE,
		RUNTIME_CLASS(CAnimationDoc),
		RUNTIME_CLASS(OCC_3dChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAnimationView3D));
	AddDocTemplate(pDocTemplateForView3d);


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

void CAnimationApp::OnAppAbout()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	CAnimationView3D *pView = (CAnimationView3D *) pChild->GetActiveView();
	pView -> OnStop();
	OCC_App::OnAppAbout();
	pView -> OnRestart();
}

