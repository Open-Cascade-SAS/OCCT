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
#include <Standard_Assert.hxx>

#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_HANDLE(Graphic3d_Camera, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Camera, Standard_Transient)

namespace
{
  // (degrees -> radians) * 0.5
  static const Standard_Real DTR_HALF = 0.5 * 0.0174532925;

  // default property values
  static const Standard_Real DEFAULT_ZNEAR = 0.001;
  static const Standard_Real DEFAULT_ZFAR  = 3000.0;

  // atomic state counter
  static volatile Standard_Integer THE_STATE_COUNTER = 0;

  // minimum camera distance
  static const Standard_Real MIN_DISTANCE = Pow (0.1, ShortRealDigits() - 2);
};

// =======================================================================
// function : Graphic3d_Camera
// purpose  :
// =======================================================================
Graphic3d_Camera::Graphic3d_Camera()
: myUp (0.0, 1.0, 0.0),
  myEye (0.0, 0.0, -1500.0),
  myCenter (0.0, 0.0, 0.0),
  myAxialScale (1.0, 1.0, 1.0),
  myProjType (Projection_Orthographic),
  myFOVy (45.0),
  myZNear (DEFAULT_ZNEAR),
  myZFar (DEFAULT_ZFAR),
  myAspect (1.0),
  myScale (1000.0),
  myZFocus (1.0),
  myZFocusType (FocusType_Relative),
  myIOD (0.05),
  myIODType (IODType_Relative)
{
  myProjectionState  = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
  myOrientationState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
}

// =======================================================================
// function : Graphic3d_Camera
// purpose  :
// =======================================================================
Graphic3d_Camera::Graphic3d_Camera (const Handle(Graphic3d_Camera)& theOther)
{
  Copy (theOther);
}

// =======================================================================
// function : CopyMappingData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyMappingData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  myFOVy            = theOtherCamera->myFOVy;
  myZNear           = theOtherCamera->myZNear;
  myZFar            = theOtherCamera->myZFar;
  myAspect          = theOtherCamera->myAspect;
  myScale           = theOtherCamera->myScale;
  myZFocus          = theOtherCamera->myZFocus;
  myZFocusType      = theOtherCamera->myZFocusType;
  myIOD             = theOtherCamera->myIOD;
  myIODType         = theOtherCamera->myIODType;
  myProjType        = theOtherCamera->myProjType;
  myProjectionState = theOtherCamera->myProjectionState;

  InvalidateProjection();
}

// =======================================================================
// function : CopyOrientationData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyOrientationData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  myUp               = theOtherCamera->myUp;
  myEye              = theOtherCamera->myEye;
  myCenter           = theOtherCamera->myCenter;
  myAxialScale       = theOtherCamera->myAxialScale;
  myOrientationState = theOtherCamera->myOrientationState;

  InvalidateOrientation();
}

// =======================================================================
// function : Copy
// purpose  :
// =======================================================================
void Graphic3d_Camera::Copy (const Handle(Graphic3d_Camera)& theOther)
{
  CopyMappingData (theOther);
  CopyOrientationData (theOther);
}

// =======================================================================
// function : SetEye
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetEye (const gp_Pnt& theEye)
{
  myEye = theEye;
  InvalidateOrientation();
}

// =======================================================================
// function : SetCenter
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCenter (const gp_Pnt& theCenter)
{
  myCenter = theCenter;
  InvalidateOrientation();
}

// =======================================================================
// function : SetUp
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetUp (const gp_Dir& theUp)
{
  myUp = theUp;
  InvalidateOrientation();
}

// =======================================================================
// function : SetAxialScale
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetAxialScale (const gp_XYZ& theAxialScale)
{
  myAxialScale = theAxialScale;
  InvalidateOrientation();
}

// =======================================================================
// function : SetDistance
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDistance (const Standard_Real theDistance)
{
  gp_Vec aCenter2Eye (Direction());
  aCenter2Eye.Reverse();

  // Camera should have non-zero distance.
  aCenter2Eye.Scale (Max (theDistance, MIN_DISTANCE));
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

  InvalidateProjection();
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
      return Distance() * 2.0 * Tan (myFOVy * M_PI / 360.0);
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

  if (anOldType == Projection_Orthographic)
  {
    if (myZNear <= RealEpsilon())
    {
      myZNear = DEFAULT_ZNEAR;
    }
    if (myZFar <= RealEpsilon())
    {
      myZFar = DEFAULT_ZFAR;
    }
  }

  myProjType = theProjectionType;

  InvalidateProjection();
}

