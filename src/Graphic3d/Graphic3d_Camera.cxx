// Created on: 2013-05-29
// Created by: Anton POLETAEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <gp_Pln.hxx>
#include <Standard_ShortReal.hxx>

#include <Graphic3d_Vec4.hxx>
#include <Graphic3d_Camera.hxx>

#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_HANDLE(Graphic3d_Camera, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Camera, Standard_Transient)

namespace
{
  // (degrees -> radians) * 0.5
  static const Standard_ShortReal DTR_HALF = 0.5f * 0.0174532925f;

  // atomic state counter
  static volatile Standard_Integer THE_STATE_COUNTER = 0;
};

// =======================================================================
// function : Graphic3d_Camera
// purpose  :
// =======================================================================
Graphic3d_Camera::Graphic3d_Camera()
: myUp (0.0, 1.0, 0.0),
  myEye (0.0, 0.0, -1500.0),
  myCenter (0.0, 0.0, 0.0),
  myProjectionShift (0.0, 0.0, 0.0),
  myAxialScale (1.0, 1.0, 1.0),
  myProjType (Projection_Orthographic),
  myFOVy (45.0),
  myZNear (0.001),
  myZFar (3000.0),
  myAspect (1.0),
  myScale (1000.0),
  myZFocus (1.0),
  myZFocusType (FocusType_Relative),
  myIOD (0.05),
  myIODType (IODType_Relative),
  myNbUpdateLocks (0)
{
  myProjectionState  = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
  myOrientationState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);

  myOrientation.InitIdentity();
  myMProjection.InitIdentity();
  myLProjection.InitIdentity();
  myRProjection.InitIdentity();

  UpdateProjection();
  UpdateOrientation();
}

// =======================================================================
// function : Graphic3d_Camera
// purpose  :
// =======================================================================
Graphic3d_Camera::Graphic3d_Camera (const Handle(Graphic3d_Camera)& theOther)
: myNbUpdateLocks (0)
{
  Copy (theOther);
}

// =======================================================================
// function : CopyMappingData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyMappingData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  myProjectionShift     = theOtherCamera->myProjectionShift;
  myFOVy                = theOtherCamera->myFOVy;
  myZNear               = theOtherCamera->myZNear;
  myZFar                = theOtherCamera->myZFar;
  myAspect              = theOtherCamera->myAspect;
  myScale               = theOtherCamera->myScale;
  myZFocus              = theOtherCamera->myZFocus;
  myZFocusType          = theOtherCamera->myZFocusType;
  myIOD                 = theOtherCamera->myIOD;
  myIODType             = theOtherCamera->myIODType;
  myProjType            = theOtherCamera->myProjType;

  myProjectionState     = theOtherCamera->myProjectionState;

  UpdateProjection();
}

// =======================================================================
// function : CopyOrientationData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyOrientationData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  myUp             = theOtherCamera->myUp;
  myEye            = theOtherCamera->myEye;
  myCenter         = theOtherCamera->myCenter;
  myAxialScale     = theOtherCamera->myAxialScale;

  myOrientationState = theOtherCamera->myOrientationState;

  UpdateOrientation();
}

// =======================================================================
// function : Copy
// purpose  :
// =======================================================================
void Graphic3d_Camera::Copy (const Handle(Graphic3d_Camera)& theOther)
{
  BeginUpdate();
  CopyMappingData (theOther);
  CopyOrientationData (theOther);
  EndUpdate();
}

// =======================================================================
// function : SetEye
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetEye (const gp_Pnt& theEye)
{
  myEye = theEye;
  UpdateOrientation();
}

// =======================================================================
// function : SetCenter
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCenter (const gp_Pnt& theCenter)
{
  myCenter = theCenter;
  UpdateOrientation();
}

// =======================================================================
// function : SetUp
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetUp (const gp_Dir& theUp)
{
  myUp = theUp;
  UpdateOrientation();
}

// =======================================================================
// function : SetProjectionShift
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetProjectionShift (const gp_Pnt& theProjShift)
{
  myProjectionShift = theProjShift;
  UpdateProjection();
}

// =======================================================================
// function : SetAxialScale
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetAxialScale (const gp_Pnt& theAxialScale)
{
  myAxialScale = theAxialScale;
  UpdateOrientation();
}

