// Copyright (c) 2016-2019 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include "AIS_ViewController.hxx"

#include <AIS_AnimationCamera.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Point.hxx>
#include <AIS_RubberBand.hxx>
#include <AIS_XRTrackedDevice.hxx>
#include <Aspect_XRSession.hxx>
#include <Aspect_Grid.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <gp_Quaternion.hxx>
#include <V3d_View.hxx>
#include <WNT_HIDSpaceMouse.hxx>

// =======================================================================
// function : AIS_ViewController
// purpose  :
// =======================================================================
AIS_ViewController::AIS_ViewController()
: myLastEventsTime    (0.0),
  myToAskNextFrame    (false),
  myMinCamDistance    (1.0),
  myRotationMode      (AIS_RotationMode_BndBoxActive),
  myNavigationMode    (AIS_NavigationMode_Orbit),
  myMouseAccel           (1.0f),
  myOrbitAccel           (1.0f),
  myToShowPanAnchorPoint (true),
  myToShowRotateCenter   (true),
  myToLockOrbitZUp       (false),
  myToInvertPitch        (false),
  myToAllowTouchZRotation(false),
  myToAllowRotation      (true),
  myToAllowPanning       (true),
  myToAllowZooming       (true),
  myToAllowZFocus        (true),
  myToAllowHighlight     (true),
  myToAllowDragging      (true),
  myToStickToRayOnZoom   (true),
  myToStickToRayOnRotation (true),
  //
  myWalkSpeedAbsolute (1.5f),
  myWalkSpeedRelative (0.1f),
  myThrustSpeed (0.0f),
  myHasThrust (false),
  //
  myViewAnimation (new AIS_AnimationCamera ("AIS_ViewController_ViewAnimation", Handle(V3d_View)())),
  myPrevMoveTo (-1, -1),
  myHasHlrOnBeforeRotation (false),
  //
  myXRPrsDevices (0, 0),
  myXRLaserTeleColor (Quantity_NOC_GREEN),
  myXRLaserPickColor (Quantity_NOC_BLUE),
  myXRLastTeleportHand(Aspect_XRTrackedDeviceRole_Other),
  myXRLastPickingHand (Aspect_XRTrackedDeviceRole_Other),
  myXRLastPickDepthLeft (Precision::Infinite()),
  myXRLastPickDepthRight(Precision::Infinite()),
  myXRTurnAngle (M_PI_4),
  myToDisplayXRAuxDevices (false),
  myToDisplayXRHands (true),
  //
  myMouseClickThreshold (3.0),
  myMouseDoubleClickInt (0.4),
  myScrollZoomRatio     (15.0f),
  myMouseActiveGesture  (AIS_MouseGesture_NONE),
  myMouseActiveIdleRotation (false),
  myMouseClickCounter   (0),
  myMousePressed        (Aspect_VKeyMouse_NONE),
  myMouseModifiers      (Aspect_VKeyFlags_NONE),
  myMouseSingleButton   (-1),
  myMouseStopDragOnUnclick (false),
  //
  myTouchToleranceScale      (1.0f),
  myTouchRotationThresholdPx (6.0f),
  myTouchZRotationThreshold  (float(2.0 * M_PI / 180.0)),
  myTouchPanThresholdPx      (4.0f),
  myTouchZoomThresholdPx     (6.0f),
  myTouchZoomRatio           (0.13f),
  //
  myNbTouchesLast (0),
  myUpdateStartPointPan  (true),
  myUpdateStartPointRot  (true),
  myUpdateStartPointZRot (true),
  //
  my3dMouseNoRotate  (false, false, false),
  my3dMouseToReverse (true,  false, false),
  my3dMouseAccelTrans  (2.0f),
  my3dMouseAccelRotate (4.0f),
  my3dMouseIsQuadric   (true),
  //
  myPanPnt3d (Precision::Infinite(), 0.0, 0.0)
{
  memset(my3dMouseButtonState, 0, sizeof(my3dMouseButtonState));
  myEventTimer.Start();
  myViewAnimation->SetOwnDuration (0.5);

  myAnchorPointPrs1 = new AIS_Point (new Geom_CartesianPoint (0.0, 0.0, 0.0));
  myAnchorPointPrs1->SetZLayer (Graphic3d_ZLayerId_Top);
  myAnchorPointPrs1->SetMutable (true);

  myAnchorPointPrs2 = new AIS_Point (new Geom_CartesianPoint (0.0, 0.0, 0.0));
  myAnchorPointPrs2->SetZLayer (Graphic3d_ZLayerId_Topmost);
  myAnchorPointPrs2->SetMutable (true);

  myRubberBand = new AIS_RubberBand (Quantity_NOC_LIGHTBLUE, Aspect_TOL_SOLID, Quantity_NOC_LIGHTBLUE4, 0.5, 1.0);
  myRubberBand->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  myRubberBand->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_UPPER));
  myRubberBand->SetDisplayMode (0);
  myRubberBand->SetMutable (true);

  myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton,                           AIS_MouseGesture_RotateOrbit);
  myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton | Aspect_VKeyFlags_CTRL,   AIS_MouseGesture_Zoom);
  myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton | Aspect_VKeyFlags_SHIFT,  AIS_MouseGesture_Pan);
  myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton | Aspect_VKeyFlags_ALT,    AIS_MouseGesture_SelectRectangle);

  myMouseGestureMap.Bind (Aspect_VKeyMouse_RightButton,                          AIS_MouseGesture_Zoom);
  myMouseGestureMap.Bind (Aspect_VKeyMouse_RightButton | Aspect_VKeyFlags_CTRL,  AIS_MouseGesture_RotateOrbit);

  myMouseGestureMap.Bind (Aspect_VKeyMouse_MiddleButton,                         AIS_MouseGesture_Pan);
  myMouseGestureMap.Bind (Aspect_VKeyMouse_MiddleButton | Aspect_VKeyFlags_CTRL, AIS_MouseGesture_Pan);

  myXRTeleportHaptic.Duration  = 3600.0f;
  myXRTeleportHaptic.Frequency = 0.1f;
  myXRTeleportHaptic.Amplitude = 0.2f;

  myXRPickingHaptic.Duration  = 0.1f;
  myXRPickingHaptic.Frequency = 4.0f;
  myXRPickingHaptic.Amplitude = 0.1f;

  myXRSelectHaptic.Duration  = 0.2f;
  myXRSelectHaptic.Frequency = 4.0f;
  myXRSelectHaptic.Amplitude = 0.5f;
}

// =======================================================================
// function : ~AIS_ViewController
// purpose  :
// =======================================================================
AIS_ViewController::~AIS_ViewController()
{
  //
}

// =======================================================================
// function : ResetViewInput
// purpose  :
// =======================================================================
void AIS_ViewController::ResetViewInput()
{
  myKeys.Reset();
  myMousePressed      = Aspect_VKeyMouse_NONE;
  myMouseModifiers    = Aspect_VKeyFlags_NONE;
  myMouseSingleButton = -1;
  myUI.Dragging.ToAbort = true;
  myMouseActiveGesture = AIS_MouseGesture_NONE;
  myMouseClickTimer.Stop();
  myMouseClickCounter = 0;
}

// =======================================================================
// function : FlushViewEvents
// purpose  :
// =======================================================================
void AIS_ViewController::FlushViewEvents (const Handle(AIS_InteractiveContext)& theCtx,
                                          const Handle(V3d_View)& theView,
                                          Standard_Boolean theToHandle)
{
  flushBuffers (theCtx, theView);
  flushGestures(theCtx, theView);
  if (theToHandle)
  {
    HandleViewEvents (theCtx, theView);
  }
}

// =======================================================================
// function : flushBuffers
// purpose  :
// =======================================================================
void AIS_ViewController::flushBuffers (const Handle(AIS_InteractiveContext)& ,
                                       const Handle(V3d_View)& )
{
  myToAskNextFrame = false;

  myGL.IsNewGesture = myUI.IsNewGesture;
  myUI.IsNewGesture = false;

  myGL.ZoomActions.Clear();
  myGL.ZoomActions.Append (myUI.ZoomActions);
  myUI.ZoomActions.Clear();

  myGL.Orientation.ToFitAll = myUI.Orientation.ToFitAll;
  myUI.Orientation.ToFitAll = false;
  if (myUI.Orientation.ToSetViewOrient)
  {
    myUI.Orientation.ToSetViewOrient = false;
    myGL.Orientation.ToSetViewOrient = true;
    myGL.Orientation.ViewOrient      = myUI.Orientation.ViewOrient;
  }

  if (myUI.MoveTo.ToHilight)
  {
    myUI.MoveTo.ToHilight = false;
    myGL.MoveTo.ToHilight = true;
    myGL.MoveTo.Point     = myUI.MoveTo.Point;
  }

  {
    myGL.Selection.Tool   = myUI.Selection.Tool;
    myGL.Selection.IsXOR  = myUI.Selection.IsXOR;
    myGL.Selection.Points = myUI.Selection.Points;
    myUI.Selection.IsXOR  = false;
    if (myUI.Selection.Tool == AIS_ViewSelectionTool_Picking)
    {
      myUI.Selection.Points.Clear();
    }
  }

  if (myUI.Selection.ToApplyTool)
  {
    myGL.Selection.ToApplyTool = true;
    myUI.Selection.ToApplyTool = false;
    myUI.Selection.Points.Clear();
  }

  if (myUI.Panning.ToStart)
  {
    myUI.Panning.ToStart = false;
    myGL.Panning.ToStart = true;
    myGL.Panning.PointStart = myUI.Panning.PointStart;
  }

  if (myUI.Panning.ToPan)
  {
    myUI.Panning.ToPan = false;
    myGL.Panning.ToPan = true;
    myGL.Panning.Delta = myUI.Panning.Delta;
  }

  if (myUI.Dragging.ToAbort)
  {
    myUI.Dragging.ToAbort = false;
    myGL.Dragging.ToAbort = true;
  }
  else if (myUI.Dragging.ToStop)
  {
    myUI.Dragging.ToStop = false;
    myGL.Dragging.ToStop = true;
  }
  else if (myUI.Dragging.ToStart)
  {
    myUI.Dragging.ToStart = false;
    myGL.Dragging.ToStart = true;
    myGL.Dragging.PointStart = myUI.Dragging.PointStart;
  }
  myGL.Dragging.PointTo = myUI.Dragging.PointTo;

  if (myUI.OrbitRotation.ToStart)
  {
    myUI.OrbitRotation.ToStart    = false;
    myGL.OrbitRotation.ToStart    = true;
    myGL.OrbitRotation.PointStart = myUI.OrbitRotation.PointStart;
  }

  if (myUI.OrbitRotation.ToRotate)
  {
    myUI.OrbitRotation.ToRotate = false;
    myGL.OrbitRotation.ToRotate = true;
    myGL.OrbitRotation.PointTo  = myUI.OrbitRotation.PointTo;
  }

  if (myUI.ViewRotation.ToStart)
  {
    myUI.ViewRotation.ToStart    = false;
    myGL.ViewRotation.ToStart    = true;
    myGL.ViewRotation.PointStart = myUI.ViewRotation.PointStart;
  }

  if (myUI.ViewRotation.ToRotate)
  {
    myUI.ViewRotation.ToRotate = false;
    myGL.ViewRotation.ToRotate = true;
    myGL.ViewRotation.PointTo  = myUI.ViewRotation.PointTo;
  }

  if (myUI.ZRotate.ToRotate)
  {
    myGL.ZRotate = myUI.ZRotate;
    myUI.ZRotate.ToRotate = false;
  }
}

// =======================================================================
// function : flushGestures
// purpose  :
// =======================================================================
void AIS_ViewController::flushGestures (const Handle(AIS_InteractiveContext)& ,
                                        const Handle(V3d_View)& theView)
{
  const Standard_Real    aTolScale = myTouchToleranceScale;
  const Standard_Integer aTouchNb  = myTouchPoints.Extent();
  if (myNbTouchesLast != aTouchNb)
  {
    myNbTouchesLast = aTouchNb;
    myGL.IsNewGesture = true;
  }
  if (aTouchNb == 1) // touch
  {
    Aspect_Touch& aTouch = myTouchPoints.ChangeFromIndex (1);
    if (myUpdateStartPointRot)
    {
      // skip rotation if have active dragged object
      if (myNavigationMode == AIS_NavigationMode_Orbit)
      {
        myGL.OrbitRotation.ToStart = true;
        myGL.OrbitRotation.PointStart = myStartRotCoord;
      }
      else
      {
        myGL.ViewRotation.ToStart = true;
        myGL.ViewRotation.PointStart = myStartRotCoord;
      }

      myUpdateStartPointRot = false;
      theView->Invalidate();
    }

    // rotation
    const Standard_Real aRotTouchTol = !aTouch.IsPreciseDevice
                                     ? aTolScale * myTouchRotationThresholdPx
                                     : gp::Resolution();
    if (Abs (aTouch.Delta().x()) + Abs(aTouch.Delta().y()) > aRotTouchTol)
    {
      const Standard_Real aRotAccel = myNavigationMode == AIS_NavigationMode_FirstPersonWalk ? myMouseAccel : myOrbitAccel;
      if (myNavigationMode == AIS_NavigationMode_Orbit)
      {
        const Graphic3d_Vec2d aRotDelta = aTouch.To - myGL.OrbitRotation.PointStart;
        myGL.OrbitRotation.ToRotate = true;
        myGL.OrbitRotation.PointTo  = myGL.OrbitRotation.PointStart + aRotDelta * aRotAccel;
        myGL.Dragging.PointTo.SetValues ((int )aTouch.To.x(), (int )aTouch.To.y());
      }
      else
      {
        const Graphic3d_Vec2d aRotDelta = aTouch.To - myGL.ViewRotation.PointStart;
        myGL.ViewRotation.ToRotate = true;
        myGL.ViewRotation.PointTo = myGL.ViewRotation.PointStart + aRotDelta * aRotAccel;
        myGL.Dragging.PointTo.SetValues ((int )aTouch.To.x(), (int )aTouch.To.y());
      }

      aTouch.From = aTouch.To;
    }
  }
  else if (aTouchNb == 2) // pinch
  {
    Aspect_Touch& aFirstTouch = myTouchPoints.ChangeFromIndex (1);
    Aspect_Touch& aLastTouch  = myTouchPoints.ChangeFromIndex (2);
    const Graphic3d_Vec2d aFrom[2] = { aFirstTouch.From, aLastTouch.From };
    const Graphic3d_Vec2d aTo[2]   = { aFirstTouch.To,   aLastTouch.To   };

    Graphic3d_Vec2d aPinchCenterStart ((aFrom[0].x() + aFrom[1].x()) / 2.0,
                                       (aFrom[0].y() + aFrom[1].y()) / 2.0);

    Standard_Real aPinchCenterXEnd = (aTo[0].x() + aTo[1].x()) / 2.0;
    Standard_Real aPinchCenterYEnd = (aTo[0].y() + aTo[1].y()) / 2.0;

    Standard_Real aPinchCenterXDev = aPinchCenterXEnd - aPinchCenterStart.x();
    Standard_Real aPinchCenterYDev = aPinchCenterYEnd - aPinchCenterStart.y();

    Standard_Real aStartSize = (aFrom[0] - aFrom[1]).Modulus();
    Standard_Real anEndSize  = (  aTo[0] -   aTo[1]).Modulus();

    Standard_Real aDeltaSize = anEndSize - aStartSize;

    bool anIsClearDev = false;

    if (myToAllowTouchZRotation)
    {
      Standard_Real A1 = aFrom[0].y() - aFrom[1].y();
      Standard_Real B1 = aFrom[1].x() - aFrom[0].x();

      Standard_Real A2 = aTo[0].y() - aTo[1].y();
      Standard_Real B2 = aTo[1].x() - aTo[0].x();

      Standard_Real aRotAngle = 0.0;

      Standard_Real aDenomenator = A1*A2 + B1*B2;
      if (aDenomenator <= Precision::Confusion())
      {
        aRotAngle = 0.0;
      }
      else
      {
        Standard_Real aNumerator = A1*B2 - A2*B1;
        aRotAngle = ATan (aNumerator / aDenomenator);
      }

      if (Abs(aRotAngle) > Standard_Real(myTouchZRotationThreshold))
      {
        myGL.ZRotate.ToRotate = true;
        myGL.ZRotate.Angle = aRotAngle;
        anIsClearDev = true;
      }
    }

    if (Abs(aDeltaSize) > aTolScale * myTouchZoomThresholdPx)
    {
      // zoom
      aDeltaSize *= Standard_Real(myTouchZoomRatio);
      Aspect_ScrollDelta aParams (Graphic3d_Vec2i (aPinchCenterStart), aDeltaSize);
      myGL.ZoomActions.Append (aParams);
      anIsClearDev = true;
    }

    const Standard_Real aPanTouchTol = !aFirstTouch.IsPreciseDevice
                                     ? aTolScale * myTouchPanThresholdPx
                                     : gp::Resolution();
    if (Abs(aPinchCenterXDev) + Abs(aPinchCenterYDev) > aPanTouchTol)
    {
      // pan
      if (myUpdateStartPointPan)
      {
        myGL.Panning.ToStart = true;
        myGL.Panning.PointStart = Graphic3d_Vec2i (myStartPanCoord);
        myUpdateStartPointPan = false;
        theView->Invalidate();
      }

      myGL.Panning.ToPan = true;
      myGL.Panning.Delta.x() = int( aPinchCenterXDev);
      myGL.Panning.Delta.y() = int(-aPinchCenterYDev);
      anIsClearDev = true;
    }

    if (anIsClearDev)
    {
      aFirstTouch.From = aFirstTouch.To;
      aLastTouch .From = aLastTouch.To;
    }
  }
}