// =======================================================================
// function : SetFOVy
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetFOVy (const Standard_Real theFOVy)
{
  myFOVy = theFOVy;
  InvalidateProjection();
}

// =======================================================================
// function : SetZRange
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZRange (const Standard_Real theZNear,
                                  const Standard_Real theZFar)
{
  Standard_ASSERT_RAISE (theZFar > theZNear, "ZFar should be greater than ZNear");
  if (!IsOrthographic())
  {
    Standard_ASSERT_RAISE (theZNear > 0.0, "Only positive Z-Near is allowed for perspective camera");
    Standard_ASSERT_RAISE (theZFar  > 0.0, "Only positive Z-Far is allowed for perspective camera");
  }

  myZNear = theZNear;
  myZFar  = theZFar;

  InvalidateProjection();
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetAspect (const Standard_Real theAspect)
{
  myAspect = theAspect;
  InvalidateProjection();
}

// =======================================================================
// function : SetZFocus
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZFocus(const FocusType theType, const Standard_Real theZFocus)
{
  myZFocusType = theType;
  myZFocus = theZFocus;
  InvalidateProjection();
}

// =======================================================================
// function : SetIOD
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetIOD (const IODType theType, const Standard_Real theIOD)
{
  myIODType = theType;
  myIOD = theIOD;
  InvalidateProjection();
}

// =======================================================================
// function : OrthogonalizeUp
// purpose  :
// =======================================================================
void Graphic3d_Camera::OrthogonalizeUp()
{
  SetUp (OrthogonalizedUp());
}

// =======================================================================
// function : OrthogonalizedUp
// purpose  :
// =======================================================================
gp_Dir Graphic3d_Camera::OrthogonalizedUp() const
{
  gp_Dir aDir  = Direction();
  gp_Dir aLeft = aDir.Crossed (Up());

  // recompute up as: up = left x direction
  return aLeft.Crossed (aDir);
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
  InvalidateOrientation();
}

// =======================================================================
// function : safePointCast
// purpose  :
// =======================================================================
static Graphic3d_Vec4d safePointCast (const gp_Pnt& thePnt)
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
  Graphic3d_Vec4d aPnt (aSafePoint.X(), aSafePoint.Y(), aSafePoint.Z(), 1.0);

  return aPnt;
}

// =======================================================================
// function : Project
// purpose  :
// =======================================================================
gp_Pnt Graphic3d_Camera::Project (const gp_Pnt& thePnt) const
{
  const Graphic3d_Mat4d& aViewMx = OrientationMatrix();
  const Graphic3d_Mat4d& aProjMx = ProjectionMatrix();

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

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
  const Graphic3d_Mat4d& aViewMx = OrientationMatrix();
  const Graphic3d_Mat4d& aProjMx = ProjectionMatrix();

  Graphic3d_Mat4d aInvView;
  Graphic3d_Mat4d aInvProj;

  // this case should never happen
  if (!aViewMx.Inverted (aInvView) || !aProjMx.Inverted (aInvProj))
  {
    return gp_Pnt (0.0, 0.0, 0.0);
  }

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

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
  const Graphic3d_Mat4d& aProjMx = ProjectionMatrix();

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

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
  const Graphic3d_Mat4d& aProjMx = ProjectionMatrix();

  Graphic3d_Mat4d aInvProj;

  // this case should never happen, but...
  if (!aProjMx.Inverted (aInvProj))
  {
    return gp_Pnt (0, 0, 0);
  }

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

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
  const Graphic3d_Mat4d& aViewMx = OrientationMatrix();

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

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
  const Graphic3d_Mat4d& aViewMx = OrientationMatrix();

  Graphic3d_Mat4d aInvView;

  if (!aViewMx.Inverted (aInvView))
  {
    return gp_Pnt(0, 0, 0);
  }

  // use compatible type of point
  Graphic3d_Vec4d aPnt = safePointCast (thePnt);

  aPnt = aInvView * aPnt; // convert to world coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
}

