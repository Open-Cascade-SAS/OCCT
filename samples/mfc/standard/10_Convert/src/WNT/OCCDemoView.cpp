// OCCDemoView.cpp : implementation of the COCCDemoView class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "OCCDemo.h"
#include "OCCDemoDoc.h"
#include "OCCDemoView.h"

#include <Graphic3d_GraphicDriver.hxx>

#define ValZWMin 1

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COCCDemoView

IMPLEMENT_DYNCREATE(COCCDemoView, CView)

BEGIN_MESSAGE_MAP(COCCDemoView, CView)
	//{{AFX_MSG_MAP(COCCDemoView)
	ON_COMMAND(ID_BUTTONAxo, OnBUTTONAxo)
	ON_COMMAND(ID_BUTTONBack, OnBUTTONBack)
	ON_COMMAND(ID_BUTTONBottom, OnBUTTONBottom)
	ON_COMMAND(ID_BUTTONFront, OnBUTTONFront)
	ON_COMMAND(ID_BUTTONHlrOn, OnBUTTONHlrOn)
	ON_COMMAND(ID_BUTTONLeft, OnBUTTONLeft)
	ON_COMMAND(ID_BUTTONPan, OnBUTTONPan)
	ON_COMMAND(ID_BUTTONPanGlo, OnBUTTONPanGlo)
	ON_COMMAND(ID_BUTTONReset, OnBUTTONReset)
	ON_COMMAND(ID_BUTTONRight, OnBUTTONRight)
	ON_COMMAND(ID_BUTTONRot, OnBUTTONRot)
	ON_COMMAND(ID_BUTTONTop, OnBUTTONTop)
	ON_COMMAND(ID_BUTTONZoomAll, OnBUTTONZoomAll)
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTONZoomProg, OnBUTTONZoomProg)
	ON_COMMAND(ID_BUTTONZoomWin, OnBUTTONZoomWin)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_BUTTONPanGlo, OnUpdateBUTTONPanGlo)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPan, OnUpdateBUTTONPan)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomProg, OnUpdateBUTTONZoomProg)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomWin, OnUpdateBUTTONZoomWin)
	ON_UPDATE_COMMAND_UI(ID_BUTTONRot, OnUpdateBUTTONRot)
	ON_COMMAND(ID_BUTTONWire, OnBUTTONWire)
	ON_COMMAND(ID_BUTTONShade, OnBUTTONShade)
	ON_UPDATE_COMMAND_UI(ID_BUTTONHlrOn, OnUpdateBUTTONHlrOn)
	ON_UPDATE_COMMAND_UI(ID_BUTTONShade, OnUpdateBUTTONShade)
	ON_UPDATE_COMMAND_UI(ID_BUTTONWire, OnUpdateBUTTONWire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COCCDemoView construction/destruction

COCCDemoView::COCCDemoView()
{
  myXmin=0;
  myYmin=0;  
  myXmax=0;
  myYmax=0;
  myCurZoom=0;
  myCurrentMode = CurAction3d_Nothing;
  myVisMode = VIS_SHADE;
  m_Pen = NULL;
  myGraphicDriver = ((COCCDemoApp*)AfxGetApp())->GetGraphicDriver();
}

COCCDemoView::~COCCDemoView()
{
	if (!myView.IsNull())
    myView->Remove();
  if (m_Pen)
    delete m_Pen;
}

BOOL COCCDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COCCDemoView drawing

void COCCDemoView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();
  
  myView = GetDocument()->GetViewer()->CreateView();
  myView->SetViewMappingDefault();
  myView->SetViewOrientationDefault();

  Handle(WNT_Window) aWNTWindow = new WNT_Window(GetSafeHwnd ());
  myView->SetWindow(aWNTWindow);
  if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

  myCurrentMode = CurAction3d_Nothing;
  myVisMode = VIS_SHADE;
  RedrawVisMode();
}

void COCCDemoView::OnDraw(CDC* /*pDC*/)
{
	COCCDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
  myView->Redraw();
}