// =======================================================================
// function : SetDistance
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDistance (const Standard_Real theDistance)
{
  gp_Vec aCenter2Eye (Direction());
  aCenter2Eye.Reverse();
  aCenter2Eye.Scale (theDistance);
  SetEye (Center().Translated (aCenter2Eye));
}

// =======================================================================
// function : Distance
// purpose  :
// =======================================================================
Standard_Real Graphic3d_Camera::Distance() const
{
  return myEye.Distance (myCenter);
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDirection (const gp_Dir& theDir)
{
  gp_Vec aScaledDir (theDir);
  aScaledDir.Scale (Distance());
  aScaledDir.Reverse();
  SetEye (Center().Translated (aScaledDir));
}

// =======================================================================
// function : Direction
// purpose  :
// =======================================================================
gp_Dir Graphic3d_Camera::Direction() const
{
  return gp_Dir (gp_Vec (myEye, myCenter));
}

// =======================================================================
// function : SetScale
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetScale (const Standard_Real theScale)
{
  myScale = theScale;

  switch (myProjType)
  {
    case Projection_Perspective  :
    case Projection_Stereo       :
    case Projection_MonoLeftEye  :
    case Projection_MonoRightEye :
    {
      Standard_Real aDistance = theScale * 0.5 / Tan(myFOVy * M_PI / 360.0);
      SetDistance (aDistance);
    }

    default :
      break;
  }

  UpdateProjection();
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
Standard_Real Graphic3d_Camera::Scale() const
{
  switch (myProjType)
  {
    case Projection_Orthographic :
      return myScale;

    // case Projection_Perspective  :
    // case Projection_Stereo       :
    // case Projection_MonoLeftEye  :
    // case Projection_MonoRightEye :
    default :
      return Distance() * 2.0 * Tan(myFOVy * M_PI / 360.0);
  }
}

// =======================================================================
// function : SetProjectionType
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetProjectionType (const Projection theProjectionType)
{
  Projection anOldType = myProjType;

  if (anOldType == theProjectionType)
  {
    return;
  }

  myProjType = theProjectionType;

  switch (myProjType)
  {
    case Projection_Orthographic :
    case Projection_Perspective  :
    case Projection_MonoLeftEye  :
    case Projection_MonoRightEye :
      myLProjection.InitIdentity();
      myRProjection.InitIdentity();
      break;

    default:
      break;
  }

  UpdateProjection();
}

// =======================================================================
// function : SetFOVy
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetFOVy (const Standard_Real theFOVy)
{
  myFOVy = theFOVy;
  UpdateProjection();
}

// =======================================================================
// function : SetZNear
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZNear (const Standard_Real theZNear)
{
  myZNear = theZNear;

  // it is important to prevent too low ZNear values relatively to ZFar
  // so we can not just pass Precision::Confusion() to it
  const Standard_Real aTolerance  = 0.001;
  const Standard_Real aMinimumZ   = myZFar * aTolerance;
  const Standard_Real aMinimumGap = aTolerance;
  // while it is possible to manually set up pretty small ZNear value,
  // it may affect project / unproject operations dramatically
  if (myZNear < aMinimumZ)
  {
    myZNear = aMinimumZ;
  }

  if (myZFar < (myZNear + aMinimumGap))
  {
    myZFar = myZNear + aMinimumGap;
  }

  UpdateProjection();
}

// =======================================================================
// function : SetZFar
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZFar (const Standard_Real theZFar)
{
  myZFar = theZFar;

  // it is important to prevent too low ZNear values relatively to ZFar
  // so we can not just pass Precision::Confusion() to it
  const Standard_Real aTolerance  = 0.001;
  const Standard_Real aMinimumGap = aTolerance;

  // while it is possible to manually set up pretty small ZNear value,
  // it may affect project / unproject operations dramatically
  if (myZFar < (myZNear + aMinimumGap))
  {
    myZFar = myZNear + aMinimumGap;
  }

  UpdateProjection();
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetAspect (const Standard_Real theAspect)
{
  myAspect = theAspect;
  UpdateProjection();
}

// =======================================================================
// function : SetZFocus
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZFocus(const FocusType theType, const Standard_Real theZFocus)
{
  myZFocusType = theType;
  myZFocus = theZFocus;
  UpdateProjection();
}

// =======================================================================
// function : SetIOD
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetIOD (const IODType theType, const Standard_Real theIOD)
{
  myIODType = theType;
  myIOD = theIOD;
  UpdateProjection();
}

// =======================================================================
// function : OrthogonalizeUp
// purpose  :
// =======================================================================
void Graphic3d_Camera::OrthogonalizeUp()
{
  gp_Dir aDir  = Direction();
  gp_Dir aLeft = aDir.Crossed (Up());

  // recompute up as: up = left x direction
  SetUp (aLeft.Crossed (aDir));
}

// =======================================================================
// function : BeginUpdate
// purpose  :
// =======================================================================
void Graphic3d_Camera::BeginUpdate()
{
  myNbUpdateLocks++;
}

// =======================================================================
// function : EndUpdate
// purpose  :
// =======================================================================
void Graphic3d_Camera::EndUpdate()
{
  if (myNbUpdateLocks > 0)
    myNbUpdateLocks--;

  // if number of locks > 0, the updates are bypassed
  UpdateProjection();
  UpdateOrientation();
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
void Graphic3d_Camera::Transform (const gp_Trsf& theTrsf)
{
  myUp.Transform (theTrsf);
  myEye.Transform (theTrsf);
  myCenter.Transform (theTrsf);
  UpdateOrientation();
}

// =======================================================================
// function : safePointCast
// purpose  :
// =======================================================================
static Graphic3d_Vec4 safePointCast (const gp_Pnt& thePnt)
{
  Standard_Real aLim = 1e15f;
  
  // have to deal with values greater then max float
  gp_Pnt aSafePoint = thePnt;
  const Standard_Real aBigFloat = aLim * 0.1f;
  if (Abs (aSafePoint.X()) > aLim)
    aSafePoint.SetX (aSafePoint.X() >= 0 ? aBigFloat : -aBigFloat);
  if (Abs (aSafePoint.Y()) > aLim)
    aSafePoint.SetY (aSafePoint.Y() >= 0 ? aBigFloat : -aBigFloat);
  if (Abs (aSafePoint.Z()) > aLim)
    aSafePoint.SetZ (aSafePoint.Z() >= 0 ? aBigFloat : -aBigFloat);

  // convert point
  Graphic3d_Vec4 aPnt (static_cast<Standard_ShortReal> (aSafePoint.X()),
                       static_cast<Standard_ShortReal> (aSafePoint.Y()),
                       static_cast<Standard_ShortReal> (aSafePoint.Z()), 1.0f);

  return aPnt;
}

// =======================================================================
// function : Project
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::Project (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aViewMx = OrientationMatrix();
  const Graphic3d_Mat4& aProjMx = ProjectionMatrix();

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aViewMx * aPnt; // convert to view coordinate space
  aPnt = aProjMx * aPnt; // convert to projection coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : UnProject
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::UnProject (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aViewMx = OrientationMatrix();
  const Graphic3d_Mat4& aProjMx = ProjectionMatrix();

  Graphic3d_Mat4 aInvView;
  Graphic3d_Mat4 aInvProj;

  // this case should never happen
  if (!aViewMx.Inverted (aInvView) || !aProjMx.Inverted (aInvProj))
  {
    return gp_Pnt (0.0, 0.0, 0.0);
  }

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aInvProj * aPnt; // convert to view coordinate space
  aPnt = aInvView * aPnt; // convert to world coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ConvertView2Proj
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::ConvertView2Proj (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aProjMx = ProjectionMatrix();

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aProjMx * aPnt; // convert to projection coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ConvertProj2View
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::ConvertProj2View (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aProjMx = ProjectionMatrix();

  Graphic3d_Mat4 aInvProj;

  // this case should never happen, but...
  if (!aProjMx.Inverted (aInvProj))
  {
    return gp_Pnt(0, 0, 0);
  }

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aInvProj * aPnt; // convert to view coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ConvertWorld2View
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::ConvertWorld2View (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aViewMx = OrientationMatrix();

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aViewMx * aPnt; // convert to view coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ConvertView2World
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::ConvertView2World (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4& aViewMx = OrientationMatrix();

  Graphic3d_Mat4 aInvView;

  if (!aViewMx.Inverted (aInvView))
  {
    return gp_Pnt(0, 0, 0);
  }

  // use compatible type of point
  Graphic3d_Vec4 aPnt = safePointCast (thePnt);

  aPnt = aInvView * aPnt; // convert to world coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ViewDimensions
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::ViewDimensions () const
{
  // view plane dimensions
  Standard_Real aSizeY = IsOrthographic() ? myScale : (2.0 * Distance() * Tan (DTR_HALF * myFOVy));
  Standard_Real aSizeX = myAspect * aSizeY;

  // and frustum depth
  return gp_Pnt (aSizeX, aSizeY, myZFar - myZNear);
}

// =======================================================================
// function : WindowLimit
// purpose  :
// =======================================================================
void Graphic3d_Camera::WindowLimit (Standard_Real& theUMin,
                                    Standard_Real& theVMin,
                                    Standard_Real& theUMax,
                                    Standard_Real& theVMax) const
{
  gp_Pnt aViewDims = ViewDimensions();
  gp_Pnt aShift    = ProjectionShift();
  theUMin = -aViewDims.X() * 0.5 - aShift.X();
  theVMin = -aViewDims.Y() * 0.5 - aShift.Y();
  theUMax = aViewDims.X() * 0.5 - aShift.X();
  theVMax = aViewDims.Y() * 0.5 - aShift.Y();
}

// =======================================================================
// function : UpdateProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::UpdateProjection()
{
  if (myNbUpdateLocks > 0)
  {
    return;
  }

  myProjectionState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);

  // sets top of frustum based on FOVy and near clipping plane
  Standard_Real aDYHalf;
  if (IsOrthographic())
  {
     aDYHalf = myScale * 0.5;
  }
  else
  {
    aDYHalf = myZNear * Tan (DTR_HALF * myFOVy);
  }

  // sets right of frustum based on aspect ratio
  Standard_Real aDXHalf = myAspect * aDYHalf;

  Standard_ShortReal aLeft      = (Standard_ShortReal) -aDXHalf;
  Standard_ShortReal aRight     = (Standard_ShortReal)  aDXHalf;
  Standard_ShortReal aBot       = (Standard_ShortReal) -aDYHalf;
  Standard_ShortReal aTop       = (Standard_ShortReal)  aDYHalf;
  Standard_ShortReal aNear      = (Standard_ShortReal)  myZNear;
  Standard_ShortReal aFar       = (Standard_ShortReal)  myZFar;
  Standard_ShortReal aShiftX    = (Standard_ShortReal)  myProjectionShift.X();
  Standard_ShortReal aShiftY    = (Standard_ShortReal)  myProjectionShift.Y();

  Standard_ShortReal aIOD = (myIODType == IODType_Relative)
        ? (Standard_ShortReal)(myIOD * Distance())
        : (Standard_ShortReal)(myIOD);

  Standard_ShortReal aFocus = (myZFocusType == FocusType_Relative)
        ? (Standard_ShortReal)(myZFocus * Distance())
        : (Standard_ShortReal)(myZFocus);

  switch (myProjType)
  {
    case Projection_Orthographic :
      OrthoProj (aLeft, aRight, aBot, aTop, aNear, aFar, aShiftX, aShiftY, myMProjection);
      break;

    case Projection_Perspective :
      PerspectiveProj (aLeft, aRight, aBot, aTop, aNear, aFar, aShiftX, aShiftY, myMProjection);
      break;

    case Projection_MonoLeftEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop, aNear,
                     aFar, aIOD, aFocus, aShiftX, aShiftY,
                     Standard_True, myMProjection);
      break;
    }

    case Projection_MonoRightEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop, aNear,
                     aFar, aIOD, aFocus, aShiftX, aShiftY,
                     Standard_False, myMProjection);
      break;
    }

    case Projection_Stereo :
    {
      PerspectiveProj (aLeft, aRight, aBot, aTop, aNear, aFar, aShiftX, aShiftY, myMProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop, aNear,
                     aFar, aIOD, aFocus, aShiftX, aShiftY,
                     Standard_True, myLProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop, aNear,
                     aFar, aIOD, aFocus, aShiftX, aShiftY,
                     Standard_False, myRProjection);
      break;
    }
  }
}

// =======================================================================
// function : UpdateOrientation
// purpose  :
// =======================================================================
void Graphic3d_Camera::UpdateOrientation()
{
  if (myNbUpdateLocks > 0)
  {
    return;
  }

  myOrientationState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);

  Graphic3d_Vec3 anEye ((Standard_ShortReal) myEye.X(),
                        (Standard_ShortReal) myEye.Y(),
                        (Standard_ShortReal) myEye.Z());

  Graphic3d_Vec3 aCenter ((Standard_ShortReal) myCenter.X(),
                          (Standard_ShortReal) myCenter.Y(),
                          (Standard_ShortReal) myCenter.Z());

  Graphic3d_Vec3 anUp ((Standard_ShortReal) myUp.X(),
                       (Standard_ShortReal) myUp.Y(),
                       (Standard_ShortReal) myUp.Z());

  Graphic3d_Vec3 anAxialScale ((Standard_ShortReal) myAxialScale.X(),
                               (Standard_ShortReal) myAxialScale.Y(),
                               (Standard_ShortReal) myAxialScale.Z());  

  LookOrientation (anEye, aCenter, anUp, anAxialScale, myOrientation);

  // Update orthogonalized Up vector
  myUp = gp_Dir (anUp.x(), anUp.y(), anUp.z());
}

// =======================================================================
// function : OrthoProj
// purpose  :
// =======================================================================
void Graphic3d_Camera::OrthoProj (const Standard_ShortReal theLeft,
                                  const Standard_ShortReal theRight,
                                  const Standard_ShortReal theBottom,
                                  const Standard_ShortReal theTop,
                                  const Standard_ShortReal theNear,
                                  const Standard_ShortReal theFar,
                                  const Standard_ShortReal theShiftX,
                                  const Standard_ShortReal theShiftY,
                                  Graphic3d_Mat4& theOutMx)
{
  // row 0
  theOutMx.ChangeValue (0, 0) = 2.0f / (theRight - theLeft);
  theOutMx.ChangeValue (0, 1) = 0.0f;
  theOutMx.ChangeValue (0, 2) = 0.0f;
  theOutMx.ChangeValue (0, 3) = - (theRight + theLeft) / (theRight - theLeft);

  // row 1
  theOutMx.ChangeValue (1, 0) = 0.0f;
  theOutMx.ChangeValue (1, 1) = 2.0f / (theTop - theBottom);
  theOutMx.ChangeValue (1, 2) = 0.0f;
  theOutMx.ChangeValue (1, 3) = - (theTop + theBottom) / (theTop - theBottom);

  // row 2
  theOutMx.ChangeValue (2, 0) = 0.0f;
  theOutMx.ChangeValue (2, 1) = 0.0f;
  theOutMx.ChangeValue (2, 2) = -2.0f / (theFar - theNear);
  theOutMx.ChangeValue (2, 3) = - (theFar + theNear) / (theFar - theNear);

  // row 3
  theOutMx.ChangeValue (3, 0) = 0.0f;
  theOutMx.ChangeValue (3, 1) = 0.0f;
  theOutMx.ChangeValue (3, 2) = 0.0f;
  theOutMx.ChangeValue (3, 3) = 1.0f;

  Graphic3d_Mat4 aViewportShift;
  aViewportShift.ChangeValue (0, 3) = theShiftX;
  aViewportShift.ChangeValue (1, 3) = theShiftY;

  theOutMx.Multiply (aViewportShift);
}

// =======================================================================
// function : PerspectiveProj
// purpose  :
// =======================================================================
void Graphic3d_Camera::PerspectiveProj (const Standard_ShortReal theLeft,
                                        const Standard_ShortReal theRight,
                                        const Standard_ShortReal theBottom,
                                        const Standard_ShortReal theTop,
                                        const Standard_ShortReal theNear,
                                        const Standard_ShortReal theFar,
                                        const Standard_ShortReal theShiftX,
                                        const Standard_ShortReal theShiftY,
                                        Graphic3d_Mat4& theOutMx)
{
  // column 0
  theOutMx.ChangeValue (0, 0) = (2.0f * theNear) / (theRight - theLeft);
  theOutMx.ChangeValue (1, 0) = 0.0f;
  theOutMx.ChangeValue (2, 0) = 0.0f;
  theOutMx.ChangeValue (3, 0) = 0.0f;

  // column 1
  theOutMx.ChangeValue (0, 1) = 0.0f;
  theOutMx.ChangeValue (1, 1) = (2.0f * theNear) / (theTop - theBottom);
  theOutMx.ChangeValue (2, 1) = 0.0f;
  theOutMx.ChangeValue (3, 1) = 0.0f;

  // column 2
  theOutMx.ChangeValue (0, 2) = (theRight + theLeft) / (theRight - theLeft);
  theOutMx.ChangeValue (1, 2) = (theTop + theBottom) / (theTop - theBottom);
  theOutMx.ChangeValue (2, 2) = -(theFar + theNear) / (theFar - theNear);
  theOutMx.ChangeValue (3, 2) = -1.0f;

  // column 3
  theOutMx.ChangeValue (0, 3) = 0.0f;
  theOutMx.ChangeValue (1, 3) = 0.0f;
  theOutMx.ChangeValue (2, 3) = -(2.0f * theFar * theNear) / (theFar - theNear);
  theOutMx.ChangeValue (3, 3) = 0.0f;

  Graphic3d_Mat4 aViewportShift;
  aViewportShift.ChangeValue (0, 3) = theShiftX;
  aViewportShift.ChangeValue (1, 3) = theShiftY;

  theOutMx.Multiply (aViewportShift);
}

// =======================================================================
// function : StereoEyeProj
// purpose  :
// =======================================================================
void Graphic3d_Camera::StereoEyeProj (const Standard_ShortReal theLeft,
                                      const Standard_ShortReal theRight,
                                      const Standard_ShortReal theBottom,
                                      const Standard_ShortReal theTop,
                                      const Standard_ShortReal theNear,
                                      const Standard_ShortReal theFar,
                                      const Standard_ShortReal theIOD,
                                      const Standard_ShortReal theZFocus,
                                      const Standard_ShortReal theShiftX,
                                      const Standard_ShortReal theShiftY,
                                      const Standard_Boolean   theIsLeft,
                                      Graphic3d_Mat4& theOutMx)
{
  Standard_ShortReal aDx = theIsLeft ? ( 0.5f * theIOD) : (-0.5f * theIOD);
  Standard_ShortReal aDXStereoShift = aDx * theNear / theZFocus;

  // construct eye projection matrix
  PerspectiveProj (theLeft  + aDXStereoShift,
                   theRight + aDXStereoShift,
                   theBottom, theTop, theNear, theFar,
                   theShiftX, theShiftY,
                   theOutMx);

  if (theIOD != 0.0f)
  {
    // X translation to cancel parallax
    theOutMx.Translate (Graphic3d_Vec3 (aDx, 0.0f, 0.0f));
  }
}

// =======================================================================
// function : LookOrientation
// purpose  :
// =======================================================================
void Graphic3d_Camera::LookOrientation (const Graphic3d_Vec3& theEye,
                                        const Graphic3d_Vec3& theLookAt,
                                        Graphic3d_Vec3& theUpDir,
                                        const Graphic3d_Vec3& theAxialScale,
                                        Graphic3d_Mat4& theOutMx)
{
  Graphic3d_Vec3 aForward = theLookAt - theEye;
  aForward.Normalize();

  // side = forward x up
  Graphic3d_Vec3 aSide = Graphic3d_Vec3::Cross (aForward, theUpDir);
  aSide.Normalize();

  // recompute up as: up = side x forward
  Graphic3d_Vec3 anUp = Graphic3d_Vec3::Cross (aSide, aForward);
  theUpDir = anUp;

  Graphic3d_Mat4 aLookMx;
  aLookMx.SetRow (0, aSide);
  aLookMx.SetRow (1, anUp);
  aLookMx.SetRow (2, -aForward);

  theOutMx.InitIdentity(); 
  theOutMx.Multiply (aLookMx); 

  theOutMx.Translate (-theEye);

  Graphic3d_Mat4 anAxialScaleMx;
  anAxialScaleMx.ChangeValue (0, 0) = theAxialScale.x();
  anAxialScaleMx.ChangeValue (1, 1) = theAxialScale.y();
  anAxialScaleMx.ChangeValue (2, 2) = theAxialScale.z();

  theOutMx.Multiply (anAxialScaleMx);
}
