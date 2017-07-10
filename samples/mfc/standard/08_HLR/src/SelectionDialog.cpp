// SelectionDialog.cpp : implementation file
//

#include "stdafx.h"

#include "SelectionDialog.h"

#include "HLRApp.h"
#include "OCC_2dView.h"
#include <ISession2D/ISession2D_Shape.h>
#include "Prs3d_Projector.hxx"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY MK_CONTROL

/////////////////////////////////////////////////////////////////////////////
// CSelectionDialog dialog

CSelectionDialog::CSelectionDialog (CHLRDoc* aDoc,CWnd* pParent /*=NULL*/)
: CDialog(CSelectionDialog::IDD, pParent)
{
  myDoc =  aDoc;
  myIsDisplayed = false;
  //{{AFX_DATA_INIT(CSelectionDialog)
  m_Algo        = 0;
  m_DisplayMode = 0;
  m_NbIsos      = 2;
  m_DrawHiddenLine = TRUE;
  m_HlrModeIsOn = TRUE;
  //}}AFX_DATA_INIT
}

void CSelectionDialog::DoDataExchange (CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectionDialog)
  DDX_Radio(pDX, IDC_PolyAlgo, m_Algo);
  DDX_Radio(pDX, IDC_DisplayDefault, m_DisplayMode);
  DDX_Text(pDX, IDC_EDIT_NBIsos, m_NbIsos);
  DDX_Check(pDX, IDC_DrawHiddenLine, m_DrawHiddenLine);
  DDX_Check(pDX, IDC_HlrModeIsOn, m_HlrModeIsOn);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectionDialog, CDialog)
  //{{AFX_MSG_MAP(CSelectionDialog)
  ON_BN_CLICKED(ID_GetShape, OnGetSelectedShapes)
  ON_BN_CLICKED(IDC_DisplayDefault, OnDisplayDefault)
  ON_BN_CLICKED(IDC_VIsoParametrics, OnVIsoParametrics)
  ON_BN_CLICKED(IDC_VApparentContour, OnVApparentContour)
  ON_BN_CLICKED(IDC_VSewingEdges, OnVSewingEdges)
  ON_BN_CLICKED(IDC_VsharpEdges, OnVsharpEdges)
  ON_BN_CLICKED(IDC_VsmoothEdges, OnVsmoothEdges)
  ON_BN_CLICKED(IDC_HsharpEdges, OnHsharpEdges)
  ON_BN_CLICKED(IDC_HsmoothEdges, OnHsmoothEdges)
  ON_BN_CLICKED(IDC_HSewingEdges, OnHSewingEdges)
  ON_BN_CLICKED(IDC_HIsoParametrics, OnHIsoParametrics)
  ON_BN_CLICKED(IDC_HApparentContour, OnHApparentContour)
  ON_EN_CHANGE(IDC_EDIT_NBIsos, OnChangeEDITNBIsos)
  ON_BN_CLICKED(IDC_Algo, OnAlgo)
  ON_BN_CLICKED(IDC_PolyAlgo, OnPolyAlgo)
  ON_BN_CLICKED(ID_Update2D, OnUpdate2D)
  ON_BN_CLICKED(IDC_TopView, OnTopView)
  ON_BN_CLICKED(IDC_BottomView, OnBottomView)
  ON_BN_CLICKED(IDC_LeftView, OnLeftView)
  ON_BN_CLICKED(IDC_RightView, OnRightView)
  ON_BN_CLICKED(IDC_FrontView, OnFrontView)
  ON_BN_CLICKED(IDC_BackView, OnBackView)
  ON_BN_CLICKED(IDC_AxoView, OnAxoView)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDC_DrawHiddenLine, OnDrawHiddenLine)
  ON_BN_CLICKED(IDC_HlrModeIsOn, OnHlrMode)
  ON_WM_DRAWITEM()
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectionDialog message handlers