// =======================================================================
// function : ViewDimensions
// purpose  :
// =======================================================================
gp_XYZ Graphic3d_Camera::ViewDimensions() const
{
  // view plane dimensions
  Standard_Real aSizeY = IsOrthographic() ? myScale : (2.0 * Distance() * Tan (DTR_HALF * myFOVy));
  Standard_Real aSizeX = myAspect * aSizeY;

  // and frustum depth
  return gp_XYZ (aSizeX, aSizeY, myZFar - myZNear);
}

// =======================================================================
// function : Frustum
// purpose  :
// =======================================================================
void Graphic3d_Camera::Frustum (gp_Pln& theLeft,
                                gp_Pln& theRight,
                                gp_Pln& theBottom,
                                gp_Pln& theTop,
                                gp_Pln& theNear,
                                gp_Pln& theFar) const
{
  gp_Vec aProjection = gp_Vec (Direction());
  gp_Vec anUp        = OrthogonalizedUp();
  gp_Vec aSide       = aProjection ^ anUp;

  Standard_ASSERT_RAISE (
    !aProjection.IsParallel (anUp, Precision::Angular()),
     "Can not derive SIDE = PROJ x UP - directions are parallel");

  theNear = gp_Pln (Eye().Translated (aProjection * ZNear()), aProjection);
  theFar  = gp_Pln (Eye().Translated (aProjection * ZFar()), -aProjection);

  Standard_Real aHScaleHor = Scale() * 0.5 * Aspect();
  Standard_Real aHScaleVer = Scale() * 0.5;

  gp_Pnt aPntLeft   = Center().Translated (aHScaleHor * -aSide);
  gp_Pnt aPntRight  = Center().Translated (aHScaleHor *  aSide);
  gp_Pnt aPntBottom = Center().Translated (aHScaleVer * -anUp);
  gp_Pnt aPntTop    = Center().Translated (aHScaleVer *  anUp);

  gp_Vec aDirLeft   =  aSide;
  gp_Vec aDirRight  = -aSide;
  gp_Vec aDirBottom =  anUp;
  gp_Vec aDirTop    = -anUp;
  if (!IsOrthographic())
  {
    Standard_Real aHFOVHor = ATan (Tan (DTR_HALF * FOVy()) * Aspect());
    Standard_Real aHFOVVer = DTR_HALF * FOVy();
    aDirLeft.Rotate   (gp_Ax1 (gp::Origin(), anUp),   aHFOVHor);
    aDirRight.Rotate  (gp_Ax1 (gp::Origin(), anUp),  -aHFOVHor);
    aDirBottom.Rotate (gp_Ax1 (gp::Origin(), aSide), -aHFOVVer);
    aDirTop.Rotate    (gp_Ax1 (gp::Origin(), aSide),  aHFOVVer);
  }

  theLeft   = gp_Pln (aPntLeft,   aDirLeft);
  theRight  = gp_Pln (aPntRight,  aDirRight);
  theBottom = gp_Pln (aPntBottom, aDirBottom);
  theTop    = gp_Pln (aPntTop,    aDirTop);
}

// =======================================================================
// function : OrientationMatrix
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::OrientationMatrix() const
{
  return *UpdateOrientation (myMatricesD).Orientation;
}

// =======================================================================
// function : OrientationMatrixF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::OrientationMatrixF() const
{
  return *UpdateOrientation (myMatricesF).Orientation;
}

// =======================================================================
// function : ProjectionMatrix
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionMatrix() const
{
  return *UpdateProjection (myMatricesD).MProjection;
}

// =======================================================================
// function : ProjectionMatrixF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionMatrixF() const
{
  return *UpdateProjection (myMatricesF).MProjection;
}

// =======================================================================
// function : ProjectionStereoLeft
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionStereoLeft() const
{
  return *UpdateProjection (myMatricesD).LProjection;
}

// =======================================================================
// function : ProjectionStereoLeftF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionStereoLeftF() const
{
  return *UpdateProjection (myMatricesF).LProjection;
}

// =======================================================================
// function : ProjectionStereoRight
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionStereoRight() const
{
  return *UpdateProjection (myMatricesD).RProjection;
}

// =======================================================================
// function : ProjectionStereoRightF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionStereoRightF() const
{
  return *UpdateProjection (myMatricesF).RProjection;
}