/////////////////////////////////////////////////////////////////////////////
// COCCDemoView diagnostics

#ifdef _DEBUG
void COCCDemoView::AssertValid() const
{
	CView::AssertValid();
}

void COCCDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COCCDemoDoc* COCCDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COCCDemoDoc)));
	return (COCCDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COCCDemoView message handlers

void COCCDemoView::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize (nType, cx, cy);
  if (!myView.IsNull())
    myView->MustBeResized();
}

void COCCDemoView::OnBUTTONBack() 
{ 
  myView->SetProj(V3d_Ypos);
}

void COCCDemoView::OnBUTTONFront() 
{ 
	myView->SetProj(V3d_Yneg);
}

void COCCDemoView::OnBUTTONBottom() 
{ 
	myView->SetProj(V3d_Zneg); 
}

void COCCDemoView::OnBUTTONTop() 
{ 
	myView->SetProj(V3d_Zpos); 
}

void COCCDemoView::OnBUTTONLeft() 
{ 
	myView->SetProj(V3d_Xneg);
}

void COCCDemoView::OnBUTTONRight() 
{ 
	myView->SetProj(V3d_Xpos);
}

void COCCDemoView::OnBUTTONAxo() 
{ 
	myView->SetProj(V3d_XposYnegZpos); 
}

void COCCDemoView::OnBUTTONPan() 
{
  myCurrentMode = CurAction3d_DynamicPanning;
}

void COCCDemoView::OnBUTTONPanGlo() 
{
  // save the current zoom value 
  myCurZoom = myView->Scale();
  // Do a Global Zoom 
  myView->FitAll();
  // Set the mode 
  myCurrentMode = CurAction3d_GlobalPanning;
}

void COCCDemoView::OnBUTTONReset() 
{
  myView->Reset();
}

void COCCDemoView::OnBUTTONRot() 
{
  myCurrentMode = CurAction3d_DynamicRotation;
}

void COCCDemoView::OnBUTTONZoomAll() 
{
  myView->FitAll();
  myView->ZFitAll();
}

void COCCDemoView::OnBUTTONZoomProg() 
{
  myCurrentMode = CurAction3d_DynamicZooming;
}

void COCCDemoView::OnBUTTONZoomWin() 
{
  myCurrentMode = CurAction3d_WindowZooming;
}

void COCCDemoView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //  save the current mouse coordinate in min 
  myXmin=point.x;  myYmin=point.y;
  myXmax=point.x;  myYmax=point.y;
  
  if ( ! (nFlags & MK_CONTROL) ) 
  {
    if (myCurrentMode == CurAction3d_DynamicRotation)
    {
      myView->SetComputedMode(Standard_False);
      myView->StartRotation(point.x,point.y);  
    }
  }
}

void COCCDemoView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if ( !(nFlags & MK_CONTROL) )
  {
    switch (myCurrentMode)
    {
    case CurAction3d_Nothing :
      myXmax=point.x;  
      myYmax=point.y;
      break;
    case CurAction3d_DynamicZooming :
      myCurrentMode = CurAction3d_Nothing;
      break;
    case CurAction3d_WindowZooming :
      DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
      myXmax=point.x;
      myYmax=point.y;
      if ((abs(myXmin-myXmax)>ValZWMin) || (abs(myYmin-myYmax)>ValZWMin))
        // Test if the zoom window is greater than a minimale window.
      {
        // Do the zoom window between Pmin and Pmax
        myView->WindowFitAll(myXmin,myYmin,myXmax,myYmax);  
      }  
      myCurrentMode = CurAction3d_Nothing;
      break;
    case CurAction3d_DynamicPanning :
      myCurrentMode = CurAction3d_Nothing;
      break;
    case CurAction3d_GlobalPanning :
      myView->Place(point.x,point.y,myCurZoom); 
      myCurrentMode = CurAction3d_Nothing;
      break;
    case  CurAction3d_DynamicRotation :
      if (myVisMode == VIS_HLR)
      {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
        myView->SetComputedMode(Standard_True);
        myView->Redraw();
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      }
      myCurrentMode = CurAction3d_Nothing;
      break;
    } //switch (myCurrentMode)
  }
}