BOOL CSelectionDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  VERIFY(TopView.AutoLoad (IDC_TopView, this));
  VERIFY(BottomView.AutoLoad (IDC_BottomView, this)) ;
  VERIFY(LeftView  .AutoLoad (IDC_LeftView  , this)) ;
  VERIFY(RightView .AutoLoad (IDC_RightView , this)) ;
  VERIFY(FrontView .AutoLoad (IDC_FrontView , this)) ;
  VERIFY(BackView  .AutoLoad (IDC_BackView  , this)) ;
  VERIFY(AxoView   .AutoLoad (IDC_AxoView   , this)) ;

  // get the View Window position to managed mouse move
  CRect BoxRect,ViewRect;
  GetWindowRect (BoxRect);
  CWnd * TheViewerWindow = GetDlgItem (IDC_HlrDlgView);
  TheViewerWindow->GetWindowRect (ViewRect);
  myPosMinX = ViewRect.TopLeft().x - BoxRect.TopLeft().x;
  myPosMaxX = ViewRect.Width() + myPosMinX;
  myPosMinY = ViewRect.TopLeft().y - BoxRect.TopLeft().y;
  myPosMaxY = myPosMinY + ViewRect.Height();

  ShowHideButton (Standard_False);
  OnDisplay (true);

  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  return TRUE;
}

void CSelectionDialog::OnDisplay (bool isFit)
{
  // GetDlgItem(IDC_HlrDlgView)->SetRedraw() wrapping of this method makes view blinking,
  // but displaying of big models may be faster.
  //GetDlgItem(IDC_HlrDlgView)->SetRedraw(true);
  if (!myIsDisplayed)
  {
    Handle(Graphic3d_GraphicDriver) aGraphicDriver =
      ((CHLRApp*)AfxGetApp())->GetGraphicDriver();

    myActiveViewer = new V3d_Viewer (aGraphicDriver);
    myActiveViewer->SetDefaultLights();
    myActiveViewer->SetLightOn();
    myActiveView = myActiveViewer->CreateView();

    Handle(WNT_Window) aWNTWindow = new WNT_Window (GetDlgItem (IDC_HlrDlgView)->GetSafeHwnd(),
                                                    Quantity_NOC_GRAY);
    myActiveView->SetComputedMode (m_HlrModeIsOn != 0);
    myActiveView->SetWindow(aWNTWindow);

    myInteractiveContext = new AIS_InteractiveContext (myActiveViewer);

    // TRIHEDRON
    Handle(Geom_Axis2Placement) aTrihedronAxis = new Geom_Axis2Placement (gp::XOY());
    myTrihedron = new AIS_Trihedron (aTrihedronAxis);

    myInteractiveContext->Display (myTrihedron, Standard_False);
    myIsDisplayed = Standard_True;
  }
  if(isFit)
  {
    myActiveView->ZFitAll();
    myActiveView->FitAll();
  }

  myActiveView->Redraw();
  //GetDlgItem (IDC_HlrDlgView)->SetRedraw (false);
}


void CSelectionDialog::SetTitle (const CString & aTitle)
{
  SetWindowText (aTitle);
}

void CSelectionDialog::UpdateViews()
{
  // Clear HLR dialog view
  myInteractiveContext->RemoveAll (Standard_False);
  myInteractiveContext->Display (myTrihedron, Standard_False);

  UpdateProjector();

  // Display chosen shapes in the HLR dialog view.
  Standard_Boolean OneOrMoreFound = Standard_False;
  for (myDoc->GetAISContext()->InitSelected();
       myDoc->GetAISContext()->MoreSelected();
       myDoc->GetAISContext()->NextSelected())
  {
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (myDoc->GetAISContext()->SelectedInteractive());
    if (!anAISShape.IsNull())
    {
      OneOrMoreFound = Standard_True;
      myInteractiveContext->Display (anAISShape, Standard_False);
    }
  }
  myInteractiveContext->UpdateCurrentViewer();
  // Apply HLR to chosen shapes and display result into the 2d view.
  Apply();
  // Update viewer
  myDoc->FitAll2DViews (Standard_False);
  // Check the selection: if no object : disable all possiblity.
  ShowHideButton (OneOrMoreFound);
  OnDisplay (true);
}

