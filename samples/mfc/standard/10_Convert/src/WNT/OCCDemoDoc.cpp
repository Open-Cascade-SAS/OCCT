// OCCDemoDoc.cpp : implementation of the COCCDemoDoc class
//

#include "stdafx.h"
#include "OCCDemo.h"

#include "OCCDemoDoc.h"
#include "OCCDemo_Presentation.h"
#include "OCCDemoView.h"

#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COCCDemoDoc

IMPLEMENT_DYNCREATE(COCCDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(COCCDemoDoc, CDocument)
	//{{AFX_MSG_MAP(COCCDemoDoc)
	ON_COMMAND(ID_BUTTONNext, OnBUTTONNext)
	ON_COMMAND(ID_BUTTONStart, OnBUTTONStart)
	ON_COMMAND(ID_BUTTONRepeat, OnBUTTONRepeat)
	ON_COMMAND(ID_BUTTONPrev, OnBUTTONPrev)
	ON_COMMAND(ID_BUTTONEnd, OnBUTTONEnd)
	ON_UPDATE_COMMAND_UI(ID_BUTTONNext, OnUpdateBUTTONNext)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPrev, OnUpdateBUTTONPrev)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_BUTTONShowResult, OnBUTTONShowResult)
	ON_UPDATE_COMMAND_UI(ID_BUTTONShowResult, OnUpdateBUTTONShowResult)
	ON_COMMAND(ID_DUMP_VIEW, OnDumpView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COCCDemoDoc construction/destruction

COCCDemoDoc::COCCDemoDoc()
{
	// TODO: add one-time construction code here
	Handle(Graphic3d_GraphicDriver) aGraphicDriver = 
		((COCCDemoApp*)AfxGetApp())->GetGraphicDriver();

	myViewer = new V3d_Viewer(aGraphicDriver);
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();
  myViewer->SetDefaultBackgroundColor (Quantity_NOC_BLACK);

	myAISContext = new AIS_InteractiveContext(myViewer);
  myShowResult = FALSE;
  myPresentation = OCCDemo_Presentation::Current;
  myPresentation->SetDocument(this);

  strcpy_s(myDataDir, "Data");
  strcpy_s(myLastPath, ".");
}

COCCDemoDoc::~COCCDemoDoc()
{
}

BOOL COCCDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
  SetTitle(myPresentation->GetName());

  myAISContext->EraseAll (Standard_False);
  myAISContext->SetDisplayMode (AIS_Shaded, Standard_True);

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
  {
    COCCDemoView* pView = (COCCDemoView*) GetNextView(pos);
    pView->Reset();
  }  

	return TRUE;
}

void COCCDemoDoc::Start()
{
  myPresentation->Init();
  OnBUTTONStart();
}

/////////////////////////////////////////////////////////////////////////////
// COCCDemoDoc serialization

void COCCDemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
		{}// TODO: add storing code here
	else
		{}// TODO: add loading code here
}

/////////////////////////////////////////////////////////////////////////////
// COCCDemoDoc diagnostics

#ifdef _DEBUG
void COCCDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COCCDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COCCDemoDoc commands

void COCCDemoDoc::OnFileNew()
{
  OnNewDocument();
  Start();
}

void COCCDemoDoc::InitViewButtons()
{
  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
  {
    COCCDemoView* pView = (COCCDemoView*) GetNextView(pos);
    pView->InitButtons();
  }
}

void COCCDemoDoc::DoSample()
{
  InitViewButtons();

  HCURSOR hOldCursor = ::GetCursor();
  HCURSOR hNewCursor = AfxGetApp()->LoadStandardCursor(IDC_APPSTARTING);

  SetCursor(hNewCursor);
  {
    try
    {
      myPresentation->DoSample();
    }
    catch (Standard_Failure const& anException)
    {
      Standard_SStream aSStream;
      aSStream << "An exception was caught: " << anException << ends;
      CString aMsg = aSStream.str().c_str();
      AfxMessageBox (aMsg);
    }
  }
  SetCursor(hOldCursor);
}

void COCCDemoDoc::OnBUTTONStart() 
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->FirstSample();
  DoSample();
}

void COCCDemoDoc::OnBUTTONEnd()
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->LastSample();
  DoSample();
}

void COCCDemoDoc::OnBUTTONRepeat() 
{
  DoSample();
}

void COCCDemoDoc::OnBUTTONNext() 
{
  if (!myPresentation->AtLastSample())
  {
    myPresentation->NextSample();
    DoSample();
  }
}

void COCCDemoDoc::OnBUTTONPrev() 
{
  if (!myPresentation->AtFirstSample())
  {
    myPresentation->PrevSample();
    DoSample();
  }
}

void COCCDemoDoc::OnUpdateBUTTONNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!myPresentation->AtLastSample());
}

void COCCDemoDoc::OnUpdateBUTTONPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!myPresentation->AtFirstSample());
}

void COCCDemoDoc::OnBUTTONShowResult()
{
  myShowResult = !myShowResult;
  if (myShowResult)
    myCResultDialog.ShowWindow(SW_SHOWNORMAL);
  else
    myCResultDialog.ShowWindow(SW_HIDE);
}

void COCCDemoDoc::OnUpdateBUTTONShowResult(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck (myShowResult);
}

void COCCDemoDoc::OnDumpView() 
{
  for (POSITION aPos = GetFirstViewPosition(); aPos != NULL;)
  {
    COCCDemoView* pView = (COCCDemoView* )GetNextView (aPos);
    pView->UpdateWindow();
  }

  myViewer->InitActiveViews();
  Handle(V3d_View) aView = myViewer->ActiveView();
  ExportView (aView);
}

void COCCDemoDoc::Fit()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	pView->FitAll();
}