void COCCDemoView::OnRButtonDown(UINT nFlags, CPoint point) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    myView->SetComputedMode(Standard_False);
    myView->StartRotation(point.x,point.y);  
  }
}

void COCCDemoView::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
  if (myVisMode == VIS_HLR)
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    myView->SetComputedMode(Standard_True);
    myView->Redraw();
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
}

void COCCDemoView::OnMouseMove(UINT nFlags, CPoint point) 
{
  //   ============================  LEFT BUTTON =======================
  if ( nFlags & MK_LBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      // move with MB1 and Control : on the dynamic zooming  
      // Do the zoom in function of mouse's coordinates  
      myView->Zoom(myXmax,myYmax,point.x,point.y); 
      // save the current mouse coordinate in max
      myXmax = point.x; 
      myYmax = point.y;	
    }
    else // if ( Ctrl )
    {
      switch (myCurrentMode)
      {
      case CurAction3d_Nothing :
        myXmax = point.x;
        myYmax = point.y;
        break;
      case CurAction3d_DynamicZooming :
        myView->Zoom(myXmax,myYmax,point.x,point.y); 
        // save the current mouse coordinate in max;
        myXmax=point.x;
        myYmax=point.y;
        break;
      case CurAction3d_WindowZooming :
        myXmax = point.x; myYmax = point.y;	
        DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False,LongDash);
        DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True,LongDash);
        break;
      case CurAction3d_DynamicPanning :
        myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
        myXmax = point.x; myYmax = point.y;	
        break;
      case CurAction3d_GlobalPanning : // nothing           
        break;
      case  CurAction3d_DynamicRotation :
        myView->Rotation(point.x,point.y);
        myView->Redraw();
        break;
      }
    }
  }
  //   ============================  MIDDLE BUTTON =======================
  else if ( nFlags & MK_MBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
      myXmax = point.x; myYmax = point.y;	
    }
  }
  //   ============================  RIGHT BUTTON =======================
  else if ( nFlags & MK_RBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      myView->Rotation(point.x,point.y);
    }
  }
  //   ============================  NO BUTTON =======================
  else
  {
    myXmax = point.x;
    myYmax = point.y;	
  }
}

void COCCDemoView::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction3d_GlobalPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_GlobalPanning);	
}

void COCCDemoView::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicPanning );	
}

void COCCDemoView::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicZooming );
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicZooming);	
}

void COCCDemoView::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction3d_WindowZooming);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_WindowZooming);	
}

void COCCDemoView::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicRotation);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicRotation);	
}

void COCCDemoView::DrawRectangle(const Standard_Integer  MinX,
                                        const Standard_Integer  MinY,
                                        const Standard_Integer  MaxX,
                                        const Standard_Integer  MaxY,
                                        const Standard_Boolean  Draw, 
                                        const LineStyle aLineStyle)
{
  static int m_DrawMode;
  if  (!m_Pen && aLineStyle ==Solid )
  {
    m_Pen = new CPen(PS_SOLID, 1, RGB(0,0,0)); m_DrawMode = R2_MERGEPENNOT;
  }
  else if (!m_Pen && aLineStyle ==Dot )
  {
    m_Pen = new CPen(PS_DOT, 1, RGB(0,0,0));   m_DrawMode = R2_XORPEN;
  }
  else if (!m_Pen && aLineStyle == ShortDash)
  {
    m_Pen = new CPen(PS_DASH, 1, RGB(255,0,0));	m_DrawMode = R2_XORPEN;
  }
  else if (!m_Pen && aLineStyle == LongDash)
  {
    m_Pen = new CPen(PS_DASH, 1, RGB(0,0,0));	m_DrawMode = R2_NOTXORPEN;
  }
  else if (aLineStyle == Default) 
  {
    m_Pen = NULL;	m_DrawMode = R2_MERGEPENNOT;
  }

  CPen* aOldPen = NULL;
  CClientDC clientDC(this);
  if (m_Pen) 
    aOldPen = clientDC.SelectObject(m_Pen);
  clientDC.SetROP2(m_DrawMode);

  static		Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
  static		Standard_Boolean m_IsVisible = Standard_False;

  if ( m_IsVisible && !Draw) // move or up  : erase at the old position 
  {
    clientDC.MoveTo(StoredMinX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMaxY); 
    clientDC.LineTo(StoredMaxX,StoredMaxY); 
    clientDC.LineTo(StoredMaxX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMinY);
    m_IsVisible = false;
  }

  StoredMinX = Min ( MinX, MaxX );
  StoredMinY = Min ( MinY, MaxY );
  StoredMaxX = Max ( MinX, MaxX );
  StoredMaxY = Max ( MinY, MaxY);

  if (Draw) // move : draw
  {
    clientDC.MoveTo(StoredMinX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMaxY); 
    clientDC.LineTo(StoredMaxX,StoredMaxY); 
    clientDC.LineTo(StoredMaxX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMinY);
    m_IsVisible = true;
  }

  if (m_Pen) 
    clientDC.SelectObject(aOldPen);
}

