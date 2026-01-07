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
#include <Message.hxx>
#include <gp_Quaternion.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_HIDSpaceMouse.hxx>

//=================================================================================================

AIS_ViewController::AIS_ViewController()
    : myLastEventsTime(0.0),
      myToAskNextFrame(false),
      myIsContinuousRedraw(false),
      myMinCamDistance(1.0),
      myRotationMode(AIS_RotationMode_BndBoxActive),
      myNavigationMode(AIS_NavigationMode_Orbit),
      myMouseAccel(1.0f),
      myOrbitAccel(1.0f),
      myToShowPanAnchorPoint(true),
      myToShowRotateCenter(true),
      myToLockOrbitZUp(false),
      myToInvertPitch(false),
      myToAllowTouchZRotation(false),
      myToAllowRotation(true),
      myToAllowPanning(true),
      myToAllowZooming(true),
      myToAllowZFocus(true),
      myToAllowHighlight(true),
      myToAllowDragging(true),
      myToStickToRayOnZoom(true),
      myToStickToRayOnRotation(true),
      //
      myWalkSpeedAbsolute(1.5f),
      myWalkSpeedRelative(0.1f),
      myThrustSpeed(0.0f),
      myHasThrust(false),
      //
      myViewAnimation(
        new AIS_AnimationCamera("AIS_ViewController_ViewAnimation", occ::handle<V3d_View>())),
      myObjAnimation(new AIS_Animation("AIS_ViewController_ObjectsAnimation")),
      myToPauseObjAnimation(false),
      myPrevMoveTo(-1, -1),
      myHasHlrOnBeforeRotation(false),
      //
      myXRPrsDevices(0, 0),
      myXRLaserTeleColor(Quantity_NOC_GREEN),
      myXRLaserPickColor(Quantity_NOC_BLUE),
      myXRLastTeleportHand(Aspect_XRTrackedDeviceRole_Other),
      myXRLastPickingHand(Aspect_XRTrackedDeviceRole_Other),
      myXRLastPickDepthLeft(Precision::Infinite()),
      myXRLastPickDepthRight(Precision::Infinite()),
      myXRTurnAngle(M_PI_4),
      myToDisplayXRAuxDevices(false),
      myToDisplayXRHands(true),
      //
      myMouseClickThreshold(3.0),
      myMouseDoubleClickInt(0.4),
      myScrollZoomRatio(15.0f),
      myMouseActiveGesture(AIS_MouseGesture_NONE),
      myMouseActiveIdleRotation(false),
      myMouseClickCounter(0),
      myMouseSingleButton(-1),
      myMouseStopDragOnUnclick(false),
      //
      myTouchToleranceScale(1.0f),
      myTouchClickThresholdPx(3.0f),
      myTouchRotationThresholdPx(6.0f),
      myTouchZRotationThreshold(float(2.0 * M_PI / 180.0)),
      myTouchPanThresholdPx(4.0f),
      myTouchZoomThresholdPx(6.0f),
      myTouchZoomRatio(0.13f),
      myTouchDraggingThresholdPx(6.0f),
      //
      myNbTouchesLast(0),
      myUpdateStartPointPan(true),
      myUpdateStartPointRot(true),
      myUpdateStartPointZRot(true),
      //
      myPanPnt3d(Precision::Infinite(), 0.0, 0.0)
{
  myViewAnimation->SetOwnDuration(0.5);

  myAnchorPointPrs1 = new AIS_Point(new Geom_CartesianPoint(0.0, 0.0, 0.0));
  myAnchorPointPrs1->SetZLayer(Graphic3d_ZLayerId_Top);
  myAnchorPointPrs1->SetMutable(true);

  myAnchorPointPrs2 = new AIS_Point(new Geom_CartesianPoint(0.0, 0.0, 0.0));
  myAnchorPointPrs2->SetZLayer(Graphic3d_ZLayerId_Topmost);
  myAnchorPointPrs2->SetMutable(true);

  myRubberBand =
    new AIS_RubberBand(Quantity_NOC_LIGHTBLUE, Aspect_TOL_SOLID, Quantity_NOC_LIGHTBLUE4, 0.5, 1.0);
  myRubberBand->SetZLayer(Graphic3d_ZLayerId_TopOSD);
  myRubberBand->SetTransformPersistence(
    new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_UPPER));
  myRubberBand->SetDisplayMode(0);
  myRubberBand->SetMutable(true);

  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_RotateOrbit);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                           | (unsigned int)Aspect_VKeyFlags_CTRL,
                         AIS_MouseGesture_Zoom);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                           | (unsigned int)Aspect_VKeyFlags_SHIFT,
                         AIS_MouseGesture_Pan);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                           | (unsigned int)Aspect_VKeyFlags_ALT,
                         AIS_MouseGesture_SelectRectangle);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                           | (unsigned int)Aspect_VKeyFlags_ALT
                           | (unsigned int)Aspect_VKeyFlags_SHIFT,
                         AIS_MouseGesture_SelectRectangle);

  myMouseSelectionSchemes.Bind((unsigned int)Aspect_VKeyMouse_LeftButton,
                               AIS_SelectionScheme_Replace);
  myMouseSelectionSchemes.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                                 | (unsigned int)Aspect_VKeyFlags_ALT,
                               AIS_SelectionScheme_Replace);
  myMouseSelectionSchemes.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                                 | (unsigned int)Aspect_VKeyFlags_SHIFT,
                               AIS_SelectionScheme_XOR);
  myMouseSelectionSchemes.Bind((unsigned int)Aspect_VKeyMouse_LeftButton
                                 | (unsigned int)Aspect_VKeyFlags_ALT
                                 | (unsigned int)Aspect_VKeyFlags_SHIFT,
                               AIS_SelectionScheme_XOR);

  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_RightButton, AIS_MouseGesture_Zoom);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_RightButton
                           | (unsigned int)Aspect_VKeyFlags_CTRL,
                         AIS_MouseGesture_RotateOrbit);

  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_MiddleButton, AIS_MouseGesture_Pan);
  myMouseGestureMap.Bind((unsigned int)Aspect_VKeyMouse_MiddleButton
                           | (unsigned int)Aspect_VKeyFlags_CTRL,
                         AIS_MouseGesture_Pan);

  myMouseGestureMapDrag.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Drag);

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

//=================================================================================================

AIS_ViewController::~AIS_ViewController() = default;

//=================================================================================================

void AIS_ViewController::ResetViewInput()
{
  myKeys.Reset();
  myMousePressed        = Aspect_VKeyMouse_NONE;
  myMouseModifiers      = Aspect_VKeyFlags_NONE;
  myMouseSingleButton   = -1;
  myUI.Dragging.ToAbort = true;
  myMouseActiveGesture  = AIS_MouseGesture_NONE;
  myMouseClickTimer.Stop();
  myMouseClickCounter = 0;
}

//=================================================================================================

void AIS_ViewController::FlushViewEvents(const occ::handle<AIS_InteractiveContext>& theCtx,
                                         const occ::handle<V3d_View>&               theView,
                                         bool                                       theToHandle)
{
  flushBuffers(theCtx, theView);
  flushGestures(theCtx, theView);

  if (theView->IsSubview())
  {
    // move input coordinates inside the view
    const NCollection_Vec2<int> aDelta = theView->View()->SubviewTopLeft();
    if (myGL.MoveTo.ToHilight || myGL.Dragging.ToStart)
    {
      myGL.MoveTo.Point -= aDelta;
    }
    if (myGL.Panning.ToStart)
    {
      myGL.Panning.PointStart -= aDelta;
    }
    if (myGL.Dragging.ToStart)
    {
      myGL.Dragging.PointStart -= aDelta;
    }
    if (myGL.Dragging.ToMove)
    {
      myGL.Dragging.PointTo -= aDelta;
    }
    if (myGL.OrbitRotation.ToStart)
    {
      myGL.OrbitRotation.PointStart -= NCollection_Vec2<double>(aDelta);
    }
    if (myGL.OrbitRotation.ToRotate)
    {
      myGL.OrbitRotation.PointTo -= NCollection_Vec2<double>(aDelta);
    }
    if (myGL.ViewRotation.ToStart)
    {
      myGL.ViewRotation.PointStart -= NCollection_Vec2<double>(aDelta);
    }
    if (myGL.ViewRotation.ToRotate)
    {
      myGL.ViewRotation.PointTo -= NCollection_Vec2<double>(aDelta);
    }
    for (NCollection_Vec2<int>& aPntIter : myGL.Selection.Points)
    {
      aPntIter -= aDelta;
    }
    for (Aspect_ScrollDelta& aZoomIter : myGL.ZoomActions)
    {
      aZoomIter.Point -= aDelta;
    }
  }

  if (theToHandle)
  {
    HandleViewEvents(theCtx, theView);
  }
}

//=================================================================================================

void AIS_ViewController::flushBuffers(const occ::handle<AIS_InteractiveContext>&,
                                      const occ::handle<V3d_View>&)
{
  myToAskNextFrame = false;

  myGL.IsNewGesture = myUI.IsNewGesture;
  myUI.IsNewGesture = false;

  myGL.ZoomActions.Clear();
  myGL.ZoomActions.Append(myUI.ZoomActions);
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
    myGL.Selection.Scheme = myUI.Selection.Scheme;
    myGL.Selection.Points = myUI.Selection.Points;
    // myGL.Selection.Scheme = AIS_SelectionScheme_UNKNOWN; // no need
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
    myUI.Panning.ToStart    = false;
    myGL.Panning.ToStart    = true;
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
  else
  {
    if (myUI.Dragging.ToStart)
    {
      myUI.Dragging.ToStart    = false;
      myGL.Dragging.ToStart    = true;
      myGL.Dragging.PointStart = myUI.Dragging.PointStart;
    }
    if (myUI.Dragging.ToConfirm)
    {
      myUI.Dragging.ToConfirm = false;
      myGL.Dragging.ToConfirm = true;
    }
    if (myUI.Dragging.ToMove)
    {
      myUI.Dragging.ToMove = false;
      myGL.Dragging.ToMove = true;
    }
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
    myGL.ZRotate          = myUI.ZRotate;
    myUI.ZRotate.ToRotate = false;
  }
}

//=================================================================================================

void AIS_ViewController::flushGestures(const occ::handle<AIS_InteractiveContext>&,
                                       const occ::handle<V3d_View>& theView)
{
  const double aTolScale = myTouchToleranceScale;
  const int    aTouchNb  = myTouchPoints.Extent();
  if (myNbTouchesLast != aTouchNb)
  {
    myNbTouchesLast   = aTouchNb;
    myGL.IsNewGesture = true;
  }
  if (aTouchNb == 1) // touch
  {
    Aspect_Touch& aTouch = myTouchPoints.ChangeFromIndex(1);
    if (myUpdateStartPointRot)
    {
      // skip rotation if have active dragged object
      if (myNavigationMode == AIS_NavigationMode_Orbit)
      {
        myGL.OrbitRotation.ToStart    = true;
        myGL.OrbitRotation.PointStart = myStartRotCoord;
      }
      else
      {
        myGL.ViewRotation.ToStart    = true;
        myGL.ViewRotation.PointStart = myStartRotCoord;
      }

      myUpdateStartPointRot = false;
      theView->Invalidate();
    }

    // rotation
    const double aRotTouchTol =
      !aTouch.IsPreciseDevice ? aTolScale * myTouchRotationThresholdPx : gp::Resolution();
    if (std::abs(aTouch.Delta().x()) + std::abs(aTouch.Delta().y()) > aRotTouchTol)
    {
      const double aRotAccel =
        myNavigationMode == AIS_NavigationMode_FirstPersonWalk ? myMouseAccel : myOrbitAccel;
      if (myNavigationMode == AIS_NavigationMode_Orbit)
      {
        const NCollection_Vec2<double> aRotDelta = aTouch.To - myGL.OrbitRotation.PointStart;
        myGL.OrbitRotation.ToRotate              = true;
        myGL.OrbitRotation.PointTo = myGL.OrbitRotation.PointStart + aRotDelta * aRotAccel;
        myGL.Dragging.ToMove       = true;
        myGL.Dragging.PointTo.SetValues((int)aTouch.To.x(), (int)aTouch.To.y());
      }
      else
      {
        const NCollection_Vec2<double> aRotDelta = aTouch.To - myGL.ViewRotation.PointStart;
        myGL.ViewRotation.ToRotate               = true;
        myGL.ViewRotation.PointTo = myGL.ViewRotation.PointStart + aRotDelta * aRotAccel;
        myGL.Dragging.ToMove      = true;
        myGL.Dragging.PointTo.SetValues((int)aTouch.To.x(), (int)aTouch.To.y());
      }

      aTouch.From = aTouch.To;
    }
  }
  else if (aTouchNb == 2) // pinch
  {
    Aspect_Touch&                  aFirstTouch = myTouchPoints.ChangeFromIndex(1);
    Aspect_Touch&                  aLastTouch  = myTouchPoints.ChangeFromIndex(2);
    const NCollection_Vec2<double> aFrom[2]    = {aFirstTouch.From, aLastTouch.From};
    const NCollection_Vec2<double> aTo[2]      = {aFirstTouch.To, aLastTouch.To};

    NCollection_Vec2<double> aPinchCenterStart((aFrom[0].x() + aFrom[1].x()) / 2.0,
                                               (aFrom[0].y() + aFrom[1].y()) / 2.0);

    double aPinchCenterXEnd = (aTo[0].x() + aTo[1].x()) / 2.0;
    double aPinchCenterYEnd = (aTo[0].y() + aTo[1].y()) / 2.0;

    double aPinchCenterXDev = aPinchCenterXEnd - aPinchCenterStart.x();
    double aPinchCenterYDev = aPinchCenterYEnd - aPinchCenterStart.y();

    double aStartSize = (aFrom[0] - aFrom[1]).Modulus();
    double anEndSize  = (aTo[0] - aTo[1]).Modulus();

    double aDeltaSize = anEndSize - aStartSize;

    bool anIsClearDev = false;

    if (myToAllowTouchZRotation)
    {
      double A1 = aFrom[0].y() - aFrom[1].y();
      double B1 = aFrom[1].x() - aFrom[0].x();

      double A2 = aTo[0].y() - aTo[1].y();
      double B2 = aTo[1].x() - aTo[0].x();

      double aRotAngle = 0.0;

      double aDenomenator = A1 * A2 + B1 * B2;
      if (aDenomenator <= Precision::Confusion())
      {
        aRotAngle = 0.0;
      }
      else
      {
        double aNumerator = A1 * B2 - A2 * B1;
        aRotAngle         = std::atan(aNumerator / aDenomenator);
      }

      if (std::abs(aRotAngle) > double(myTouchZRotationThreshold))
      {
        myGL.ZRotate.ToRotate = true;
        myGL.ZRotate.Angle    = aRotAngle;
        anIsClearDev          = true;
      }
    }

    if (std::abs(aDeltaSize) > aTolScale * myTouchZoomThresholdPx)
    {
      // zoom
      aDeltaSize *= double(myTouchZoomRatio);
      Aspect_ScrollDelta aParams(NCollection_Vec2<int>(aPinchCenterStart), aDeltaSize);
      myGL.ZoomActions.Append(aParams);
      anIsClearDev = true;
    }

    const double aPanTouchTol =
      !aFirstTouch.IsPreciseDevice ? aTolScale * myTouchPanThresholdPx : gp::Resolution();
    if (std::abs(aPinchCenterXDev) + std::abs(aPinchCenterYDev) > aPanTouchTol)
    {
      // pan
      if (myUpdateStartPointPan)
      {
        myGL.Panning.ToStart    = true;
        myGL.Panning.PointStart = NCollection_Vec2<int>(myStartPanCoord);
        myUpdateStartPointPan   = false;
        theView->Invalidate();
      }

      myGL.Panning.ToPan     = true;
      myGL.Panning.Delta.x() = int(aPinchCenterXDev);
      myGL.Panning.Delta.y() = int(-aPinchCenterYDev);
      anIsClearDev           = true;
    }

    if (anIsClearDev)
    {
      aFirstTouch.From = aFirstTouch.To;
      aLastTouch.From  = aLastTouch.To;
    }
  }
}

