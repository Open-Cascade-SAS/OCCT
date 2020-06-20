// AnimationView3D.cpp : implementation of the CAnimationView3D class
//

#include "stdafx.h"

#include "AnimationView3D.h"

#include "AnimationApp.h"
#include "ShadingDialog.h"
#include "AnimationDoc.h"

#include <AIS_RubberBand.hxx>

#ifdef _DEBUG
//#define new DEBUG_NEW by CasCade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static int rotCount = 0;

// for elastic bean selection
#define ValZWMin 1


/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D

IMPLEMENT_DYNCREATE(CAnimationView3D, CView)

BEGIN_MESSAGE_MAP(CAnimationView3D, CView)
	//{{AFX_MSG_MAP(CAnimationView3D)
	ON_COMMAND(ID_BUTTONAxo, OnBUTTONAxo)
	ON_COMMAND(ID_BUTTONBack, OnBUTTONBack)
	ON_COMMAND(ID_BUTTONBottom, OnBUTTONBottom)
	ON_COMMAND(ID_BUTTONFront, OnBUTTONFront)
	ON_COMMAND(ID_BUTTONHlrOff, OnBUTTONHlrOff)
	ON_COMMAND(ID_BUTTONHlrOn, OnBUTTONHlrOn)
	ON_COMMAND(ID_BUTTONLeft, OnBUTTONLeft)
	ON_COMMAND(ID_BUTTONPan, OnBUTTONPan)
	ON_COMMAND(ID_BUTTONPanGlo, OnBUTTONPanGlo)
	ON_COMMAND(ID_BUTTONReset, OnBUTTONReset)
	ON_COMMAND(ID_BUTTONRight, OnBUTTONRight)
	ON_COMMAND(ID_BUTTONRot, OnBUTTONRot)
	ON_COMMAND(ID_BUTTONTop, OnBUTTONTop)
	ON_COMMAND(ID_BUTTONZoomAll, OnBUTTONZoomAll)
    ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTONZoomProg, OnBUTTONZoomProg)
	ON_COMMAND(ID_BUTTONZoomWin, OnBUTTONZoomWin)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_BUTTONHlrOff, OnUpdateBUTTONHlrOff)
	ON_UPDATE_COMMAND_UI(ID_BUTTONHlrOn, OnUpdateBUTTONHlrOn)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPanGlo, OnUpdateBUTTONPanGlo)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPan, OnUpdateBUTTONPan)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomProg, OnUpdateBUTTONZoomProg)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomWin, OnUpdateBUTTONZoomWin)
	ON_UPDATE_COMMAND_UI(ID_BUTTONRot, OnUpdateBUTTONRot)

	ON_COMMAND(ID_Modify_ChangeBackground     , OnChangeBackground)
	ON_WM_TIMER()
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_RESTART, OnRestart)

	ON_COMMAND(ID_BUTTONFly, OnBUTTONFly)
	ON_COMMAND(ID_BUTTONTurn, OnBUTTONTurn)
	ON_UPDATE_COMMAND_UI(ID_BUTTONFly, OnUpdateBUTTONFly)
	ON_UPDATE_COMMAND_UI(ID_BUTTONTurn, OnUpdateBUTTONTurn)
	//}}AFX_MSG_MAP
// CasCade

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D construction/destruction

CAnimationView3D::CAnimationView3D()
: myCurZoom (0.0),
  myHlrModeIsOn (Standard_False),
  myIsTurnStarted (Standard_False),
  myUpdateRequests (0),
  myCurrentMode  (CurrentAction3d_Nothing)
{
  myDefaultGestures = myMouseGestureMap;
}

CAnimationView3D::~CAnimationView3D()
{
    myView->Remove();
}

BOOL CAnimationView3D::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D drawing


// ================================================================
// Function : update3dView
// Purpose  :
// ================================================================
void CAnimationView3D::update3dView()
{
  if (myView.IsNull())
  {
    return;
  }

  if (++myUpdateRequests == 1)
  {
    Invalidate (FALSE);
    UpdateWindow();
  }
}

