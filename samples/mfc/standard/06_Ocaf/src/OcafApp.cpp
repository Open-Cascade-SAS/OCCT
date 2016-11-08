// OcafApp.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#include "OcafApp.h"

#include "OcafMainFrm.h"
#include <OCC_3dChildFrame.h>
#include "OcafDoc.h"
#include <OCC_3dView.h>

#include "direct.h"
#include <OSD_Environment.hxx>

#include <BinDrivers.hxx>
#include <XmlDrivers.hxx>

/////////////////////////////////////////////////////////////////////////////
// COcafApp

BEGIN_MESSAGE_MAP(COcafApp, CWinApp)
	//{{AFX_MSG_MAP(COcafApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// COcafApp construction

COcafApp::COcafApp() : OCC_App()
{
  myApp = new TOcaf_Application();
  SampleName = "OCAF"; //for about dialog
  SetSamplePath (L"..\\..\\06_Ocaf");

  // load persistence
  BinDrivers::DefineFormat (myApp);
  XmlDrivers::DefineFormat (myApp);

  try
  {
    UnitsAPI::SetLocalSystem(UnitsAPI_MDTV);
  }
  catch (Standard_Failure)
  {
    AfxMessageBox (L"Fatal Error in units initialisation");
  }
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COcafApp object

COcafApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COcafApp initialization

BOOL COcafApp::InitInstance()
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

    pDocTemplateForView3d  = new CMultiDocTemplate(
		IDR_3DTYPE,
		RUNTIME_CLASS(COcafDoc),
		RUNTIME_CLASS(OCC_3dChildFrame), 
		RUNTIME_CLASS(OCC_3dView));
	AddDocTemplate(pDocTemplateForView3d);

	// create main MDI Frame window
	OcafMainFrame* pMainFrame = new OcafMainFrame;
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
// COcafApp commands

BOOL COcafApp::IsViewExisting(CDocument * pDoc, CRuntimeClass * pViewClass, CView * & pView)
{
  ASSERT_VALID(pDoc);
  ASSERT(pViewClass != (CRuntimeClass *)NULL );

  POSITION position = pDoc->GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = pDoc->GetNextView(position);
    ASSERT_VALID(pCurrentView);
    if (pCurrentView->IsKindOf(pViewClass))
	{
      pView = pCurrentView;
	  return TRUE;
	}
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COCcafApp message handlers

//================================================================
// Function : COcafApp::OnFileOpen()
// Purpose  :
//================================================================
void COcafApp::OnFileOpen() 
{
	CFileDialog aDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,
	   L"OCAFSample(Binary) (*.cbf)|*.cbf|OCAFSample(XML) (*.xml)|*.xml||");

	if (aDlg.DoModal() != IDOK) 
        return;

  OpenDocumentFile (aDlg.GetPathName());
}
