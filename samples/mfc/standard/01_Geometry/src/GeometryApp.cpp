// GeometryApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "GeometryApp.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "GeometryDoc.h"
#include "GeometryView.h"
#include "GeometryView2d.h"
#include "ChildFrm2D.h"
#include "GeometryView2d.h"

/////////////////////////////////////////////////////////////////////////////
// CGeometryApp construction

CGeometryApp::CGeometryApp() : OCC_App()
{
  SampleName = "Geometry"; //for about dialog
  SetSamplePath (L"..\\..\\01_Geometry");
}

CGeometryApp::~CGeometryApp()
{
  delete pDocTemplateForView2d;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CGeometryApp object

CGeometryApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGeometryApp initialization

BOOL CGeometryApp::InitInstance()
{
  AfxInitRichEdit();
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

  pDocTemplateForView3d = new CMultiDocTemplate(
    IDR_3DTYPE,
    RUNTIME_CLASS(CGeometryDoc),
    RUNTIME_CLASS(CChildFrame), // custom MDI child frame
    RUNTIME_CLASS(CGeometryView));
  AddDocTemplate(pDocTemplateForView3d);

  pDocTemplateForView2d = new CMultiDocTemplate(
    IDR_2DTYPE,
    RUNTIME_CLASS(CGeometryDoc),
    RUNTIME_CLASS(CChildFrame2D), // custom MDI child frame
    RUNTIME_CLASS(CGeometryView2D));
  //AddDocTemplate(pDocTemplateForView2d);

  // create main MDI Frame window
  CMainFrame* pMainFrame = new CMainFrame;
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
// CGeometryApp commands

//===================================================

CFrameWnd*  CGeometryApp::CreateView2D(CGeometryDoc* pDoc )
{
  ASSERT_VALID(pDoc);
  ASSERT_VALID(pDocTemplateForView2d);
  CRuntimeClass * pViewClass = RUNTIME_CLASS(CGeometryView2D);
  ASSERT(pViewClass != (CRuntimeClass *)NULL );
  // Create a new frame window
  CFrameWnd* pNewFrame = pDocTemplateForView2d->CreateNewFrame(pDoc, NULL);
  pDocTemplateForView2d->InitialUpdateFrame(pNewFrame, pDoc);
return pNewFrame;
}

BOOL CGeometryApp::IsViewExisting(CDocument * pDoc, CRuntimeClass * pViewClass, CView * & pView)
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