// ================================================================
// Function : redraw3dView
// Purpose  :
// ================================================================
void CAnimationView3D::redraw3dView()
{
  if (!myView.IsNull())
  {
    FlushViewEvents (GetDocument()->GetAISContext(), myView, true);
  }
}

// ================================================================
// Function : handleViewRedraw
// Purpose  :
// ================================================================
void CAnimationView3D::handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                         const Handle(V3d_View)& theView)
{
  myUpdateRequests = 0;
  if (myAnimTimer.IsStarted())
  {
    GetDocument()->OnMyTimer (myAnimTimer.ElapsedTime());
    setAskNextFrame();
  }
  AIS_ViewController::handleViewRedraw (theCtx, theView);
}

void CAnimationView3D::OnDraw(CDC* /*pDC*/)
{
  // always redraw immediate layer (dynamic highlighting) on Paint event,
  // and redraw entire view content only when it is explicitly invalidated (V3d_View::Invalidate())
  myView->InvalidateImmediate();
  FlushViewEvents (GetDocument()->GetInteractiveContext(), myView, true);
}

void CAnimationView3D::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  // TODO: Add your specialized code here and/or call the base class
  //	myView = GetDocument()->GetViewer()->CreateView();
  Handle(V3d_Viewer) aViewer = GetDocument()->GetViewer();
  aViewer->SetDefaultTypeOfView (V3d_PERSPECTIVE);

  myView = aViewer->CreateView();
  myView->SetImmediateUpdate (false);

  // store for restore state after rotation (witch is in Degenerated mode)
  myHlrModeIsOn = myView->ComputedMode();

  Handle(WNT_Window) aWNTWindow = new WNT_Window (GetSafeHwnd());
  myView->SetWindow(aWNTWindow);
  if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

  // store the mode ( nothing , dynamic zooming, dynamic ... )
  myCurrentMode = CurrentAction3d_Nothing;

  CFrameWnd* pParentFrm = GetParentFrame();
  pParentFrm->ActivateFrame(SW_SHOWMAXIMIZED);
}

// =======================================================================
// function : defineMouseGestures
// purpose  :
// =======================================================================
void CAnimationView3D::defineMouseGestures()
{
  myMouseGestureMap.Clear();
  switch (myCurrentMode)
  {
    case CurrentAction3d_Nothing:
    {
      myMouseGestureMap = myDefaultGestures;
      break;
    }
    case CurrentAction3d_DynamicZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
      break;
    }
    case CurrentAction3d_GlobalPanning:
    {
      break;
    }
    case CurrentAction3d_WindowZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
      break;
    }
    case CurrentAction3d_DynamicPanning:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
      break;
    }
    case CurrentAction3d_DynamicRotation:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_RotateOrbit);
      break;
    }
    case CurrentAction3d_Fly:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_RotateView);
      break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D diagnostics

#ifdef _DEBUG
void CAnimationView3D::AssertValid() const
{
	CView::AssertValid();
}

void CAnimationView3D::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAnimationDoc* CAnimationView3D::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnimationDoc)));
	return (CAnimationDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D message handlers
void CAnimationView3D::OnFileExportImage()
{
  GetDocument()->ExportView (myView);
}

void CAnimationView3D::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize (nType, cx, cy);
  if (cx != 0
   && cy != 0
   && !myView.IsNull())
  {
    myView->Window()->DoResize();
    myView->MustBeResized();
    myView->Invalidate();
    update3dView();
  }
}

void CAnimationView3D::OnBUTTONBack() 
{ myView->SetProj(V3d_Ypos); } // See the back View
void CAnimationView3D::OnBUTTONFront() 
{ myView->SetProj(V3d_Yneg); } // See the front View

void CAnimationView3D::OnBUTTONBottom() 
{ myView->SetProj(V3d_Zneg); } // See the bottom View
void CAnimationView3D::OnBUTTONTop() 
{ myView->SetProj(V3d_Zpos); } // See the top View	