// =======================================================================
// function : UpdateViewOrientation
// purpose  :
// =======================================================================
void AIS_ViewController::UpdateViewOrientation (V3d_TypeOfOrientation theOrientation,
                                                bool theToFitAll)
{
  myUI.Orientation.ToFitAll = theToFitAll;
  myUI.Orientation.ToSetViewOrient = true;
  myUI.Orientation.ViewOrient = theOrientation;
}

// =======================================================================
// function : SelectInViewer
// purpose  :
// =======================================================================
void AIS_ViewController::SelectInViewer (const Graphic3d_Vec2i& thePnt,
                                         const bool theIsXOR)
{
  if (myUI.Selection.Tool != AIS_ViewSelectionTool_Picking)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Picking;
    myUI.Selection.Points.Clear();
  }

  myUI.Selection.IsXOR = theIsXOR;
  myUI.Selection.Points.Append (thePnt);
}

// =======================================================================
// function : SelectInViewer
// purpose  :
// =======================================================================
void AIS_ViewController::SelectInViewer (const NCollection_Sequence<Graphic3d_Vec2i>& thePnts,
                                         const bool theIsXOR)
{
  myUI.Selection.IsXOR = theIsXOR;
  myUI.Selection.Points = thePnts;
  myUI.Selection.ToApplyTool = true;
  if (thePnts.Length() == 1)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Picking;
  }
  else if (thePnts.Length() == 2)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_RubberBand;
  }
  else
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Polygon;
  }
}

// =======================================================================
// function : UpdateRubberBand
// purpose  :
// =======================================================================
void AIS_ViewController::UpdateRubberBand (const Graphic3d_Vec2i& thePntFrom,
                                           const Graphic3d_Vec2i& thePntTo,
                                           const bool theIsXOR)
{
  myUI.Selection.Tool = AIS_ViewSelectionTool_RubberBand;
  myUI.Selection.IsXOR = theIsXOR;
  myUI.Selection.Points.Clear();
  myUI.Selection.Points.Append (thePntFrom);
  myUI.Selection.Points.Append (thePntTo);
}

// =======================================================================
// function : UpdatePolySelection
// purpose  :
// =======================================================================
void AIS_ViewController::UpdatePolySelection (const Graphic3d_Vec2i& thePnt,
                                              bool theToAppend)
{
  if (myUI.Selection.Tool != AIS_ViewSelectionTool_Polygon)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Polygon;
    myUI.Selection.Points.Clear();
  }

  if (myUI.Selection.Points.IsEmpty())
  {
    myUI.Selection.Points.Append (thePnt);
  }
  else if (theToAppend
        && myUI.Selection.Points.Last() != thePnt)
  {
    myUI.Selection.Points.Append (thePnt);
  }
  else
  {
    myUI.Selection.Points.ChangeLast() = thePnt;
  }
}

// =======================================================================
// function : UpdateZoom
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateZoom (const Aspect_ScrollDelta& theDelta)
{
  if (!myUI.ZoomActions.IsEmpty())
  {
    if (myUI.ZoomActions.ChangeLast().Point == theDelta.Point)
    {
      myUI.ZoomActions.ChangeLast().Delta += theDelta.Delta;
      return false;
    }
  }

  myUI.ZoomActions.Append (theDelta);
  return true;
}

// =======================================================================
// function : UpdateZRotation
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateZRotation (double theAngle)
{
  if (!ToAllowTouchZRotation())
  {
    return false;
  }

  myUI.ZRotate.Angle = myUI.ZRotate.ToRotate
                     ? myUI.ZRotate.Angle + theAngle
                     : theAngle;
  if (myUI.ZRotate.ToRotate)
  {
    return false;
  }
  myUI.ZRotate.ToRotate = true;
  return true;
}

// =======================================================================
// function : UpdateMouseScroll
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateMouseScroll (const Aspect_ScrollDelta& theDelta)
{
  Aspect_ScrollDelta aDelta = theDelta;
  aDelta.Delta *= myScrollZoomRatio;
  return UpdateZoom (aDelta);
}

// =======================================================================
// function : UpdateMouseClick
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateMouseClick (const Graphic3d_Vec2i& thePoint,
                                           Aspect_VKeyMouse theButton,
                                           Aspect_VKeyFlags theModifiers,
                                           bool theIsDoubleClick)
{
  (void )theIsDoubleClick;
  if (theButton == Aspect_VKeyMouse_LeftButton)
  {
    SelectInViewer (thePoint, (theModifiers & Aspect_VKeyFlags_SHIFT) != 0);
    return true;
  }
  return false;
}

// =======================================================================
// function : UpdateMouseButtons
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateMouseButtons (const Graphic3d_Vec2i& thePoint,
                                             Aspect_VKeyMouse theButtons,
                                             Aspect_VKeyFlags theModifiers,
                                             bool theIsEmulated)
{
  bool toUpdateView = false;
  const double aTolClick = (theIsEmulated ? myTouchToleranceScale : 1.0) * myMouseClickThreshold;
  if (theButtons == Aspect_VKeyMouse_NONE
   && myMouseSingleButton > 0)
  {
    const Graphic3d_Vec2i aDelta = thePoint - myMousePressPoint;
    if (double(aDelta.cwiseAbs().maxComp()) < aTolClick)
    {
      ++myMouseClickCounter;
      const bool isDoubleClick = myMouseClickCounter == 2
                              && myMouseClickTimer.IsStarted()
                              && myMouseClickTimer.ElapsedTime() <= myMouseDoubleClickInt;

      myMouseClickTimer.Stop();
      myMouseClickTimer.Reset();
      myMouseClickTimer.Start();
      if (isDoubleClick)
      {
        myMouseClickCounter = 0;
      }
      toUpdateView = UpdateMouseClick (thePoint, (Aspect_VKeyMouse )myMouseSingleButton, theModifiers, isDoubleClick) || toUpdateView;
    }
    else
    {
      myMouseClickTimer.Stop();
      myMouseClickCounter = 0;
      myMouseStopDragOnUnclick = false;
      myUI.Dragging.ToStop = true;
      toUpdateView = true;
    }
    myMouseSingleButton = -1;
  }
  else if (theButtons == Aspect_VKeyMouse_NONE)
  {
    myMouseSingleButton = -1;
    if (myMouseStopDragOnUnclick)
    {
      myMouseStopDragOnUnclick = false;
      myUI.Dragging.ToStop = true;
      toUpdateView = true;
    }
  }
  else if (myMouseSingleButton == -1)
  {
    if ((theButtons & Aspect_VKeyMouse_LeftButton) == Aspect_VKeyMouse_LeftButton)
    {
      myMouseSingleButton = Aspect_VKeyMouse_LeftButton;
    }
    else if ((theButtons & Aspect_VKeyMouse_RightButton) == Aspect_VKeyMouse_RightButton)
    {
      myMouseSingleButton = Aspect_VKeyMouse_RightButton;
    }
    else if ((theButtons & Aspect_VKeyMouse_MiddleButton) == Aspect_VKeyMouse_MiddleButton)
    {
      myMouseSingleButton = Aspect_VKeyMouse_MiddleButton;
    }
    else
    {
      myMouseSingleButton = 0;
    }
    if (myMouseSingleButton != 0)
    {
      if (myMouseClickCounter == 1)
      {
        const Graphic3d_Vec2i aDelta = thePoint - myMousePressPoint;
        if (double(aDelta.cwiseAbs().maxComp()) >= aTolClick)
        {
          myMouseClickTimer.Stop();
          myMouseClickCounter = 0;
        }
      }
      myMousePressPoint = thePoint;
    }
  }
  else
  {
    myMouseSingleButton = 0;

    myUI.Dragging.ToAbort = true;
    toUpdateView = true;
  }

  const AIS_MouseGesture aPrevGesture = myMouseActiveGesture;
  myMouseModifiers = theModifiers;
  myMousePressed   = theButtons;
  if (theIsEmulated
   || myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    myMouseActiveIdleRotation = false;
    myMouseActiveGesture = AIS_MouseGesture_NONE;
    if (theButtons != 0)
    {
      myMousePressPoint    = thePoint;
      myMouseProgressPoint = myMousePressPoint;
    }

    if (myMouseGestureMap.Find (theButtons | theModifiers, myMouseActiveGesture))
    {
      switch (myMouseActiveGesture)
      {
        case AIS_MouseGesture_RotateView:
        case AIS_MouseGesture_RotateOrbit:
        {
          if (myToAllowRotation)
          {
            myUpdateStartPointRot = true;
          }
          else
          {
            myMouseActiveGesture = AIS_MouseGesture_NONE;
          }
          break;
        }
        case AIS_MouseGesture_Pan:
        {
          if (myToAllowPanning)
          {
            myUpdateStartPointPan = true;
          }
          else
          {
            myMouseActiveGesture = AIS_MouseGesture_NONE;
          }
          break;
        }
        case AIS_MouseGesture_Zoom:
        case AIS_MouseGesture_ZoomWindow:
        {
          if (!myToAllowZooming)
          {
            myMouseActiveGesture = AIS_MouseGesture_NONE;
          }
          break;
        }
        case AIS_MouseGesture_SelectRectangle:
        {
          break;
        }
        case AIS_MouseGesture_SelectLasso:
        {
          UpdatePolySelection (thePoint, true);
          break;
        }
        case AIS_MouseGesture_NONE:
        {
          break;
        }
      }
    }

    if (theButtons == Aspect_VKeyMouse_LeftButton
     && theModifiers == Aspect_VKeyFlags_NONE
     && myToAllowDragging)
    {
      myUI.Dragging.ToStart = true;
      myUI.Dragging.PointStart = thePoint;
    }
  }

  if (aPrevGesture != myMouseActiveGesture)
  {
    if (aPrevGesture == AIS_MouseGesture_SelectRectangle
     || aPrevGesture == AIS_MouseGesture_SelectLasso
     || aPrevGesture == AIS_MouseGesture_ZoomWindow)
    {
      myUI.Selection.ToApplyTool = true;
    }

    myUI.IsNewGesture = true;
    toUpdateView = true;
  }
  return toUpdateView;
}