//=================================================================================================

void AIS_ViewController::UpdateViewOrientation(V3d_TypeOfOrientation theOrientation,
                                               bool                  theToFitAll)
{
  myUI.Orientation.ToFitAll        = theToFitAll;
  myUI.Orientation.ToSetViewOrient = true;
  myUI.Orientation.ViewOrient      = theOrientation;
}

//=================================================================================================

void AIS_ViewController::SelectInViewer(const NCollection_Vec2<int>& thePnt,
                                        const AIS_SelectionScheme    theScheme)
{
  if (myUI.Selection.Tool != AIS_ViewSelectionTool_Picking)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Picking;
    myUI.Selection.Points.Clear();
  }

  myUI.Selection.Scheme = theScheme;
  myUI.Selection.Points.Append(thePnt);
}

//=================================================================================================

void AIS_ViewController::SelectInViewer(const NCollection_Sequence<NCollection_Vec2<int>>& thePnts,
                                        const AIS_SelectionScheme theScheme)
{
  myUI.Selection.Scheme      = theScheme;
  myUI.Selection.Points      = thePnts;
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

//=================================================================================================

void AIS_ViewController::UpdateRubberBand(const NCollection_Vec2<int>& thePntFrom,
                                          const NCollection_Vec2<int>& thePntTo)
{
  myUI.Selection.Tool = AIS_ViewSelectionTool_RubberBand;
  myUI.Selection.Points.Clear();
  myUI.Selection.Points.Append(thePntFrom);
  myUI.Selection.Points.Append(thePntTo);
}

//=================================================================================================

void AIS_ViewController::UpdatePolySelection(const NCollection_Vec2<int>& thePnt, bool theToAppend)
{
  if (myUI.Selection.Tool != AIS_ViewSelectionTool_Polygon)
  {
    myUI.Selection.Tool = AIS_ViewSelectionTool_Polygon;
    myUI.Selection.Points.Clear();
  }

  if (myUI.Selection.Points.IsEmpty())
  {
    myUI.Selection.Points.Append(thePnt);
  }
  else if (theToAppend && myUI.Selection.Points.Last() != thePnt)
  {
    myUI.Selection.Points.Append(thePnt);
  }
  else
  {
    myUI.Selection.Points.ChangeLast() = thePnt;
  }
}

//=================================================================================================

bool AIS_ViewController::UpdateZoom(const Aspect_ScrollDelta& theDelta)
{
  if (!myUI.ZoomActions.IsEmpty())
  {
    if (myUI.ZoomActions.ChangeLast().Point == theDelta.Point)
    {
      myUI.ZoomActions.ChangeLast().Delta += theDelta.Delta;
      return false;
    }
  }

  myUI.ZoomActions.Append(theDelta);
  return true;
}

//=================================================================================================

bool AIS_ViewController::UpdateZRotation(double theAngle)
{
  if (!ToAllowTouchZRotation())
  {
    return false;
  }

  myUI.ZRotate.Angle = myUI.ZRotate.ToRotate ? myUI.ZRotate.Angle + theAngle : theAngle;
  if (myUI.ZRotate.ToRotate)
  {
    return false;
  }
  myUI.ZRotate.ToRotate = true;
  return true;
}

//=================================================================================================

bool AIS_ViewController::UpdateMouseScroll(const Aspect_ScrollDelta& theDelta)
{
  Aspect_ScrollDelta aDelta = theDelta;
  aDelta.Delta *= myScrollZoomRatio;
  return UpdateZoom(aDelta);
}

//=================================================================================================

bool AIS_ViewController::UpdateMouseClick(const NCollection_Vec2<int>& thePoint,
                                          Aspect_VKeyMouse             theButton,
                                          Aspect_VKeyFlags             theModifiers,
                                          bool                         theIsDoubleClick)
{
  (void)theIsDoubleClick;

  if (myToPauseObjAnimation && !myObjAnimation.IsNull() && !myObjAnimation->IsStopped())
  {
    myObjAnimation->Pause();
  }

  AIS_SelectionScheme aScheme = AIS_SelectionScheme_UNKNOWN;
  if (myMouseSelectionSchemes.Find(theButton | theModifiers, aScheme))
  {
    SelectInViewer(thePoint, aScheme);
    return true;
  }
  return false;
}

//=================================================================================================

bool AIS_ViewController::UpdateMouseButtons(const NCollection_Vec2<int>& thePoint,
                                            Aspect_VKeyMouse             theButtons,
                                            Aspect_VKeyFlags             theModifiers,
                                            bool                         theIsEmulated)
{
  bool         toUpdateView = false;
  const double aTolClick    = (theIsEmulated ? myTouchToleranceScale : 1.0) * myMouseClickThreshold;
  if (theButtons == Aspect_VKeyMouse_NONE && myMouseSingleButton > 0)
  {
    const NCollection_Vec2<int> aDelta = thePoint - myMousePressPoint;
    if (double(aDelta.cwiseAbs().maxComp()) < aTolClick)
    {
      ++myMouseClickCounter;

      const bool isCounterValid = myMouseClickCounter == 2;
      const bool isTimerStarted = myMouseClickTimer.IsStarted();
      const bool isTimerElapsed = myMouseClickTimer.ElapsedTime() > myMouseDoubleClickInt;

      const bool isTimerValid = isTimerStarted && !isTimerElapsed;

      const bool isDoubleClick = isCounterValid && isTimerValid;

      if (!isTimerValid)
      {
        myMouseClickCounter = 1;
      }

      myMouseClickCounter %= 2;

      myMouseClickTimer.Stop();
      myMouseClickTimer.Reset();
      myMouseClickTimer.Start();
      if (isDoubleClick)
      {
        myMouseClickCounter = 0;
      }
      toUpdateView = UpdateMouseClick(thePoint,
                                      (Aspect_VKeyMouse)myMouseSingleButton,
                                      theModifiers,
                                      isDoubleClick)
                     || toUpdateView;
    }
    else
    {
      myMouseClickTimer.Stop();
      myMouseClickCounter      = 0;
      myMouseStopDragOnUnclick = false;
      myUI.Dragging.ToStop     = true;
      toUpdateView             = true;
    }
    myMouseSingleButton = -1;
  }
  else if (theButtons == Aspect_VKeyMouse_NONE)
  {
    myMouseSingleButton = -1;
    if (myMouseStopDragOnUnclick)
    {
      myMouseStopDragOnUnclick = false;
      myUI.Dragging.ToStop     = true;
      toUpdateView             = true;
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
        const NCollection_Vec2<int> aDelta = thePoint - myMousePressPoint;
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
    toUpdateView          = true;
  }

  const AIS_MouseGesture aPrevGesture   = myMouseActiveGesture;
  const Aspect_VKeyMouse aPrevButtons   = myMousePressed;
  const Aspect_VKeyFlags aPrevModifiers = myMouseModifiers;
  myMouseModifiers                      = theModifiers;
  myMousePressed                        = theButtons;
  if (theIsEmulated || myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    myMouseActiveIdleRotation = false;
    myMouseActiveGesture      = AIS_MouseGesture_NONE;
    if (theButtons != 0)
    {
      myMousePressPoint    = thePoint;
      myMouseProgressPoint = myMousePressPoint;
    }

    if (myMouseGestureMap.Find(theButtons | theModifiers, myMouseActiveGesture))
    {
      switch (myMouseActiveGesture)
      {
        case AIS_MouseGesture_RotateView:
        case AIS_MouseGesture_RotateOrbit: {
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
        case AIS_MouseGesture_Pan: {
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
        case AIS_MouseGesture_ZoomVertical: {
          if (!myToAllowZooming)
          {
            myMouseActiveGesture = AIS_MouseGesture_NONE;
          }
          break;
        }
        case AIS_MouseGesture_SelectRectangle: {
          break;
        }
        case AIS_MouseGesture_SelectLasso: {
          UpdatePolySelection(thePoint, true);
          break;
        }
        case AIS_MouseGesture_Drag: {
          if (myToAllowDragging)
          {
            myUI.Dragging.ToStart    = true;
            myUI.Dragging.PointStart = thePoint;
          }
          else
          {
            myMouseActiveGesture = AIS_MouseGesture_NONE;
          }
          break;
        }
        case AIS_MouseGesture_NONE: {
          break;
        }
      }
    }

    AIS_MouseGesture aSecGesture = AIS_MouseGesture_NONE;
    if (myMouseGestureMapDrag.Find(theButtons | theModifiers, aSecGesture))
    {
      if (aSecGesture == AIS_MouseGesture_Drag && myToAllowDragging)
      {
        myUI.Dragging.ToStart    = true;
        myUI.Dragging.PointStart = thePoint;
        if (myMouseActiveGesture == AIS_MouseGesture_NONE)
        {
          myMouseActiveGesture = aSecGesture;
        }
      }
    }
  }

  if (aPrevGesture != myMouseActiveGesture)
  {
    if (aPrevGesture == AIS_MouseGesture_SelectRectangle
        || aPrevGesture == AIS_MouseGesture_SelectLasso
        || aPrevGesture == AIS_MouseGesture_ZoomWindow)
    {
      myUI.Selection.ToApplyTool = true;
      myUI.Selection.Scheme      = AIS_SelectionScheme_Replace;
      myMouseSelectionSchemes.Find(aPrevButtons | aPrevModifiers, myUI.Selection.Scheme);
    }

    myUI.IsNewGesture = true;
    toUpdateView      = true;
  }

  return toUpdateView;
}

//=================================================================================================

bool AIS_ViewController::UpdateMousePosition(const NCollection_Vec2<int>& thePoint,
                                             Aspect_VKeyMouse             theButtons,
                                             Aspect_VKeyFlags             theModifiers,
                                             bool                         theIsEmulated)
{
  myMousePositionLast = thePoint;
  if (myMouseSingleButton > 0)
  {
    const double aTolClick = (theIsEmulated ? myTouchToleranceScale : 1.0) * myMouseClickThreshold;
    const NCollection_Vec2<int> aPressDelta = thePoint - myMousePressPoint;
    if (double(aPressDelta.cwiseAbs().maxComp()) >= aTolClick)
    {
      myMouseClickTimer.Stop();
      myMouseClickCounter      = 0;
      myMouseSingleButton      = -1;
      myMouseStopDragOnUnclick = true;
      myUI.Dragging.ToConfirm  = true;
    }
  }

  bool                  toUpdateView = false;
  NCollection_Vec2<int> aDelta       = thePoint - myMouseProgressPoint;
  if (!theIsEmulated && myNavigationMode == AIS_NavigationMode_FirstPersonWalk)
  {
    if (!myMouseActiveIdleRotation || myMouseActiveGesture != AIS_MouseGesture_RotateView)
    {
      myMouseActiveIdleRotation = true;
      myMouseActiveGesture      = AIS_MouseGesture_RotateView;
      myMousePressPoint         = thePoint;
      myMouseProgressPoint      = thePoint;
      myUpdateStartPointRot     = false;
      myUI.ViewRotation.ToStart = true;
      myUI.ViewRotation.PointStart.SetValues(thePoint.x(), thePoint.y());
      myUI.ViewRotation.ToRotate = false;
      aDelta.SetValues(0, 0);
    }
  }
  else
  {
    if (myMouseActiveIdleRotation && myMouseActiveGesture == AIS_MouseGesture_RotateView)
    {
      myMouseActiveGesture = AIS_MouseGesture_NONE;
    }
    myMouseActiveIdleRotation = false;
  }

  if (myMouseModifiers != theModifiers
      && UpdateMouseButtons(thePoint, theButtons, theModifiers, theIsEmulated))
  {
    toUpdateView = true;
  }

  switch (myMouseActiveGesture)
  {
    case AIS_MouseGesture_SelectRectangle:
    case AIS_MouseGesture_ZoomWindow: {
      UpdateRubberBand(myMousePressPoint, thePoint);
      if (myMouseActiveGesture == AIS_MouseGesture_ZoomWindow)
      {
        myUI.Selection.Tool = AIS_ViewSelectionTool_ZoomWindow;
      }
      toUpdateView = true;
      break;
    }
    case AIS_MouseGesture_SelectLasso: {
      UpdatePolySelection(thePoint, true);
      toUpdateView = true;
      break;
    }
    case AIS_MouseGesture_RotateOrbit:
    case AIS_MouseGesture_RotateView: {
      if (!myToAllowRotation)
      {
        break;
      }
      if (myUpdateStartPointRot)
      {
        if (myMouseActiveGesture == AIS_MouseGesture_RotateOrbit)
        {
          myUI.OrbitRotation.ToStart = true;
          myUI.OrbitRotation.PointStart.SetValues(myMousePressPoint.x(), myMousePressPoint.y());
        }
        else
        {
          myUI.ViewRotation.ToStart = true;
          myUI.ViewRotation.PointStart.SetValues(myMousePressPoint.x(), myMousePressPoint.y());
        }
        myUpdateStartPointRot = false;
      }

      const double aRotTol =
        theIsEmulated ? double(myTouchToleranceScale) * myTouchRotationThresholdPx : 0.0;
      const NCollection_Vec2<double> aDeltaF(aDelta);
      if (std::abs(aDeltaF.x()) + std::abs(aDeltaF.y()) > aRotTol)
      {
        const double aRotAccel =
          myNavigationMode == AIS_NavigationMode_FirstPersonWalk ? myMouseAccel : myOrbitAccel;
        const NCollection_Vec2<int> aRotDelta = thePoint - myMousePressPoint;
        if (myMouseActiveGesture == AIS_MouseGesture_RotateOrbit)
        {
          myUI.OrbitRotation.ToRotate = true;
          myUI.OrbitRotation.PointTo =
            NCollection_Vec2<double>(myMousePressPoint.x(), myMousePressPoint.y())
            + NCollection_Vec2<double>(aRotDelta.x(), aRotDelta.y()) * aRotAccel;
        }
        else
        {
          myUI.ViewRotation.ToRotate = true;
          myUI.ViewRotation.PointTo =
            NCollection_Vec2<double>(myMousePressPoint.x(), myMousePressPoint.y())
            + NCollection_Vec2<double>(aRotDelta.x(), aRotDelta.y()) * aRotAccel;
        }

        myUI.Dragging.ToMove  = true;
        myUI.Dragging.PointTo = thePoint;

        myMouseProgressPoint = thePoint;
        toUpdateView         = true;
      }
      break;
    }
    case AIS_MouseGesture_Zoom:
    case AIS_MouseGesture_ZoomVertical: {
      if (!myToAllowZooming)
      {
        break;
      }
      const double aZoomTol =
        theIsEmulated ? double(myTouchToleranceScale) * myTouchZoomThresholdPx : 0.0;
      const double aScrollDelta =
        myMouseActiveGesture == AIS_MouseGesture_Zoom ? aDelta.x() : aDelta.y();
      if (std::abs(aScrollDelta) > aZoomTol)
      {
        if (UpdateZoom(Aspect_ScrollDelta(aScrollDelta)))
        {
          toUpdateView = true;
        }

        myUI.Dragging.ToMove  = true;
        myUI.Dragging.PointTo = thePoint;

        myMouseProgressPoint = thePoint;
      }
      break;
    }
    case AIS_MouseGesture_Pan: {
      if (!myToAllowPanning)
      {
        break;
      }
      const double aPanTol =
        theIsEmulated ? double(myTouchToleranceScale) * myTouchPanThresholdPx : 0.0;
      const NCollection_Vec2<double> aDeltaF(aDelta);
      if (std::abs(aDeltaF.x()) + std::abs(aDeltaF.y()) > aPanTol)
      {
        if (myUpdateStartPointPan)
        {
          myUI.Panning.ToStart = true;
          myUI.Panning.PointStart.SetValues(myMousePressPoint.x(), myMousePressPoint.y());
          myUpdateStartPointPan = false;
        }

        aDelta.y() = -aDelta.y();
        if (myUI.Panning.ToPan)
        {
          myUI.Panning.Delta += aDelta;
        }
        else
        {
          myUI.Panning.ToPan = true;
          myUI.Panning.Delta = aDelta;
        }

        myUI.Dragging.ToMove  = true;
        myUI.Dragging.PointTo = thePoint;

        myMouseProgressPoint = thePoint;

        toUpdateView = true;
      }
      break;
    }
    case AIS_MouseGesture_Drag: {
      if (!myToAllowDragging)
      {
        break;
      }

      const double aDragTol =
        theIsEmulated ? double(myTouchToleranceScale) * myTouchDraggingThresholdPx : 0.0;
      if (double(std::abs(aDelta.x()) + std::abs(aDelta.y())) > aDragTol)
      {
        const double aRotAccel =
          myNavigationMode == AIS_NavigationMode_FirstPersonWalk ? myMouseAccel : myOrbitAccel;
        const NCollection_Vec2<int> aRotDelta = thePoint - myMousePressPoint;
        myUI.ViewRotation.ToRotate            = true;
        myUI.ViewRotation.PointTo =
          NCollection_Vec2<double>(myMousePressPoint.x(), myMousePressPoint.y())
          + NCollection_Vec2<double>(aRotDelta.x(), aRotDelta.y()) * aRotAccel;
        myUI.Dragging.ToMove  = true;
        myUI.Dragging.PointTo = thePoint;

        myMouseProgressPoint = thePoint;
        toUpdateView         = true;
      }
      break;
    }
    default: {
      break;
    }
  }

  if (theButtons == Aspect_VKeyMouse_NONE && myNavigationMode != AIS_NavigationMode_FirstPersonWalk
      && !theIsEmulated && !HasTouchPoints() && myToAllowHighlight)
  {
    myUI.MoveTo.ToHilight = true;
    myUI.MoveTo.Point     = thePoint;
    toUpdateView          = true;
  }
  return toUpdateView;
}

//=================================================================================================

void AIS_ViewController::AddTouchPoint(size_t                          theId,
                                       const NCollection_Vec2<double>& thePnt,
                                       bool                            theClearBefore)
{
  myUI.MoveTo.ToHilight = false;
  Aspect_WindowInputListener::AddTouchPoint(theId, thePnt, theClearBefore);

  myTouchClick.From = NCollection_Vec2<double>(-1.0);
  if (myTouchPoints.Extent() == 1)
  {
    myTouchClick.From     = thePnt;
    myUpdateStartPointRot = true;
    myStartRotCoord       = thePnt;
    if (myToAllowDragging)
    {
      myUI.Dragging.ToStart = true;
      myUI.Dragging.PointStart.SetValues((int)thePnt.x(), (int)thePnt.y());
    }
  }
  else if (myTouchPoints.Extent() == 2)
  {
    myUI.Dragging.ToAbort = true;

    myUpdateStartPointPan = true;
    myStartPanCoord       = thePnt;
  }
  myUI.IsNewGesture = true;
}

//=================================================================================================

bool AIS_ViewController::RemoveTouchPoint(size_t theId, bool theClearSelectPnts)
{
  if (!Aspect_WindowInputListener::RemoveTouchPoint(theId, theClearSelectPnts))
  {
    return false;
  }

  if (myTouchPoints.Extent() == 1)
  {
    // avoid incorrect transition from pinch to one finger
    Aspect_Touch& aFirstTouch = myTouchPoints.ChangeFromIndex(1);
    aFirstTouch.To            = aFirstTouch.From;

    myStartRotCoord       = aFirstTouch.To;
    myUpdateStartPointRot = true;
  }
  else if (myTouchPoints.Extent() == 2)
  {
    myStartPanCoord       = myTouchPoints.FindFromIndex(1).To;
    myUpdateStartPointPan = true;
  }
  else if (myTouchPoints.IsEmpty())
  {
    if (theClearSelectPnts)
    {
      myUI.Selection.ToApplyTool = true;
    }

    myUI.Dragging.ToStop = true;

    if (theId == (size_t)-1)
    {
      // abort clicking
      myTouchClick.From = NCollection_Vec2<double>(-1);
    }
    else if (myTouchClick.From.minComp() >= 0.0)
    {
      bool isDoubleClick = false;
      if (myTouchDoubleTapTimer.IsStarted()
          && myTouchDoubleTapTimer.ElapsedTime() <= myMouseDoubleClickInt)
      {
        isDoubleClick = true;
      }
      else
      {
        myTouchDoubleTapTimer.Stop();
        myTouchDoubleTapTimer.Reset();
        myTouchDoubleTapTimer.Start();
      }

      // emulate mouse click
      UpdateMouseClick(NCollection_Vec2<int>(myTouchClick.From),
                       Aspect_VKeyMouse_LeftButton,
                       Aspect_VKeyFlags_NONE,
                       isDoubleClick);
    }
  }
  myUI.IsNewGesture = true;
  return true;
}

//=================================================================================================

void AIS_ViewController::UpdateTouchPoint(size_t theId, const NCollection_Vec2<double>& thePnt)
{
  Aspect_WindowInputListener::UpdateTouchPoint(theId, thePnt);

  const double aTouchTol = double(myTouchToleranceScale) * double(myTouchClickThresholdPx);
  if (myTouchPoints.Extent() == 1 && (myTouchClick.From - thePnt).cwiseAbs().maxComp() > aTouchTol)
  {
    myTouchClick.From.SetValues(-1.0, -1.0);
  }
}

//=================================================================================================

bool AIS_ViewController::Update3dMouse(const WNT_HIDSpaceMouse& theEvent)
{
  bool toUpdate = false;
  toUpdate      = update3dMouseTranslation(theEvent) || toUpdate;
  toUpdate      = (myToAllowRotation && update3dMouseRotation(theEvent)) || toUpdate;
  toUpdate      = update3dMouseKeys(theEvent) || toUpdate;
  return toUpdate;
}

//=================================================================================================

void AIS_ViewController::SetNavigationMode(AIS_NavigationMode theMode)
{
  myNavigationMode = theMode;

  // abort rotation
  myUI.OrbitRotation.ToStart  = false;
  myUI.OrbitRotation.ToRotate = false;
  myUI.ViewRotation.ToStart   = false;
  myUI.ViewRotation.ToRotate  = false;
}

//=================================================================================================

void AIS_ViewController::KeyDown(Aspect_VKey theKey, double theTime, double thePressure)
{
  Aspect_WindowInputListener::KeyDown(theKey, theTime, thePressure);
}

//=================================================================================================

void AIS_ViewController::KeyUp(Aspect_VKey theKey, double theTime)
{
  Aspect_WindowInputListener::KeyUp(theKey, theTime);
}

//=================================================================================================

void AIS_ViewController::KeyFromAxis(Aspect_VKey theNegative,
                                     Aspect_VKey thePositive,
                                     double      theTime,
                                     double      thePressure)
{
  Aspect_WindowInputListener::KeyFromAxis(theNegative, thePositive, theTime, thePressure);
}

//=================================================================================================

AIS_WalkDelta AIS_ViewController::FetchNavigationKeys(double theCrouchRatio, double theRunRatio)
{
  AIS_WalkDelta aWalk;

  // navigation keys
  double aPrevEventTime = 0.0, aNewEventTime = 0.0;
  updateEventsTime(aPrevEventTime, aNewEventTime);

  double aDuration = 0.0, aPressure = 1.0;
  if (std::abs(myThrustSpeed) > gp::Resolution())
  {
    if (myHasThrust)
    {
      aWalk[AIS_WalkTranslation_Forward].Value = myThrustSpeed * (aNewEventTime - aPrevEventTime);
    }
    myHasThrust      = true;
    myToAskNextFrame = true;
  }
  else
  {
    myHasThrust = false;
  }

  aWalk.SetRunning(theRunRatio > 1.0 && myKeys.IsKeyDown(Aspect_VKey_Shift));
  if (myKeys.HoldDuration(Aspect_VKey_NavJump, aNewEventTime, aDuration))
  {
    myKeys.KeyUp(Aspect_VKey_NavJump, aNewEventTime);
    aWalk.SetDefined(true);
    aWalk.SetJumping(true);
  }
  if (!aWalk.IsJumping() && theCrouchRatio < 1.0
      && myKeys.HoldDuration(Aspect_VKey_NavCrouch, aNewEventTime, aDuration))
  {
    aWalk.SetDefined(true);
    aWalk.SetRunning(false);
    aWalk.SetCrouching(true);
  }

  const double aMaxDuration = aNewEventTime - aPrevEventTime;
  const double aRunRatio    = aWalk.IsRunning()     ? theRunRatio
                              : aWalk.IsCrouching() ? theCrouchRatio
                                                    : 1.0;
  if (myKeys.HoldDuration(Aspect_VKey_NavForward, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Forward].Value += aProgress;
    aWalk[AIS_WalkTranslation_Forward].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Forward].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavBackward, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Forward].Value += -aProgress;
    aWalk[AIS_WalkTranslation_Forward].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Forward].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavSlideLeft, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Side].Value    = -aProgress;
    aWalk[AIS_WalkTranslation_Side].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Side].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavSlideRight, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aProgress *= aRunRatio;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Side].Value    = aProgress;
    aWalk[AIS_WalkTranslation_Side].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Side].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavLookLeft, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Yaw].Value    = aProgress;
    aWalk[AIS_WalkRotation_Yaw].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Yaw].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavLookRight, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Yaw].Value    = -aProgress;
    aWalk[AIS_WalkRotation_Yaw].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Yaw].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavLookUp, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Pitch].Value    = !myToInvertPitch ? -aProgress : aProgress;
    aWalk[AIS_WalkRotation_Pitch].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Pitch].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavLookDown, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Pitch].Value    = !myToInvertPitch ? aProgress : -aProgress;
    aWalk[AIS_WalkRotation_Pitch].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Pitch].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavRollCCW, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Roll].Value    = -aProgress;
    aWalk[AIS_WalkRotation_Roll].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Roll].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavRollCW, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration)) * aPressure;
    aWalk.SetDefined(true);
    aWalk[AIS_WalkRotation_Roll].Value    = aProgress;
    aWalk[AIS_WalkRotation_Roll].Pressure = aPressure;
    aWalk[AIS_WalkRotation_Roll].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavSlideUp, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Up].Value    = aProgress;
    aWalk[AIS_WalkTranslation_Up].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Up].Duration = aDuration;
  }
  if (myKeys.HoldDuration(Aspect_VKey_NavSlideDown, aNewEventTime, aDuration, aPressure))
  {
    double aProgress = std::abs(std::min(aMaxDuration, aDuration));
    aWalk.SetDefined(true);
    aWalk[AIS_WalkTranslation_Up].Value    = -aProgress;
    aWalk[AIS_WalkTranslation_Up].Pressure = aPressure;
    aWalk[AIS_WalkTranslation_Up].Duration = aDuration;
  }
  return aWalk;
}