void CSelectionDialog::OnGetSelectedShapes()
{
  // Create new displayable shape.
  myDisplayableShape = new ISession2D_Shape();
  UpdateProjector();
  myDisplayableShape->SetNbIsos (m_NbIsos);

  // Clear HLR dialog view
  myInteractiveContext->RemoveAll (Standard_False);
  myInteractiveContext->Display (myTrihedron, Standard_False);

  Standard_Boolean OneOrMoreFound = Standard_False;
  for (myDoc->GetAISContext()->InitSelected();
       myDoc->GetAISContext()->MoreSelected();
       myDoc->GetAISContext()->NextSelected())
  {
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (myDoc->GetAISContext()->SelectedInteractive());

    if (!anAISShape.IsNull())
    {
      OneOrMoreFound = Standard_True;
      TopoDS_Shape aShape = anAISShape->Shape();
      myDisplayableShape->Add (aShape);
      Handle(AIS_Shape) aSelectedShape = new AIS_Shape (aShape);
      myInteractiveContext->Display (aSelectedShape, Standard_False);
    }
  }
  myInteractiveContext->UpdateCurrentViewer();
  // Apply HLR to chosen shapes and display result into the 2d view.
  Apply();
  // Update viewer
  myDoc->FitAll2DViews (Standard_False);

  // Check the selection: if no object : disable all possiblity.
  ShowHideButton (OneOrMoreFound);
  OnDisplay (true);
}

void CSelectionDialog::Apply()
{
  SetCursor(AfxGetApp()->LoadStandardCursor (IDC_WAIT));
  myDoc->GetInteractiveContext2D()->RemoveAll (Standard_False);
  UpdateData (true);

  Standard_Integer aDisplayMode = m_DisplayMode;

  if (m_Algo == 1)
  {
    aDisplayMode += 100;
  }

  if (!m_DrawHiddenLine)
  {
    aDisplayMode += 1000;
  }

  myDoc->GetInteractiveContext2D()->Display (myDisplayableShape,  // object
                                             aDisplayMode,
                                             aDisplayMode,
                                             Standard_True); // Redraw

  SetCursor(AfxGetApp()->LoadStandardCursor (IDC_ARROW));
}

void CSelectionDialog::UpdateProjector()
{
  Standard_Real DX,DY,DZ,XAt,YAt,ZAt, Vx,Vy,Vz ;
  myActiveView->Proj(DX,DY,DZ);
  myActiveView->At(XAt,YAt,ZAt);
  myActiveView->Up( Vx,Vy,Vz );
  OnDisplay(false);
  Standard_Boolean IsPerspective = (myActiveView->Type() == V3d_PERSPECTIVE);
  Standard_Real aFocus = 1;
  Prs3d_Projector aPrs3dProjector(IsPerspective,aFocus,DX,DY,DZ,XAt,YAt,ZAt,Vx,Vy,Vz);
  HLRAlgo_Projector aProjector = aPrs3dProjector.Projector();

  if (myDisplayableShape.IsNull()) return;
  myDisplayableShape->SetProjector(aProjector);
}