// =======================================================================
// function : UpdateMousePosition
// purpose  :
// =======================================================================
bool AIS_ViewController::UpdateMousePosition (const Graphic3d_Vec2i& thePoint,
                                              Aspect_VKeyMouse theButtons,
                                              Aspect_VKeyFlags theModifiers,
                                              bool theIsEmulated)
{
  myMousePositionLast = thePoint;
  if (myMouseSingleButton > 0)
  {
    const double aTolClick = (theIsEmulated ? myTouchToleranceScale : 1.0) * myMouseClickThreshold;
    const Graphic3d_Vec2i aPressDelta = thePoint - myMousePressPoint;
    if (double(aPressDelta.cwiseAbs().maxComp()) >= aTolClick)
    {
      myMouseClickTimer.Stop();
      myMouseClickCounter = 0;
      myMouseSingleButton = -1;
      myMouseStopDragOnUnclick = true;
    }
  }

  bool toUpdateView = false;
  Graphic3d_Vec2i aDelta = thePoint - myMouseProgressPoint;
  if (!theIsEmulated
    && myNavigationMode == AIS_NavigationMode_FirstPersonWalk)
  {
    if (!myMouseActiveIdleRotation
      || myMouseActiveGesture != AIS_MouseGesture_RotateView)
    {
      myMouseActiveIdleRotation = true;
      myMouseActiveGesture = AIS_MouseGesture_RotateView;
      myMousePressPoint     = thePoint;
      myMouseProgressPoint  = thePoint;
      myUpdateStartPointRot = false;
      myUI.ViewRotation.ToStart = true;
      myUI.ViewRotation.PointStart.SetValues (thePoint.x(), thePoint.y());
      myUI.ViewRotation.ToRotate = false;
      aDelta.SetValues (0, 0);
    }
  }
  else
  {
    if (myMouseActiveIdleRotation
     && myMouseActiveGesture == AIS_MouseGesture_RotateView)
    {
      myMouseActiveGesture = AIS_MouseGesture_NONE;
    }
    myMouseActiveIdleRotation = false;
  }

  if (myMouseModifiers != theModifiers
   && UpdateMouseButtons (thePoint, theButtons, theModifiers, theIsEmulated))
  {
    toUpdateView = true;
  }

  switch (myMouseActiveGesture)
  {
    case AIS_MouseGesture_SelectRectangle:
    case AIS_MouseGesture_ZoomWindow:
    {
      UpdateRubberBand (myMousePressPoint, thePoint);
      if (myMouseActiveGesture == AIS_MouseGesture_ZoomWindow)
      {
        myUI.Selection.Tool = AIS_ViewSelectionTool_ZoomWindow;
      }
      toUpdateView = true;
      break;
    }
    case AIS_MouseGesture_SelectLasso:
    {
      UpdatePolySelection (thePoint, true);
      toUpdateView = true;
      break;
    }
    case AIS_MouseGesture_RotateOrbit:
    case AIS_MouseGesture_RotateView:
    {
      if (!myToAllowRotation)
      {
        break;
      }
      if (myUpdateStartPointRot)
      {
        if (myMouseActiveGesture == AIS_MouseGesture_RotateOrbit)
        {
          myUI.OrbitRotation.ToStart = true;
          myUI.OrbitRotation.PointStart.SetValues (myMousePressPoint.x(), myMousePressPoint.y());
        }
        else
        {
          myUI.ViewRotation.ToStart = true;
          myUI.ViewRotation.PointStart.SetValues (myMousePressPoint.x(), myMousePressPoint.y());
        }
        myUpdateStartPointRot = false;
      }

      const double aRotTol = theIsEmulated
                           ? double(myTouchToleranceScale) * myTouchRotationThresholdPx
                           : 0.0;
      if (double (Abs (aDelta.x()) + Abs (aDelta.y())) > aRotTol)
      {
        const double aRotAccel = myNavigationMode == AIS_NavigationMode_FirstPersonWalk ? myMouseAccel : myOrbitAccel;
        const Graphic3d_Vec2i aRotDelta = thePoint - myMousePressPoint;
        if (myMouseActiveGesture == AIS_MouseGesture_RotateOrbit)
        {
          myUI.OrbitRotation.ToRotate = true;
          myUI.OrbitRotation.PointTo = Graphic3d_Vec2d (myMousePressPoint.x(), myMousePressPoint.y())
                                     + Graphic3d_Vec2d (aRotDelta.x(), aRotDelta.y()) * aRotAccel;
        }
        else
        {
          myUI.ViewRotation.ToRotate = true;
          myUI.ViewRotation.PointTo = Graphic3d_Vec2d (myMousePressPoint.x(), myMousePressPoint.y())
                                    + Graphic3d_Vec2d (aRotDelta.x(), aRotDelta.y()) * aRotAccel;
        }
        myUI.Dragging.PointTo = thePoint;

        myMouseProgressPoint = thePoint;
        toUpdateView = true;
      }
      break;
    }
    case AIS_MouseGesture_Zoom:
    {
      if (!myToAllowZooming)
      {
        break;
      }
      const double aZoomTol = theIsEmulated
                            ? double(myTouchToleranceScale) * myTouchZoomThresholdPx
                            : 0.0;
      if (double (Abs (aDelta.x())) > aZoomTol)
      {
        if (UpdateZoom (Aspect_ScrollDelta (aDelta.x())))
        {
          toUpdateView = true;
        }
        myMouseProgressPoint = thePoint;
      }
      break;
    }
    case AIS_MouseGesture_Pan:
    {
      if (!myToAllowPanning)
      {
        break;
      }
      const double aPanTol = theIsEmulated
                           ? double(myTouchToleranceScale) * myTouchPanThresholdPx
                           : 0.0;
      if (double (Abs (aDelta.x()) + Abs (aDelta.y())) > aPanTol)
      {
        if (myUpdateStartPointPan)
        {
          myUI.Panning.ToStart = true;
          myUI.Panning.PointStart.SetValues (myMousePressPoint.x(), myMousePressPoint.y());
          myUpdateStartPointPan = false;
        }

        aDelta.y() = -aDelta.y();
        myMouseProgressPoint = thePoint;
        if (myUI.Panning.ToPan)
        {
          myUI.Panning.Delta += aDelta;
        }
        else
        {
          myUI.Panning.ToPan = true;
          myUI.Panning.Delta = aDelta;
        }
        toUpdateView = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  if (theButtons == Aspect_VKeyMouse_NONE
  &&  myNavigationMode != AIS_NavigationMode_FirstPersonWalk
  && !theIsEmulated
  && !HasTouchPoints()
  &&  myToAllowHighlight)
  {
    myUI.MoveTo.ToHilight = true;
    myUI.MoveTo.Point = thePoint;
    toUpdateView = true;
  }
  return toUpdateView;
}

// =======================================================================
// function : AddTouchPoint
// purpose  :
// =======================================================================
void AIS_ViewController::AddTouchPoint (Standard_Size theId,
                                        const Graphic3d_Vec2d& thePnt,
                                        Standard_Boolean theClearBefore)
{
  myUI.MoveTo.ToHilight = false;
  if (theClearBefore)
  {
    RemoveTouchPoint ((Standard_Size )-1);
  }

  myTouchPoints.Add (theId, Aspect_Touch (thePnt, false));
  if (myTouchPoints.Extent() == 1)
  {
    myUpdateStartPointRot = true;
    myStartRotCoord = thePnt;
    if (myToAllowDragging)
    {
      myUI.Dragging.ToStart = true;
      myUI.Dragging.PointStart.SetValues ((int )thePnt.x(), (int )thePnt.y());
    }
  }
  else if (myTouchPoints.Extent() == 2)
  {
    myUI.Dragging.ToAbort = true;

    myUpdateStartPointPan = true;
    myStartPanCoord = thePnt;
  }
  myUI.IsNewGesture = true;
}

// =======================================================================
// function : RemoveTouchPoint
// purpose  :
// =======================================================================
bool AIS_ViewController::RemoveTouchPoint (Standard_Size theId,
                                           Standard_Boolean theClearSelectPnts)
{
  if (theId == (Standard_Size )-1)
  {
    myTouchPoints.Clear (false);
  }
  else
  {
    const Standard_Integer anOldExtent = myTouchPoints.Extent();
    myTouchPoints.RemoveKey (theId);
    if (myTouchPoints.Extent() == anOldExtent)
    {
      return false;
    }
  }

  if (myTouchPoints.Extent() == 1)
  {
    // avoid incorrect transition from pinch to one finger
    Aspect_Touch& aFirstTouch = myTouchPoints.ChangeFromIndex (1);
    aFirstTouch.To = aFirstTouch.From;

    myStartRotCoord = aFirstTouch.To;
    myUpdateStartPointRot = true;
  }
  else if (myTouchPoints.Extent() == 2)
  {
    myStartPanCoord = myTouchPoints.FindFromIndex (1).To;
    myUpdateStartPointPan = true;
  }
  else if (myTouchPoints.IsEmpty())
  {
    if (theClearSelectPnts)
    {
      myUI.Selection.ToApplyTool = true;
    }

    myUI.Dragging.ToStop = true;
  }
  myUI.IsNewGesture = true;
  return true;
}

// =======================================================================
// function : UpdateTouchPoint
// purpose  :
// =======================================================================
void AIS_ViewController::UpdateTouchPoint (Standard_Size theId,
                                           const Graphic3d_Vec2d& thePnt)
{
  if (Aspect_Touch* aTouch = myTouchPoints.ChangeSeek (theId))
  {
    aTouch->To = thePnt;
  }
  else
  {
    AddTouchPoint (theId, thePnt);
  }
}

// =======================================================================
// function : Update3dMouse
// purpose  :
// =======================================================================
bool AIS_ViewController::Update3dMouse (const WNT_HIDSpaceMouse& theEvent)
{
  bool toUpdate = false;
  toUpdate = update3dMouseTranslation (theEvent) || toUpdate;
  toUpdate = update3dMouseRotation (theEvent) || toUpdate;
  toUpdate = update3dMouseKeys (theEvent) || toUpdate;
  return toUpdate;
}

// =======================================================================
// function : update3dMouseTranslation
// purpose  :
// =======================================================================
bool AIS_ViewController::update3dMouseTranslation (const WNT_HIDSpaceMouse& theEvent)
{
  if (!theEvent.IsTranslation())
  {
    return false;
  }

  bool isIdle = true;
  const double aTimeStamp = EventTime();
  const Graphic3d_Vec3d aTrans = theEvent.Translation (isIdle, my3dMouseIsQuadric) * my3dMouseAccelTrans;
  myKeys.KeyFromAxis (Aspect_VKey_NavSlideLeft, Aspect_VKey_NavSlideRight, aTimeStamp, aTrans.x());
  myKeys.KeyFromAxis (Aspect_VKey_NavForward,   Aspect_VKey_NavBackward,   aTimeStamp, aTrans.y());
  myKeys.KeyFromAxis (Aspect_VKey_NavSlideUp,   Aspect_VKey_NavSlideDown,  aTimeStamp, aTrans.z());
  return true;
}

// =======================================================================
// function : update3dMouseRotation
// purpose  :
// =======================================================================
bool AIS_ViewController::update3dMouseRotation (const WNT_HIDSpaceMouse& theEvent)
{
  if (!theEvent.IsRotation()
   || !myToAllowRotation)
  {
    return false;
  }

  bool isIdle = true, toUpdate = false;
  const double aTimeStamp = EventTime();
  const Graphic3d_Vec3d aRot3 = theEvent.Rotation (isIdle, my3dMouseIsQuadric) * my3dMouseAccelRotate;
  if (!my3dMouseNoRotate.x())
  {
    KeyFromAxis (Aspect_VKey_NavLookUp,   Aspect_VKey_NavLookDown,  aTimeStamp, !my3dMouseToReverse.x() ? aRot3.x() : -aRot3.x());
    toUpdate = true;
  }
  if (!my3dMouseNoRotate.y())
  {
    KeyFromAxis (Aspect_VKey_NavRollCW,   Aspect_VKey_NavRollCCW,   aTimeStamp, !my3dMouseToReverse.y() ? aRot3.y() : -aRot3.y());
    toUpdate = true;
  }
  if (!my3dMouseNoRotate.z())
  {
    KeyFromAxis (Aspect_VKey_NavLookLeft, Aspect_VKey_NavLookRight, aTimeStamp, !my3dMouseToReverse.z() ? aRot3.z() : -aRot3.z());
    toUpdate = true;
  }
  return toUpdate;
}

// =======================================================================
// function : update3dMouseKeys
// purpose  :
// =======================================================================
bool AIS_ViewController::update3dMouseKeys (const WNT_HIDSpaceMouse& theEvent)
{
  bool toUpdate = false;
  const double aTimeStamp = EventTime();
  if (theEvent.IsKeyState())
  {
    const uint32_t aKeyState = theEvent.KeyState();
    for (unsigned short aKeyBit = 0; aKeyBit < 32; ++aKeyBit)
    {
      const bool isPressed  = (aKeyState & (1 << aKeyBit)) != 0;
      const bool isReleased = my3dMouseButtonState[aKeyBit] && !isPressed;
      //const bool isRepeated = my3dMouseButtonState[aKeyBit] &&  isPressed;
      my3dMouseButtonState[aKeyBit] = isPressed;
      if (!isReleased && !isPressed)
      {
        continue;
      }

      const Aspect_VKey aVKey = theEvent.HidToSpaceKey (aKeyBit);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        toUpdate = true;
        if (isPressed)
        {
          KeyDown (aVKey, aTimeStamp);
        }
        else
        {
          KeyUp (aVKey, aTimeStamp);
        }
      }
    }
  }
  return toUpdate;
}

// =======================================================================
// function : SetNavigationMode
// purpose  :
// =======================================================================
void AIS_ViewController::SetNavigationMode (AIS_NavigationMode theMode)
{
  myNavigationMode = theMode;

  // abort rotation
  myUI.OrbitRotation.ToStart  = false;
  myUI.OrbitRotation.ToRotate = false;
  myUI.ViewRotation.ToStart   = false;
  myUI.ViewRotation.ToRotate  = false;
}

// =======================================================================
// function : KeyDown
// purpose  :
// =======================================================================
void AIS_ViewController::KeyDown (Aspect_VKey theKey,
                                  double theTime,
                                  double thePressure)
{
  myKeys.KeyDown (theKey, theTime, thePressure);
}

// =======================================================================
// function : KeyUp
// purpose  :
// =======================================================================
void AIS_ViewController::KeyUp (Aspect_VKey theKey,
                                double theTime)
{
  myKeys.KeyUp (theKey, theTime);
}

// =======================================================================
// function : KeyFromAxis
// purpose  :
// =======================================================================
void AIS_ViewController::KeyFromAxis (Aspect_VKey theNegative,
                                      Aspect_VKey thePositive,
                                      double theTime,
                                      double thePressure)
{
  myKeys.KeyFromAxis (theNegative, thePositive, theTime, thePressure);
}

// =======================================================================
// function : FetchNavigationKeys
// purpose  :
// =======================================================================
AIS_WalkDelta AIS_ViewController::FetchNavigationKeys (Standard_Real theCrouchRatio,
                                                       Standard_Real theRunRatio)
{
  AIS_WalkDelta aWalk;

  // navigation keys
  double aPrevEventTime = 0.0, aNewEventTime = 0.0;
  updateEventsTime (aPrevEventTime, aNewEventTime);

  double aDuration = 0.0, aPressure = 1.0;
  if (Abs (myThrustSpeed) > gp::Resolution())
  {
    if (myHasThrust)
    {
      aWalk[AIS_WalkTranslation_Forward].Value = myThrustSpeed * (aNewEventTime - aPrevEventTime);
    }
    myHasThrust = true;
    myToAskNextFrame = true;
  }
  else
  {
    myHasThrust = false;
  }

  aWalk.SetRunning (theRunRatio > 1.0
                 && myKeys.IsKeyDown (Aspect_VKey_Shift));
  if (myKeys.HoldDuration (Aspect_VKey_NavJump, aNewEventTime, aDuration))
  {
    myKeys.KeyUp (Aspect_VKey_NavJump, aNewEventTime);
    aWalk.SetJumping (true);
  }
  if (!aWalk.IsJumping()
   && theCrouchRatio < 1.0
   && myKeys.HoldDuration (Aspect_VKey_NavCrouch, aNewEventTime, aDuration))
  {
    aWalk.SetRunning (false);
    aWalk.SetCrouching (true);
  }

  const double aMaxDuration = aNewEventTime - aPrevEventTime;
  const double aRunRatio = aWalk.IsRunning()
                         ? theRunRatio
                         : aWalk.IsCrouching()
                          ? theCrouchRatio
                          : 1.0;
  if (myKeys.HoldDuration (Aspect_VKey_NavForward, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk[AIS_WalkTranslation_Forward].Value += aProgress;
    aWalk[AIS_WalkTranslation_Forward].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Forward].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavBackward, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk[AIS_WalkTranslation_Forward].Value += -aProgress;
    aWalk[AIS_WalkTranslation_Forward].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Forward].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavSlideLeft, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk[AIS_WalkTranslation_Side].Value = -aProgress;
    aWalk[AIS_WalkTranslation_Side].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Side].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavSlideRight, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk[AIS_WalkTranslation_Side].Value = aProgress;
    aWalk[AIS_WalkTranslation_Side].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Side].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavLookLeft, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Yaw].Value = aProgress;
    aWalk[AIS_WalkRotation_Yaw].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Yaw].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavLookRight, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Yaw].Value = -aProgress;
    aWalk[AIS_WalkRotation_Yaw].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Yaw].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavLookUp, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Pitch].Value = !myToInvertPitch ? -aProgress : aProgress;
    aWalk[AIS_WalkRotation_Pitch].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Pitch].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavLookDown, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Pitch].Value = !myToInvertPitch ? aProgress : -aProgress;
    aWalk[AIS_WalkRotation_Pitch].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Pitch].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavRollCCW, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Roll].Value = -aProgress;
    aWalk[AIS_WalkRotation_Roll].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Roll].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavRollCW, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration)) * aPressure;
    aWalk[AIS_WalkRotation_Roll].Value = aProgress;
    aWalk[AIS_WalkRotation_Roll].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Roll].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavSlideUp, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aWalk[AIS_WalkTranslation_Up].Value = aProgress;
    aWalk[AIS_WalkTranslation_Up].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Up].Duration = aDuration;
  }
  if (myKeys.HoldDuration (Aspect_VKey_NavSlideDown, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = Abs (Min (aMaxDuration, aDuration));
    aWalk[AIS_WalkTranslation_Up].Value = -aProgress;
    aWalk[AIS_WalkTranslation_Up].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Up].Duration = aDuration;
  }
  return aWalk;
}