// =======================================================================
// function : UpdateProjection
// purpose  :
// =======================================================================
template <typename Elem_t>
Graphic3d_Camera::TransformMatrices<Elem_t>&
  Graphic3d_Camera::UpdateProjection (TransformMatrices<Elem_t>& theMatrices) const
{
  if (theMatrices.IsProjectionValid())
  {
    return theMatrices; // for inline accessors
  }

  theMatrices.InitProjection();

  // sets top of frustum based on FOVy and near clipping plane
  Elem_t aScale   = static_cast<Elem_t> (myScale);
  Elem_t aZNear   = static_cast<Elem_t> (myZNear);
  Elem_t aZFar    = static_cast<Elem_t> (myZFar);
  Elem_t anAspect = static_cast<Elem_t> (myAspect);
  Elem_t aDYHalf = 0.0;
  if (IsOrthographic())
  {
    aDYHalf = aScale * Elem_t (0.5);
  }
  else
  {
    aDYHalf = aZNear * Elem_t (Tan (DTR_HALF * myFOVy));
  }

  // sets right of frustum based on aspect ratio
  Elem_t aDXHalf = anAspect * aDYHalf;
  Elem_t aLeft   = -aDXHalf;
  Elem_t aRight  =  aDXHalf;
  Elem_t aBot    = -aDYHalf;
  Elem_t aTop    =  aDYHalf;

  Elem_t aIOD  = myIODType == IODType_Relative 
    ? static_cast<Elem_t> (myIOD * Distance())
    : static_cast<Elem_t> (myIOD);

  Elem_t aFocus = myZFocusType == FocusType_Relative 
    ? static_cast<Elem_t> (myZFocus * Distance())
    : static_cast<Elem_t> (myZFocus);

  switch (myProjType)
  {
    case Projection_Orthographic :
      OrthoProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, *theMatrices.MProjection);
      break;

    case Projection_Perspective :
      PerspectiveProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, *theMatrices.MProjection);
      break;

    case Projection_MonoLeftEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_True, *theMatrices.MProjection);
      break;
    }

    case Projection_MonoRightEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_False, *theMatrices.MProjection);
      break;
    }

    case Projection_Stereo :
    {
      PerspectiveProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, *theMatrices.MProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_True,
                     *theMatrices.LProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_False,
                     *theMatrices.RProjection);
      break;
    }
  }

  return theMatrices; // for inline accessors
}

// =======================================================================
// function : UpdateOrientation
// purpose  :
// =======================================================================
template <typename Elem_t>
Graphic3d_Camera::TransformMatrices<Elem_t>&
  Graphic3d_Camera::UpdateOrientation (TransformMatrices<Elem_t>& theMatrices) const
{
  if (theMatrices.IsOrientationValid())
  {
    return theMatrices; // for inline accessors
  }

  theMatrices.InitOrientation();

  NCollection_Vec3<Elem_t> anEye (static_cast<Elem_t> (myEye.X()),
                                  static_cast<Elem_t> (myEye.Y()),
                                  static_cast<Elem_t> (myEye.Z()));

  NCollection_Vec3<Elem_t> aCenter (static_cast<Elem_t> (myCenter.X()),
                                    static_cast<Elem_t> (myCenter.Y()),
                                    static_cast<Elem_t> (myCenter.Z()));

  NCollection_Vec3<Elem_t> anUp (static_cast<Elem_t> (myUp.X()),
                                 static_cast<Elem_t> (myUp.Y()),
                                 static_cast<Elem_t> (myUp.Z()));

  NCollection_Vec3<Elem_t> anAxialScale (static_cast<Elem_t> (myAxialScale.X()),
                                         static_cast<Elem_t> (myAxialScale.Y()),
                                         static_cast<Elem_t> (myAxialScale.Z()));

  LookOrientation (anEye, aCenter, anUp, anAxialScale, *theMatrices.Orientation);

  return theMatrices; // for inline accessors
}

// =======================================================================
// function : InvalidateProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::InvalidateProjection()
{
  myMatricesD.ResetProjection();
  myMatricesF.ResetProjection();
  myProjectionState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
}

