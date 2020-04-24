// OCC_2dView.cpp: implementation of the OCC_2dView class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "OCC_2dView.h"

#include "OCC_App.h"
#include "OCC_2dDoc.h"
#include "resource2d\RectangularGrid.h"
#include "resource2d\CircularGrid.h"

#include "Quantity_Color.hxx"
#include "Quantity_NameOfColor.hxx"

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView

IMPLEMENT_DYNCREATE(OCC_2dView, OCC_BaseView)

BEGIN_MESSAGE_MAP(OCC_2dView, OCC_BaseView)
	//{{AFX_MSG_MAP(OCC_2dView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
  ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
  ON_COMMAND(ID_BUTTON2DGridRectLines, OnBUTTONGridRectLines)
  ON_COMMAND(ID_BUTTON2DGridRectPoints, OnBUTTONGridRectPoints)
  ON_COMMAND(ID_BUTTON2DGridCircLines, OnBUTTONGridCircLines)
  ON_COMMAND(ID_BUTTON2DGridCircPoints, OnBUTTONGridCircPoints)
  ON_COMMAND(ID_BUTTON2DGridValues, OnBUTTONGridValues)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DGridValues, OnUpdateBUTTONGridValues)
  ON_COMMAND(ID_BUTTON2DGridCancel, OnBUTTONGridCancel)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DGridCancel, OnUpdateBUTTONGridCancel)
  ON_WM_SIZE()
  ON_COMMAND(ID_BUTTON2DFitAll, OnBUTTONFitAll)
  ON_COMMAND(ID_BUTTON2DGlobPanning, OnBUTTONGlobPanning)
  ON_COMMAND(ID_BUTTON2DPanning, OnBUTTONPanning)
  ON_COMMAND(ID_BUTTON2DZoomProg, OnBUTTONZoomProg)
  ON_COMMAND(ID_BUTTON2DZoomWin, OnBUTTONZoomWin)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DGlobPanning, OnUpdateBUTTON2DGlobPanning)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DPanning, OnUpdateBUTTON2DPanning)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DZoomProg, OnUpdateBUTTON2DZoomProg)
  ON_UPDATE_COMMAND_UI(ID_BUTTON2DZoomWin, OnUpdateBUTTON2DZoomWin)
  ON_COMMAND(ID_Modify_ChangeBackground ,OnChangeBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView construction/destruction

OCC_2dView::OCC_2dView()
{
  myToAllowRotation = false;
  myDefaultGestures.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_SelectRectangle);
  myMouseGestureMap = myDefaultGestures;
}

OCC_2dView::~OCC_2dView()
{
  //
}

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView drawing

void OCC_2dView::OnInitialUpdate()
{
  OCC_BaseView::OnInitialUpdate();

  // initialize the grids dialogs
  TheRectangularGridDialog.Create(CRectangularGrid::IDD, NULL);
  TheCircularGridDialog.Create(CCircularGrid::IDD, NULL);
  TheRectangularGridDialog.SetViewer (myView->Viewer());
  TheCircularGridDialog.SetViewer (myView->Viewer());
}

void OCC_2dView::OnFileExportImage()
{
  GetDocument()->ExportView (myView);
}

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView diagnostics

#ifdef _DEBUG
void OCC_2dView::AssertValid() const
{
  CView::AssertValid();
}

void OCC_2dView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