void CSelectionDialog::ShowHideButton(Standard_Boolean EnableButton)
{
  UpdateData(true);

  GetDlgItem(ID_Update2D          )->EnableWindow(EnableButton);
  GetDlgItem(IDC_DisplayDefault   )->EnableWindow(EnableButton);
  GetDlgItem(IDC_PolyAlgo         )->EnableWindow(EnableButton);
  GetDlgItem(IDC_Algo             )->EnableWindow(EnableButton);

  GetDlgItem(IDC_VsharpEdges      )->EnableWindow(EnableButton);
  GetDlgItem(IDC_VsmoothEdges     )->EnableWindow(EnableButton);
  GetDlgItem(IDC_VSewingEdges     )->EnableWindow(EnableButton);
  GetDlgItem(IDC_VApparentContour )->EnableWindow(EnableButton);
  GetDlgItem(IDC_VIsoParametrics  )->EnableWindow(EnableButton);

  GetDlgItem(IDC_DrawHiddenLine   )->EnableWindow(EnableButton);

  GetDlgItem(IDC_HsmoothEdges     )->EnableWindow(EnableButton && m_DrawHiddenLine);
  GetDlgItem(IDC_HSewingEdges     )->EnableWindow(EnableButton && m_DrawHiddenLine);
  GetDlgItem(IDC_HApparentContour )->EnableWindow(EnableButton && m_DrawHiddenLine);
  GetDlgItem(IDC_HsharpEdges      )->EnableWindow(EnableButton && m_DrawHiddenLine);
  GetDlgItem(IDC_HIsoParametrics  )->EnableWindow(EnableButton && m_DrawHiddenLine);

  GetDlgItem(IDC_EDIT_NBIsos      )->EnableWindow(EnableButton);
  GetDlgItem(IDC_STATIC_NbIsos    )->EnableWindow(EnableButton);

  if(m_Algo == 0)
  {
    if (m_DisplayMode == 5 || m_DisplayMode == 10)
    {
      m_DisplayMode=0;
    }

    GetDlgItem(IDC_VIsoParametrics)->EnableWindow(false);
    GetDlgItem(IDC_HIsoParametrics)->EnableWindow(false);
    GetDlgItem(IDC_STATIC_NbIsos)  ->EnableWindow(false);
    GetDlgItem(IDC_EDIT_NBIsos)    ->EnableWindow(false);
  }
  else
  {
    GetDlgItem(IDC_VIsoParametrics)->EnableWindow(true);
    GetDlgItem(IDC_HIsoParametrics)->EnableWindow(m_DrawHiddenLine);
    GetDlgItem(IDC_STATIC_NbIsos)  ->EnableWindow(true);
    GetDlgItem(IDC_EDIT_NBIsos)    ->EnableWindow(true);
  }
  UpdateData(false);
} 

void CSelectionDialog::OnDisplayDefault() 
{ Apply(); }
void CSelectionDialog::OnVIsoParametrics() 
{ Apply(); }
void CSelectionDialog::OnVApparentContour() 
{ Apply(); }
void CSelectionDialog::OnVSewingEdges() 
{ Apply(); }
void CSelectionDialog::OnVsharpEdges() 
{ Apply(); }
void CSelectionDialog::OnVsmoothEdges() 
{ Apply(); }
void CSelectionDialog::OnHsharpEdges() 
{ Apply(); }
void CSelectionDialog::OnHsmoothEdges() 
{ Apply(); }
void CSelectionDialog::OnHSewingEdges() 
{ Apply(); }
void CSelectionDialog::OnHIsoParametrics() 
{ Apply(); }
void CSelectionDialog::OnHApparentContour() 
{ Apply(); }

void CSelectionDialog::OnChangeEDITNBIsos() 
{
  UpdateData(true);
  myDisplayableShape->SetNbIsos(m_NbIsos);
  Apply(); 
}
void CSelectionDialog::OnAlgo() 
{
  ShowHideButton();
  Apply();
}

void CSelectionDialog::OnPolyAlgo() 
{
  ShowHideButton();
  Apply();
}

void CSelectionDialog::OnDrawHiddenLine() 
{
  UpdateData(true);
  if(m_DisplayMode >=6 )
  {
    m_DisplayMode=0;
    UpdateData(false);
  }
  ShowHideButton();
  Apply();
}

void CSelectionDialog::OnUpdate2D() 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  myDoc->ActivateFrame(RUNTIME_CLASS(OCC_2dView),SW_NORMAL);
  UpdateProjector();
  Apply();
  myDoc->FitAll2DViews(Standard_False);
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CSelectionDialog::OnTopView() 
{
  myActiveView->SetProj(V3d_Zpos);
  OnDisplay(true);
}

void CSelectionDialog::OnBottomView() 
{
  myActiveView->SetProj(V3d_Zneg);
  OnDisplay(true);
}

void CSelectionDialog::OnLeftView() 
{
  myActiveView->SetProj(V3d_Xneg);
  OnDisplay(true);
}

void CSelectionDialog::OnRightView() 
{
  myActiveView->SetProj(V3d_Xpos);
  OnDisplay(true);
}

