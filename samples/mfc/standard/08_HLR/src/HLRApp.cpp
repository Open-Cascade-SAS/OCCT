// HLRApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "HLRApp.h"

#include <OCC_MainFrame.h>
#include <OCC_3dChildFrame.h>
#include "HLRDoc.h"
#include <OCC_2dChildFrame.h>
#include "HLRView2D.h"
#include <OCC_3dView.h>
// End CasCade

#ifdef _DEBUG
// CasCade :
//#define new DEBUG_NEW
// End CasCade

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHLRApp construction

CHLRApp::CHLRApp() : OCC_App()
{
  SampleName = "HLR"; //for about dialog
  SetSamplePath (L"..\\..\\08_HLR");
}

CHLRApp::~CHLRApp()
{
  delete pDocTemplateForView2d;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHLRApp object

CHLRApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHLRApp initialization

BOOL CHLRApp::InitInstance()
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

  // CasCade :


  pDocTemplateForView2d = new CMultiDocTemplate(
    IDR_2DTYPE,
    RUNTIME_CLASS(CHLRDoc),
    RUNTIME_CLASS(OCC_2dChildFrame), // custom MDI child frame
    RUNTIME_CLASS(CHLRView2D));

// AddDocTemplate(pDocTemplateForView2d);

// End CasCade
  pDocTemplateForView3d  = new CMultiDocTemplate(
    IDR_3DTYPE,
    RUNTIME_CLASS(CHLRDoc),
    RUNTIME_CLASS(OCC_3dChildFrame), // custom MDI child frame
    RUNTIME_CLASS(OCC_3dView));

  AddDocTemplate(pDocTemplateForView3d);


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
  pMainFrame->MDITile(MDITILE_VERTICAL);
  pMainFrame->ShowWindow(m_nCmdShow);
  pMainFrame->UpdateWindow();

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHLRApp commands

//===================================================

CFrameWnd*  CHLRApp::CreateView2D(CHLRDoc* pDoc )
{
  ASSERT_VALID(pDoc);
  ASSERT_VALID(pDocTemplateForView2d);
  CRuntimeClass * pViewClass = RUNTIME_CLASS(OCC_2dView);
  ASSERT(pViewClass != (CRuntimeClass *)NULL );
  // Create a new frame window
  CFrameWnd* pNewFrame = pDocTemplateForView2d->CreateNewFrame(pDoc, NULL);
  ASSERT_VALID(pDoc);
  pDocTemplateForView2d->InitialUpdateFrame(pNewFrame, pDoc);
  ASSERT_VALID(pDoc);
return pNewFrame;
}

//===================================================
CFrameWnd*  CHLRApp::CreateView3D(CHLRDoc* pDoc )
{
  ASSERT_VALID(pDoc);
  ASSERT_VALID(pDocTemplateForView3d);
  CRuntimeClass * pViewClass = RUNTIME_CLASS(OCC_3dView);
  ASSERT(pViewClass != (CRuntimeClass *)NULL );
  // Create a new frame window
  CFrameWnd* pNewFrame = pDocTemplateForView3d->CreateNewFrame(pDoc, NULL);
  pDocTemplateForView3d->InitialUpdateFrame(pNewFrame, pDoc);
return pNewFrame;

}


BOOL CHLRApp::IsViewExisting(CDocument * pDoc, CRuntimeClass * pViewClass, CView * & pView)
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