void CAnimationView3D::OnBUTTONLeft() 
{ myView->SetProj(V3d_Xneg); } // See the left View	
void CAnimationView3D::OnBUTTONRight() 
{ myView->SetProj(V3d_Xpos); } // See the right View

void CAnimationView3D::OnBUTTONAxo() 
{ myView->SetProj(V3d_XposYnegZpos); } // See the axonometric View

void CAnimationView3D::OnBUTTONHlrOff() 
{
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
}

void CAnimationView3D::OnBUTTONHlrOn() 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  myHlrModeIsOn = Standard_True;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CAnimationView3D::OnBUTTONPan() 
{  
  setCurrentAction (CurrentAction3d_DynamicPanning); 
}

void CAnimationView3D::OnBUTTONPanGlo() 
{
  // save the current zoom value 
  myCurZoom = myView->Scale();
  // Do a Global Zoom 
  myView->FitAll();
  // Set the mode 
  setCurrentAction (CurrentAction3d_GlobalPanning);
}

void CAnimationView3D::OnBUTTONReset() 
{
  myView->Reset(); 
}

void CAnimationView3D::OnBUTTONRot() 
{   setCurrentAction (CurrentAction3d_DynamicRotation); }


void CAnimationView3D::OnBUTTONZoomAll() 
{
  FitAll();
}

void CAnimationView3D::OnBUTTONZoomProg() 
{  setCurrentAction (CurrentAction3d_DynamicZooming); }

void CAnimationView3D::OnBUTTONZoomWin() 
{  setCurrentAction (CurrentAction3d_WindowZooming); }

void CAnimationView3D::OnBUTTONFly() 
{  setCurrentAction (CurrentAction3d_Fly); }

void CAnimationView3D::OnBUTTONTurn() 
{  setCurrentAction (CurrentAction3d_Turn); }


void CAnimationView3D::OnLButtonDown(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_LeftButton, aFlags, false);
  myClickPos.SetValues (thePoint.x, thePoint.y);
  myIsTurnStarted = myCurrentMode == CurrentAction3d_Turn && aFlags == Aspect_VKeyFlags_NONE;
  update3dView();
}

void CAnimationView3D::OnLButtonUp(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_LeftButton, aFlags, false);
  if (myCurrentMode == CurrentAction3d_GlobalPanning)
  {
    myView->Place (thePoint.x, thePoint.y, myCurZoom);
    myView->Invalidate();
  }
  if (myCurrentMode != CurrentAction3d_Nothing)
  {
    setCurrentAction (CurrentAction3d_Nothing);
    myIsTurnStarted = false;
  }
  update3dView();
}

void CAnimationView3D::OnMButtonDown(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_MiddleButton, aFlags, false);
  update3dView();
}

void CAnimationView3D::OnMButtonUp(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_MiddleButton, aFlags, false);
  update3dView();
  if (myCurrentMode != CurrentAction3d_Nothing)
  {
    setCurrentAction (CurrentAction3d_Nothing);
  }
}

void CAnimationView3D::OnRButtonDown(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_RightButton, aFlags, false);
  update3dView();
  myClickPos.SetValues (thePoint.x, thePoint.y);
}

void CAnimationView3D::OnRButtonUp(UINT theFlags, CPoint thePoint) 
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_RightButton, aFlags, false);
  update3dView();
  if (myCurrentMode != CurrentAction3d_Nothing)
  {
    setCurrentAction (CurrentAction3d_Nothing);
  }
  if (aFlags == Aspect_VKeyFlags_NONE
   && (myClickPos - Graphic3d_Vec2i (thePoint.x, thePoint.y)).cwiseAbs().maxComp() <= 4)
  {
    GetDocument()->Popup (thePoint.x, thePoint.y, myView);
  }
}