OCC_2dDoc* OCC_2dView::GetDocument() // non-debug version is inline
{
  //ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_2dDoc)));
  return (OCC_2dDoc*)m_pDocument;
}
#endif //_DEBUG
void OCC_2dView::OnBUTTONGridRectLines() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myView->Window()->Size(aWidth,aHeight);
  aViewer->SetRectangularGridGraphicValues(aWidth,aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
  FitAll();

  if (TheCircularGridDialog.IsWindowVisible())
  {
    TheCircularGridDialog.ShowWindow(SW_HIDE);
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridRectPoints() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myView->Window()->Size(aWidth,aHeight);
  aViewer->SetRectangularGridGraphicValues(aWidth,aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Points);
  FitAll();

  if (TheCircularGridDialog.IsWindowVisible())
  {
    TheCircularGridDialog.ShowWindow(SW_HIDE);
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridCircLines() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myView->Window()->Size(aWidth,aHeight);
  aViewer->SetCircularGridGraphicValues(aWidth>aHeight?aWidth:aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Circular, Aspect_GDM_Lines);
  FitAll();
 

  if (TheRectangularGridDialog.IsWindowVisible())
  {
    TheRectangularGridDialog.ShowWindow(SW_HIDE);
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridCircPoints() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myView->Window()->Size(aWidth,aHeight);
  aViewer->SetCircularGridGraphicValues(aWidth>aHeight?aWidth:aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Circular, Aspect_GDM_Points);
  FitAll();
  if (TheRectangularGridDialog.IsWindowVisible())
  {
    TheRectangularGridDialog.ShowWindow(SW_HIDE);
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridValues() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  Aspect_GridType  TheGridtype = aViewer->GridType();

  switch( TheGridtype ) 
  {
  case  Aspect_GT_Rectangular:
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
    break;
  case  Aspect_GT_Circular:
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
    break;
  default :
    throw Standard_Failure("invalid Aspect_GridType");
  }
}
void OCC_2dView::OnUpdateBUTTONGridValues(CCmdUI* pCmdUI) 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  pCmdUI-> Enable( aViewer->IsActive() );
}

void OCC_2dView::OnBUTTONGridCancel() 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  aViewer->DeactivateGrid();
  TheRectangularGridDialog.ShowWindow(SW_HIDE);
  TheCircularGridDialog.ShowWindow(SW_HIDE);
  aViewer->Update();
}
void OCC_2dView::OnUpdateBUTTONGridCancel(CCmdUI* pCmdUI) 
{
  Handle(V3d_Viewer) aViewer = myView->Viewer();
  pCmdUI-> Enable( aViewer->IsActive() );	
}

void OCC_2dView::OnSize(UINT nType, int cx, int cy)
{
  OCC_BaseView::OnSize (nType, cx, cy);
  // Take care : This fonction is call before OnInitialUpdate
  if (!myView.IsNull())
    myView->MustBeResized(); 
}

void OCC_2dView::OnBUTTONFitAll() 
{
  myView->FitAll();
}

void OCC_2dView::OnBUTTONGlobPanning() 
{
  //save the current zoom value
  myCurZoom = myView->Scale();  

  // Do a Global Zoom 
  myView->FitAll();

  // Set the mode
  setCurrentAction (CurAction3d_GlobalPanning);
}
void OCC_2dView::OnBUTTONPanning() 
{
  setCurrentAction (CurAction3d_DynamicPanning);
}
void OCC_2dView::OnBUTTONZoomProg() 
{
  setCurrentAction (CurAction3d_DynamicZooming);
}
void OCC_2dView::OnBUTTONZoomWin() 
{
  setCurrentAction (CurAction3d_WindowZooming);
}
void OCC_2dView::OnChangeBackground() 
{
  Standard_Real R1, G1, B1;
  Handle(Aspect_Window) aWindow = myView->Window();
  Aspect_Background ABack = aWindow->Background();
  Quantity_Color aColor = ABack.Color();
  aColor.Values(R1,G1,B1,Quantity_TOC_RGB);
  COLORREF m_clr ;
  m_clr = RGB(R1*255,G1*255,B1*255);

  CColorDialog dlgColor(m_clr);
  if (dlgColor.DoModal() == IDOK)
  {
    m_clr = dlgColor.GetColor();
    R1 = GetRValue(m_clr)/255.;
    G1 = GetGValue(m_clr)/255.;
    B1 = GetBValue(m_clr)/255.;
    aColor.SetValues(R1,G1,B1,Quantity_TOC_RGB);
    myView->SetBackgroundColor(aColor);
    myView->Update();
  }	
}


void OCC_2dView::OnUpdateBUTTON2DGlobPanning(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_GlobalPanning);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_GlobalPanning);
}

void OCC_2dView::OnUpdateBUTTON2DPanning(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_DynamicPanning);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_DynamicPanning);
}

void OCC_2dView::OnUpdateBUTTON2DZoomProg(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_DynamicZooming);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_DynamicZooming);
}

void OCC_2dView::OnUpdateBUTTON2DZoomWin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_WindowZooming);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_WindowZooming);		
}