void CSelectionDialog::OnFrontView() 
{
  myActiveView->SetProj(V3d_Yneg);
  OnDisplay(true);
}

void CSelectionDialog::OnBackView() 
{
  myActiveView->SetProj(V3d_Ypos);
  OnDisplay(true);
}

void CSelectionDialog::OnAxoView() 
{
  myActiveView->SetProj(V3d_XposYnegZpos);
  OnDisplay(true);
}

void CSelectionDialog::OnHlrMode()
{
  UpdateData(true);

  if (!m_HlrModeIsOn)
  {
    myActiveView->SetComputedMode (m_HlrModeIsOn != 0);
    myActiveView->Redraw();
  }
  else
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    myActiveView->SetComputedMode (m_HlrModeIsOn != 0);
    myActiveView->Redraw();
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
  OnDisplay(false);
}

void CSelectionDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CDialog::OnLButtonDown(nFlags, point);

  if ((myPosMinX > point.x)||(myPosMaxX < point.x) ||
    (myPosMinY > point.y) ||(myPosMaxY < point.y))
    return;

  //  save the current mouse coordinate
  myXmax=point.x;  myYmax=point.y;
}

void CSelectionDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
  CDialog::OnLButtonUp(nFlags, point);

  if ((myPosMinX > point.x)||(myPosMaxX < point.x) ||
    (myPosMinY > point.y) ||(myPosMaxY < point.y))
    return;
}

void CSelectionDialog::OnRButtonDown(UINT nFlags, CPoint point) 
{
  CDialog::OnRButtonDown(nFlags, point);

  if ((myPosMinX > point.x) || (myPosMaxX < point.x) ||
      (myPosMinY > point.y) || (myPosMaxY < point.y))
  {
    return;
  }

  if ( nFlags & CASCADESHORTCUTKEY )
  {
    if (m_HlrModeIsOn)
    {
      myActiveView->SetComputedMode (Standard_False);
    }
    myActiveView->StartRotation (point.x, point.y);
    OnDisplay (false);
  }
}

void CSelectionDialog::OnRButtonUp(UINT nFlags, CPoint point) 
{
  CDialog::OnRButtonUp(nFlags, point);

  if ((myPosMinX > point.x) || (myPosMaxX < point.x) ||
      (myPosMinY > point.y) || (myPosMaxY < point.y))
  {
    return;
  }

  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  // reset the good HLR mode according to the stored one
  //   --> dynamic rotation may have change it
  if (m_HlrModeIsOn != 0)
  {
    myActiveView->SetComputedMode (true);
    myActiveView->Redraw();
  }
  OnDisplay(false);
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CSelectionDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
  CDialog::OnMouseMove(nFlags, point);

  if ((myPosMinX > point.x)||(myPosMaxX < point.x) ||
    (myPosMinY > point.y) ||(myPosMaxY < point.y))
    return;

  if ( nFlags & MK_LBUTTON)
  {//left button
    if ( nFlags & CASCADESHORTCUTKEY ) 
    {
      // move with MB1 and Control : on the dynamic zooming  
      // Do the zoom in function of mouse's coordinates  
      myActiveView->Zoom(myXmax,myYmax,point.x,point.y); 
      // save the current mouse coordinate 
      myXmax = point.x;    myYmax = point.y;	
    }// if ( nFlags & CASCADESHORTCUTKEY )  else 
  }
  else if ( nFlags & MK_MBUTTON)
  {
    if ( nFlags & CASCADESHORTCUTKEY ) 
    {
      myActiveView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
      myXmax = point.x; myYmax = point.y;	
    }
  }
  else if ( nFlags & MK_RBUTTON)
  {//right button
    if ( nFlags & CASCADESHORTCUTKEY ) 
    {
      myActiveView->Rotation(point.x,point.y);
    }
  } 
  else
  {// No buttons 
    myXmax = point.x; myYmax = point.y;	
  }
  OnDisplay(false);
}

void CSelectionDialog::OnOK() 
{
  CDialog::OnOK();
}

void CSelectionDialog::OnPaint() 
{
  OnDisplay(false);
}