void COCCDemoView::InitButtons()
{
  myXmin=0;
  myYmin=0;  
  myXmax=0;
  myYmax=0;
  myCurZoom=0;
  myCurrentMode = CurAction3d_Nothing;
}

void COCCDemoView::Reset()
{
  InitButtons();
  myVisMode = VIS_SHADE;
  if (!myView.IsNull())
  {
    RedrawVisMode();
    myView->Reset();
  }
}

void COCCDemoView::RedrawVisMode()
{
  switch (myVisMode)
  {
  case VIS_WIREFRAME:
    GetDocument()->GetAISContext()->SetDisplayMode (AIS_WireFrame, Standard_True);
    myView->SetComputedMode (Standard_False);
    myView->Redraw();
    break;
  case VIS_SHADE:
    GetDocument()->GetAISContext()->SetDisplayMode (AIS_Shaded, Standard_True);
    myView->SetComputedMode (Standard_False);
    myView->Redraw();
    break;
  case VIS_HLR:
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    myView->SetComputedMode (Standard_True);
    myView->Redraw();
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    GetDocument()->GetAISContext()->SetDisplayMode (AIS_WireFrame, Standard_True);
    break;
  }
}

void COCCDemoView::OnBUTTONWire() 
{
  myVisMode = VIS_WIREFRAME;
  RedrawVisMode();
}

void COCCDemoView::OnBUTTONShade() 
{
  myVisMode = VIS_SHADE;
  RedrawVisMode();
}

void COCCDemoView::OnBUTTONHlrOn() 
{
  myVisMode = VIS_HLR;
  RedrawVisMode();
}

void COCCDemoView::OnUpdateBUTTONWire(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myVisMode == VIS_WIREFRAME);
	pCmdUI->Enable   (myVisMode != VIS_WIREFRAME);	
}

void COCCDemoView::OnUpdateBUTTONShade(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myVisMode == VIS_SHADE);
	pCmdUI->Enable   (myVisMode != VIS_SHADE);	
}

void COCCDemoView::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myVisMode == VIS_HLR);
	pCmdUI->Enable   (myVisMode != VIS_HLR);	
}

void COCCDemoView::GetViewAt (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
{
  myView->At (theX, theY, theZ);
}

void COCCDemoView::SetViewAt (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
{
  myView->SetAt (theX, theY, theZ);
}

void COCCDemoView::GetViewEye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z)
{
	myView->Eye(X,Y,Z);
}

void COCCDemoView::SetViewEye(Standard_Real X, Standard_Real Y, Standard_Real Z)
{
	myView->SetEye(X,Y,Z);
}

Standard_Real COCCDemoView::GetViewScale()
{
	return myView->Scale();
}

void COCCDemoView::SetViewScale(Standard_Real Coef)
{
	myView->SetScale(Coef);
}

void COCCDemoView::Translate (const Standard_Real theX, const Standard_Real theY)
{
  myView->Panning (theX, theY);
}