// =======================================================================
// function : InvalidateOrientation
// purpose  :
// =======================================================================
void Graphic3d_Camera::InvalidateOrientation()
{
  myMatricesD.ResetOrientation();
  myMatricesF.ResetOrientation();
  myOrientationState = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
}

// =======================================================================
// function : OrthoProj
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::OrthoProj (const Elem_t theLeft,
                                  const Elem_t theRight,
                                  const Elem_t theBottom,
                                  const Elem_t theTop,
                                  const Elem_t theNear,
                                  const Elem_t theFar,
                                  NCollection_Mat4<Elem_t>& theOutMx)
{
  // row 0
  theOutMx.ChangeValue (0, 0) = Elem_t (2.0) / (theRight - theLeft);
  theOutMx.ChangeValue (0, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (0, 2) = Elem_t (0.0);
  theOutMx.ChangeValue (0, 3) = - (theRight + theLeft) / (theRight - theLeft);

  // row 1
  theOutMx.ChangeValue (1, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 1) = Elem_t (2.0) / (theTop - theBottom);
  theOutMx.ChangeValue (1, 2) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 3) = - (theTop + theBottom) / (theTop - theBottom);

  // row 2
  theOutMx.ChangeValue (2, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 2) = Elem_t (-2.0) / (theFar - theNear);
  theOutMx.ChangeValue (2, 3) = - (theFar + theNear) / (theFar - theNear);

  // row 3
  theOutMx.ChangeValue (3, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 2) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 3) = Elem_t (1.0);
}

// =======================================================================
// function : PerspectiveProj
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::PerspectiveProj (const Elem_t theLeft,
                                        const Elem_t theRight,
                                        const Elem_t theBottom,
                                        const Elem_t theTop,
                                        const Elem_t theNear,
                                        const Elem_t theFar,
                                        NCollection_Mat4<Elem_t>& theOutMx)
{
  // column 0
  theOutMx.ChangeValue (0, 0) = (Elem_t (2.0) * theNear) / (theRight - theLeft);
  theOutMx.ChangeValue (1, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 0) = Elem_t (0.0);

  // column 1
  theOutMx.ChangeValue (0, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 1) = (Elem_t (2.0) * theNear) / (theTop - theBottom);
  theOutMx.ChangeValue (2, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 1) = Elem_t (0.0);

  // column 2
  theOutMx.ChangeValue (0, 2) = (theRight + theLeft) / (theRight - theLeft);
  theOutMx.ChangeValue (1, 2) = (theTop + theBottom) / (theTop - theBottom);
  theOutMx.ChangeValue (2, 2) = -(theFar + theNear) / (theFar - theNear);
  theOutMx.ChangeValue (3, 2) = Elem_t (-1.0);

  // column 3
  theOutMx.ChangeValue (0, 3) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 3) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 3) = -(Elem_t (2.0) * theFar * theNear) / (theFar - theNear);
  theOutMx.ChangeValue (3, 3) = Elem_t (0.0);
}

// =======================================================================
// function : StereoEyeProj
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::StereoEyeProj (const Elem_t theLeft,
                                      const Elem_t theRight,
                                      const Elem_t theBottom,
                                      const Elem_t theTop,
                                      const Elem_t theNear,
                                      const Elem_t theFar,
                                      const Elem_t theIOD,
                                      const Elem_t theZFocus,
                                      const Standard_Boolean theIsLeft,
                                      NCollection_Mat4<Elem_t>& theOutMx)
{
  Elem_t aDx = theIsLeft ? Elem_t (0.5) * theIOD : Elem_t (-0.5) * theIOD;
  Elem_t aDXStereoShift = aDx * theNear / theZFocus;

  // construct eye projection matrix
  PerspectiveProj (theLeft  + aDXStereoShift,
                   theRight + aDXStereoShift,
                   theBottom, theTop, theNear, theFar,
                   theOutMx);

  if (theIOD != Elem_t (0.0))
  {
    // X translation to cancel parallax
    theOutMx.Translate (NCollection_Vec3<Elem_t> (aDx, Elem_t (0.0), Elem_t (0.0)));
  }
}