// =======================================================================
// function : AbortViewAnimation
// purpose  :
// =======================================================================
void AIS_ViewController::AbortViewAnimation()
{
  if (!myViewAnimation.IsNull()
   && !myViewAnimation->IsStopped())
  {
    myViewAnimation->Stop();
    myViewAnimation->SetView (Handle(V3d_View)());
  }
}

// =======================================================================
// function : handlePanning
// purpose  :
// =======================================================================
void AIS_ViewController::handlePanning (const Handle(V3d_View)& theView)
{
  if (!myGL.Panning.ToPan
   || !myToAllowPanning)
  {
    return;
  }

  AbortViewAnimation();

  const Handle(Graphic3d_Camera)& aCam = theView->Camera();
  if (aCam->IsOrthographic()
  || !hasPanningAnchorPoint())
  {
    theView->Pan (myGL.Panning.Delta.x(), myGL.Panning.Delta.y());
    theView->Invalidate();
    theView->View()->SynchronizeXRPosedToBaseCamera();
    return;
  }

  Graphic3d_Vec2i aWinSize;
  theView->Window()->Size (aWinSize.x(), aWinSize.y());

  const gp_Dir& aDir = aCam->Direction();
  const gp_Ax3 aCameraCS (aCam->Center(), aDir.Reversed(), aDir ^ aCam->Up());
  const gp_XYZ anEyeToPnt = myPanPnt3d.XYZ() - aCam->Eye().XYZ();
  const gp_Pnt aViewDims = aCam->ViewDimensions (anEyeToPnt.Dot (aCam->Direction().XYZ())); // view dimensions at 3D point
  const Graphic3d_Vec2d aDxy (-aViewDims.X() * myGL.Panning.Delta.x() / double(aWinSize.x()),
                              -aViewDims.X() * myGL.Panning.Delta.y() / double(aWinSize.x()));

  //theView->Translate (aCam, aDxy.x(), aDxy.y());
  gp_Trsf aPanTrsf;
  const gp_Vec aCameraPan = gp_Vec (aCameraCS.XDirection()) * aDxy.x()
                          + gp_Vec (aCameraCS.YDirection()) * aDxy.y();
  aPanTrsf.SetTranslation (aCameraPan);
  aCam->Transform (aPanTrsf);
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

// =======================================================================
// function : handleZRotate
// purpose  :
// =======================================================================
void AIS_ViewController::handleZRotate (const Handle(V3d_View)& theView)
{
  if (!myGL.ZRotate.ToRotate
   || !myToAllowRotation)
  {
    return;
  }

  AbortViewAnimation();

  Graphic3d_Vec2i aViewPort;
  theView->Window()->Size (aViewPort.x(), aViewPort.y());
  Graphic3d_Vec2d aRotPnt (0.99 * aViewPort.x(),
                           0.5  * aViewPort.y());
  theView->StartRotation (int(aRotPnt.x()), int(aRotPnt.y()), 0.4);
  aRotPnt.y() += myGL.ZRotate.Angle * aViewPort.y();
  theView->Rotation (int(aRotPnt.x()), int(aRotPnt.y()));
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

// =======================================================================
// function : handleZoom
// purpose  :
// =======================================================================
void AIS_ViewController::handleZoom (const Handle(V3d_View)& theView,
                                     const Aspect_ScrollDelta& theParams,
                                     const gp_Pnt* thePnt)
{
  if (!myToAllowZooming)
  {
    return;
  }

  AbortViewAnimation();

  const Handle(Graphic3d_Camera)& aCam = theView->Camera();
  if (thePnt != NULL)
  {
    const double aViewDist = Max (myMinCamDistance, (thePnt->XYZ() - aCam->Eye().XYZ()).Modulus());
    aCam->SetCenter (aCam->Eye().XYZ() + aCam->Direction().XYZ() * aViewDist);
  }

  if (!theParams.HasPoint())
  {
    Standard_Real aCoeff = Abs(theParams.Delta) / 100.0 + 1.0;
    aCoeff = theParams.Delta > 0.0 ? aCoeff : 1.0 / aCoeff;
    theView->SetZoom (aCoeff, true);
    theView->Invalidate();
    theView->View()->SynchronizeXRPosedToBaseCamera();
    return;
  }

  // integer delta is too rough for small smooth increments
  //theView->StartZoomAtPoint (theParams.Point.x(), theParams.Point.y());
  //theView->ZoomAtPoint (0, 0, (int )theParams.Delta, (int )theParams.Delta);

  double aDZoom = Abs (theParams.Delta) / 100.0 + 1.0;
  aDZoom = (theParams.Delta > 0.0) ? aDZoom : 1.0 / aDZoom;
  if (aDZoom <= 0.0)
  {
    return;
  }

  const Graphic3d_Vec2d aViewDims (aCam->ViewDimensions().X(), aCam->ViewDimensions().Y());

  // ensure that zoom will not be too small or too big
  double aCoef = aDZoom;
  if (aViewDims.x() < aCoef * Precision::Confusion())
  {
    aCoef = aViewDims.x() / Precision::Confusion();
  }
  else if (aViewDims.x() > aCoef * 1e12)
  {
    aCoef = aViewDims.x() / 1e12;
  }
  if (aViewDims.y() < aCoef * Precision::Confusion())
  {
    aCoef = aViewDims.y() / Precision::Confusion();
  }
  else if (aViewDims.y() > aCoef * 1e12)
  {
    aCoef = aViewDims.y() / 1e12;
  }

  Graphic3d_Vec2d aZoomAtPointXYv (0.0, 0.0);
  theView->Convert (theParams.Point.x(), theParams.Point.y(),
                    aZoomAtPointXYv.x(), aZoomAtPointXYv.y());
  Graphic3d_Vec2d aDxy = aZoomAtPointXYv / aCoef;
  aCam->SetScale (aCam->Scale() / aCoef);

  const gp_Dir& aDir = aCam->Direction();
  const gp_Ax3 aCameraCS (aCam->Center(), aDir.Reversed(), aDir ^ aCam->Up());

  // pan back to the point
  aDxy = aZoomAtPointXYv - aDxy;
  if (thePnt != NULL)
  {
    // zoom at 3D point with perspective projection
    const gp_XYZ anEyeToPnt = thePnt->XYZ() - aCam->Eye().XYZ();
    aDxy.SetValues (anEyeToPnt.Dot (aCameraCS.XDirection().XYZ()),
                    anEyeToPnt.Dot (aCameraCS.YDirection().XYZ()));

    // view dimensions at 3D point
    const gp_Pnt aViewDims1 = aCam->ViewDimensions (anEyeToPnt.Dot (aCam->Direction().XYZ()));

    Graphic3d_Vec2i aWinSize;
    theView->Window()->Size (aWinSize.x(), aWinSize.y());
    const Graphic3d_Vec2d aPanFromCenterPx (double(theParams.Point.x()) - 0.5 * double(aWinSize.x()),
                                            double(aWinSize.y() - theParams.Point.y() - 1) - 0.5 * double(aWinSize.y()));
    aDxy.x() += -aViewDims1.X() * aPanFromCenterPx.x() / double(aWinSize.x());
    aDxy.y() += -aViewDims1.Y() * aPanFromCenterPx.y() / double(aWinSize.y());
  }

  //theView->Translate (aCam, aDxy.x(), aDxy.y());
  gp_Trsf aPanTrsf;
  const gp_Vec aCameraPan = gp_Vec (aCameraCS.XDirection()) * aDxy.x()
                          + gp_Vec (aCameraCS.YDirection()) * aDxy.y();
  aPanTrsf.SetTranslation (aCameraPan);
  aCam->Transform (aPanTrsf);
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

// =======================================================================
// function : handleZFocusScroll
// purpose  :
// =======================================================================
void AIS_ViewController::handleZFocusScroll (const Handle(V3d_View)& theView,
                                             const Aspect_ScrollDelta& theParams)
{
  if (!myToAllowZFocus
   || !theView->Camera()->IsStereo())
  {
    return;
  }

  Standard_Real aFocus = theView->Camera()->ZFocus() + (theParams.Delta > 0.0 ? 0.05 : -0.05);
  if (aFocus > 0.2
   && aFocus < 2.0)
  {
    theView->Camera()->SetZFocus (theView->Camera()->ZFocusType(), aFocus);
    theView->Invalidate();
  }
}

// =======================================================================
// function : handleOrbitRotation
// purpose  :
// =======================================================================
void AIS_ViewController::handleOrbitRotation (const Handle(V3d_View)& theView,
                                              const gp_Pnt& thePnt,
                                              bool theToLockZUp)
{
  if (!myToAllowRotation)
  {
    return;
  }

  const Handle(Graphic3d_Camera)& aCam = theView->View()->IsActiveXR()
                                       ? theView->View()->BaseXRCamera()
                                       : theView->Camera();
  if (myGL.OrbitRotation.ToStart)
  {
    // default alternatives
    //if (myRotationMode == AIS_RotationMode_BndBoxActive) theView->StartRotation (myGL.RotateAtPoint.x(), myGL.RotateAtPoint.y());
    //theView->Rotate (0.0, 0.0, 0.0, thePnt.X(), thePnt.Y(), thePnt.Z(), true);

    myRotatePnt3d      = thePnt;
    myCamStartOpUp     = aCam->Up();
    myCamStartOpDir    = aCam->Direction();
    myCamStartOpEye    = aCam->Eye();
    myCamStartOpCenter = aCam->Center();

    gp_Trsf aTrsf;
    aTrsf.SetTransformation (gp_Ax3 (myRotatePnt3d, aCam->OrthogonalizedUp(), aCam->Direction()),
                             gp_Ax3 (myRotatePnt3d, gp::DZ(), gp::DX()));
    const gp_Quaternion aRot = aTrsf.GetRotation();
    aRot.GetEulerAngles (gp_YawPitchRoll, myRotateStartYawPitchRoll[0], myRotateStartYawPitchRoll[1], myRotateStartYawPitchRoll[2]);

    aTrsf.Invert();
    myCamStartOpToEye    = gp_Vec (myRotatePnt3d, aCam->Eye()).Transformed (aTrsf);
    myCamStartOpToCenter = gp_Vec (myRotatePnt3d, aCam->Center()).Transformed (aTrsf);

    theView->Invalidate();
  }

  if (!myGL.OrbitRotation.ToRotate)
  {
    return;
  }

  AbortViewAnimation();
  if (theToLockZUp)
  {
    // amend camera to exclude roll angle (put camera Up vector to plane containing global Z and view direction)
    Graphic3d_Vec2i aWinXY;
    theView->Window()->Size (aWinXY.x(), aWinXY.y());
    double aYawAngleDelta   =  ((myGL.OrbitRotation.PointStart.x() - myGL.OrbitRotation.PointTo.x()) / double (aWinXY.x())) * (M_PI * 0.5);
    double aPitchAngleDelta = -((myGL.OrbitRotation.PointStart.y() - myGL.OrbitRotation.PointTo.y()) / double (aWinXY.y())) * (M_PI * 0.5);
    double aPitchAngleNew = 0.0, aRoll = 0.0;
    const double aYawAngleNew = myRotateStartYawPitchRoll[0] + aYawAngleDelta;
    if (!theView->View()->IsActiveXR())
    {
      aPitchAngleNew = Max (Min (myRotateStartYawPitchRoll[1] + aPitchAngleDelta, M_PI * 0.5 - M_PI / 180.0), -M_PI * 0.5 + M_PI / 180.0);
      aRoll = 0.0;
    }

    gp_Quaternion aRot;
    aRot.SetEulerAngles (gp_YawPitchRoll, aYawAngleNew, aPitchAngleNew, aRoll);
    gp_Trsf aTrsfRot;
    aTrsfRot.SetRotation (aRot);

    const gp_Dir aNewUp = gp::DZ().Transformed (aTrsfRot);
    aCam->SetUp (aNewUp);
    aCam->SetEyeAndCenter (myRotatePnt3d.XYZ() + myCamStartOpToEye   .Transformed (aTrsfRot).XYZ(),
                           myRotatePnt3d.XYZ() + myCamStartOpToCenter.Transformed (aTrsfRot).XYZ());

    aCam->OrthogonalizeUp();
  }
  else
  {
    // default alternatives
    //if (myRotationMode == AIS_RotationMode_BndBoxActive) theView->Rotation (myGL.RotateToPoint.x(), myGL.RotateToPoint.y());
    //theView->Rotate (aDX, aDY, aDZ, myRotatePnt3d.X(), myRotatePnt3d.Y(), myRotatePnt3d.Z(), false);

    // restore previous camera state
    aCam->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
    aCam->SetUp (myCamStartOpUp);
    aCam->SetDirectionFromEye (myCamStartOpDir);

    Graphic3d_Vec2d aWinXY;
    theView->Size (aWinXY.x(), aWinXY.y());
    const Standard_Real rx = (Standard_Real )theView->Convert (aWinXY.x());
    const Standard_Real ry = (Standard_Real )theView->Convert (aWinXY.y());

    const double THE_2PI = M_PI * 2.0;
    double aDX = (myGL.OrbitRotation.PointTo.x() - myGL.OrbitRotation.PointStart.x()) * M_PI / rx;
    double aDY = (myGL.OrbitRotation.PointStart.y() - myGL.OrbitRotation.PointTo.y()) * M_PI / ry;

    if     (aDX > 0.0) { while (aDX >  THE_2PI) { aDX -= THE_2PI; } }
    else if(aDX < 0.0) { while (aDX < -THE_2PI) { aDX += THE_2PI; } }
    if     (aDY > 0.0) { while (aDY >  THE_2PI) { aDY -= THE_2PI; } }
    else if(aDY < 0.0) { while (aDY < -THE_2PI) { aDY += THE_2PI; } }

    // rotate camera around 3 initial axes
    gp_Dir aCamDir (aCam->Direction().Reversed());
    gp_Dir aCamUp  (aCam->Up());
    gp_Dir aCamSide(aCamUp.Crossed (aCamDir));

    gp_Trsf aRot[2], aTrsf;
    aRot[0].SetRotation (gp_Ax1 (myRotatePnt3d, aCamUp),  -aDX);
    aRot[1].SetRotation (gp_Ax1 (myRotatePnt3d, aCamSide), aDY);
    aTrsf.Multiply (aRot[0]);
    aTrsf.Multiply (aRot[1]);

    aCam->Transform (aTrsf);
  }

  theView->Invalidate();
  theView->View()->SynchronizeXRBaseToPosedCamera();
}

// =======================================================================
// function : handleViewRotation
// purpose  :
// =======================================================================
void AIS_ViewController::handleViewRotation (const Handle(V3d_View)& theView,
                                             double theYawExtra,
                                             double thePitchExtra,
                                             double theRoll,
                                             bool theToRestartOnIncrement)
{
  if (!myToAllowRotation)
  {
    return;
  }

  const Handle(Graphic3d_Camera)& aCam = theView->Camera();
  const bool toRotateAnyway = Abs (theYawExtra)   > gp::Resolution()
                           || Abs (thePitchExtra) > gp::Resolution()
                           || Abs (theRoll - myRotateStartYawPitchRoll[2]) > gp::Resolution();
  if (toRotateAnyway
   && theToRestartOnIncrement)
  {
    myGL.ViewRotation.ToStart = true;
    myGL.ViewRotation.PointTo = myGL.ViewRotation.PointStart;
  }
  if (myGL.ViewRotation.ToStart)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTransformation (gp_Ax3 (gp::Origin(), aCam->OrthogonalizedUp(), aCam->Direction()),
                             gp_Ax3 (gp::Origin(), gp::DZ(), gp::DX()));
    const gp_Quaternion aRot = aTrsf.GetRotation();
    double aRollDummy = 0.0;
    aRot.GetEulerAngles (gp_YawPitchRoll, myRotateStartYawPitchRoll[0], myRotateStartYawPitchRoll[1], aRollDummy);
  }
  if (toRotateAnyway)
  {
    myRotateStartYawPitchRoll[0] += theYawExtra;
    myRotateStartYawPitchRoll[1] += thePitchExtra;
    myRotateStartYawPitchRoll[2]  = theRoll;
    myGL.ViewRotation.ToRotate = true;
  }

  if (!myGL.ViewRotation.ToRotate)
  {
    return;
  }

  AbortViewAnimation();

  Graphic3d_Vec2i aWinXY;
  theView->Window()->Size (aWinXY.x(), aWinXY.y());
  double aYawAngleDelta   =  ((myGL.ViewRotation.PointStart.x() - myGL.ViewRotation.PointTo.x()) / double (aWinXY.x())) * (M_PI * 0.5);
  double aPitchAngleDelta = -((myGL.ViewRotation.PointStart.y() - myGL.ViewRotation.PointTo.y()) / double (aWinXY.y())) * (M_PI * 0.5);
  const double aPitchAngleNew = Max (Min (myRotateStartYawPitchRoll[1] + aPitchAngleDelta, M_PI * 0.5 - M_PI / 180.0), -M_PI * 0.5 + M_PI / 180.0);
  const double aYawAngleNew   = myRotateStartYawPitchRoll[0] + aYawAngleDelta;
  gp_Quaternion aRot;
  aRot.SetEulerAngles (gp_YawPitchRoll, aYawAngleNew, aPitchAngleNew, theRoll);
  gp_Trsf aTrsfRot;
  aTrsfRot.SetRotation (aRot);

  const gp_Dir aNewUp  = gp::DZ().Transformed (aTrsfRot);
  const gp_Dir aNewDir = gp::DX().Transformed (aTrsfRot);
  aCam->SetUp (aNewUp);
  aCam->SetDirectionFromEye (aNewDir);
  aCam->OrthogonalizeUp();
  theView->Invalidate();
}

// =======================================================================
// function : PickPoint
// purpose  :
// =======================================================================
bool AIS_ViewController::PickPoint (gp_Pnt& thePnt,
                                    const Handle(AIS_InteractiveContext)& theCtx,
                                    const Handle(V3d_View)& theView,
                                    const Graphic3d_Vec2i& theCursor,
                                    bool theToStickToPickRay)
{
  ResetPreviousMoveTo();

  const Handle(StdSelect_ViewerSelector3d)& aSelector = theCtx->MainSelector();
  aSelector->Pick (theCursor.x(), theCursor.y(), theView);
  if (aSelector->NbPicked() < 1)
  {
    return false;
  }

  const SelectMgr_SortCriterion& aPicked = aSelector->PickedData (1);
  if (theToStickToPickRay
  && !Precision::IsInfinite (aPicked.Depth))
  {
    thePnt = aSelector->GetManager().DetectedPoint (aPicked.Depth);
  }
  else
  {
    thePnt = aSelector->PickedPoint (1);
  }
  return !Precision::IsInfinite (thePnt.X())
      && !Precision::IsInfinite (thePnt.Y())
      && !Precision::IsInfinite (thePnt.Z());
}

// =======================================================================
// function : GravityPoint
// purpose  :
// =======================================================================
gp_Pnt AIS_ViewController::GravityPoint (const Handle(AIS_InteractiveContext)& theCtx,
                                         const Handle(V3d_View)& theView)
{
  switch (myRotationMode)
  {
    case AIS_RotationMode_PickLast:
    case AIS_RotationMode_PickCenter:
    {
      Graphic3d_Vec2i aCursor ((int )myGL.OrbitRotation.PointStart.x(), (int )myGL.OrbitRotation.PointStart.y());
      if (myRotationMode == AIS_RotationMode_PickCenter)
      {
        Graphic3d_Vec2i aViewPort;
        theView->Window()->Size (aViewPort.x(), aViewPort.y());
        aCursor = aViewPort / 2;
      }

      gp_Pnt aPnt;
      if (PickPoint (aPnt, theCtx, theView, aCursor, myToStickToRayOnRotation))
      {
        return aPnt;
      }
      break;
    }
    case AIS_RotationMode_CameraAt:
    {
      const Handle(Graphic3d_Camera)& aCam = theView->Camera();
      return aCam->Center();
    }
    case AIS_RotationMode_BndBoxScene:
    {
      Bnd_Box aBndBox = theView->View()->MinMaxValues (false);
      if (!aBndBox.IsVoid())
      {
        return (aBndBox.CornerMin().XYZ() + aBndBox.CornerMax().XYZ()) * 0.5;
      }
      break;
    }
    case AIS_RotationMode_BndBoxActive:
      break;
  }

  return theCtx ->GravityPoint (theView);
}

// =======================================================================
// function : FitAllAuto
// purpose  :
// =======================================================================
void AIS_ViewController::FitAllAuto (const Handle(AIS_InteractiveContext)& theCtx,
                                     const Handle(V3d_View)& theView)
{
  const Bnd_Box aBoxSel = theCtx->BoundingBoxOfSelection();
  const double aFitMargin = 0.01;
  if (aBoxSel.IsVoid())
  {
    theView->FitAll (aFitMargin, false);
    return;
  }

  // fit all algorithm is not 100% stable - so compute some precision to compare equal camera values
  const double  aFitTol = (aBoxSel.CornerMax().XYZ() - aBoxSel.CornerMin().XYZ()).Modulus() * 0.000001;
  const Bnd_Box aBoxAll = theView->View()->MinMaxValues();

  const Handle(Graphic3d_Camera)& aCam = theView->Camera();
  Handle(Graphic3d_Camera) aCameraSel = new Graphic3d_Camera (aCam);
  Handle(Graphic3d_Camera) aCameraAll = new Graphic3d_Camera (aCam);
  theView->FitMinMax (aCameraSel, aBoxSel, aFitMargin);
  theView->FitMinMax (aCameraAll, aBoxAll, aFitMargin);
  if (aCameraSel->Center().IsEqual (aCam->Center(),     aFitTol)
   && Abs (aCameraSel->Scale()    - aCam->Scale())    < aFitTol
   && Abs (aCameraSel->Distance() - aCam->Distance()) < aFitTol)
  {
    // fit all entire view on second FitALL request
    aCam->Copy (aCameraAll);
  }
  else
  {
    aCam->Copy (aCameraSel);
  }
}

// =======================================================================
// function : handleViewOrientationKeys
// purpose  :
// =======================================================================
void AIS_ViewController::handleViewOrientationKeys (const Handle(AIS_InteractiveContext)& theCtx,
                                                    const Handle(V3d_View)& theView)
{
  if (myNavigationMode == AIS_NavigationMode_FirstPersonWalk)
  {
    return;
  }

  Handle(Graphic3d_Camera) aCameraBack;
  struct ViewKeyAction
  {
    Aspect_VKey Key;
    V3d_TypeOfOrientation Orientation;
  };
  static const ViewKeyAction THE_VIEW_KEYS[] =
  {
    { Aspect_VKey_ViewTop,          V3d_TypeOfOrientation_Zup_Top },
    { Aspect_VKey_ViewBottom,       V3d_TypeOfOrientation_Zup_Bottom },
    { Aspect_VKey_ViewLeft,         V3d_TypeOfOrientation_Zup_Left },
    { Aspect_VKey_ViewRight,        V3d_TypeOfOrientation_Zup_Right },
    { Aspect_VKey_ViewFront,        V3d_TypeOfOrientation_Zup_Front },
    { Aspect_VKey_ViewBack,         V3d_TypeOfOrientation_Zup_Back },
    { Aspect_VKey_ViewAxoLeftProj,  V3d_TypeOfOrientation_Zup_AxoLeft },
    { Aspect_VKey_ViewAxoRightProj, V3d_TypeOfOrientation_Zup_AxoRight },
    { Aspect_VKey_ViewRoll90CW,     (V3d_TypeOfOrientation )-1},
    { Aspect_VKey_ViewRoll90CCW,    (V3d_TypeOfOrientation )-1},
    { Aspect_VKey_ViewFitAll,       (V3d_TypeOfOrientation )-1}
  };
  {
    Standard_Mutex::Sentry aLock (myKeys.Mutex());
    const size_t aNbKeys = sizeof(THE_VIEW_KEYS) / sizeof(*THE_VIEW_KEYS);
    const double anEventTime = EventTime();
    for (size_t aKeyIter = 0; aKeyIter < aNbKeys; ++aKeyIter)
    {
      const ViewKeyAction& aKeyAction = THE_VIEW_KEYS[aKeyIter];
      if (!myKeys.IsKeyDown (aKeyAction.Key))
      {
        continue;
      }

      myKeys.KeyUp (aKeyAction.Key, anEventTime);
      if (aCameraBack.IsNull())
      {
        aCameraBack = theView->Camera();
        theView->SetCamera (new Graphic3d_Camera (aCameraBack));
      }
      if (aKeyAction.Orientation != (V3d_TypeOfOrientation )-1)
      {
        theView->SetProj (aKeyAction.Orientation);
        FitAllAuto (theCtx, theView);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewRoll90CW)
      {
        const double aTwist = theView->Twist() + M_PI / 2.0;
        theView->SetTwist (aTwist);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewRoll90CCW)
      {
        const double aTwist = theView->Twist() - M_PI / 2.0;
        theView->SetTwist (aTwist);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewFitAll)
      {
        FitAllAuto (theCtx, theView);
      }
    }
  }

  if (aCameraBack.IsNull())
  {
    return;
  }

  Handle(Graphic3d_Camera) aCameraNew = theView->Camera();
  theView->SetCamera (aCameraBack);
  const Graphic3d_Mat4d anOrientMat1 = aCameraBack->OrientationMatrix();
  const Graphic3d_Mat4d anOrientMat2 = aCameraNew ->OrientationMatrix();
  if (anOrientMat1 != anOrientMat2)
  {
    const Handle(AIS_AnimationCamera)& aCamAnim = myViewAnimation;
    aCamAnim->SetView (theView);
    aCamAnim->SetStartPts (0.0);
    aCamAnim->SetCameraStart (new Graphic3d_Camera (aCameraBack));
    aCamAnim->SetCameraEnd   (new Graphic3d_Camera (aCameraNew));
    aCamAnim->StartTimer (0.0, 1.0, true, false);
  }
}

// =======================================================================
// function : handleNavigationKeys
// purpose  :
// =======================================================================
AIS_WalkDelta AIS_ViewController::handleNavigationKeys (const Handle(AIS_InteractiveContext)& ,
                                                        const Handle(V3d_View)& theView)
{
  // navigation keys
  double aCrouchRatio = 1.0, aRunRatio = 1.0;
  if (myNavigationMode == AIS_NavigationMode_FirstPersonFlight)
  {
    aRunRatio = 3.0;
  }

  const double aRotSpeed = 0.5;
  const double aWalkSpeedCoef = WalkSpeedRelative();
  AIS_WalkDelta aWalk = FetchNavigationKeys (aCrouchRatio, aRunRatio);
  if (aWalk.IsJumping())
  {
    // ask more frames
    setAskNextFrame();
    theView->Invalidate();
  }
  if (aWalk.IsEmpty())
  {
    return aWalk;
  }
  else if (myGL.OrbitRotation.ToRotate
        || myGL.OrbitRotation.ToStart)
  {
    return aWalk;
  }

  gp_XYZ aMin, aMax;
  const Bnd_Box aBndBox = theView->View()->MinMaxValues();
  if (!aBndBox.IsVoid())
  {
    aMin = aBndBox.CornerMin().XYZ();
    aMax = aBndBox.CornerMax().XYZ();
  }
  double aBndDiam = Max (Max (aMax.X() - aMin.X(), aMax.Y() - aMin.Y()), aMax.Z() - aMin.Z());
  if (aBndDiam <= gp::Resolution())
  {
    aBndDiam = 0.001;
  }

  const double aWalkSpeed = myNavigationMode != AIS_NavigationMode_Orbit
                         && myNavigationMode != AIS_NavigationMode_FirstPersonFlight
                          ? theView->View()->UnitFactor() * WalkSpeedAbsolute()
                          : aWalkSpeedCoef * aBndDiam;
  const Handle(Graphic3d_Camera)& aCam = theView->View()->IsActiveXR()
                                       ? theView->View()->BaseXRCamera()
                                       : theView->Camera();

  // move forward in plane XY and up along Z
  const gp_Dir anUp = ToLockOrbitZUp() ? gp::DZ() : aCam->OrthogonalizedUp();
  if (aWalk.ToMove()
   && myToAllowPanning)
  {
    const gp_Vec aSide = -aCam->SideRight();
    gp_XYZ aFwd = aCam->Direction().XYZ();
    aFwd -= anUp.XYZ() * (anUp.XYZ() * aFwd);

    gp_XYZ aMoveVec;
    if (!aWalk[AIS_WalkTranslation_Forward].IsEmpty())
    {
      if (!aCam->IsOrthographic())
      {
        aMoveVec += aFwd * aWalk[AIS_WalkTranslation_Forward].Value * aWalk[AIS_WalkTranslation_Forward].Pressure * aWalkSpeed;
      }
    }
    if (!aWalk[AIS_WalkTranslation_Side].IsEmpty())
    {
      aMoveVec += aSide.XYZ() * aWalk[AIS_WalkTranslation_Side].Value * aWalk[AIS_WalkTranslation_Side].Pressure * aWalkSpeed;
    }
    if (!aWalk[AIS_WalkTranslation_Up].IsEmpty())
    {
      aMoveVec += anUp.XYZ() * aWalk[AIS_WalkTranslation_Up].Value * aWalk[AIS_WalkTranslation_Up].Pressure * aWalkSpeed;
    }
    {
      if (aCam->IsOrthographic())
      {
        if (!aWalk[AIS_WalkTranslation_Forward].IsEmpty())
        {
          const double aZoomDelta = aWalk[AIS_WalkTranslation_Forward].Value * aWalk[AIS_WalkTranslation_Forward].Pressure * aWalkSpeedCoef;
          handleZoom (theView, Aspect_ScrollDelta (aZoomDelta * 100.0), NULL);
        }
      }

      gp_Trsf aTrsfTranslate;
      aTrsfTranslate.SetTranslation (aMoveVec);
      aCam->Transform (aTrsfTranslate);
    }
  }

  if (myNavigationMode == AIS_NavigationMode_Orbit
   && myToAllowRotation)
  {
    if (!aWalk[AIS_WalkRotation_Yaw].IsEmpty())
    {
      gp_Trsf aTrsfRot;
      aTrsfRot.SetRotation (gp_Ax1 (aCam->Eye(), anUp), aWalk[AIS_WalkRotation_Yaw].Value * aRotSpeed);
      aCam->Transform (aTrsfRot);
    }
    if (!aWalk[AIS_WalkRotation_Pitch].IsEmpty())
    {
      const gp_Vec aSide = -aCam->SideRight();
      gp_Trsf aTrsfRot;
      aTrsfRot.SetRotation (gp_Ax1 (aCam->Eye(), aSide), -aWalk[AIS_WalkRotation_Pitch].Value * aRotSpeed);
      aCam->Transform (aTrsfRot);
    }
    if (!aWalk[AIS_WalkRotation_Roll].IsEmpty()
     && !ToLockOrbitZUp())
    {
      gp_Trsf aTrsfRot;
      aTrsfRot.SetRotation (gp_Ax1 (aCam->Center(), aCam->Direction()), aWalk[AIS_WalkRotation_Roll].Value * aRotSpeed);
      aCam->Transform (aTrsfRot);
    }
  }

  // ask more frames
  setAskNextFrame();
  theView->Invalidate();
  theView->View()->SynchronizeXRBaseToPosedCamera();
  return aWalk;
}

// =======================================================================
// function : handleCameraActions
// purpose  :
// =======================================================================
void AIS_ViewController::handleCameraActions (const Handle(AIS_InteractiveContext)& theCtx,
                                              const Handle(V3d_View)& theView,
                                              const AIS_WalkDelta& theWalk)
{
  // apply view actions
  if (myGL.Orientation.ToSetViewOrient)
  {
    theView->SetProj (myGL.Orientation.ViewOrient);
    myGL.Orientation.ToFitAll = true;
  }

  // apply fit all
  if (myGL.Orientation.ToFitAll)
  {
    const double aFitMargin = 0.01;
    theView->FitAll (aFitMargin, false);
    theView->Invalidate();
    myGL.Orientation.ToFitAll = false;
  }

  if (myGL.IsNewGesture)
  {
    if (myAnchorPointPrs1->HasInteractiveContext())
    {
      theCtx->Remove (myAnchorPointPrs1, false);
      if (!theView->Viewer()->ZLayerSettings (myAnchorPointPrs1->ZLayer()).IsImmediate())
      {
        theView->Invalidate();
      }
      else
      {
        theView->InvalidateImmediate();
      }
    }
    if (myAnchorPointPrs2->HasInteractiveContext())
    {
      theCtx->Remove (myAnchorPointPrs2, false);
      if (!theView->Viewer()->ZLayerSettings (myAnchorPointPrs2->ZLayer()).IsImmediate())
      {
        theView->Invalidate();
      }
      else
      {
        theView->InvalidateImmediate();
      }
    }

    if (myHasHlrOnBeforeRotation)
    {
      myHasHlrOnBeforeRotation = false;
      theView->SetComputedMode (true);
      theView->Invalidate();
    }
  }

  if (myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    if (myGL.Panning.ToStart
     && myToAllowPanning)
    {
      gp_Pnt aPanPnt (Precision::Infinite(), 0.0, 0.0);
      if (!theView->Camera()->IsOrthographic())
      {
        bool toStickToRay = false;
        if (myGL.Panning.PointStart.x() >= 0
         && myGL.Panning.PointStart.y() >= 0)
        {
          PickPoint (aPanPnt, theCtx, theView, myGL.Panning.PointStart, toStickToRay);
        }
        if (Precision::IsInfinite (aPanPnt.X()))
        {
          Graphic3d_Vec2i aWinSize;
          theView->Window()->Size (aWinSize.x(), aWinSize.y());
          PickPoint (aPanPnt, theCtx, theView, aWinSize / 2, toStickToRay);
        }
        if (!Precision::IsInfinite (aPanPnt.X())
          && myToShowPanAnchorPoint)
        {
          gp_Trsf aPntTrsf;
          aPntTrsf.SetTranslation (gp_Vec (aPanPnt.XYZ()));
          theCtx->SetLocation (myAnchorPointPrs2, aPntTrsf);
        }
      }
      setPanningAnchorPoint (aPanPnt);
    }

    if (myToShowPanAnchorPoint
    &&  hasPanningAnchorPoint()
    &&  myGL.Panning.ToPan
    && !myGL.IsNewGesture
    && !myAnchorPointPrs2->HasInteractiveContext())
    {
      theCtx->Display (myAnchorPointPrs2, 0, -1, false, AIS_DS_Displayed);
    }

    handlePanning (theView);
    handleZRotate (theView);
  }

  if ((myNavigationMode == AIS_NavigationMode_Orbit
    || myGL.OrbitRotation.ToStart
    || myGL.OrbitRotation.ToRotate)
   && myToAllowRotation)
  {
    if (myGL.OrbitRotation.ToStart
    && !myHasHlrOnBeforeRotation)
    {
      myHasHlrOnBeforeRotation = theView->ComputedMode();
      if (myHasHlrOnBeforeRotation)
      {
        theView->SetComputedMode (false);
      }
    }

    gp_Pnt aGravPnt;
    if (myGL.OrbitRotation.ToStart)
    {
      aGravPnt = GravityPoint (theCtx, theView);
      if (myToShowRotateCenter)
      {
        gp_Trsf aPntTrsf;
        aPntTrsf.SetTranslation (gp_Vec (aGravPnt.XYZ()));
        theCtx->SetLocation (myAnchorPointPrs1, aPntTrsf);
        theCtx->SetLocation (myAnchorPointPrs2, aPntTrsf);
      }
    }

    if (myToShowRotateCenter
    &&  myGL.OrbitRotation.ToRotate
    && !myGL.IsNewGesture
    && !myAnchorPointPrs1->HasInteractiveContext())
    {
      theCtx->Display (myAnchorPointPrs1, 0, -1, false, AIS_DS_Displayed);
      theCtx->Display (myAnchorPointPrs2, 0, -1, false, AIS_DS_Displayed);
    }
    handleOrbitRotation (theView, aGravPnt,
                         myToLockOrbitZUp || myNavigationMode != AIS_NavigationMode_Orbit);
  }

  if ((myNavigationMode != AIS_NavigationMode_Orbit
    || myGL.ViewRotation.ToStart
    || myGL.ViewRotation.ToRotate)
   && myToAllowRotation)
  {
    if (myGL.ViewRotation.ToStart
    && !myHasHlrOnBeforeRotation)
    {
      myHasHlrOnBeforeRotation = theView->ComputedMode();
      if (myHasHlrOnBeforeRotation)
      {
        theView->SetComputedMode (false);
      }
    }

    double aRoll = 0.0;
    if (!theWalk[AIS_WalkRotation_Roll].IsEmpty()
     && !myToLockOrbitZUp)
    {
      aRoll = (M_PI / 12.0) * theWalk[AIS_WalkRotation_Roll].Pressure;
      aRoll *= Min (1000.0  * theWalk[AIS_WalkRotation_Roll].Duration, 100.0) / 100.0;
      if (theWalk[AIS_WalkRotation_Roll].Value < 0.0)
      {
        aRoll = -aRoll;
      }
    }

    handleViewRotation (theView, theWalk[AIS_WalkRotation_Yaw].Value, theWalk[AIS_WalkRotation_Pitch].Value, aRoll,
                        myNavigationMode == AIS_NavigationMode_FirstPersonFlight);
  }

  if (!myGL.ZoomActions.IsEmpty())
  {
    for (NCollection_Sequence<Aspect_ScrollDelta>::Iterator aZoomIter (myGL.ZoomActions); aZoomIter.More(); aZoomIter.Next())
    {
      Aspect_ScrollDelta aZoomParams = aZoomIter.Value();
      if (myToAllowZFocus
       && (aZoomParams.Flags & Aspect_VKeyFlags_CTRL) != 0
       && theView->Camera()->IsStereo())
      {
        handleZFocusScroll (theView, aZoomParams);
        continue;
      }

      if (!myToAllowZooming)
      {
        continue;
      }

      if (!theView->Camera()->IsOrthographic())
      {
        gp_Pnt aPnt;
        if (aZoomParams.HasPoint()
         && PickPoint (aPnt, theCtx, theView, aZoomParams.Point, myToStickToRayOnZoom))
        {
          handleZoom (theView, aZoomParams, &aPnt);
          continue;
        }

        Graphic3d_Vec2i aWinSize;
        theView->Window()->Size (aWinSize.x(), aWinSize.y());
        if (PickPoint (aPnt, theCtx, theView, aWinSize / 2, myToStickToRayOnZoom))
        {
          aZoomParams.ResetPoint(); // do not pretend to zoom at 'nothing'
          handleZoom (theView, aZoomParams, &aPnt);
          continue;
        }
      }
      handleZoom (theView, aZoomParams, NULL);
    }
    myGL.ZoomActions.Clear();
  }
}

// =======================================================================
// function : handleXRInput
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRInput (const Handle(AIS_InteractiveContext)& theCtx,
                                        const Handle(V3d_View)& theView,
                                        const AIS_WalkDelta& )
{
  theView->View()->ProcessXRInput();
  if (!theView->View()->IsActiveXR())
  {
    return;
  }
  if (myXRCameraTmp.IsNull())
  {
    myXRCameraTmp = new Graphic3d_Camera();
  }
  handleXRTurnPad (theCtx, theView);
  handleXRTeleport(theCtx, theView);
  handleXRPicking (theCtx, theView);
}

// =======================================================================
// function : handleXRTurnPad
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRTurnPad (const Handle(AIS_InteractiveContext)& ,
                                          const Handle(V3d_View)& theView)
{
  if (myXRTurnAngle <= 0.0
  || !theView->View()->IsActiveXR())
  {
    return;
  }

  // turn left/right at 45 degrees on left/right trackpad clicks
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole = aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const Handle(Aspect_XRAction)& aPadClickAct = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTrackPadClick);
    const Handle(Aspect_XRAction)& aPadPosAct   = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTrackPadPosition);
    if (aPadClickAct.IsNull()
    ||  aPadPosAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aPadClick = theView->View()->XRSession()->GetDigitalActionData (aPadClickAct);
    const Aspect_XRAnalogActionData  aPadPos   = theView->View()->XRSession()->GetAnalogActionData (aPadPosAct);
    if (aPadClick.IsActive
     && aPadClick.IsPressed
     && aPadClick.IsChanged
     && aPadPos.IsActive
     && Abs (aPadPos.VecXYZ.y()) < 0.5f
     && Abs (aPadPos.VecXYZ.x()) > 0.7f)
    {
      gp_Trsf aTrsfTurn;
      aTrsfTurn.SetRotation (gp_Ax1 (gp::Origin(), theView->View()->BaseXRCamera()->Up()), aPadPos.VecXYZ.x() < 0.0f ? myXRTurnAngle : -myXRTurnAngle);
      theView->View()->TurnViewXRCamera (aTrsfTurn);
      break;
    }
  }
}

