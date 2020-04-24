// OCC_BaseView.cpp: implementation of the OCC_BaseView class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "OCC_BaseView.h"

BEGIN_MESSAGE_MAP(OCC_BaseView, CView)
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_MOUSELEAVE()
  ON_WM_NCMOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
OCC_BaseView::OCC_BaseView()
: myUpdateRequests (0),
  myCurZoom (0.0),
  myCurrentMode (CurAction3d_Nothing)
{
  myDefaultGestures = myMouseGestureMap;
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
OCC_BaseView::~OCC_BaseView()
{
  //
}

//=======================================================================
//function : GetDocument
//purpose  :
//=======================================================================
OCC_BaseDoc* OCC_BaseView::GetDocument() // non-debug version is inline
{
	return (OCC_BaseDoc*)m_pDocument;
}

// =======================================================================
// function : PostNcDestroy
// purpose  :
// =======================================================================
void OCC_BaseView::PostNcDestroy()
{
  if (!myView.IsNull())
  {
    myView->Remove();
    myView.Nullify();
  }
  CView::PostNcDestroy();
}

// =======================================================================
// function : PreCreateWindow
// purpose  :
// =======================================================================
BOOL OCC_BaseView::PreCreateWindow (CREATESTRUCT& cs)
{
  cs.lpszClass = ::AfxRegisterWndClass (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow (cs);
}

// =======================================================================
// function : OnInitialUpdate
// purpose  :
// =======================================================================
void OCC_BaseView::OnInitialUpdate()
{
  myCurrentMode = CurAction3d_Nothing;
  CView::OnInitialUpdate();
  if (!myView.IsNull())
  {
    return;
  }

  myView = GetAISContext()->CurrentViewer()->CreateView();
  myView->SetImmediateUpdate (false);
  myView->SetComputedMode (Standard_False);

  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myView->Viewer()->Driver());
  myView->Camera()->SetProjectionType (aDriver->Options().contextStereo
                                     ? Graphic3d_Camera::Projection_Stereo
                                     : Graphic3d_Camera::Projection_Orthographic);

  Handle(WNT_Window) aWNTWindow = new WNT_Window (GetSafeHwnd());
  myView->SetWindow (aWNTWindow);
  if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

  myView->Redraw();
  myView->Invalidate();
}

// ================================================================
// Function : GetAISContext
// Purpose  :
// ================================================================
const Handle(AIS_InteractiveContext)& OCC_BaseView::GetAISContext() const
{
  return ((OCC_BaseDoc*)m_pDocument)->GetInteractiveContext();
}

// ================================================================
// Function : update3dView
// Purpose  :
// ================================================================
void OCC_BaseView::update3dView()
{
  if (!myView.IsNull())
  {
    if (++myUpdateRequests == 1)
    {
      Invalidate (FALSE);
      UpdateWindow();
    }
  }
}

// ================================================================
// Function : redraw3dView
// Purpose  :
// ================================================================
void OCC_BaseView::redraw3dView()
{
  if (!myView.IsNull())
  {
    FlushViewEvents (GetAISContext(), myView, true);
  }
}

// ================================================================
// Function : handleViewRedraw
// Purpose  :
// ================================================================
void OCC_BaseView::handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                      const Handle(V3d_View)& theView)
{
  myUpdateRequests = 0;
  AIS_ViewController::handleViewRedraw (theCtx, theView);
}

// ================================================================
// Function : OnSelectionChanged
// Purpose  :
// ================================================================
void OCC_BaseView::OnSelectionChanged (const Handle(AIS_InteractiveContext)& theCtx,
                                       const Handle(V3d_View)& theView)
{
  AIS_ViewController::OnSelectionChanged (theCtx, theView);
  GetDocument()->OnSelectionChanged (theCtx, theView);
}

// =======================================================================
// function : OnDraw
// purpose  :
// =======================================================================
void OCC_BaseView::OnDraw (CDC* )
{
  // always redraw immediate layer (dynamic highlighting) on Paint event,
  // and redraw entire view content only when it is explicitly invalidated (V3d_View::Invalidate())
  myView->InvalidateImmediate();
  FlushViewEvents (GetAISContext(), myView, true);
}