// =======================================================================
// function : LookOrientation
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::LookOrientation (const NCollection_Vec3<Elem_t>& theEye,
                                        const NCollection_Vec3<Elem_t>& theLookAt,
                                        const NCollection_Vec3<Elem_t>& theUpDir,
                                        const NCollection_Vec3<Elem_t>& theAxialScale,
                                        NCollection_Mat4<Elem_t>& theOutMx)
{
  NCollection_Vec3<Elem_t> aForward = theLookAt - theEye;
  aForward.Normalize();

  // side = forward x up
  NCollection_Vec3<Elem_t> aSide = NCollection_Vec3<Elem_t>::Cross (aForward, theUpDir);
  aSide.Normalize();

  // recompute up as: up = side x forward
  NCollection_Vec3<Elem_t> anUp = NCollection_Vec3<Elem_t>::Cross (aSide, aForward);

  NCollection_Mat4<Elem_t> aLookMx;
  aLookMx.SetRow (0, aSide);
  aLookMx.SetRow (1, anUp);
  aLookMx.SetRow (2, -aForward);

  theOutMx.InitIdentity();
  theOutMx.Multiply (aLookMx);
  theOutMx.Translate (-theEye);

  NCollection_Mat4<Elem_t> anAxialScaleMx;
  anAxialScaleMx.ChangeValue (0, 0) = theAxialScale.x();
  anAxialScaleMx.ChangeValue (1, 1) = theAxialScale.y();
  anAxialScaleMx.ChangeValue (2, 2) = theAxialScale.z();

  theOutMx.Multiply (anAxialScaleMx);
}