// =======================================================================
// function : handleXRTeleport
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRTeleport (const Handle(AIS_InteractiveContext)& theCtx,
                                           const Handle(V3d_View)& theView)
{
  if (!theView->View()->IsActiveXR())
  {
    return;
  }

  // teleport on forward trackpad unclicks
  const Aspect_XRTrackedDeviceRole aTeleOld = myXRLastTeleportHand;
  myXRLastTeleportHand = Aspect_XRTrackedDeviceRole_Other;
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole = aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const Standard_Integer aDeviceId = theView->View()->XRSession()->NamedTrackedDevice (aRole);
    if (aDeviceId == -1)
    {
      continue;
    }

    const Handle(Aspect_XRAction)& aPadClickAct = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTrackPadClick);
    const Handle(Aspect_XRAction)& aPadPosAct   = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTrackPadPosition);
    if (aPadClickAct.IsNull()
    ||  aPadPosAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aPadClick = theView->View()->XRSession()->GetDigitalActionData (aPadClickAct);
    const Aspect_XRAnalogActionData  aPadPos   = theView->View()->XRSession()->GetAnalogActionData (aPadPosAct);
    const bool isPressed =  aPadClick.IsPressed;
    const bool isClicked = !aPadClick.IsPressed
                        &&  aPadClick.IsChanged;
    if (aPadClick.IsActive
     && (isPressed || isClicked)
     && aPadPos.IsActive
     && aPadPos.VecXYZ.y() > 0.6f
     && Abs (aPadPos.VecXYZ.x()) < 0.5f)
    {
      const Aspect_TrackedDevicePose& aPose = theView->View()->XRSession()->TrackedPoses()[aDeviceId];
      if (!aPose.IsValidPose)
      {
        continue;
      }

      myXRLastTeleportHand = aRole;
      Standard_Real& aPickDepth = aRole == Aspect_XRTrackedDeviceRole_LeftHand ? myXRLastPickDepthLeft : myXRLastPickDepthRight;
      aPickDepth = Precision::Infinite();
      Graphic3d_Vec3 aPickNorm;
      const gp_Trsf aHandBase = theView->View()->PoseXRToWorld (aPose.Orientation);
      const Standard_Real aHeadHeight = theView->View()->XRSession()->HeadPose().TranslationPart().Y();
      {
        const Standard_Integer aPickedId = handleXRMoveTo (theCtx, theView, aPose.Orientation, false);
        if (aPickedId >= 1)
        {
          const SelectMgr_SortCriterion& aPickedData = theCtx->MainSelector()->PickedData (aPickedId);
          aPickNorm = aPickedData.Normal;
          if (aPickNorm.SquareModulus() > ShortRealEpsilon())
          {
            aPickDepth = aPickedData.Point.Distance (aHandBase.TranslationPart());
          }
        }
      }
      if (isClicked)
      {
        myXRLastTeleportHand = Aspect_XRTrackedDeviceRole_Other;
        if (!Precision::IsInfinite (aPickDepth))
        {
          const gp_Dir aTeleDir = -gp::DZ().Transformed (aHandBase);
          const gp_Dir anUpDir  = theView->View()->BaseXRCamera()->Up();

          bool isHorizontal = false;
          gp_Dir aPickNormDir (aPickNorm.x(), aPickNorm.y(), aPickNorm.z());
          if (anUpDir.IsEqual ( aPickNormDir, M_PI_4)
           || anUpDir.IsEqual (-aPickNormDir, M_PI_4))
          {
            isHorizontal = true;
          }

          gp_Pnt aNewEye = aHandBase.TranslationPart();
          if (isHorizontal)
          {
            aNewEye  = aHandBase.TranslationPart()
                     + aTeleDir.XYZ() * aPickDepth
                     + anUpDir.XYZ() * aHeadHeight;
          }
          else
          {
            if (aPickNormDir.Dot (aTeleDir) < 0.0)
            {
              aPickNormDir.Reverse();
            }
            aNewEye  = aHandBase.TranslationPart()
                     + aTeleDir.XYZ() * aPickDepth
                     - aPickNormDir.XYZ() * aHeadHeight / 4;
          }

          theView->View()->PosedXRCamera()->MoveEyeTo (aNewEye);
          theView->View()->ComputeXRBaseCameraFromPosed (theView->View()->PosedXRCamera(), theView->View()->XRSession()->HeadPose());
        }
      }
      break;
    }
  }

  if (myXRLastTeleportHand != aTeleOld)
  {
    if (aTeleOld != Aspect_XRTrackedDeviceRole_Other)
    {
      if (const Handle(Aspect_XRAction)& aHaptic = theView->View()->XRSession()->GenericAction (aTeleOld, Aspect_XRGenericAction_OutputHaptic))
      {
        theView->View()->XRSession()->AbortHapticVibrationAction (aHaptic);
      }
    }
    if (myXRLastTeleportHand != Aspect_XRTrackedDeviceRole_Other)
    {
      if (const Handle(Aspect_XRAction)& aHaptic = theView->View()->XRSession()->GenericAction (myXRLastTeleportHand, Aspect_XRGenericAction_OutputHaptic))
      {
        theView->View()->XRSession()->TriggerHapticVibrationAction (aHaptic, myXRTeleportHaptic);
      }
    }
  }
}