// =======================================================================
// function : defineMouseGestures
// purpose  :
// =======================================================================
void OCC_BaseView::defineMouseGestures()
{
  myMouseGestureMap.Clear();
  AIS_MouseGesture aRot = AIS_MouseGesture_RotateOrbit;
  switch (myCurrentMode)
  {
    case CurAction3d_Nothing:
    {
      myMouseGestureMap = myDefaultGestures;
      break;
    }
    case CurAction3d_DynamicZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
      break;
    }
    case CurAction3d_GlobalPanning:
    {
      break;
    }
    case CurAction3d_WindowZooming:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
      break;
    }
    case CurAction3d_DynamicPanning:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
      break;
    }
    case CurAction3d_DynamicRotation:
    {
      myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, aRot);
      break;
    }
  }
}

// =======================================================================
// function : OnMouseMove
// purpose  :
// =======================================================================
void OCC_BaseView::OnMouseMove (UINT theFlags, CPoint thePoint)
{
  TRACKMOUSEEVENT aMouseEvent;          // for WM_MOUSELEAVE
  aMouseEvent.cbSize = sizeof(aMouseEvent);
  aMouseEvent.dwFlags = TME_LEAVE;
  aMouseEvent.hwndTrack = m_hWnd;
  aMouseEvent.dwHoverTime = HOVER_DEFAULT;
  if (!::_TrackMouseEvent (&aMouseEvent)) { TRACE("Track ERROR!\n"); }

  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  if (UpdateMousePosition (Graphic3d_Vec2i (thePoint.x, thePoint.y), PressedMouseButtons(), aFlags, false))
  {
    update3dView();
  }
}

// =======================================================================
// function : OnLButtonDown
// purpose  :
// =======================================================================
void OCC_BaseView::OnLButtonDown (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_LeftButton, aFlags, false);
  update3dView();
}

// =======================================================================
// function : OnLButtonUp
// purpose  :
// =======================================================================
void OCC_BaseView::OnLButtonUp (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_LeftButton, aFlags, false);
  if (myCurrentMode == CurAction3d_GlobalPanning)
  {
    myView->Place (thePoint.x, thePoint.y, myCurZoom);
    myView->Invalidate();
  }
  if (myCurrentMode != CurAction3d_Nothing)
  {
    setCurrentAction (CurAction3d_Nothing);
  }
  update3dView();
}

// =======================================================================
// function : OnMButtonDown
// purpose  :
// =======================================================================
void OCC_BaseView::OnMButtonDown (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_MiddleButton, aFlags, false);
  update3dView();
}

// =======================================================================
// function : OnMButtonUp
// purpose  :
// =======================================================================
void OCC_BaseView::OnMButtonUp (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_MiddleButton, aFlags, false);
  update3dView();
  if (myCurrentMode != CurAction3d_Nothing)
  {
    setCurrentAction (CurAction3d_Nothing);
  }
}

// =======================================================================
// function : OnRButtonDown
// purpose  :
// =======================================================================
void OCC_BaseView::OnRButtonDown (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  PressMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_RightButton, aFlags, false);
  update3dView();
  myClickPos.SetValues (thePoint.x, thePoint.y);
}

// =======================================================================
// function : OnRButtonUp
// purpose  :
// =======================================================================
void OCC_BaseView::OnRButtonUp (UINT theFlags, CPoint thePoint)
{
  const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (theFlags);
  ReleaseMouseButton (Graphic3d_Vec2i (thePoint.x, thePoint.y), Aspect_VKeyMouse_RightButton, aFlags, false);
  update3dView();
  if (myCurrentMode != CurAction3d_Nothing)
  {
    setCurrentAction (CurAction3d_Nothing);
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
BOOL OCC_BaseView::OnMouseWheel (UINT theFlags, short theDelta, CPoint thePoint)
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

// =======================================================================
// function : OnSize
// purpose  :
// =======================================================================
void OCC_BaseView::OnSize (UINT nType, int cx, int cy)
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

// =======================================================================
// function : OnMouseLeave
// purpose  :
// =======================================================================
void OCC_BaseView::OnMouseLeave()
{
  CPoint aCursorPos;
  if (GetCursorPos (&aCursorPos))
  {
    ReleaseMouseButton (Graphic3d_Vec2i (aCursorPos.x, aCursorPos.y),
                        PressedMouseButtons(),
                        Aspect_VKeyMouse_NONE,
                        false);
  }
}