//=================================================================================================

void AIS_ViewController::AbortViewAnimation()
{
  if (!myViewAnimation.IsNull() && !myViewAnimation->IsStopped())
  {
    myViewAnimation->Stop();
    myViewAnimation->SetView(occ::handle<V3d_View>());
  }
}

//=================================================================================================

void AIS_ViewController::handlePanning(const occ::handle<V3d_View>& theView)
{
  if (!myGL.Panning.ToPan || !myToAllowPanning)
  {
    return;
  }

  AbortViewAnimation();

  const occ::handle<Graphic3d_Camera>& aCam = theView->Camera();
  if (aCam->IsOrthographic() || !hasPanningAnchorPoint())
  {
    theView->Pan(myGL.Panning.Delta.x(), myGL.Panning.Delta.y());
    theView->Invalidate();
    theView->View()->SynchronizeXRPosedToBaseCamera();
    return;
  }

  NCollection_Vec2<int> aWinSize;
  theView->Window()->Size(aWinSize.x(), aWinSize.y());

  const gp_Dir& aDir = aCam->Direction();
  const gp_Ax3  aCameraCS(aCam->Center(), aDir.Reversed(), aDir ^ aCam->Up());
  const gp_XYZ  anEyeToPnt = myPanPnt3d.XYZ() - aCam->Eye().XYZ();
  // clang-format off
  const gp_Pnt aViewDims = aCam->ViewDimensions (anEyeToPnt.Dot (aCam->Direction().XYZ())); // view dimensions at 3D point
  // clang-format on
  const NCollection_Vec2<double> aDxy(
    -aViewDims.X() * myGL.Panning.Delta.x() / double(aWinSize.x()),
    -aViewDims.X() * myGL.Panning.Delta.y() / double(aWinSize.x()));

  // theView->Translate (aCam, aDxy.x(), aDxy.y());
  gp_Trsf      aPanTrsf;
  const gp_Vec aCameraPan =
    gp_Vec(aCameraCS.XDirection()) * aDxy.x() + gp_Vec(aCameraCS.YDirection()) * aDxy.y();
  aPanTrsf.SetTranslation(aCameraPan);
  aCam->Transform(aPanTrsf);
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

//=================================================================================================

void AIS_ViewController::handleZRotate(const occ::handle<V3d_View>& theView)
{
  if (!myGL.ZRotate.ToRotate || !myToAllowRotation)
  {
    return;
  }

  AbortViewAnimation();

  NCollection_Vec2<int> aViewPort;
  theView->Window()->Size(aViewPort.x(), aViewPort.y());
  NCollection_Vec2<double> aRotPnt(0.99 * aViewPort.x(), 0.5 * aViewPort.y());
  theView->StartRotation(int(aRotPnt.x()), int(aRotPnt.y()), 0.4);
  aRotPnt.y() += myGL.ZRotate.Angle * aViewPort.y();
  theView->Rotation(int(aRotPnt.x()), int(aRotPnt.y()));
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

//=================================================================================================

void AIS_ViewController::handleZoom(const occ::handle<V3d_View>& theView,
                                    const Aspect_ScrollDelta&    theParams,
                                    const gp_Pnt*                thePnt)
{
  if (!myToAllowZooming)
  {
    return;
  }

  AbortViewAnimation();

  const occ::handle<Graphic3d_Camera>& aCam = theView->Camera();
  if (thePnt != nullptr)
  {
    const double aViewDist =
      std::max(myMinCamDistance, (thePnt->XYZ() - aCam->Eye().XYZ()).Modulus());
    aCam->SetCenter(aCam->Eye().XYZ() + aCam->Direction().XYZ() * aViewDist);
  }

  if (!theParams.HasPoint())
  {
    double aCoeff = std::abs(theParams.Delta) / 100.0 + 1.0;
    aCoeff        = theParams.Delta > 0.0 ? aCoeff : 1.0 / aCoeff;
    theView->SetZoom(aCoeff, true);
    theView->Invalidate();
    theView->View()->SynchronizeXRPosedToBaseCamera();
    return;
  }

  // integer delta is too rough for small smooth increments
  // theView->StartZoomAtPoint (theParams.Point.x(), theParams.Point.y());
  // theView->ZoomAtPoint (0, 0, (int )theParams.Delta, (int )theParams.Delta);

  double aDZoom = std::abs(theParams.Delta) / 100.0 + 1.0;
  aDZoom        = (theParams.Delta > 0.0) ? aDZoom : 1.0 / aDZoom;
  if (aDZoom <= 0.0)
  {
    return;
  }

  const NCollection_Vec2<double> aViewDims(aCam->ViewDimensions().X(), aCam->ViewDimensions().Y());

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

  NCollection_Vec2<double> aZoomAtPointXYv(0.0, 0.0);
  theView->Convert(theParams.Point.x(),
                   theParams.Point.y(),
                   aZoomAtPointXYv.x(),
                   aZoomAtPointXYv.y());
  NCollection_Vec2<double> aDxy = aZoomAtPointXYv / aCoef;
  aCam->SetScale(aCam->Scale() / aCoef);

  const gp_Dir& aDir = aCam->Direction();
  const gp_Ax3  aCameraCS(aCam->Center(), aDir.Reversed(), aDir ^ aCam->Up());

  // pan back to the point
  aDxy = aZoomAtPointXYv - aDxy;
  if (thePnt != nullptr)
  {
    // zoom at 3D point with perspective projection
    const gp_XYZ anEyeToPnt = thePnt->XYZ() - aCam->Eye().XYZ();
    aDxy.SetValues(anEyeToPnt.Dot(aCameraCS.XDirection().XYZ()),
                   anEyeToPnt.Dot(aCameraCS.YDirection().XYZ()));

    // view dimensions at 3D point
    const gp_Pnt aViewDims1 = aCam->ViewDimensions(anEyeToPnt.Dot(aCam->Direction().XYZ()));

    NCollection_Vec2<int> aWinSize;
    theView->Window()->Size(aWinSize.x(), aWinSize.y());
    const NCollection_Vec2<double> aWinSizeF(aWinSize);
    const NCollection_Vec2<double> aPanFromCenterPx(
      double(theParams.Point.x()) - 0.5 * aWinSizeF.x(),
      aWinSizeF.y() - double(theParams.Point.y()) - 1.0 - 0.5 * aWinSizeF.y());
    aDxy.x() += -aViewDims1.X() * aPanFromCenterPx.x() / aWinSizeF.x();
    aDxy.y() += -aViewDims1.Y() * aPanFromCenterPx.y() / aWinSizeF.y();
  }

  // theView->Translate (aCam, aDxy.x(), aDxy.y());
  gp_Trsf      aPanTrsf;
  const gp_Vec aCameraPan =
    gp_Vec(aCameraCS.XDirection()) * aDxy.x() + gp_Vec(aCameraCS.YDirection()) * aDxy.y();
  aPanTrsf.SetTranslation(aCameraPan);
  aCam->Transform(aPanTrsf);
  theView->Invalidate();
  theView->View()->SynchronizeXRPosedToBaseCamera();
}

//=================================================================================================

void AIS_ViewController::handleZFocusScroll(const occ::handle<V3d_View>& theView,
                                            const Aspect_ScrollDelta&    theParams)
{
  if (!myToAllowZFocus || !theView->Camera()->IsStereo())
  {
    return;
  }

  double aFocus = theView->Camera()->ZFocus() + (theParams.Delta > 0.0 ? 0.05 : -0.05);
  if (aFocus > 0.2 && aFocus < 2.0)
  {
    theView->Camera()->SetZFocus(theView->Camera()->ZFocusType(), aFocus);
    theView->Invalidate();
  }
}

//=================================================================================================

void AIS_ViewController::handleOrbitRotation(const occ::handle<V3d_View>& theView,
                                             const gp_Pnt&                thePnt,
                                             bool                         theToLockZUp)
{
  if (!myToAllowRotation)
  {
    return;
  }

  const occ::handle<Graphic3d_Camera>& aCam =
    theView->View()->IsActiveXR() ? theView->View()->BaseXRCamera() : theView->Camera();
  if (myGL.OrbitRotation.ToStart)
  {
    // default alternatives
    // if (myRotationMode == AIS_RotationMode_BndBoxActive) theView->StartRotation
    // (myGL.RotateAtPoint.x(), myGL.RotateAtPoint.y()); theView->Rotate (0.0, 0.0, 0.0, thePnt.X(),
    // thePnt.Y(), thePnt.Z(), true);

    myRotatePnt3d      = thePnt;
    myCamStartOpUp     = aCam->Up();
    myCamStartOpDir    = aCam->Direction();
    myCamStartOpEye    = aCam->Eye();
    myCamStartOpCenter = aCam->Center();

    theView->Invalidate();
  }

  if (!myGL.OrbitRotation.ToRotate)
  {
    return;
  }

  AbortViewAnimation();

  // Note: gimbal lock can occur when user manually positions camera in specific orientations
  // where myCamStartOpUp.Crossed(myCamStartOpDir) becomes too vertical, causing rotation issues
  if (theToLockZUp)
  {
    // Quaternion-based rotation matching original Euler logic
    NCollection_Vec2<int> aWinXY;
    theView->Window()->Size(aWinXY.x(), aWinXY.y());

    // Calculate deltas exactly as in original
    double aYawAngleDelta =
      ((myGL.OrbitRotation.PointStart.x() - myGL.OrbitRotation.PointTo.x()) / double(aWinXY.x()))
      * (M_PI * 0.5);
    double aPitchAngleDelta =
      ((myGL.OrbitRotation.PointTo.y() - myGL.OrbitRotation.PointStart.y()) / double(aWinXY.y()))
      * (M_PI * 0.5);

    // Z-up locking: clamp pitch to prevent camera flipping at top/bottom
    if (std::abs(aPitchAngleDelta) > gp::Resolution())
    {
      // Calculate current pitch angle from camera direction (like original Euler)
      const double aCurrentPitch = asin(-myCamStartOpDir.Z()); // Negative Z for proper orientation

      // Clamp to +-89 degrees (leave 1 degree margin)
      const double aPitchAngleNew = std::clamp(aCurrentPitch + aPitchAngleDelta,
                                               -M_PI * 0.5 + M_PI / 180.0,
                                               M_PI * 0.5 - M_PI / 180.0);
      aPitchAngleDelta            = aPitchAngleNew - aCurrentPitch; // Use clamped delta
    }

    // Apply transformations only when needed
    const bool hasYaw   = std::abs(aYawAngleDelta) > Precision::Angular();
    const bool hasPitch = std::abs(aPitchAngleDelta) > Precision::Angular();

    if (hasYaw || hasPitch)
    {
      gp_Trsf aCombinedTrsf;

      if (hasYaw)
      {
        gp_Trsf aYawTrsf;
        aYawTrsf.SetRotation(gp_Ax1(myRotatePnt3d, gp::DZ()), aYawAngleDelta);
        aCombinedTrsf.Multiply(aYawTrsf);
      }

      if (hasPitch)
      {
        gp_Trsf aPitchTrsf;
        aPitchTrsf.SetRotation(gp_Ax1(myRotatePnt3d, myCamStartOpUp.Crossed(myCamStartOpDir)),
                               aPitchAngleDelta);
        aCombinedTrsf.Multiply(aPitchTrsf);
      }

      // Apply to camera using the original vectors
      aCam->SetUp(myCamStartOpUp.Transformed(aCombinedTrsf));
      aCam->SetEyeAndCenter(
        myRotatePnt3d.XYZ()
          + gp_Vec(myRotatePnt3d, myCamStartOpEye).Transformed(aCombinedTrsf).XYZ(),
        myRotatePnt3d.XYZ()
          + gp_Vec(myRotatePnt3d, myCamStartOpCenter).Transformed(aCombinedTrsf).XYZ());
    }
    else
    {
      // No rotation needed - use original vectors
      aCam->SetUp(myCamStartOpUp);
      aCam->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
    }

    aCam->OrthogonalizeUp();
  }
  else
  {
    // default alternatives
    // if (myRotationMode == AIS_RotationMode_BndBoxActive) theView->Rotation
    // (myGL.RotateToPoint.x(), myGL.RotateToPoint.y()); theView->Rotate (aDX, aDY, aDZ,
    // myRotatePnt3d.X(), myRotatePnt3d.Y(), myRotatePnt3d.Z(), false);

    // restore previous camera state
    aCam->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
    aCam->SetUp(myCamStartOpUp);
    aCam->SetDirectionFromEye(myCamStartOpDir);

    NCollection_Vec2<double> aWinXY;
    theView->Size(aWinXY.x(), aWinXY.y());
    const double rx = (double)theView->Convert(aWinXY.x());
    const double ry = (double)theView->Convert(aWinXY.y());

    const double THE_2PI = M_PI * 2.0;
    double aDX = (myGL.OrbitRotation.PointTo.x() - myGL.OrbitRotation.PointStart.x()) * M_PI / rx;
    double aDY = (myGL.OrbitRotation.PointStart.y() - myGL.OrbitRotation.PointTo.y()) * M_PI / ry;

    if (aDX > 0.0)
    {
      while (aDX > THE_2PI)
      {
        aDX -= THE_2PI;
      }
    }
    else if (aDX < 0.0)
    {
      while (aDX < -THE_2PI)
      {
        aDX += THE_2PI;
      }
    }
    if (aDY > 0.0)
    {
      while (aDY > THE_2PI)
      {
        aDY -= THE_2PI;
      }
    }
    else if (aDY < 0.0)
    {
      while (aDY < -THE_2PI)
      {
        aDY += THE_2PI;
      }
    }

    // rotate camera around 3 initial axes
    gp_Dir aCamDir(aCam->Direction().Reversed());
    gp_Dir aCamUp(aCam->Up());
    gp_Dir aCamSide(aCamUp.Crossed(aCamDir));

    gp_Trsf aRot[2], aTrsf;
    aRot[0].SetRotation(gp_Ax1(myRotatePnt3d, aCamUp), -aDX);
    aRot[1].SetRotation(gp_Ax1(myRotatePnt3d, aCamSide), aDY);
    aTrsf.Multiply(aRot[0]);
    aTrsf.Multiply(aRot[1]);

    aCam->Transform(aTrsf);
  }

  theView->Invalidate();
  theView->View()->SynchronizeXRBaseToPosedCamera();
}

//=================================================================================================

void AIS_ViewController::handleViewRotation(const occ::handle<V3d_View>& theView,
                                            double                       theYawExtra,
                                            double                       thePitchExtra,
                                            double                       theRoll,
                                            bool                         theToRestartOnIncrement)
{
  if (!myToAllowRotation)
  {
    return;
  }

  const occ::handle<Graphic3d_Camera>& aCam = theView->Camera();
  const bool aRollIsChanged = std::abs(theRoll - myCurrentRollAngle) > gp::Resolution();
  const bool toRotateAnyway = std::abs(theYawExtra) > gp::Resolution()
                              || std::abs(thePitchExtra) > gp::Resolution() || aRollIsChanged;

  // Store old and new roll values for later processing
  const double anOldRollAngle = myCurrentRollAngle;
  const double aNewRollAngle  = theRoll;
  if (toRotateAnyway && theToRestartOnIncrement)
  {
    myGL.ViewRotation.ToStart = true;
    myGL.ViewRotation.PointTo = myGL.ViewRotation.PointStart;
  }

  if (myGL.ViewRotation.ToStart)
  {
    // Store initial camera state
    myCamStartOpUp  = aCam->Up();
    myCamStartOpDir = aCam->Direction();
  }

  if (toRotateAnyway)
  {
    myGL.ViewRotation.ToRotate = true;
    myCurrentRollAngle         = theRoll;
  }

  if (!myGL.ViewRotation.ToRotate)
  {
    return;
  }

  AbortViewAnimation();

  // Calculate rotation deltas from mouse movement
  NCollection_Vec2<int> aWinXY;
  theView->Window()->Size(aWinXY.x(), aWinXY.y());
  double aYawAngleDelta =
    ((myGL.ViewRotation.PointStart.x() - myGL.ViewRotation.PointTo.x()) / double(aWinXY.x()))
    * (M_PI * 0.5);
  double aPitchAngleDelta =
    -((myGL.ViewRotation.PointStart.y() - myGL.ViewRotation.PointTo.y()) / double(aWinXY.y()))
    * (M_PI * 0.5);

  // Add extra rotation from parameters
  aYawAngleDelta += theYawExtra;
  aPitchAngleDelta += thePitchExtra;

  // Clamp pitch to prevent gimbal lock
  const double aCurrentPitch      = asin(-myCamStartOpDir.Z());
  const double aPitchAngleClamped = std::clamp(aCurrentPitch + aPitchAngleDelta,
                                               -M_PI * 0.5 + M_PI / 180.0,
                                               M_PI * 0.5 - M_PI / 180.0);
  aPitchAngleDelta                = aPitchAngleClamped - aCurrentPitch;

  // Apply yaw and pitch transformations to stored camera vectors
  gp_Dir aBaseUp  = myCamStartOpUp;
  gp_Dir aBaseDir = myCamStartOpDir;

  const bool hasYaw   = std::abs(aYawAngleDelta) > Precision::Angular();
  const bool hasPitch = std::abs(aPitchAngleDelta) > Precision::Angular();

  if (hasYaw || hasPitch)
  {
    gp_Trsf aCombinedTrsf;

    if (hasYaw)
    {
      gp_Trsf aYawTrsf;
      aYawTrsf.SetRotation(gp_Ax1(gp::Origin(), gp::DZ()), aYawAngleDelta);
      aCombinedTrsf.Multiply(aYawTrsf);
    }

    if (hasPitch)
    {
      gp_Trsf      aPitchTrsf;
      const gp_Vec aPitchAxis = myCamStartOpUp.Crossed(myCamStartOpDir);
      aPitchTrsf.SetRotation(gp_Ax1(gp::Origin(), aPitchAxis), aPitchAngleDelta);
      aCombinedTrsf.Multiply(aPitchTrsf);
    }

    aBaseUp  = myCamStartOpUp.Transformed(aCombinedTrsf);
    aBaseDir = myCamStartOpDir.Transformed(aCombinedTrsf);
  }

  // Apply roll transformation - handle old/new roll if changed
  if (aRollIsChanged)
  {
    // First remove old roll, then apply new roll
    gp_Trsf anInverseOldRollTrsf;
    anInverseOldRollTrsf.SetRotation(gp_Ax1(gp::Origin(), aBaseDir), -anOldRollAngle);
    aBaseUp = aBaseUp.Transformed(anInverseOldRollTrsf);

    gp_Trsf aNewRollTrsf;
    aNewRollTrsf.SetRotation(gp_Ax1(gp::Origin(), aBaseDir), aNewRollAngle);
    aBaseUp = aBaseUp.Transformed(aNewRollTrsf);
  }

  aCam->SetUp(aBaseUp);
  aCam->SetDirectionFromEye(aBaseDir);
  aCam->OrthogonalizeUp();
  theView->Invalidate();
}

//=================================================================================================

bool AIS_ViewController::PickPoint(gp_Pnt&                                    thePnt,
                                   const occ::handle<AIS_InteractiveContext>& theCtx,
                                   const occ::handle<V3d_View>&               theView,
                                   const NCollection_Vec2<int>&               theCursor,
                                   bool                                       theToStickToPickRay)
{
  ResetPreviousMoveTo();

  const occ::handle<StdSelect_ViewerSelector3d>& aSelector = theCtx->MainSelector();
  aSelector->Pick(theCursor.x(), theCursor.y(), theView);
  if (aSelector->NbPicked() < 1)
  {
    return false;
  }

  const SelectMgr_SortCriterion& aPicked = aSelector->PickedData(1);
  if (theToStickToPickRay && !Precision::IsInfinite(aPicked.Depth))
  {
    thePnt = aSelector->GetManager().DetectedPoint(aPicked.Depth);
  }
  else
  {
    thePnt = aSelector->PickedPoint(1);
  }
  return !Precision::IsInfinite(thePnt.X()) && !Precision::IsInfinite(thePnt.Y())
         && !Precision::IsInfinite(thePnt.Z());
}

//=================================================================================================

bool AIS_ViewController::PickAxis(gp_Pnt&                                    theTopPnt,
                                  const occ::handle<AIS_InteractiveContext>& theCtx,
                                  const occ::handle<V3d_View>&               theView,
                                  const gp_Ax1&                              theAxis)
{
  ResetPreviousMoveTo();

  const occ::handle<StdSelect_ViewerSelector3d>& aSelector = theCtx->MainSelector();
  aSelector->Pick(theAxis, theView);
  if (aSelector->NbPicked() < 1)
  {
    return false;
  }

  const SelectMgr_SortCriterion& aPickedData = aSelector->PickedData(1);
  theTopPnt                                  = aPickedData.Point;
  return !Precision::IsInfinite(theTopPnt.X()) && !Precision::IsInfinite(theTopPnt.Y())
         && !Precision::IsInfinite(theTopPnt.Z());
}

//=================================================================================================

gp_Pnt AIS_ViewController::GravityPoint(const occ::handle<AIS_InteractiveContext>& theCtx,
                                        const occ::handle<V3d_View>&               theView)
{
  switch (myRotationMode)
  {
    case AIS_RotationMode_PickLast:
    case AIS_RotationMode_PickCenter: {
      NCollection_Vec2<int> aCursor((int)myGL.OrbitRotation.PointStart.x(),
                                    (int)myGL.OrbitRotation.PointStart.y());
      if (myRotationMode == AIS_RotationMode_PickCenter)
      {
        NCollection_Vec2<int> aViewPort;
        theView->Window()->Size(aViewPort.x(), aViewPort.y());
        aCursor = aViewPort / 2;
      }

      gp_Pnt aPnt;
      if (PickPoint(aPnt, theCtx, theView, aCursor, myToStickToRayOnRotation))
      {
        return aPnt;
      }
      break;
    }
    case AIS_RotationMode_CameraAt: {
      const occ::handle<Graphic3d_Camera>& aCam = theView->Camera();
      return aCam->Center();
    }
    case AIS_RotationMode_BndBoxScene: {
      Bnd_Box aBndBox = theView->View()->MinMaxValues(false);
      if (!aBndBox.IsVoid())
      {
        return (aBndBox.CornerMin().XYZ() + aBndBox.CornerMax().XYZ()) * 0.5;
      }
      break;
    }
    case AIS_RotationMode_BndBoxActive:
      break;
  }

  return theCtx->GravityPoint(theView);
}

//=================================================================================================

void AIS_ViewController::FitAllAuto(const occ::handle<AIS_InteractiveContext>& theCtx,
                                    const occ::handle<V3d_View>&               theView)
{
  const Bnd_Box aBoxSel    = theCtx->BoundingBoxOfSelection(theView);
  const double  aFitMargin = 0.01;
  if (aBoxSel.IsVoid())
  {
    theView->FitAll(aFitMargin, false);
    return;
  }

  // fit all algorithm is not 100% stable - so compute some precision to compare equal camera values
  const double aFitTol =
    (aBoxSel.CornerMax().XYZ() - aBoxSel.CornerMin().XYZ()).Modulus() * 0.000001;
  const Bnd_Box aBoxAll = theView->View()->MinMaxValues();

  const occ::handle<Graphic3d_Camera>& aCam       = theView->Camera();
  occ::handle<Graphic3d_Camera>        aCameraSel = new Graphic3d_Camera(aCam);
  occ::handle<Graphic3d_Camera>        aCameraAll = new Graphic3d_Camera(aCam);
  theView->FitMinMax(aCameraSel, aBoxSel, aFitMargin);
  theView->FitMinMax(aCameraAll, aBoxAll, aFitMargin);
  if (aCameraSel->Center().IsEqual(aCam->Center(), aFitTol)
      && std::abs(aCameraSel->Scale() - aCam->Scale()) < aFitTol
      && std::abs(aCameraSel->Distance() - aCam->Distance()) < aFitTol)
  {
    // fit all entire view on second FitALL request
    aCam->Copy(aCameraAll);
  }
  else
  {
    aCam->Copy(aCameraSel);
  }
}

//=================================================================================================

void AIS_ViewController::handleViewOrientationKeys(
  const occ::handle<AIS_InteractiveContext>& theCtx,
  const occ::handle<V3d_View>&               theView)
{
  if (myNavigationMode == AIS_NavigationMode_FirstPersonWalk)
  {
    return;
  }

  occ::handle<Graphic3d_Camera> aCameraBack;

  struct ViewKeyAction
  {
    Aspect_VKey           Key;
    V3d_TypeOfOrientation Orientation;
  };

  static const ViewKeyAction THE_VIEW_KEYS[] = {
    {Aspect_VKey_ViewTop, V3d_TypeOfOrientation_Zup_Top},
    {Aspect_VKey_ViewBottom, V3d_TypeOfOrientation_Zup_Bottom},
    {Aspect_VKey_ViewLeft, V3d_TypeOfOrientation_Zup_Left},
    {Aspect_VKey_ViewRight, V3d_TypeOfOrientation_Zup_Right},
    {Aspect_VKey_ViewFront, V3d_TypeOfOrientation_Zup_Front},
    {Aspect_VKey_ViewBack, V3d_TypeOfOrientation_Zup_Back},
    {Aspect_VKey_ViewAxoLeftProj, V3d_TypeOfOrientation_Zup_AxoLeft},
    {Aspect_VKey_ViewAxoRightProj, V3d_TypeOfOrientation_Zup_AxoRight},
    {Aspect_VKey_ViewRoll90CW, (V3d_TypeOfOrientation)-1},
    {Aspect_VKey_ViewRoll90CCW, (V3d_TypeOfOrientation)-1},
    {Aspect_VKey_ViewFitAll, (V3d_TypeOfOrientation)-1}};
  {
    const size_t aNbKeys     = sizeof(THE_VIEW_KEYS) / sizeof(*THE_VIEW_KEYS);
    const double anEventTime = EventTime();
    for (size_t aKeyIter = 0; aKeyIter < aNbKeys; ++aKeyIter)
    {
      const ViewKeyAction& aKeyAction = THE_VIEW_KEYS[aKeyIter];
      if (!myKeys.IsKeyDown(aKeyAction.Key))
      {
        continue;
      }

      myKeys.KeyUp(aKeyAction.Key, anEventTime);
      if (aCameraBack.IsNull())
      {
        aCameraBack = theView->Camera();
        theView->SetCamera(new Graphic3d_Camera(aCameraBack));
      }
      if (aKeyAction.Orientation != (V3d_TypeOfOrientation)-1)
      {
        theView->SetProj(aKeyAction.Orientation);
        FitAllAuto(theCtx, theView);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewRoll90CW)
      {
        const double aTwist = theView->Twist() + M_PI / 2.0;
        theView->SetTwist(aTwist);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewRoll90CCW)
      {
        const double aTwist = theView->Twist() - M_PI / 2.0;
        theView->SetTwist(aTwist);
      }
      else if (aKeyAction.Key == Aspect_VKey_ViewFitAll)
      {
        FitAllAuto(theCtx, theView);
      }
    }
  }

  if (aCameraBack.IsNull())
  {
    return;
  }

  occ::handle<Graphic3d_Camera> aCameraNew = theView->Camera();
  theView->SetCamera(aCameraBack);
  const NCollection_Mat4<double> anOrientMat1 = aCameraBack->OrientationMatrix();
  const NCollection_Mat4<double> anOrientMat2 = aCameraNew->OrientationMatrix();
  if (anOrientMat1 != anOrientMat2)
  {
    const occ::handle<AIS_AnimationCamera>& aCamAnim = myViewAnimation;
    aCamAnim->SetView(theView);
    aCamAnim->SetStartPts(0.0);
    aCamAnim->SetCameraStart(new Graphic3d_Camera(aCameraBack));
    aCamAnim->SetCameraEnd(new Graphic3d_Camera(aCameraNew));
    aCamAnim->StartTimer(0.0, 1.0, true, false);
  }
}

//=================================================================================================

AIS_WalkDelta AIS_ViewController::handleNavigationKeys(const occ::handle<AIS_InteractiveContext>&,
                                                       const occ::handle<V3d_View>& theView)
{
  // navigation keys
  double aCrouchRatio = 1.0, aRunRatio = 1.0;
  if (myNavigationMode == AIS_NavigationMode_FirstPersonFlight)
  {
    aRunRatio = 3.0;
  }

  const double  aRotSpeed      = 0.5;
  const double  aWalkSpeedCoef = WalkSpeedRelative();
  AIS_WalkDelta aWalk          = FetchNavigationKeys(aCrouchRatio, aRunRatio);
  if (aWalk.IsJumping())
  {
    // ask more frames
    setAskNextFrame();
    theView->Invalidate();
  }
  if (aWalk.IsEmpty())
  {
    if (aWalk.IsDefined())
    {
      setAskNextFrame();
    }
    return aWalk;
  }
  else if (myGL.OrbitRotation.ToRotate || myGL.OrbitRotation.ToStart)
  {
    return aWalk;
  }

  gp_XYZ        aMin, aMax;
  const Bnd_Box aBndBox = theView->View()->MinMaxValues();
  if (!aBndBox.IsVoid())
  {
    aMin = aBndBox.CornerMin().XYZ();
    aMax = aBndBox.CornerMax().XYZ();
  }
  double aBndDiam =
    std::max(std::max(aMax.X() - aMin.X(), aMax.Y() - aMin.Y()), aMax.Z() - aMin.Z());
  if (aBndDiam <= gp::Resolution())
  {
    aBndDiam = 0.001;
  }

  const double                         aWalkSpeed = myNavigationMode != AIS_NavigationMode_Orbit
                                && myNavigationMode != AIS_NavigationMode_FirstPersonFlight
                                                      ? theView->View()->UnitFactor() * WalkSpeedAbsolute()
                                                      : aWalkSpeedCoef * aBndDiam;
  const occ::handle<Graphic3d_Camera>& aCam =
    theView->View()->IsActiveXR() ? theView->View()->BaseXRCamera() : theView->Camera();

  // move forward in plane XY and up along Z
  const gp_Dir anUp = ToLockOrbitZUp() ? gp::DZ() : aCam->OrthogonalizedUp();
  if (aWalk.ToMove() && myToAllowPanning)
  {
    const gp_Vec aSide = -aCam->SideRight();
    gp_XYZ       aFwd  = aCam->Direction().XYZ();
    aFwd -= anUp.XYZ() * (anUp.XYZ() * aFwd);

    gp_XYZ aMoveVec;
    if (!aWalk[AIS_WalkTranslation_Forward].IsEmpty())
    {
      if (!aCam->IsOrthographic())
      {
        aMoveVec += aFwd * aWalk[AIS_WalkTranslation_Forward].Value
                    * aWalk[AIS_WalkTranslation_Forward].Pressure * aWalkSpeed;
      }
    }
    if (!aWalk[AIS_WalkTranslation_Side].IsEmpty())
    {
      aMoveVec += aSide.XYZ() * aWalk[AIS_WalkTranslation_Side].Value
                  * aWalk[AIS_WalkTranslation_Side].Pressure * aWalkSpeed;
    }
    if (!aWalk[AIS_WalkTranslation_Up].IsEmpty())
    {
      aMoveVec += anUp.XYZ() * aWalk[AIS_WalkTranslation_Up].Value
                  * aWalk[AIS_WalkTranslation_Up].Pressure * aWalkSpeed;
    }
    {
      if (aCam->IsOrthographic())
      {
        if (!aWalk[AIS_WalkTranslation_Forward].IsEmpty())
        {
          const double aZoomDelta = aWalk[AIS_WalkTranslation_Forward].Value
                                    * aWalk[AIS_WalkTranslation_Forward].Pressure * aWalkSpeedCoef;
          handleZoom(theView, Aspect_ScrollDelta(aZoomDelta * 100.0), nullptr);
        }
      }

      gp_Trsf aTrsfTranslate;
      aTrsfTranslate.SetTranslation(aMoveVec);
      aCam->Transform(aTrsfTranslate);
    }
  }

  if (myNavigationMode == AIS_NavigationMode_Orbit && myToAllowRotation)
  {
    if (!aWalk[AIS_WalkRotation_Yaw].IsEmpty())
    {
      gp_Trsf aTrsfRot;
      aTrsfRot.SetRotation(gp_Ax1(aCam->Eye(), anUp),
                           aWalk[AIS_WalkRotation_Yaw].Value * aRotSpeed);
      aCam->Transform(aTrsfRot);
    }
    if (!aWalk[AIS_WalkRotation_Pitch].IsEmpty())
    {
      const gp_Vec aSide = -aCam->SideRight();
      gp_Trsf      aTrsfRot;
      aTrsfRot.SetRotation(gp_Ax1(aCam->Eye(), aSide),
                           -aWalk[AIS_WalkRotation_Pitch].Value * aRotSpeed);
      aCam->Transform(aTrsfRot);
    }
    if (!aWalk[AIS_WalkRotation_Roll].IsEmpty() && !ToLockOrbitZUp())
    {
      gp_Trsf aTrsfRot;
      aTrsfRot.SetRotation(gp_Ax1(aCam->Center(), aCam->Direction()),
                           aWalk[AIS_WalkRotation_Roll].Value * aRotSpeed);
      aCam->Transform(aTrsfRot);
    }
  }

  // ask more frames
  setAskNextFrame();
  theView->Invalidate();
  theView->View()->SynchronizeXRBaseToPosedCamera();
  return aWalk;
}

//=================================================================================================

void AIS_ViewController::handleCameraActions(const occ::handle<AIS_InteractiveContext>& theCtx,
                                             const occ::handle<V3d_View>&               theView,
                                             const AIS_WalkDelta&                       theWalk)
{
  // apply view actions
  if (myGL.Orientation.ToSetViewOrient)
  {
    theView->SetProj(myGL.Orientation.ViewOrient);
    myGL.Orientation.ToFitAll = true;
  }

  // apply fit all
  if (myGL.Orientation.ToFitAll)
  {
    const double aFitMargin = 0.01;
    theView->FitAll(aFitMargin, false);
    theView->Invalidate();
    myGL.Orientation.ToFitAll = false;
  }

  NCollection_List<occ::handle<AIS_InteractiveObject>> anObjects;
  theCtx->DisplayedObjects(anObjects);
  for (NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator anObjIter(anObjects);
       anObjIter.More();
       anObjIter.Next())
  {
    anObjIter.Value()->RecomputeTransformation(theView->Camera());
  }

  if (myGL.IsNewGesture)
  {
    if (myAnchorPointPrs1->HasInteractiveContext())
    {
      theCtx->Remove(myAnchorPointPrs1, false);
      if (!theView->Viewer()->ZLayerSettings(myAnchorPointPrs1->ZLayer()).IsImmediate())
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
      theCtx->Remove(myAnchorPointPrs2, false);
      if (!theView->Viewer()->ZLayerSettings(myAnchorPointPrs2->ZLayer()).IsImmediate())
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
      theView->SetComputedMode(true);
      theView->Invalidate();
    }
  }

  if (myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    if (myGL.Panning.ToStart && myToAllowPanning)
    {
      gp_Pnt aPanPnt(Precision::Infinite(), 0.0, 0.0);
      if (!theView->Camera()->IsOrthographic())
      {
        bool toStickToRay = false;
        if (myGL.Panning.PointStart.x() >= 0 && myGL.Panning.PointStart.y() >= 0)
        {
          PickPoint(aPanPnt, theCtx, theView, myGL.Panning.PointStart, toStickToRay);
        }
        if (Precision::IsInfinite(aPanPnt.X()))
        {
          NCollection_Vec2<int> aWinSize;
          theView->Window()->Size(aWinSize.x(), aWinSize.y());
          PickPoint(aPanPnt, theCtx, theView, aWinSize / 2, toStickToRay);
        }
        if (!Precision::IsInfinite(aPanPnt.X()) && myToShowPanAnchorPoint)
        {
          gp_Trsf aPntTrsf;
          aPntTrsf.SetTranslation(gp_Vec(aPanPnt.XYZ()));
          theCtx->SetLocation(myAnchorPointPrs2, aPntTrsf);
        }
      }
      setPanningAnchorPoint(aPanPnt);
    }

    if (myToShowPanAnchorPoint && hasPanningAnchorPoint() && myGL.Panning.ToPan
        && !myGL.IsNewGesture && !myAnchorPointPrs2->HasInteractiveContext())
    {
      theCtx->Display(myAnchorPointPrs2, 0, -1, false, AIS_DS_Displayed);
    }

    handlePanning(theView);
    handleZRotate(theView);
  }

  if ((myNavigationMode == AIS_NavigationMode_Orbit || myGL.OrbitRotation.ToStart
       || myGL.OrbitRotation.ToRotate)
      && myToAllowRotation)
  {
    if (myGL.OrbitRotation.ToStart && !myHasHlrOnBeforeRotation)
    {
      myHasHlrOnBeforeRotation = theView->ComputedMode();
      if (myHasHlrOnBeforeRotation)
      {
        theView->SetComputedMode(false);
      }
    }

    gp_Pnt aGravPnt;
    if (myGL.OrbitRotation.ToStart)
    {
      aGravPnt = GravityPoint(theCtx, theView);
      if (myToShowRotateCenter)
      {
        gp_Trsf aPntTrsf;
        aPntTrsf.SetTranslation(gp_Vec(aGravPnt.XYZ()));
        theCtx->SetLocation(myAnchorPointPrs1, aPntTrsf);
        theCtx->SetLocation(myAnchorPointPrs2, aPntTrsf);
      }
    }

    if (myToShowRotateCenter && myGL.OrbitRotation.ToRotate && !myGL.IsNewGesture
        && !myAnchorPointPrs1->HasInteractiveContext())
    {
      theCtx->Display(myAnchorPointPrs1, 0, -1, false, AIS_DS_Displayed);
      theCtx->Display(myAnchorPointPrs2, 0, -1, false, AIS_DS_Displayed);
    }
    handleOrbitRotation(theView,
                        aGravPnt,
                        myToLockOrbitZUp || myNavigationMode != AIS_NavigationMode_Orbit);
  }

  if ((myNavigationMode != AIS_NavigationMode_Orbit || myGL.ViewRotation.ToStart
       || myGL.ViewRotation.ToRotate)
      && myToAllowRotation)
  {
    if (myGL.ViewRotation.ToStart && !myHasHlrOnBeforeRotation)
    {
      myHasHlrOnBeforeRotation = theView->ComputedMode();
      if (myHasHlrOnBeforeRotation)
      {
        theView->SetComputedMode(false);
      }
    }

    double aRoll = 0.0;
    if (!theWalk[AIS_WalkRotation_Roll].IsEmpty() && !myToLockOrbitZUp)
    {
      aRoll = (M_PI / 12.0) * theWalk[AIS_WalkRotation_Roll].Pressure;
      aRoll *= std::min(1000.0 * theWalk[AIS_WalkRotation_Roll].Duration, 100.0) / 100.0;
      if (theWalk[AIS_WalkRotation_Roll].Value < 0.0)
      {
        aRoll = -aRoll;
      }
    }

    handleViewRotation(theView,
                       theWalk[AIS_WalkRotation_Yaw].Value,
                       theWalk[AIS_WalkRotation_Pitch].Value,
                       aRoll,
                       myNavigationMode == AIS_NavigationMode_FirstPersonFlight);
  }

  if (!myGL.ZoomActions.IsEmpty())
  {
    for (NCollection_Sequence<Aspect_ScrollDelta>::Iterator aZoomIter(myGL.ZoomActions);
         aZoomIter.More();
         aZoomIter.Next())
    {
      Aspect_ScrollDelta aZoomParams = aZoomIter.Value();
      if (myToAllowZFocus && (aZoomParams.Flags & Aspect_VKeyFlags_CTRL) != 0
          && theView->Camera()->IsStereo())
      {
        handleZFocusScroll(theView, aZoomParams);
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
            && PickPoint(aPnt, theCtx, theView, aZoomParams.Point, myToStickToRayOnZoom))
        {
          handleZoom(theView, aZoomParams, &aPnt);
          continue;
        }

        NCollection_Vec2<int> aWinSize;
        theView->Window()->Size(aWinSize.x(), aWinSize.y());
        if (PickPoint(aPnt, theCtx, theView, aWinSize / 2, myToStickToRayOnZoom))
        {
          aZoomParams.ResetPoint(); // do not pretend to zoom at 'nothing'
          handleZoom(theView, aZoomParams, &aPnt);
          continue;
        }
      }
      handleZoom(theView, aZoomParams, nullptr);
    }
    myGL.ZoomActions.Clear();
  }
}

//=================================================================================================

void AIS_ViewController::handleXRInput(const occ::handle<AIS_InteractiveContext>& theCtx,
                                       const occ::handle<V3d_View>&               theView,
                                       const AIS_WalkDelta&)
{
  theView->View()->ProcessXRInput();
  if (!theView->View()->IsActiveXR())
  {
    return;
  }
  handleXRTurnPad(theCtx, theView);
  handleXRTeleport(theCtx, theView);
  handleXRPicking(theCtx, theView);
}

//=================================================================================================

void AIS_ViewController::handleXRTurnPad(const occ::handle<AIS_InteractiveContext>&,
                                         const occ::handle<V3d_View>& theView)
{
  if (myXRTurnAngle <= 0.0 || !theView->View()->IsActiveXR())
  {
    return;
  }

  // turn left/right at 45 degrees on left/right trackpad clicks
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole =
      aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const occ::handle<Aspect_XRAction>& aPadClickAct =
      theView->View()->XRSession()->GenericAction(aRole, Aspect_XRGenericAction_InputTrackPadClick);
    const occ::handle<Aspect_XRAction>& aPadPosAct =
      theView->View()->XRSession()->GenericAction(aRole,
                                                  Aspect_XRGenericAction_InputTrackPadPosition);
    if (aPadClickAct.IsNull() || aPadPosAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aPadClick =
      theView->View()->XRSession()->GetDigitalActionData(aPadClickAct);
    const Aspect_XRAnalogActionData aPadPos =
      theView->View()->XRSession()->GetAnalogActionData(aPadPosAct);
    if (aPadClick.IsActive && aPadClick.IsPressed && aPadClick.IsChanged && aPadPos.IsActive
        && std::abs(aPadPos.VecXYZ.y()) < 0.5f && std::abs(aPadPos.VecXYZ.x()) > 0.7f)
    {
      gp_Trsf aTrsfTurn;
      aTrsfTurn.SetRotation(gp_Ax1(gp::Origin(), theView->View()->BaseXRCamera()->Up()),
                            aPadPos.VecXYZ.x() < 0.0f ? myXRTurnAngle : -myXRTurnAngle);
      theView->View()->TurnViewXRCamera(aTrsfTurn);
      break;
    }
  }
}

//=================================================================================================

void AIS_ViewController::handleXRTeleport(const occ::handle<AIS_InteractiveContext>& theCtx,
                                          const occ::handle<V3d_View>&               theView)
{
  if (!theView->View()->IsActiveXR())
  {
    return;
  }

  // teleport on forward trackpad unclicks
  const Aspect_XRTrackedDeviceRole aTeleOld = myXRLastTeleportHand;
  myXRLastTeleportHand                      = Aspect_XRTrackedDeviceRole_Other;
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole =
      aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const int aDeviceId = theView->View()->XRSession()->NamedTrackedDevice(aRole);
    if (aDeviceId == -1)
    {
      continue;
    }

    const occ::handle<Aspect_XRAction>& aPadClickAct =
      theView->View()->XRSession()->GenericAction(aRole, Aspect_XRGenericAction_InputTrackPadClick);
    const occ::handle<Aspect_XRAction>& aPadPosAct =
      theView->View()->XRSession()->GenericAction(aRole,
                                                  Aspect_XRGenericAction_InputTrackPadPosition);
    if (aPadClickAct.IsNull() || aPadPosAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aPadClick =
      theView->View()->XRSession()->GetDigitalActionData(aPadClickAct);
    const Aspect_XRAnalogActionData aPadPos =
      theView->View()->XRSession()->GetAnalogActionData(aPadPosAct);
    const bool isPressed = aPadClick.IsPressed;
    const bool isClicked = !aPadClick.IsPressed && aPadClick.IsChanged;
    if (aPadClick.IsActive && (isPressed || isClicked) && aPadPos.IsActive
        && aPadPos.VecXYZ.y() > 0.6f && std::abs(aPadPos.VecXYZ.x()) < 0.5f)
    {
      const Aspect_TrackedDevicePose& aPose =
        theView->View()->XRSession()->TrackedPoses()[aDeviceId];
      if (!aPose.IsValidPose)
      {
        continue;
      }

      myXRLastTeleportHand = aRole;
      double& aPickDepth   = aRole == Aspect_XRTrackedDeviceRole_LeftHand ? myXRLastPickDepthLeft
                                                                          : myXRLastPickDepthRight;
      aPickDepth           = Precision::Infinite();
      NCollection_Vec3<float> aPickNorm;
      const gp_Trsf           aHandBase = theView->View()->PoseXRToWorld(aPose.Orientation);
      const double aHeadHeight = theView->View()->XRSession()->HeadPose().TranslationPart().Y();
      {
        const int aPickedId = handleXRMoveTo(theCtx, theView, aPose.Orientation, false);
        if (aPickedId >= 1)
        {
          const SelectMgr_SortCriterion& aPickedData =
            theCtx->MainSelector()->PickedData(aPickedId);
          aPickNorm = aPickedData.Normal;
          if (aPickNorm.SquareModulus() > ShortRealEpsilon())
          {
            aPickDepth = aPickedData.Point.Distance(aHandBase.TranslationPart());
          }
        }
      }
      if (isClicked)
      {
        myXRLastTeleportHand = Aspect_XRTrackedDeviceRole_Other;
        if (!Precision::IsInfinite(aPickDepth))
        {
          const gp_Dir aTeleDir = -gp::DZ().Transformed(aHandBase);
          const gp_Dir anUpDir  = theView->View()->BaseXRCamera()->Up();

          bool   isHorizontal = false;
          gp_Dir aPickNormDir(aPickNorm.x(), aPickNorm.y(), aPickNorm.z());
          if (anUpDir.IsEqual(aPickNormDir, M_PI_4) || anUpDir.IsEqual(-aPickNormDir, M_PI_4))
          {
            isHorizontal = true;
          }

          gp_Pnt aNewEye = aHandBase.TranslationPart();
          if (isHorizontal)
          {
            aNewEye = aHandBase.TranslationPart() + aTeleDir.XYZ() * aPickDepth
                      + anUpDir.XYZ() * aHeadHeight;
          }
          else
          {
            if (aPickNormDir.Dot(aTeleDir) < 0.0)
            {
              aPickNormDir.Reverse();
            }
            aNewEye = aHandBase.TranslationPart() + aTeleDir.XYZ() * aPickDepth
                      - aPickNormDir.XYZ() * aHeadHeight / 4;
          }

          theView->View()->PosedXRCamera()->MoveEyeTo(aNewEye);
          theView->View()->ComputeXRBaseCameraFromPosed(theView->View()->PosedXRCamera(),
                                                        theView->View()->XRSession()->HeadPose());
        }
      }
      break;
    }
  }

  if (myXRLastTeleportHand != aTeleOld)
  {
    if (aTeleOld != Aspect_XRTrackedDeviceRole_Other)
    {
      if (const occ::handle<Aspect_XRAction>& aHaptic =
            theView->View()->XRSession()->GenericAction(aTeleOld,
                                                        Aspect_XRGenericAction_OutputHaptic))
      {
        theView->View()->XRSession()->AbortHapticVibrationAction(aHaptic);
      }
    }
    if (myXRLastTeleportHand != Aspect_XRTrackedDeviceRole_Other)
    {
      if (const occ::handle<Aspect_XRAction>& aHaptic =
            theView->View()->XRSession()->GenericAction(myXRLastTeleportHand,
                                                        Aspect_XRGenericAction_OutputHaptic))
      {
        theView->View()->XRSession()->TriggerHapticVibrationAction(aHaptic, myXRTeleportHaptic);
      }
    }
  }
}

//=================================================================================================

void AIS_ViewController::handleXRPicking(const occ::handle<AIS_InteractiveContext>& theCtx,
                                         const occ::handle<V3d_View>&               theView)
{
  if (!theView->View()->IsActiveXR())
  {
    return;
  }

  // handle selection on trigger clicks
  Aspect_XRTrackedDeviceRole aPickDevOld = myXRLastPickingHand;
  myXRLastPickingHand                    = Aspect_XRTrackedDeviceRole_Other;
  for (int aHand = 0; aHand < 2; ++aHand)
  {
    const Aspect_XRTrackedDeviceRole aRole =
      aHand == 0 ? Aspect_XRTrackedDeviceRole_RightHand : Aspect_XRTrackedDeviceRole_LeftHand;
    const occ::handle<Aspect_XRAction>& aTrigClickAct =
      theView->View()->XRSession()->GenericAction(aRole, Aspect_XRGenericAction_InputTriggerClick);
    const occ::handle<Aspect_XRAction>& aTrigPullAct =
      theView->View()->XRSession()->GenericAction(aRole, Aspect_XRGenericAction_InputTriggerPull);
    if (aTrigClickAct.IsNull() || aTrigPullAct.IsNull())
    {
      continue;
    }

    const Aspect_XRDigitalActionData aTrigClick =
      theView->View()->XRSession()->GetDigitalActionData(aTrigClickAct);
    const Aspect_XRAnalogActionData aTrigPos =
      theView->View()->XRSession()->GetAnalogActionData(aTrigPullAct);
    if (aTrigPos.IsActive && std::abs(aTrigPos.VecXYZ.x()) > 0.1f)
    {
      myXRLastPickingHand = aRole;
      handleXRHighlight(theCtx, theView);
      if (aTrigClick.IsActive && aTrigClick.IsPressed && aTrigClick.IsChanged)
      {
        theCtx->SelectDetected();
        OnSelectionChanged(theCtx, theView);
        if (const occ::handle<Aspect_XRAction>& aHaptic =
              theView->View()->XRSession()->GenericAction(myXRLastPickingHand,
                                                          Aspect_XRGenericAction_OutputHaptic))
        {
          theView->View()->XRSession()->TriggerHapticVibrationAction(aHaptic, myXRSelectHaptic);
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

//=================================================================================================

void AIS_ViewController::OnSelectionChanged(const occ::handle<AIS_InteractiveContext>&,
                                            const occ::handle<V3d_View>&)
{
}

//=================================================================================================

void AIS_ViewController::OnSubviewChanged(const occ::handle<AIS_InteractiveContext>&,
                                          const occ::handle<V3d_View>&,
                                          const occ::handle<V3d_View>&)
{
}

//=================================================================================================

void AIS_ViewController::OnObjectDragged(const occ::handle<AIS_InteractiveContext>& theCtx,
                                         const occ::handle<V3d_View>&               theView,
                                         AIS_DragAction                             theAction)
{
  switch (theAction)
  {
    case AIS_DragAction_Start: {
      myDragObject.Nullify();
      myDragOwner.Nullify();
      if (!theCtx->HasDetected())
      {
        return;
      }

      const occ::handle<SelectMgr_EntityOwner>& aDetectedOwner = theCtx->DetectedOwner();
      occ::handle<AIS_InteractiveObject>        aDetectedPrs =
        occ::down_cast<AIS_InteractiveObject>(aDetectedOwner->Selectable());

      if (aDetectedPrs->ProcessDragging(theCtx,
                                        theView,
                                        aDetectedOwner,
                                        myGL.Dragging.PointStart,
                                        myGL.Dragging.PointTo,
                                        theAction))
      {
        myDragObject = aDetectedPrs;
        myDragOwner  = aDetectedOwner;
      }
      return;
    }
    case AIS_DragAction_Confirmed: {
      if (myDragObject.IsNull())
      {
        return;
      }

      myDragObject->ProcessDragging(theCtx,
                                    theView,
                                    myDragOwner,
                                    myGL.Dragging.PointStart,
                                    myGL.Dragging.PointTo,
                                    theAction);
      return;
    }
    case AIS_DragAction_Update: {
      if (myDragObject.IsNull())
      {
        return;
      }

      if (occ::handle<SelectMgr_EntityOwner> aGlobOwner = myDragObject->GlobalSelOwner())
      {
        theCtx->SetSelectedState(aGlobOwner, true);
      }

      myDragObject->ProcessDragging(theCtx,
                                    theView,
                                    myDragOwner,
                                    myGL.Dragging.PointStart,
                                    myGL.Dragging.PointTo,
                                    theAction);
      theView->Invalidate();
      return;
    }
    case AIS_DragAction_Abort: {
      if (myDragObject.IsNull())
      {
        return;
      }

      myGL.Dragging.PointTo = myGL.Dragging.PointStart;
      OnObjectDragged(theCtx, theView, AIS_DragAction_Update);

      myDragObject->ProcessDragging(theCtx,
                                    theView,
                                    myDragOwner,
                                    myGL.Dragging.PointStart,
                                    myGL.Dragging.PointTo,
                                    theAction);
      [[fallthrough]];
    }
    case AIS_DragAction_Stop: {
      if (myDragObject.IsNull())
      {
        return;
      }

      if (occ::handle<SelectMgr_EntityOwner> aGlobOwner = myDragObject->GlobalSelOwner())
      {
        theCtx->SetSelectedState(aGlobOwner, false);
      }

      myDragObject->ProcessDragging(theCtx,
                                    theView,
                                    myDragOwner,
                                    myGL.Dragging.PointStart,
                                    myGL.Dragging.PointTo,
                                    theAction);
      theView->Invalidate();
      myDragObject.Nullify();
      myDragOwner.Nullify();
      return;
    }
  }
}

//=================================================================================================

void AIS_ViewController::contextLazyMoveTo(const occ::handle<AIS_InteractiveContext>& theCtx,
                                           const occ::handle<V3d_View>&               theView,
                                           const NCollection_Vec2<int>&               thePnt)
{
  if (myPrevMoveTo == thePnt
      || myHasHlrOnBeforeRotation) // ignore highlighting in-between rotation of HLR view
  {
    return;
  }

  myPrevMoveTo = thePnt;

  occ::handle<SelectMgr_EntityOwner> aLastPicked = theCtx->DetectedOwner();

  // Picking relies on the camera frustum (including Z-range) - so make temporary AutoZFit()
  // and then restore previous frustum to avoid immediate layer rendering issues if View has not
  // been invalidated.
  const double aZNear = theView->Camera()->ZNear(), aZFar = theView->Camera()->ZFar();
  theView->AutoZFit();
  theCtx->MoveTo(thePnt.x(), thePnt.y(), theView, false);
  theView->Camera()->SetZRange(aZNear, aZFar);

  occ::handle<SelectMgr_EntityOwner> aNewPicked = theCtx->DetectedOwner();

  if (theView->Viewer()->IsGridActive() && theView->Viewer()->GridEcho())
  {
    if (aNewPicked.IsNull())
    {
      NCollection_Vec3<double> aPnt3d;
      theView->ConvertToGrid(thePnt.x(), thePnt.y(), aPnt3d[0], aPnt3d[1], aPnt3d[2]);
      theView->Viewer()->ShowGridEcho(theView, Graphic3d_Vertex(aPnt3d[0], aPnt3d[1], aPnt3d[2]));
      theView->InvalidateImmediate();
    }
    else
    {
      theView->Viewer()->HideGridEcho(theView);
      theView->InvalidateImmediate();
    }
  }

  if (aLastPicked != aNewPicked || (!aNewPicked.IsNull() && aNewPicked->IsForcedHilight()))
  {
    // dynamic highlight affects all Views
    for (NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter(
           theView->Viewer()->ActiveViewIterator());
         aViewIter.More();
         aViewIter.Next())
    {
      const occ::handle<V3d_View>& aView = aViewIter.Value();
      aView->InvalidateImmediate();
    }
  }
}

//=================================================================================================

void AIS_ViewController::handleSelectionPick(const occ::handle<AIS_InteractiveContext>& theCtx,
                                             const occ::handle<V3d_View>&               theView)
{
  if (myGL.Selection.Tool == AIS_ViewSelectionTool_Picking && !myGL.Selection.Points.IsEmpty())
  {
    for (NCollection_Sequence<NCollection_Vec2<int>>::Iterator aPntIter(myGL.Selection.Points);
         aPntIter.More();
         aPntIter.Next())
    {
      const bool hadPrevMoveTo = HasPreviousMoveTo();
      contextLazyMoveTo(theCtx, theView, aPntIter.Value());
      if (!hadPrevMoveTo)
      {
        ResetPreviousMoveTo();
      }

      theCtx->SelectDetected(myGL.Selection.Scheme);

      // selection affects all Views
      theView->Viewer()->Invalidate();

      OnSelectionChanged(theCtx, theView);
    }

    myGL.Selection.Points.Clear();
  }
}

//=================================================================================================

void AIS_ViewController::handleSelectionPoly(const occ::handle<AIS_InteractiveContext>& theCtx,
                                             const occ::handle<V3d_View>&               theView)
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
        myRubberBand->SetRectangle(myGL.Selection.Points.First().x(),
                                   -myGL.Selection.Points.First().y(),
                                   myGL.Selection.Points.Last().x(),
                                   -myGL.Selection.Points.Last().y());
      }
      else
      {
        NCollection_Vec2<int> aPrev(IntegerLast(), IntegerLast());
        for (NCollection_Sequence<NCollection_Vec2<int>>::Iterator aSelIter(myGL.Selection.Points);
             aSelIter.More();
             aSelIter.Next())
        {
          NCollection_Vec2<int> aPntNew =
            NCollection_Vec2<int>(aSelIter.Value().x(), -aSelIter.Value().y());
          if (aPntNew != aPrev)
          {
            aPrev = aPntNew;
            myRubberBand->AddPoint(
              NCollection_Vec2<int>(aSelIter.Value().x(), -aSelIter.Value().y()));
          }
        }
      }

      myRubberBand->SetPolygonClosed(anIsRubber);
      try
      {
        theCtx->Display(myRubberBand, 0, -1, false, AIS_DS_Displayed);
      }
      catch (const Standard_Failure& theEx)
      {
        Message::SendWarning(
          TCollection_AsciiString("Internal error while displaying rubber-band: ")
          + theEx.ExceptionType() + ", " + theEx.what());
        myRubberBand->ClearPoints();
      }
      if (!theView->Viewer()->ZLayerSettings(myRubberBand->ZLayer()).IsImmediate())
      {
        theView->Invalidate();
      }
      else
      {
        theView->InvalidateImmediate();
      }
    }
    else if (!myRubberBand.IsNull() && myRubberBand->HasInteractiveContext())
    {
      theCtx->Remove(myRubberBand, false);
      myRubberBand->ClearPoints();
    }
  }

  if (myGL.Selection.ToApplyTool)
  {
    myGL.Selection.ToApplyTool = false;
    if (theCtx->IsDisplayed(myRubberBand))
    {
      theCtx->Remove(myRubberBand, false);
      {
        const NCollection_Sequence<NCollection_Vec2<int>>& aPoints = myRubberBand->Points();
        if (aPoints.Size() == 4 && aPoints.Value(1).x() == aPoints.Value(2).x()
            && aPoints.Value(3).x() == aPoints.Value(4).x()
            && aPoints.Value(1).y() == aPoints.Value(4).y()
            && aPoints.Value(2).y() == aPoints.Value(3).y())
        {
          const NCollection_Vec2<int> aPnt1(aPoints.Value(1).x(), -aPoints.Value(1).y());
          const NCollection_Vec2<int> aPnt2(aPoints.Value(3).x(), -aPoints.Value(3).y());
          if (myGL.Selection.Tool == AIS_ViewSelectionTool_ZoomWindow)
          {
            theView->WindowFitAll(aPnt1.x(), aPnt1.y(), aPnt2.x(), aPnt2.y());
            theView->Invalidate();
          }
          else
          {
            theCtx->MainSelector()->AllowOverlapDetection(aPnt1.y()
                                                          != std::min(aPnt1.y(), aPnt2.y()));
            theCtx->SelectRectangle(
              NCollection_Vec2<int>(std::min(aPnt1.x(), aPnt2.x()), std::min(aPnt1.y(), aPnt2.y())),
              NCollection_Vec2<int>(std::max(aPnt1.x(), aPnt2.x()), std::max(aPnt1.y(), aPnt2.y())),
              theView,
              myGL.Selection.Scheme);
            theCtx->MainSelector()->AllowOverlapDetection(false);
          }
        }
        else if (aPoints.Length() >= 3)
        {
          NCollection_Array1<gp_Pnt2d>           aPolyline(1, aPoints.Length());
          NCollection_Array1<gp_Pnt2d>::Iterator aPolyIter(aPolyline);
          for (NCollection_Sequence<NCollection_Vec2<int>>::Iterator aSelIter(aPoints);
               aSelIter.More();
               aSelIter.Next(), aPolyIter.Next())
          {
            const NCollection_Vec2<int>& aNewPnt = aSelIter.Value();
            aPolyIter.ChangeValue()              = gp_Pnt2d(aNewPnt.x(), -aNewPnt.y());
          }

          theCtx->SelectPolygon(aPolyline, theView, myGL.Selection.Scheme);
          theCtx->MainSelector()->AllowOverlapDetection(false);
        }
      }

      myRubberBand->ClearPoints();
      if (myGL.Selection.Tool != AIS_ViewSelectionTool_ZoomWindow)
      {
        // selection affects all Views
        theView->Viewer()->Invalidate();
        OnSelectionChanged(theCtx, theView);
      }
    }
  }
}

//=================================================================================================

void AIS_ViewController::handleDynamicHighlight(const occ::handle<AIS_InteractiveContext>& theCtx,
                                                const occ::handle<V3d_View>&               theView)
{
  if ((myGL.MoveTo.ToHilight || myGL.Dragging.ToStart)
      && myNavigationMode != AIS_NavigationMode_FirstPersonWalk)
  {
    const NCollection_Vec2<int>& aMoveToPnt =
      myGL.MoveTo.ToHilight ? myGL.MoveTo.Point : myGL.Dragging.PointStart;
    if (myGL.Dragging.ToStart && (!myGL.MoveTo.ToHilight || !myToAllowHighlight)
        && !HasPreviousMoveTo())
    {
      contextLazyMoveTo(theCtx, theView, aMoveToPnt);
      ResetPreviousMoveTo();
      OnObjectDragged(theCtx, theView, AIS_DragAction_Start);
      theCtx->ClearDetected();
    }
    else if (myToAllowHighlight)
    {
      if (myPrevMoveTo != aMoveToPnt
          || (!theView->View()->IsActiveXR()
              && (myGL.OrbitRotation.ToRotate || myGL.ViewRotation.ToRotate
                  || theView->IsInvalidated())))
      {
        ResetPreviousMoveTo();
        contextLazyMoveTo(theCtx, theView, aMoveToPnt);
      }
      if (myGL.Dragging.ToStart)
      {
        OnObjectDragged(theCtx, theView, AIS_DragAction_Start);
      }
    }

    myGL.MoveTo.ToHilight = false;
  }

  if (!myDragObject.IsNull())
  {
    if (myGL.Dragging.ToAbort)
    {
      OnObjectDragged(theCtx, theView, AIS_DragAction_Abort);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation.ToRotate  = false;
    }
    else if (myGL.Dragging.ToStop)
    {
      OnObjectDragged(theCtx, theView, AIS_DragAction_Stop);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation.ToRotate  = false;
    }
    else if (myGL.Dragging.ToMove)
    {
      if (myGL.Dragging.ToConfirm)
      {
        OnObjectDragged(theCtx, theView, AIS_DragAction_Confirmed);
      }
      OnObjectDragged(theCtx, theView, AIS_DragAction_Update);
      myGL.OrbitRotation.ToRotate = false;
      myGL.ViewRotation.ToRotate  = false;
      myGL.Panning.ToPan          = false;
      myGL.ZoomActions.Clear();
    }
  }
}

//=================================================================================================

void AIS_ViewController::handleMoveTo(const occ::handle<AIS_InteractiveContext>& theCtx,
                                      const occ::handle<V3d_View>&               theView)
{
  handleSelectionPick(theCtx, theView);
  handleDynamicHighlight(theCtx, theView);
  handleSelectionPoly(theCtx, theView);
}

//=================================================================================================

void AIS_ViewController::handleViewRedraw(const occ::handle<AIS_InteractiveContext>&,
                                          const occ::handle<V3d_View>& theView)
{
  occ::handle<V3d_View> aParentView = theView->IsSubview() ? theView->ParentView() : theView;

  // manage animation state
  if (!myViewAnimation.IsNull() && !myViewAnimation->IsStopped())
  {
    myViewAnimation->UpdateTimer();
    ResetPreviousMoveTo();
    setAskNextFrame();
  }

  if (!myObjAnimation.IsNull() && !myObjAnimation->IsStopped())
  {
    myObjAnimation->UpdateTimer();
    ResetPreviousMoveTo();
    setAskNextFrame();
  }

  if (myIsContinuousRedraw)
  {
    myToAskNextFrame = true;
  }
  if (theView->View()->IsActiveXR())
  {
    // VR requires continuous rendering
    myToAskNextFrame = true;
  }

  for (int aSubViewPass = 0; aSubViewPass < 2; ++aSubViewPass)
  {
    const bool isSubViewPass = (aSubViewPass == 0);
    for (NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter(
           theView->Viewer()->ActiveViewIterator());
         aViewIter.More();
         aViewIter.Next())
    {
      const occ::handle<V3d_View>& aView = aViewIter.Value();
      if (isSubViewPass && !aView->IsSubview())
      {
        for (const occ::handle<V3d_View>& aSubviewIter : aView->Subviews())
        {
          if (aSubviewIter->Viewer() != theView->Viewer())
          {
            if (aSubviewIter->IsInvalidated())
            {
              if (aSubviewIter->ComputedMode())
              {
                aSubviewIter->Update();
              }
              else
              {
                aSubviewIter->Redraw();
              }
            }
            else if (aSubviewIter->IsInvalidatedImmediate())
            {
              aSubviewIter->RedrawImmediate();
            }
          }
        }
        continue;
      }
      else if (!isSubViewPass && aView->IsSubview())
      {
        continue;
      }

      if (aView->IsInvalidated() || (myToAskNextFrame && aView == theView))
      {
        if (aView->ComputedMode())
        {
          aView->Update();
        }
        else
        {
          aView->Redraw();
        }

        if (aView->IsSubview())
        {
          aView->ParentView()->InvalidateImmediate();
        }
      }
      else if (aView->IsInvalidatedImmediate())
      {
        if (aView->IsSubview())
        {
          aView->ParentView()->InvalidateImmediate();
        }

        aView->RedrawImmediate();
      }
    }
  }
  if (theView->IsSubview() && theView->Viewer() != aParentView->Viewer())
  {
    aParentView->RedrawImmediate();
  }

  if (myToAskNextFrame)
  {
    // ask more frames
    aParentView->Window()->InvalidateContent(occ::handle<Aspect_DisplayConnection>());
  }
}

//=================================================================================================

int AIS_ViewController::handleXRMoveTo(const occ::handle<AIS_InteractiveContext>& theCtx,
                                       const occ::handle<V3d_View>&               theView,
                                       const gp_Trsf&                             thePose,
                                       const bool                                 theToHighlight)
{
  // ResetPreviousMoveTo();
  const gp_Ax1 aViewAxis   = theView->View()->ViewAxisInWorld(thePose);
  int          aPickResult = 0;
  if (theToHighlight)
  {
    theCtx->MoveTo(aViewAxis, theView, false);
    if (!theCtx->DetectedOwner().IsNull())
    {
      aPickResult = 1;
    }
  }
  else
  {
    theCtx->MainSelector()->Pick(aViewAxis, theView);
    if (theCtx->MainSelector()->NbPicked() >= 1)
    {
      aPickResult = 1;
    }
  }

  return aPickResult;
}

//=================================================================================================

void AIS_ViewController::handleXRHighlight(const occ::handle<AIS_InteractiveContext>& theCtx,
                                           const occ::handle<V3d_View>&               theView)
{
  if (myXRLastPickingHand != Aspect_XRTrackedDeviceRole_LeftHand
      && myXRLastPickingHand != Aspect_XRTrackedDeviceRole_RightHand)
  {
    return;
  }

  const int aDeviceId = theView->View()->XRSession()->NamedTrackedDevice(myXRLastPickingHand);
  if (aDeviceId == -1)
  {
    return;
  }

  const Aspect_TrackedDevicePose& aPose = theView->View()->XRSession()->TrackedPoses()[aDeviceId];
  if (!aPose.IsValidPose)
  {
    return;
  }

  occ::handle<SelectMgr_EntityOwner> aDetOld = theCtx->DetectedOwner();
  handleXRMoveTo(theCtx, theView, aPose.Orientation, true);
  if (!theCtx->DetectedOwner().IsNull() && theCtx->DetectedOwner() != aDetOld)
  {
    if (const occ::handle<Aspect_XRAction>& aHaptic =
          theView->View()->XRSession()->GenericAction(myXRLastPickingHand,
                                                      Aspect_XRGenericAction_OutputHaptic))
    {
      theView->View()->XRSession()->TriggerHapticVibrationAction(aHaptic, myXRPickingHaptic);
    }
  }

  double& aPickDepth = myXRLastPickingHand == Aspect_XRTrackedDeviceRole_LeftHand
                         ? myXRLastPickDepthLeft
                         : myXRLastPickDepthRight;
  aPickDepth         = Precision::Infinite();
  if (theCtx->MainSelector()->NbPicked() > 0)
  {
    const gp_Trsf                  aHandBase = theView->View()->PoseXRToWorld(aPose.Orientation);
    const SelectMgr_SortCriterion& aPicked   = theCtx->MainSelector()->PickedData(1);
    aPickDepth                               = aPicked.Point.Distance(aHandBase.TranslationPart());
  }
}

//=================================================================================================

void AIS_ViewController::handleXRPresentations(const occ::handle<AIS_InteractiveContext>& theCtx,
                                               const occ::handle<V3d_View>&               theView)
{
  if (!theView->View()->IsActiveXR() || (!myToDisplayXRAuxDevices && !myToDisplayXRHands))
  {
    for (NCollection_Array1<occ::handle<AIS_XRTrackedDevice>>::Iterator aPrsIter(myXRPrsDevices);
         aPrsIter.More();
         aPrsIter.Next())
    {
      if (!aPrsIter.Value().IsNull() && aPrsIter.Value()->HasInteractiveContext())
      {
        theCtx->Remove(aPrsIter.Value(), false);
      }
      aPrsIter.ChangeValue().Nullify();
    }
    return;
  }

  if (myXRPrsDevices.Length() != theView->View()->XRSession()->TrackedPoses().Length())
  {
    for (NCollection_Array1<occ::handle<AIS_XRTrackedDevice>>::Iterator aPrsIter(myXRPrsDevices);
         aPrsIter.More();
         aPrsIter.Next())
    {
      if (!aPrsIter.Value().IsNull())
      {
        theCtx->Remove(aPrsIter.Value(), false);
      }
    }
    myXRPrsDevices.Resize(theView->View()->XRSession()->TrackedPoses().Lower(),
                          theView->View()->XRSession()->TrackedPoses().Upper(),
                          false);
  }

  const int aHeadDevice =
    theView->View()->XRSession()->NamedTrackedDevice(Aspect_XRTrackedDeviceRole_Head);
  const int aLeftDevice =
    theView->View()->XRSession()->NamedTrackedDevice(Aspect_XRTrackedDeviceRole_LeftHand);
  const int aRightDevice =
    theView->View()->XRSession()->NamedTrackedDevice(Aspect_XRTrackedDeviceRole_RightHand);
  for (int aDeviceIter = theView->View()->XRSession()->TrackedPoses().Lower();
       aDeviceIter <= theView->View()->XRSession()->TrackedPoses().Upper();
       ++aDeviceIter)
  {
    const Aspect_TrackedDevicePose& aPose =
      theView->View()->XRSession()->TrackedPoses()[aDeviceIter];
    occ::handle<AIS_XRTrackedDevice>& aPosePrs = myXRPrsDevices[aDeviceIter];
    if (!aPose.IsValidPose)
    {
      continue;
    }

    const bool isHand = aDeviceIter == aLeftDevice || aDeviceIter == aRightDevice;
    if ((!myToDisplayXRHands && isHand) || (!myToDisplayXRAuxDevices && !isHand))
    {
      if (!aPosePrs.IsNull() && aPosePrs->HasInteractiveContext())
      {
        theCtx->Remove(aPosePrs, false);
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

    if (!aPosePrs.IsNull() && aPosePrs->UnitFactor() != (float)theView->View()->UnitFactor())
    {
      theCtx->Remove(aPosePrs, false);
      aPosePrs.Nullify();
    }

    if (aPosePrs.IsNull())
    {
      occ::handle<Image_Texture>              aTexture;
      occ::handle<Graphic3d_ArrayOfTriangles> aTris;
      if (aDeviceIter != aHeadDevice)
      {
        aTris = theView->View()->XRSession()->LoadRenderModel(aDeviceIter, aTexture);
      }
      if (!aTris.IsNull())
      {
        aPosePrs = new AIS_XRTrackedDevice(aTris, aTexture);
      }
      else
      {
        aPosePrs = new AIS_XRTrackedDevice();
      }
      aPosePrs->SetUnitFactor((float)theView->View()->UnitFactor());
      aPosePrs->SetMutable(true);
      aPosePrs->SetInfiniteState(true);
    }
    aPosePrs->SetRole(aRole);

    if (!aPosePrs->HasInteractiveContext())
    {
      theCtx->Display(aPosePrs, 0, -1, false);
    }

    gp_Trsf aPoseLocal = aPose.Orientation;
    if (aDeviceIter == aHeadDevice)
    {
      // show headset position on floor level
      aPoseLocal.SetTranslationPart(
        gp_Vec(aPoseLocal.TranslationPart().X(), 0.0, aPoseLocal.TranslationPart().Z()));
    }
    const gp_Trsf aPoseWorld = theView->View()->PoseXRToWorld(aPoseLocal);
    theCtx->SetLocation(aPosePrs, aPoseWorld);

    double aLaserLen = 0.0;
    if (isHand && aPosePrs->Role() == myXRLastPickingHand)
    {
      aLaserLen = myXRLastPickingHand == Aspect_XRTrackedDeviceRole_LeftHand
                    ? myXRLastPickDepthLeft
                    : myXRLastPickDepthRight;
      if (Precision::IsInfinite(aLaserLen))
      {
        const Bnd_Box aViewBox = theView->View()->MinMaxValues(true);
        if (!aViewBox.IsVoid())
        {
          aLaserLen = std::sqrt(aViewBox.SquareExtent());
        }
        else
        {
          aLaserLen = 100.0;
        }
      }
      aPosePrs->SetLaserColor(myXRLaserPickColor);
    }
    else if (isHand && aPosePrs->Role() == myXRLastTeleportHand)
    {
      aLaserLen = myXRLastTeleportHand == Aspect_XRTrackedDeviceRole_LeftHand
                    ? myXRLastPickDepthLeft
                    : myXRLastPickDepthRight;
      if (Precision::IsInfinite(aLaserLen))
      {
        const Bnd_Box aViewBox = theView->View()->MinMaxValues(true);
        if (!aViewBox.IsVoid())
        {
          aLaserLen = std::sqrt(aViewBox.SquareExtent());
        }
        else
        {
          aLaserLen = 100.0;
        }
      }
      aPosePrs->SetLaserColor(myXRLaserTeleColor);
    }
    aPosePrs->SetLaserLength((float)aLaserLen);
  }
}

//=================================================================================================

void AIS_ViewController::HandleViewEvents(const occ::handle<AIS_InteractiveContext>& theCtx,
                                          const occ::handle<V3d_View>&               theView)
{
  const bool wasImmediateUpdate = theView->SetImmediateUpdate(false);

  occ::handle<V3d_View> aPickedView;
  if (theView->IsSubview() || !theView->Subviews().IsEmpty())
  {
    // activate another subview on mouse click
    bool                  toPickSubview = false;
    NCollection_Vec2<int> aClickPoint;
    if (myGL.Selection.Tool == AIS_ViewSelectionTool_Picking && !myGL.Selection.Points.IsEmpty())
    {
      aClickPoint   = myGL.Selection.Points.Last();
      toPickSubview = true;
    }
    else if (!myGL.ZoomActions.IsEmpty())
    {
      // aClickPoint = myGL.ZoomActions.Last().Point;
      // toPickSubview = true;
    }

    if (toPickSubview)
    {
      if (theView->IsSubview())
      {
        aClickPoint += theView->View()->SubviewTopLeft();
      }
      occ::handle<V3d_View> aParent = !theView->IsSubview() ? theView : theView->ParentView();
      aPickedView                   = aParent->PickSubview(aClickPoint);
    }
  }

  handleViewOrientationKeys(theCtx, theView);
  const AIS_WalkDelta aWalk = handleNavigationKeys(theCtx, theView);
  handleXRInput(theCtx, theView, aWalk);
  if (theView->View()->IsActiveXR())
  {
    theView->View()->SetupXRPosedCamera();
  }
  handleMoveTo(theCtx, theView);
  handleCameraActions(theCtx, theView, aWalk);
  // clang-format off
  theView->View()->SynchronizeXRPosedToBaseCamera(); // handleCameraActions() may modify posed camera position - copy this modifications also to the base camera
  // clang-format on
  handleXRPresentations(theCtx, theView);

  handleViewRedraw(theCtx, theView);
  theView->View()->UnsetXRPosedCamera();

  theView->SetImmediateUpdate(wasImmediateUpdate);

  if (!aPickedView.IsNull() && aPickedView != theView)
  {
    OnSubviewChanged(theCtx, theView, aPickedView);
  }

  // make sure to not process the same events twice
  myGL.Reset();
  myToAskNextFrame = false;
}