// =======================================================================
// function : handleXRPicking
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRPicking (const Handle(AIS_InteractiveContext)& theCtx,
                                          const Handle(V3d_View)& theView)
{
  if (!theView->View()->IsActiveXR())
  {
    return;
  }

  // handle selection on trigger clicks
  Aspect_XRTrackedDeviceRole aPickDevOld = myXRLastPickingHand;
  myXRLastPickingHand = Aspect_XRTrackedDeviceRole_Other;
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole = aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const Handle(Aspect_XRAction)& aTrigClickAct = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTriggerClick);
    const Handle(Aspect_XRAction)& aTrigPullAct  = theView->View()->XRSession()->GenericAction (aRole, Aspect_XRGenericAction_InputTriggerPull);
    if (aTrigClickAct.IsNull()
    ||  aTrigPullAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aTrigClick = theView->View()->XRSession()->GetDigitalActionData (aTrigClickAct);
    const Aspect_XRAnalogActionData  aTrigPos   = theView->View()->XRSession()->GetAnalogActionData (aTrigPullAct);
    if (aTrigPos.IsActive
     && Abs (aTrigPos.VecXYZ.x()) > 0.1f)
    {
      myXRLastPickingHand = aRole;
      handleXRHighlight (theCtx, theView);
      if (aTrigClick.IsActive
       && aTrigClick.IsPressed
       && aTrigClick.IsChanged)
      {
        theCtx->Select (false);
        OnSelectionChanged (theCtx, theView);
        if (const Handle(Aspect_XRAction)& aHaptic = theView->View()->XRSession()->GenericAction (myXRLastPickingHand, Aspect_XRGenericAction_OutputHaptic))
        {
          theView->View()->XRSession()->TriggerHapticVibrationAction (aHaptic, myXRSelectHaptic);
        }
      }
      break;
    }
  }
  if (myXRLastPickingHand != aPickDevOld)
  {
    theCtx->ClearDetected();
  }
}