// =======================================================================
// function : OnMouseWheel
// purpose  :
// =======================================================================
BOOL CAnimationView3D::OnMouseWheel (UINT theFlags, short theDelta, CPoint thePoint)
{
  const Standard_Real aDeltaF = Standard_Real(theDelta) / Standard_Real(WHEEL_DELTA);
  CPoint aCursorPnt = thePoint;
  ScreenToClient (&aCursorPnt);
  const Graphic3d_Vec2i  aPos (aCursorPnt.x, aCursorPnt.y);
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  if (UpdateMouseScroll (Aspect_ScrollDelta (aPos, aDeltaF, aFlags)))
  {
    update3dView();
  }
  return true;
}

void CAnimationView3D::OnMouseMove(UINT theFlags, CPoint thePoint) 
{
  TRACKMOUSEEVENT aMouseEvent;          // for WM_MOUSELEAVE
  aMouseEvent.cbSize = sizeof(aMouseEvent);
  aMouseEvent.dwFlags = TME_LEAVE;
  aMouseEvent.hwndTrack = m_hWnd;
  aMouseEvent.dwHoverTime = HOVER_DEFAULT;
  if (!::_TrackMouseEvent (&aMouseEvent)) { TRACE("Track ERROR!\n"); }

  const Graphic3d_Vec2i aNewPnt (thePoint.x, thePoint.y);
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  if (UpdateMousePosition (aNewPnt, PressedMouseButtons(), aFlags, false))
  {
    update3dView();
  }

  if (myIsTurnStarted)
  {
    Graphic3d_Vec2i aWinSize;
    myView->Window()->Size (aWinSize.x(), aWinSize.y());
    const Graphic3d_Vec2i aCenter = aWinSize / 2;
    if (myClickPos != aCenter
     && aNewPnt != aCenter
     && aNewPnt != myClickPos)
    {
      const Graphic3d_Vec2i aVecFrom = myClickPos - aCenter;
      const Graphic3d_Vec2i aVecTo   = aNewPnt - aCenter;
      const gp_Dir aDirFrom (aVecFrom.x() / double(aWinSize.x() / 2), aVecFrom.y() / double(aWinSize.y() / 2), 0.0);
      const gp_Dir aDirTo   (aVecTo.x()   / double(aWinSize.x() / 2),   aVecTo.y() / double(aWinSize.y() / 2), 0.0);
      double anAngle = aDirFrom.AngleWithRef (aDirTo, gp::DZ());

      myView->SetTwist (myView->Twist() + anAngle);
      myView->Invalidate();
      update3dView();
      myClickPos = aNewPnt;
    }
  }
}

void CAnimationView3D::OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (!myHlrModeIsOn);
  pCmdUI->Enable   (myHlrModeIsOn);
}

void CAnimationView3D::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myHlrModeIsOn);
  pCmdUI->Enable   (!myHlrModeIsOn);
}

void CAnimationView3D::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_GlobalPanning);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_GlobalPanning);	
	
}

void CAnimationView3D::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicPanning);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicPanning );	
}

void CAnimationView3D::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicZooming );
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicZooming);	
}

void CAnimationView3D::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_WindowZooming);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_WindowZooming);	
}

void CAnimationView3D::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicRotation);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicRotation);	
}

void CAnimationView3D::OnUpdateBUTTONFly(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_Fly);
}

void CAnimationView3D::OnUpdateBUTTONTurn(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_Turn);
}

void CAnimationView3D::OnChangeBackground() 
{
	Standard_Real R1;
	Standard_Real G1;
	Standard_Real B1;
    myView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	COLORREF m_clr ;
	m_clr = RGB(R1*255,G1*255,B1*255);

	CColorDialog dlgColor(m_clr);
	if (dlgColor.DoModal() == IDOK)
	{
		m_clr = dlgColor.GetColor();
		R1 = GetRValue(m_clr)/255.;
		G1 = GetGValue(m_clr)/255.;
		B1 = GetBValue(m_clr)/255.;
        myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	}
    myView->Redraw();
}

//==========================================================================================
//==========================================================================================
//==========================================================================================

void CAnimationView3D::OnStop() 
{
  myAnimTimer.Pause();
}

void CAnimationView3D::OnRestart() 
{
  myAnimTimer.Start();
  update3dView();
}