//=============================================================================
//function : ZFitAll
//purpose  :
//=============================================================================
void Graphic3d_Camera::ZFitAll (const Standard_Real theScaleFactor, const Bnd_Box& theMinMax, const Bnd_Box& theGraphicBB)
{
  Standard_ASSERT_RAISE (theScaleFactor > 0.0, "Zero or negative scale factor is not allowed.");

  // Method changes ZNear and ZFar planes of camera so as to fit the graphical structures
  // by their real boundaries (computed ignoring infinite flag) into the viewing volume.
  // In addition to the graphical boundaries, the usual min max used for fitting perspective
  // camera. To avoid numeric errors for perspective camera the negative ZNear values are
  // fixed using tolerance distance, relative to boundaries size. The tolerance distance
  // should be computed using information on boundaries of primary application actors,
  // (e.g. representing the displayed model) - to ensure that they are not unreasonably clipped.

  if (theGraphicBB.IsVoid())
  {
    // ShortReal precision factor used to add meaningful tolerance to
    // ZNear, ZFar values in order to avoid equality after type conversion
    // to ShortReal matrices type.
    const Standard_Real aPrecision = 1.0 / Pow (10.0, ShortRealDigits() - 1);

    Standard_Real aZFar  = Distance() * 3.0;
    Standard_Real aZNear = 0.0;

    if (!IsOrthographic())
    {
      if (aZFar < aPrecision)
      {
        // Invalid case when both values are negative
        aZNear = aPrecision;
        aZFar  = aPrecision * 2.0;
      }
      else if (aZNear < Abs (aZFar) * aPrecision)
      {
        // Z is less than 0.0, try to fix it using any appropriate z-scale
        aZNear = Abs (aZFar) * aPrecision;
      }
    }

    SetZRange (aZNear, aZFar);
    return;
  }

  // Measure depth of boundary points from camera eye
  NCollection_Sequence<gp_Pnt> aPntsToMeasure;

  Standard_Real aGraphicBB[6]; // real graphical boundaries (not accounting infinite flag).
  theGraphicBB.Get (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2], aGraphicBB[3], aGraphicBB[4], aGraphicBB[5]);

  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[1], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[4], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[4], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[1], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[1], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[4], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[4], aGraphicBB[5]));

  if (!theMinMax.IsVoid() && !theMinMax.IsWhole())
  {
    Standard_Real aMinMax[6]; // applicative min max boundaries
    theMinMax.Get (aMinMax[0], aMinMax[1], aMinMax[2], aMinMax[3], aMinMax[4], aMinMax[5]);

    aPntsToMeasure.Append (gp_Pnt (aMinMax[0], aMinMax[1], aMinMax[2]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[0], aMinMax[1], aMinMax[5]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[0], aMinMax[4], aMinMax[2]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[0], aMinMax[4], aMinMax[5]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[3], aMinMax[1], aMinMax[2]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[3], aMinMax[1], aMinMax[5]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[3], aMinMax[4], aMinMax[2]));
    aPntsToMeasure.Append (gp_Pnt (aMinMax[3], aMinMax[4], aMinMax[5]));
  }

  // Camera eye plane
  gp_Dir aCamDir = Direction();
  gp_Pnt aCamEye = myEye;
  gp_Pln aCamPln (aCamEye, aCamDir);

  Standard_Real aModelMinDist   = RealLast();
  Standard_Real aModelMaxDist   = RealFirst();
  Standard_Real aGraphicMinDist = RealLast();
  Standard_Real aGraphicMaxDist = RealFirst();

  const gp_XYZ& anAxialScale = myAxialScale;

  // Get minimum and maximum distances to the eye plane
  Standard_Integer aCounter = 0;
  NCollection_Sequence<gp_Pnt>::Iterator aPntIt(aPntsToMeasure);
  for (; aPntIt.More(); aPntIt.Next())
  {
    gp_Pnt aMeasurePnt = aPntIt.Value();

    aMeasurePnt = gp_Pnt (aMeasurePnt.X() * anAxialScale.X(),
                          aMeasurePnt.Y() * anAxialScale.Y(),
                          aMeasurePnt.Z() * anAxialScale.Z());

    Standard_Real aDistance = aCamPln.Distance (aMeasurePnt);

    // Check if the camera is intruded into the scene
    if (aCamDir.IsOpposite (gp_Vec (aCamEye, aMeasurePnt), M_PI * 0.5))
    {
      aDistance *= -1;
    }

    // the first eight points are from theGraphicBB, the last eight points are from theMinMax
    // (they can be absent).
    Standard_Real& aChangeMinDist = aCounter >= 8 ? aModelMinDist : aGraphicMinDist;
    Standard_Real& aChangeMaxDist = aCounter >= 8 ? aModelMaxDist : aGraphicMaxDist;
    aChangeMinDist = Min (aDistance, aChangeMinDist);
    aChangeMaxDist = Max (aDistance, aChangeMaxDist);
    aCounter++;
  }

  // Compute depth of bounding box center
  Standard_Real aMidDepth  = (aGraphicMinDist + aGraphicMaxDist) * 0.5;
  Standard_Real aHalfDepth = (aGraphicMaxDist - aGraphicMinDist) * 0.5;

  // ShortReal precision factor used to add meaningful tolerance to
  // ZNear, ZFar values in order to avoid equality after type conversion
  // to ShortReal matrices type.
  const Standard_Real aPrecision = Pow (0.1, ShortRealDigits() - 2);

  // Compute enlarged or shrank near and far z ranges
  Standard_Real aZNear = aMidDepth - aHalfDepth * theScaleFactor;
  Standard_Real aZFar  = aMidDepth + aHalfDepth * theScaleFactor;
  aZNear              -= aPrecision * 0.5;
  aZFar               += aPrecision * 0.5;

  if (!IsOrthographic())
  {
    if (aZFar >= aPrecision)
    {
      // To avoid numeric errors... (See comments in the beginning of the method).
      // Choose between model distance and graphical distance, as the model boundaries
      // might be infinite if all structures have infinite flag.
      const Standard_Real aGraphicDepth = aGraphicMaxDist >= aGraphicMinDist
        ? aGraphicMaxDist - aGraphicMinDist : RealLast();

      const Standard_Real aModelDepth = aModelMaxDist >= aModelMinDist
        ? aModelMaxDist - aModelMinDist : RealLast();

      const Standard_Real aMinDepth = Min (aModelDepth, aGraphicDepth);
      const Standard_Real aZTolerance =
        Max (Abs (aMinDepth) * aPrecision, aPrecision);

      if (aZNear < aZTolerance)
      {
        aZNear = aZTolerance;
      }
    }
    else // aZFar < aPrecision - Invalid case when both ZNear and ZFar are negative
    {
      aZNear = aPrecision;
      aZFar  = aPrecision * 2.0;
    }
  }

  // If range is too small
  if (aZFar < (aZNear + Abs (aZFar) * aPrecision))
  {
    aZFar = aZNear + Abs (aZFar) * aPrecision;
  }

  SetZRange (aZNear, aZFar);
}