// =======================================================================
// function : OnSelectionChanged
// purpose  :
// =======================================================================
void AIS_ViewController::OnSelectionChanged (const Handle(AIS_InteractiveContext)& ,
                                             const Handle(V3d_View)& )
{
  //
}

// =======================================================================
// function : OnObjectDragged
// purpose  :
// =======================================================================
void AIS_ViewController::OnObjectDragged (const Handle(AIS_InteractiveContext)& theCtx,
                                          const Handle(V3d_View)& theView,
                                          AIS_DragAction theAction)
{
  switch (theAction)
  {
    case AIS_DragAction_Start:
    {
      myDragObject.Nullify();
      myDragOwner.Nullify();
      if (!theCtx->HasDetected())
      {
        return;
      }

      const Handle(SelectMgr_EntityOwner)& aDetectedOwner = theCtx->DetectedOwner();
      Handle(AIS_InteractiveObject) aDetectedPrs = Handle(AIS_InteractiveObject)::DownCast (aDetectedOwner->Selectable());

      if (aDetectedPrs->ProcessDragging (theCtx, theView, aDetectedOwner, myGL.Dragging.PointStart,
                                         myGL.Dragging.PointTo, theAction))
      {
        myDragObject = aDetectedPrs;
        myDragOwner = aDetectedOwner;
      }
      return;
    }
    case AIS_DragAction_Update:
    {
      if (myDragObject.IsNull())
      {
        return;
      }

      if (Handle(SelectMgr_EntityOwner) aGlobOwner = myDragObject->GlobalSelOwner())
      {
        theCtx->SetSelectedState (aGlobOwner, true);
      }

      myDragObject->ProcessDragging (theCtx, theView, myDragOwner, myGL.Dragging.PointStart,
                                     myGL.Dragging.PointTo, theAction);
      theView->Invalidate();
      return;
    }
    case AIS_DragAction_Abort:
    {
      if (myDragObject.IsNull())
      {
        return;
      }

      myGL.Dragging.PointTo = myGL.Dragging.PointStart;
      OnObjectDragged (theCtx, theView, AIS_DragAction_Update);

      myDragObject->ProcessDragging (theCtx, theView, myDragOwner, myGL.Dragging.PointStart,
                                     myGL.Dragging.PointTo, theAction);
      Standard_FALLTHROUGH
    }
    case AIS_DragAction_Stop:
    {
      if (myDragObject.IsNull())
      {
        return;
      }

      if (Handle(SelectMgr_EntityOwner) aGlobOwner = myDragObject->GlobalSelOwner())
      {
        theCtx->SetSelectedState (aGlobOwner, false);
      }

      myDragObject->ProcessDragging (theCtx, theView, myDragOwner, myGL.Dragging.PointStart,
                                     myGL.Dragging.PointTo, theAction);
      theView->Invalidate();
      myDragObject.Nullify();
      myDragOwner.Nullify();
      return;
    }
  }
}

// =======================================================================
// function : contextLazyMoveTo
// purpose  :
// =======================================================================
void AIS_ViewController::contextLazyMoveTo (const Handle(AIS_InteractiveContext)& theCtx,
                                            const Handle(V3d_View)& theView,
                                            const Graphic3d_Vec2i& thePnt)
{
  if (myPrevMoveTo == thePnt
   || myHasHlrOnBeforeRotation) // ignore highlighting in-between rotation of HLR view
  {
    return;
  }

  myPrevMoveTo = thePnt;

  Handle(SelectMgr_EntityOwner) aLastPicked = theCtx->DetectedOwner();

  // Picking relies on the camera frustum (including Z-range) - so make temporary AutoZFit()
  // and then restore previous frustum to avoid immediate layer rendering issues if View has not been invalidated.
  const Standard_Real aZNear = theView->Camera()->ZNear(), aZFar = theView->Camera()->ZFar();
  theView->AutoZFit();
  theCtx->MoveTo (thePnt.x(), thePnt.y(), theView, false);
  theView->Camera()->SetZRange (aZNear, aZFar);

  Handle(SelectMgr_EntityOwner) aNewPicked = theCtx->DetectedOwner();

  if (theView->Viewer()->Grid()->IsActive()
   && theView->Viewer()->GridEcho())
  {
    if (aNewPicked.IsNull())
    {
      Graphic3d_Vec3d aPnt3d;
      theView->ConvertToGrid (thePnt.x(), thePnt.y(), aPnt3d[0], aPnt3d[1], aPnt3d[2]);
      theView->Viewer()->ShowGridEcho (theView, Graphic3d_Vertex (aPnt3d[0], aPnt3d[1], aPnt3d[2]));
      theView->InvalidateImmediate();
    }
    else
    {
      theView->Viewer()->HideGridEcho (theView);
      theView->InvalidateImmediate();
    }
  }

  if (aLastPicked != aNewPicked
   || (!aNewPicked.IsNull() && aNewPicked->IsForcedHilight()))
  {
    // dynamic highlight affects all Views
    for (V3d_ListOfViewIterator aViewIter (theView->Viewer()->ActiveViewIterator()); aViewIter.More(); aViewIter.Next())
    {
      const Handle(V3d_View)& aView = aViewIter.Value();
      aView->InvalidateImmediate();
    }
  }
}

// =======================================================================
// function : handleSelectionPick
// purpose  :
// =======================================================================
void AIS_ViewController::handleSelectionPick (const Handle(AIS_InteractiveContext)& theCtx,
                                              const Handle(V3d_View)& theView)
{
  if (myGL.Selection.Tool == AIS_ViewSelectionTool_Picking
  && !myGL.Selection.Points.IsEmpty())
  {
    for (NCollection_Sequence<Graphic3d_Vec2i>::Iterator aPntIter (myGL.Selection.Points); aPntIter.More(); aPntIter.Next())
    {
      const bool hadPrevMoveTo = HasPreviousMoveTo();
      contextLazyMoveTo (theCtx, theView, aPntIter.Value());
      if (!hadPrevMoveTo)
      {
        ResetPreviousMoveTo();
      }

      if (myGL.Selection.IsXOR)
      {
        theCtx->ShiftSelect (false);
      }
      else
      {
        theCtx->Select (false);
      }

      // selection affects all Views
      theView->Viewer()->Invalidate();

      OnSelectionChanged (theCtx, theView);
    }

    myGL.Selection.Points.Clear();
  }
}

// =======================================================================
// function : handleSelectionPoly
// purpose  :
// =======================================================================
void AIS_ViewController::handleSelectionPoly (const Handle(AIS_InteractiveContext)& theCtx,
                                              const Handle(V3d_View)& theView)
{
  // rubber-band & window polygon selection
  if (myGL.Selection.Tool == AIS_ViewSelectionTool_RubberBand
   || myGL.Selection.Tool == AIS_ViewSelectionTool_Polygon
   || myGL.Selection.Tool == AIS_ViewSelectionTool_ZoomWindow)
  {
    if (!myGL.Selection.Points.IsEmpty())
    {
      myRubberBand->ClearPoints();
      myRubberBand->SetToUpdate();

      const bool anIsRubber = myGL.Selection.Tool == AIS_ViewSelectionTool_RubberBand
                           || myGL.Selection.Tool == AIS_ViewSelectionTool_ZoomWindow;
      if (anIsRubber)
      {
        myRubberBand->SetRectangle (myGL.Selection.Points.First().x(), -myGL.Selection.Points.First().y(),
                                    myGL.Selection.Points.Last().x(),  -myGL.Selection.Points.Last().y());
      }
      else
      {
        Graphic3d_Vec2i aPrev (IntegerLast(), IntegerLast());
        for (NCollection_Sequence<Graphic3d_Vec2i>::Iterator aSelIter (myGL.Selection.Points); aSelIter.More(); aSelIter.Next())
        {
          Graphic3d_Vec2i aPntNew = Graphic3d_Vec2i (aSelIter.Value().x(), -aSelIter.Value().y());
          if (aPntNew != aPrev)
          {
            aPrev = aPntNew;
            myRubberBand->AddPoint (Graphic3d_Vec2i (aSelIter.Value().x(), -aSelIter.Value().y()));
          }
        }
      }

      myRubberBand->SetPolygonClosed (anIsRubber);
      try
      {
        theCtx->Display (myRubberBand, 0, -1, false, AIS_DS_Displayed);
      }
      catch (const Standard_Failure& theEx)
      {
        Message::SendWarning (TCollection_AsciiString ("Internal error while displaying rubber-band: ")
                            + theEx.DynamicType()->Name() + ", " + theEx.GetMessageString());
        myRubberBand->ClearPoints();
      }
      if (!theView->Viewer()->ZLayerSettings (myRubberBand->ZLayer()).IsImmediate())
      {
        theView->Invalidate();
      }
      else
      {
        theView->InvalidateImmediate();
      }
    }
    else if (!myRubberBand.IsNull()
           && myRubberBand->HasInteractiveContext())
    {
      theCtx->Remove (myRubberBand, false);
      myRubberBand->ClearPoints();
    }
  }

  if (myGL.Selection.ToApplyTool)
  {
    myGL.Selection.ToApplyTool = false;
    if (theCtx->IsDisplayed (myRubberBand))
    {
      theCtx->Remove (myRubberBand, false);
      {
        const NCollection_Sequence<Graphic3d_Vec2i>& aPoints = myRubberBand->Points();
        if (aPoints.Size() == 4
         && aPoints.Value (1).x() == aPoints.Value (2).x()
         && aPoints.Value (3).x() == aPoints.Value (4).x()
         && aPoints.Value (1).y() == aPoints.Value (4).y()
         && aPoints.Value (2).y() == aPoints.Value (3).y())
        {
          const Graphic3d_Vec2i aPnt1 (aPoints.Value (1).x(), -aPoints.Value (1).y());
          const Graphic3d_Vec2i aPnt2 (aPoints.Value (3).x(), -aPoints.Value (3).y());
          if (myGL.Selection.Tool == AIS_ViewSelectionTool_ZoomWindow)
          {
            theView->WindowFitAll (aPnt1.x(), aPnt1.y(), aPnt2.x(), aPnt2.y());
            theView->Invalidate();
          }
          else
          {
            theCtx->MainSelector()->AllowOverlapDetection (aPnt1.y() != Min (aPnt1.y(), aPnt2.y()));
            if (myGL.Selection.IsXOR)
            {
              theCtx->ShiftSelect (Min (aPnt1.x(), aPnt2.x()), Min (aPnt1.y(), aPnt2.y()),
                                   Max (aPnt1.x(), aPnt2.x()), Max (aPnt1.y(), aPnt2.y()),
                                   theView, false);
            }
            else
            {
              theCtx->Select (Min (aPnt1.x(), aPnt2.x()), Min (aPnt1.y(), aPnt2.y()),
                              Max (aPnt1.x(), aPnt2.x()), Max (aPnt1.y(), aPnt2.y()),
                              theView, false);
            }
            theCtx->MainSelector()->AllowOverlapDetection (false);
          }
        }
        else if (aPoints.Length() >= 3)
        {
          TColgp_Array1OfPnt2d aPolyline (1, aPoints.Length());
          TColgp_Array1OfPnt2d::Iterator aPolyIter (aPolyline);
          for (NCollection_Sequence<Graphic3d_Vec2i>::Iterator aSelIter (aPoints);
               aSelIter.More(); aSelIter.Next(), aPolyIter.Next())
          {
            const Graphic3d_Vec2i& aNewPnt = aSelIter.Value();
            aPolyIter.ChangeValue() = gp_Pnt2d (aNewPnt.x(), -aNewPnt.y());
          }

          if (myGL.Selection.IsXOR)
          {
            theCtx->ShiftSelect (aPolyline, theView, false);
          }
          else
          {
            theCtx->Select (aPolyline, theView, false);
          }
          theCtx->MainSelector()->AllowOverlapDetection (false);
        }
      }

      myRubberBand->ClearPoints();
      if (myGL.Selection.Tool != AIS_ViewSelectionTool_ZoomWindow)
      {
        // selection affects all Views
        theView->Viewer()->Invalidate();
        OnSelectionChanged (theCtx, theView);
      }
    }
  }
}

