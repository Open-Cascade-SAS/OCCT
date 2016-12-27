// HLRDoc.cpp : implementation of the CHLRDoc class
//


#include "stdafx.h"

#include "HLRDoc.h"
#include "HLRApp.h"
#include <OCC_2dView.h>
#include <OCC_3dView.h>


#include <ImportExport/ImportExport.h>
#include "AISDialogs.h"
#include <AIS_ListOfInteractive.hxx>

#ifdef _DEBUG
//#define new DEBUG_NEW  // by cascade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc

IMPLEMENT_DYNCREATE(CHLRDoc, CDocument)


BEGIN_MESSAGE_MAP(CHLRDoc, OCC_3dBaseDoc)
  //{{AFX_MSG_MAP(CHLRDoc)
  ON_COMMAND(ID_WINDOW_NEW3D, OnWindowNew3d)
  ON_COMMAND(ID_WINDOW_NEW2D, OnWindowNew2d)
  ON_COMMAND(ID_FILE_HLR, OnBUTTONHLRDialog)
  ON_COMMAND(ID_FILE_IMPORT_BREP, OnFileImportBrep)
  ON_COMMAND(ID_BUTTON_HLRDialog, OnBUTTONHLRDialog)
  ON_COMMAND(ID_OBJECT_ERASE, OnObjectErase)
  //}}AFX_MSG_MAP


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc construction/destruction

CHLRDoc::CHLRDoc()
{
  // TODO: add one-time construction code here
  Handle(Graphic3d_GraphicDriver) theGraphicDriver = 
    ((CHLRApp*)AfxGetApp())->GetGraphicDriver();

  // VIEWER 3D
  myViewer = new V3d_Viewer (theGraphicDriver);
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  myAISContext =new AIS_InteractiveContext (myViewer);

  // 2D VIEWER: exploit V3d viewer for 2D visualization
  my2DViewer = new V3d_Viewer (theGraphicDriver);
  my2DViewer->SetCircularGridValues (0, 0, 10, 8, 0);
  my2DViewer->SetRectangularGridValues (0, 0, 10, 10, 0);

  //Set projection mode for 2D visualization
  my2DViewer->SetDefaultViewProj (V3d_Zpos);

  myInteractiveContext2D = new AIS_InteractiveContext (my2DViewer);

  CFrameWnd* pFrame2d = ((CHLRApp*)AfxGetApp())->CreateView2D (this);
  pFrame2d->ShowWindow (SW_SHOWNORMAL);
  myCSelectionDialogIsCreated = false;
}

CHLRDoc::~CHLRDoc()
{
  if (myCSelectionDialogIsCreated)
  {
    myCSelectionDialog->ShowWindow(SW_ERASE);
    delete myCSelectionDialog;
  }
}

void CHLRDoc::OnWindowNew2d()
{
  ((CHLRApp*)AfxGetApp())->CreateView2D(this);
}

void CHLRDoc::OnWindowNew3d()
{
  ((CHLRApp*)AfxGetApp())->CreateView3D(this);
}

//  nCmdShow could be :    ( default is SW_RESTORE )
// SW_HIDE   SW_SHOWNORMAL   SW_NORMAL   
// SW_SHOWMINIMIZED     SW_SHOWMAXIMIZED    
// SW_MAXIMIZE          SW_SHOWNOACTIVATE   
// SW_SHOW              SW_MINIMIZE         
// SW_SHOWMINNOACTIVE   SW_SHOWNA           
// SW_RESTORE           SW_SHOWDEFAULT      
// SW_MAX    

// use pViewClass = RUNTIME_CLASS( CHLRView3D ) for 3D Views
// use pViewClass = RUNTIME_CLASS( CHLRView2D ) for 2D Views

void CHLRDoc::ActivateFrame(CRuntimeClass* pViewClass,int nCmdShow)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf(pViewClass) )
    {
      ASSERT_VALID(pCurrentView);
      CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      pParentFrm->ActivateFrame(nCmdShow);
    }
  }
}

void CHLRDoc::FitAll2DViews(Standard_Boolean UpdateViewer)
{
  if (UpdateViewer)   my2DViewer->Update();
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf(RUNTIME_CLASS(OCC_2dView)) )
    {
      ASSERT_VALID(pCurrentView);
      ((OCC_2dView*)pCurrentView)->GetV2dView()->FitAll();
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc diagnostics

#ifdef _DEBUG
void CHLRDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CHLRDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc commands
void CHLRDoc::OnBUTTONHLRDialog()
{
  if (!myCSelectionDialogIsCreated)
  {
    myCSelectionDialog = new CSelectionDialog(this,AfxGetMainWnd());
    myCSelectionDialog->Create(CSelectionDialog::IDD, AfxGetMainWnd());
    myCSelectionDialogIsCreated = true;
  }
  myCSelectionDialog->ShowWindow(SW_RESTORE);
  myCSelectionDialog->UpdateWindow();
}

void CHLRDoc::OnFileImportBrep() 
{   CImportExport::ReadBREP(myAISContext);
	Fit();
}
void CHLRDoc::Fit()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf(RUNTIME_CLASS(OCC_3dView)) )
    {
      ((OCC_3dView *) pCurrentView)->FitAll();
    }
  }
}

void CHLRDoc::OnObjectErase()
{
  Standard_Boolean toUpdateDisplayable = Standard_False;
  myAISContext->InitSelected();
  while (myAISContext->MoreSelected())
  {
    if (myAISContext->SelectedInteractive()->Type() == AIS_KOI_Shape && myCSelectionDialogIsCreated)
    {
      myCSelectionDialog->DiplayableShape()->Remove (Handle(AIS_Shape)::DownCast (myAISContext->SelectedInteractive())->Shape());
      toUpdateDisplayable = Standard_True;
    }

    myAISContext->Erase (myAISContext->SelectedInteractive(), Standard_True);
    myAISContext->InitSelected();
  }

  myAISContext->ClearSelected (Standard_True);

  if (toUpdateDisplayable)
  {
    // Update view in the HLR dialog if list of displayable shapes has been changed.
    myCSelectionDialog->UpdateViews();
  }
}