// =======================================================================
// function : handleDynamicHighlight
// purpose  :
// =======================================================================
void AIS_ViewController::handleDynamicHighlight (const Handle(AIS_InteractiveContext)& theCtx,
                                                 const Handle(V3d_View)& theView)
{
  if ((myGL.MoveTo.ToHilight || myGL.Dragging.ToStart)
   && myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    const Graphic3d_Vec2i& aMoveToPnt = myGL.MoveTo.ToHilight ? myGL.MoveTo.Point : myGL.Dragging.PointStart;
    if (myGL.Dragging.ToStart && (!myGL.MoveTo.ToHilight || !myToAllowHighlight)
     && !HasPreviousMoveTo())
    {
      contextLazyMoveTo (theCtx, theView, aMoveToPnt);
      ResetPreviousMoveTo();
      OnObjectDragged (theCtx, theView, AIS_DragAction_Start);
      theCtx->ClearDetected();
    }
    else if (myToAllowHighlight)
    {
      if (myPrevMoveTo != aMoveToPnt
       || (!theView->View()->IsActiveXR()
        && (myGL.OrbitRotation.ToRotate
         || myGL.ViewRotation.ToRotate
         || theView->IsInvalidated())))
      {
        ResetPreviousMoveTo();
        contextLazyMoveTo (theCtx, theView, aMoveToPnt);
      }
      if (myGL.Dragging.ToStart)
      {
        OnObjectDragged (theCtx, theView, AIS_DragAction_Start);
      }
    }

    myGL.MoveTo.ToHilight = false;
  }

  if (!myDragObject.IsNull())
  {
    if (myGL.Dragging.ToAbort)
    {
      OnObjectDragged (theCtx, theView, AIS_DragAction_Abort);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation .ToRotate = false;
    }
    else if (myGL.Dragging.ToStop)
    {
      OnObjectDragged (theCtx, theView, AIS_DragAction_Stop);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation .ToRotate = false;
    }
    else if (myGL.OrbitRotation.ToRotate
          || myGL.ViewRotation.ToRotate)
    {
      OnObjectDragged (theCtx, theView, AIS_DragAction_Update);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation .ToRotate = false;
    }
  }
}

// =======================================================================
// function : handleMoveTo
// purpose  :
// =======================================================================
void AIS_ViewController::handleMoveTo (const Handle(AIS_InteractiveContext)& theCtx,
                                       const Handle(V3d_View)& theView)
{
  handleSelectionPick   (theCtx, theView);
  handleDynamicHighlight(theCtx, theView);
  handleSelectionPoly   (theCtx, theView);
}

// =======================================================================
// function : handleViewRedraw
// purpose  :
// =======================================================================
void AIS_ViewController::handleViewRedraw (const Handle(AIS_InteractiveContext)& ,
                                           const Handle(V3d_View)& theView)
{
  // manage animation state
  if (!myViewAnimation.IsNull()
   && !myViewAnimation->IsStopped())
  {
    myViewAnimation->UpdateTimer();
    ResetPreviousMoveTo();
    setAskNextFrame();
  }

  if (theView->View()->IsActiveXR())
  {
    // VR requires continuous rendering
    myToAskNextFrame = true;
  }

  for (V3d_ListOfViewIterator aViewIter (theView->Viewer()->ActiveViewIterator()); aViewIter.More(); aViewIter.Next())
  {
    const Handle(V3d_View)& aView = aViewIter.Value();
    if (aView->IsInvalidated()
     || (myToAskNextFrame && aView == theView))
    {
      if (aView->ComputedMode())
      {
        aView->Update();
      }
      else
      {
        aView->Redraw();
      }
    }
    else if (aView->IsInvalidatedImmediate())
    {
      aView->RedrawImmediate();
    }
  }

  if (myToAskNextFrame)
  {
    // ask more frames
    theView->Window()->InvalidateContent (Handle(Aspect_DisplayConnection)());
  }
}

// =======================================================================
// function : handleXRMoveTo
// purpose  :
// =======================================================================
Standard_Integer AIS_ViewController::handleXRMoveTo (const Handle(AIS_InteractiveContext)& theCtx,
                                                     const Handle(V3d_View)& theView,
                                                     const gp_Trsf& thePose,
                                                     const Standard_Boolean theToHighlight)
{
  //ResetPreviousMoveTo();
  Standard_Integer aPickResult = 0;

  Handle(Graphic3d_Camera) aCamBack = theView->Camera();
  myXRCameraTmp->Copy (aCamBack);
  theView->View()->ComputeXRPosedCameraFromBase (*myXRCameraTmp, thePose);
  theView->SetCamera (myXRCameraTmp);
  Graphic3d_Vec2i aPickPixel;
  theView->Window()->Size (aPickPixel.x(), aPickPixel.y());
  aPickPixel /= 2;
  const Standard_Integer aSelTolerBack = theCtx->MainSelector()->CustomPixelTolerance();
  theCtx->MainSelector()->SetPixelTolerance (1);
  theView->AutoZFit();
  if (theToHighlight)
  {
    theCtx->MoveTo (aPickPixel.x(), aPickPixel.y(), theView, false);
    if (!theCtx->DetectedOwner().IsNull())
    {
      // ignore 2D objects
      for (aPickResult = 1; !theCtx->DetectedOwner()->Selectable()->TransformPersistence().IsNull(); ++aPickResult)
      {
        if (theCtx->HilightNextDetected (theView, false) <= 1)
        {
          theCtx->ClearDetected();
          aPickResult = 0;
          break;
        }
      }
    }
  }
  else
  {
    theCtx->MainSelector()->Pick (aPickPixel.x(), aPickPixel.y(), theView);
    for (Standard_Integer aPickIter = 1; aPickIter <= theCtx->MainSelector()->NbPicked(); ++aPickIter)
    {
      const SelectMgr_SortCriterion& aPickedData = theCtx->MainSelector()->PickedData (aPickIter);
      if (!aPickedData.Entity->OwnerId()->Selectable()->TransformPersistence().IsNull())
      {
        // skip 2d objects
        continue;
      }

      aPickResult = aPickIter;
      break;
    }
  }
  theCtx->MainSelector()->SetPixelTolerance (aSelTolerBack);
  theView->SetCamera (aCamBack);
  return aPickResult;
}

// =======================================================================
// function : handleXRHighlight
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRHighlight (const Handle(AIS_InteractiveContext)& theCtx,
                                            const Handle(V3d_View)& theView)
{
  if (myXRLastPickingHand != Aspect_XRTrackedDeviceRole_LeftHand
   && myXRLastPickingHand != Aspect_XRTrackedDeviceRole_RightHand)
  {
    return;
  }

  const Standard_Integer aDeviceId = theView->View()->XRSession()->NamedTrackedDevice (myXRLastPickingHand);
  if (aDeviceId == -1)
  {
    return;
  }

  const Aspect_TrackedDevicePose& aPose = theView->View()->XRSession()->TrackedPoses()[aDeviceId];
  if (!aPose.IsValidPose)
  {
    return;
  }

  Handle(SelectMgr_EntityOwner) aDetOld = theCtx->DetectedOwner();
  handleXRMoveTo (theCtx, theView, aPose.Orientation, true);
  if (!theCtx->DetectedOwner().IsNull()
    && theCtx->DetectedOwner() != aDetOld)
  {
    if (const Handle(Aspect_XRAction)& aHaptic = theView->View()->XRSession()->GenericAction (myXRLastPickingHand, Aspect_XRGenericAction_OutputHaptic))
    {
      theView->View()->XRSession()->TriggerHapticVibrationAction (aHaptic, myXRPickingHaptic);
    }
  }

  Standard_Real& aPickDepth = myXRLastPickingHand == Aspect_XRTrackedDeviceRole_LeftHand ? myXRLastPickDepthLeft : myXRLastPickDepthRight;
  aPickDepth = Precision::Infinite();
  if (theCtx->MainSelector()->NbPicked() > 0)
  {
    const gp_Trsf aHandBase = theView->View()->PoseXRToWorld (aPose.Orientation);
    const SelectMgr_SortCriterion& aPicked = theCtx->MainSelector()->PickedData (1);
    aPickDepth = aPicked.Point.Distance (aHandBase.TranslationPart());
  }
}

// =======================================================================
// function : handleXRPresentations
// purpose  :
// =======================================================================
void AIS_ViewController::handleXRPresentations (const Handle(AIS_InteractiveContext)& theCtx,
                                                const Handle(V3d_View)& theView)
{
  if (!theView->View()->IsActiveXR()
   || (!myToDisplayXRAuxDevices
    && !myToDisplayXRHands))
  {
    for (NCollection_Array1<Handle(AIS_XRTrackedDevice)>::Iterator aPrsIter (myXRPrsDevices); aPrsIter.More(); aPrsIter.Next())
    {
      if (!aPrsIter.Value().IsNull()
        && aPrsIter.Value()->HasInteractiveContext())
      {
        theCtx->Remove (aPrsIter.Value(), false);
      }
      aPrsIter.ChangeValue().Nullify();
    }
    return;
  }

  if (myXRPrsDevices.Length() != theView->View()->XRSession()->TrackedPoses().Length())
  {
    for (NCollection_Array1<Handle(AIS_XRTrackedDevice)>::Iterator aPrsIter (myXRPrsDevices); aPrsIter.More(); aPrsIter.Next())
    {
      if (!aPrsIter.Value().IsNull())
      {
        theCtx->Remove (aPrsIter.Value(), false);
      }
    }
    myXRPrsDevices.Resize (theView->View()->XRSession()->TrackedPoses().Lower(), theView->View()->XRSession()->TrackedPoses().Upper(), false);
  }

  const Standard_Integer aHeadDevice  = theView->View()->XRSession()->NamedTrackedDevice (Aspect_XRTrackedDeviceRole_Head);
  const Standard_Integer aLeftDevice  = theView->View()->XRSession()->NamedTrackedDevice (Aspect_XRTrackedDeviceRole_LeftHand);
  const Standard_Integer aRightDevice = theView->View()->XRSession()->NamedTrackedDevice (Aspect_XRTrackedDeviceRole_RightHand);
  for (Standard_Integer aDeviceIter = theView->View()->XRSession()->TrackedPoses().Lower(); aDeviceIter <= theView->View()->XRSession()->TrackedPoses().Upper(); ++aDeviceIter)
  {
    const Aspect_TrackedDevicePose& aPose = theView->View()->XRSession()->TrackedPoses()[aDeviceIter];
    Handle(AIS_XRTrackedDevice)& aPosePrs = myXRPrsDevices[aDeviceIter];
    if (!aPose.IsValidPose)
    {
      continue;
    }

    const bool isHand = aDeviceIter == aLeftDevice
                     || aDeviceIter == aRightDevice;
    if ((!myToDisplayXRHands && isHand)
     || (!myToDisplayXRAuxDevices && !isHand))
    {
      if (!aPosePrs.IsNull()
        && aPosePrs->HasInteractiveContext())
      {
        theCtx->Remove (aPosePrs, false);
      }
      continue;
    }

    Aspect_XRTrackedDeviceRole aRole = Aspect_XRTrackedDeviceRole_Other;
    if (aDeviceIter == aLeftDevice)
    {
      aRole = Aspect_XRTrackedDeviceRole_LeftHand;
    }
    else if (aDeviceIter == aRightDevice)
    {
      aRole = Aspect_XRTrackedDeviceRole_RightHand;
    }

    if (!aPosePrs.IsNull()
      && aPosePrs->UnitFactor() != (float )theView->View()->UnitFactor())
    {
      theCtx->Remove (aPosePrs, false);
      aPosePrs.Nullify();
    }

    if (aPosePrs.IsNull())
    {
      Handle(Image_Texture) aTexture;
      Handle(Graphic3d_ArrayOfTriangles) aTris;
      if (aDeviceIter != aHeadDevice)
      {
        aTris = theView->View()->XRSession()->LoadRenderModel (aDeviceIter, aTexture);
      }
      if (!aTris.IsNull())
      {
        aPosePrs = new AIS_XRTrackedDevice (aTris, aTexture);
      }
      else
      {
        aPosePrs = new AIS_XRTrackedDevice();
      }
      aPosePrs->SetUnitFactor ((float )theView->View()->UnitFactor());
      aPosePrs->SetMutable (true);
      aPosePrs->SetInfiniteState (true);
    }
    aPosePrs->SetRole (aRole);

    if (!aPosePrs->HasInteractiveContext())
    {
      theCtx->Display (aPosePrs, 0, -1, false);
    }

    gp_Trsf aPoseLocal = aPose.Orientation;
    if (aDeviceIter == aHeadDevice)
    {
      // show headset position on floor level
      aPoseLocal.SetTranslationPart (gp_Vec (aPoseLocal.TranslationPart().X(), 0.0, aPoseLocal.TranslationPart().Z()));
    }
    const gp_Trsf aPoseWorld = theView->View()->PoseXRToWorld (aPoseLocal);
    theCtx->SetLocation (aPosePrs, aPoseWorld);

    Standard_Real aLaserLen = 0.0;
    if (isHand
      && aPosePrs->Role() == myXRLastPickingHand)
    {
      aLaserLen = myXRLastPickingHand == Aspect_XRTrackedDeviceRole_LeftHand ? myXRLastPickDepthLeft : myXRLastPickDepthRight;
      if (Precision::IsInfinite (aLaserLen))
      {
        const Bnd_Box aViewBox = theView->View()->MinMaxValues (true);
        if (!aViewBox.IsVoid())
        {
          aLaserLen = Sqrt (aViewBox.SquareExtent());
        }
        else
        {
          aLaserLen = 100.0;
        }
      }
      aPosePrs->SetLaserColor (myXRLaserPickColor);
    }
    else if (isHand
          && aPosePrs->Role() == myXRLastTeleportHand)
    {
      aLaserLen = myXRLastTeleportHand == Aspect_XRTrackedDeviceRole_LeftHand ? myXRLastPickDepthLeft : myXRLastPickDepthRight;
      if (Precision::IsInfinite (aLaserLen))
      {
        const Bnd_Box aViewBox = theView->View()->MinMaxValues (true);
        if (!aViewBox.IsVoid())
        {
          aLaserLen = Sqrt (aViewBox.SquareExtent());
        }
        else
        {
          aLaserLen = 100.0;
        }
      }
      aPosePrs->SetLaserColor (myXRLaserTeleColor);
    }
    aPosePrs->SetLaserLength ((float )aLaserLen);
  }
}

// =======================================================================
// function : HandleViewEvents
// purpose  :
// =======================================================================
void AIS_ViewController::HandleViewEvents (const Handle(AIS_InteractiveContext)& theCtx,
                                           const Handle(V3d_View)& theView)
{
  const bool wasImmediateUpdate = theView->SetImmediateUpdate (false);

  handleViewOrientationKeys (theCtx, theView);
  const AIS_WalkDelta aWalk = handleNavigationKeys (theCtx, theView);
  handleXRInput (theCtx, theView, aWalk);
  if (theView->View()->IsActiveXR())
  {
    theView->View()->SetupXRPosedCamera();
  }
  handleMoveTo (theCtx, theView);
  handleCameraActions (theCtx, theView, aWalk);
  theView->View()->SynchronizeXRPosedToBaseCamera(); // handleCameraActions() may modify posed camera position - copy this modifications also to the base camera
  handleXRPresentations (theCtx, theView);

  handleViewRedraw (theCtx, theView);
  theView->View()->UnsetXRPosedCamera();

  theView->SetImmediateUpdate (wasImmediateUpdate);

  // make sure to not process the same events twice
  myGL.Reset();
  myToAskNextFrame = false;
}
