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

#include <Graphic3d_Camera.hxx>

#include <gp_Pln.hxx>
#include <gp_QuaternionNLerp.hxx>
#include <gp_QuaternionSLerp.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Atomic.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Camera,Standard_Transient)

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

  // z-range tolerance compatible with for floating point.
  static Standard_Real zEpsilon()
  {
    return FLT_EPSILON;
  }

  // relative z-range tolerance compatible with for floating point.
  static Standard_Real zEpsilon (const Standard_Real theValue)
  {
    Standard_Real anAbsValue = Abs (theValue);
    if (anAbsValue <= (double)FLT_MIN)
    {
      return FLT_MIN;
    }
    Standard_Real aLogRadix = Log10 (anAbsValue) / Log10 (FLT_RADIX);
    Standard_Real aExp = Floor (aLogRadix);
    return FLT_EPSILON * Pow (FLT_RADIX, aExp);
  }

  //! Convert camera definition to Ax3
  gp_Ax3 cameraToAx3 (const Graphic3d_Camera& theCamera)
  {
    const gp_Dir aBackDir(gp_Vec(theCamera.Center(), theCamera.Eye()));
    const gp_Dir anXAxis (theCamera.Up().Crossed (aBackDir));
    const gp_Dir anYAxis (aBackDir      .Crossed (anXAxis));
    const gp_Dir aZAxis  (anXAxis       .Crossed (anYAxis));
    return gp_Ax3 (gp_Pnt (0.0, 0.0, 0.0), aZAxis, anXAxis);
  }
}

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
  myFOVyTan (Tan (DTR_HALF * 45.0)),
  myZNear (DEFAULT_ZNEAR),
  myZFar (DEFAULT_ZFAR),
  myAspect (1.0),
  myScale (1000.0),
  myZFocus (1.0),
  myZFocusType (FocusType_Relative),
  myIOD (0.05),
  myIODType (IODType_Relative)
{
  myWorldViewProjState.Initialize ((Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER),
                                   (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER),
                                   this);
}

// =======================================================================
// function : Graphic3d_Camera
// purpose  :
// =======================================================================
Graphic3d_Camera::Graphic3d_Camera (const Handle(Graphic3d_Camera)& theOther)
{
  myWorldViewProjState.Initialize (this);

  Copy (theOther);
}

// =======================================================================
// function : CopyMappingData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyMappingData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  SetFOVy           (theOtherCamera->FOVy());
  SetZRange         (theOtherCamera->ZNear(), theOtherCamera->ZFar());
  SetAspect         (theOtherCamera->Aspect());
  SetScale          (theOtherCamera->Scale());
  SetZFocus         (theOtherCamera->ZFocusType(), theOtherCamera->ZFocus());
  SetIOD            (theOtherCamera->GetIODType(), theOtherCamera->IOD());
  SetProjectionType (theOtherCamera->ProjectionType());
  SetTile           (theOtherCamera->myTile);
}

// =======================================================================
// function : CopyOrientationData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyOrientationData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  SetUp         (theOtherCamera->Up());
  SetEye        (theOtherCamera->Eye());
  SetCenter     (theOtherCamera->Center());
  SetAxialScale (theOtherCamera->AxialScale());
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
  if (Eye().IsEqual (theEye, 0.0))
  {
    return;
  }

  myEye = theEye;
  InvalidateOrientation();
}

// =======================================================================
// function : SetCenter
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCenter (const gp_Pnt& theCenter)
{
  if (Center().IsEqual (theCenter, 0.0))
  {
    return;
  }

  myCenter = theCenter;
  InvalidateOrientation();
}

// =======================================================================
// function : SetUp
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetUp (const gp_Dir& theUp)
{
  if (Up().IsEqual (theUp, 0.0))
  {
    return;
  }

  myUp = theUp;
  InvalidateOrientation();
}

// =======================================================================
// function : SetAxialScale
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetAxialScale (const gp_XYZ& theAxialScale)
{
  if (AxialScale().IsEqual (theAxialScale, 0.0))
  {
    return;
  }

  myAxialScale = theAxialScale;
  InvalidateOrientation();
}

// =======================================================================
// function : SetDistance
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDistance (const Standard_Real theDistance)
{
  if (Distance() == theDistance)
  {
    return;
  }

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
  if (Direction().IsEqual (theDir, 0.0))
  {
    return;
  }

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
  if (Scale() == theScale)
  {
    return;
  }

  myScale = theScale;

  switch (myProjType)
  {
    case Projection_Perspective  :
    case Projection_Stereo       :
    case Projection_MonoLeftEye  :
    case Projection_MonoRightEye :
    {
      Standard_Real aDistance = theScale * 0.5 / myFOVyTan;
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
      return Distance() * 2.0 * myFOVyTan;
  }
}

// =======================================================================
// function : SetProjectionType
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetProjectionType (const Projection theProjectionType)
{
  Projection anOldType = ProjectionType();

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
  if (FOVy() == theFOVy)
  {
    return;
  }

  myFOVy = theFOVy;
  myFOVyTan = Tan(DTR_HALF * myFOVy);

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

  if (ZNear() == theZNear
   && ZFar () == theZFar)
  {
    return;
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
  if (Aspect() == theAspect)
  {
    return;
  }

  myAspect = theAspect;

  InvalidateProjection();
}

// =======================================================================
// function : SetZFocus
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetZFocus(const FocusType theType, const Standard_Real theZFocus)
{
  if (ZFocusType() == theType
   && ZFocus    () == theZFocus)
  {
    return;
  }

  myZFocusType = theType;
  myZFocus     = theZFocus;

  InvalidateProjection();
}

// =======================================================================
// function : SetIOD
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetIOD (const IODType theType, const Standard_Real theIOD)
{
  if (GetIODType() == theType
   && IOD       () == theIOD)
  {
    return;
  }

  myIODType = theType;
  myIOD     = theIOD;

  InvalidateProjection();
}

// =======================================================================
// function : SetTile
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetTile (const Graphic3d_CameraTile& theTile)
{
  if (myTile == theTile)
  {
    return;
  }

  myTile = theTile;
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
  if (theTrsf.Form() == gp_Identity)
  {
    return;
  }

  SetUp     (myUp.Transformed (theTrsf));
  SetEye    (myEye.Transformed (theTrsf));
  SetCenter (myCenter.Transformed (theTrsf));
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
gp_XYZ Graphic3d_Camera::ViewDimensions (const Standard_Real theZValue) const
{
  // view plane dimensions
  Standard_Real aSize = IsOrthographic() ? myScale : (2.0 * theZValue * myFOVyTan);
  Standard_Real aSizeX, aSizeY;
  if (myAspect > 1.0)
  {
    aSizeX = aSize * myAspect;
    aSizeY = aSize;
  }
  else
  {
    aSizeX = aSize;
    aSizeY = aSize / myAspect;
  }

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
  return UpdateOrientation (myMatricesD).Orientation;
}

// =======================================================================
// function : OrientationMatrixF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::OrientationMatrixF() const
{
  return UpdateOrientation (myMatricesF).Orientation;
}

// =======================================================================
// function : ProjectionMatrix
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionMatrix() const
{
  return UpdateProjection (myMatricesD).MProjection;
}

// =======================================================================
// function : ProjectionMatrixF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionMatrixF() const
{
  return UpdateProjection (myMatricesF).MProjection;
}

// =======================================================================
// function : ProjectionStereoLeft
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionStereoLeft() const
{
  return UpdateProjection (myMatricesD).LProjection;
}

// =======================================================================
// function : ProjectionStereoLeftF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionStereoLeftF() const
{
  return UpdateProjection (myMatricesF).LProjection;
}

// =======================================================================
// function : ProjectionStereoRight
// purpose  :
// =======================================================================
const Graphic3d_Mat4d& Graphic3d_Camera::ProjectionStereoRight() const
{
  return UpdateProjection (myMatricesD).RProjection;
}

// =======================================================================
// function : ProjectionStereoRightF
// purpose  :
// =======================================================================
const Graphic3d_Mat4& Graphic3d_Camera::ProjectionStereoRightF() const
{
  return UpdateProjection (myMatricesF).RProjection;
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
  Elem_t aDXHalf = 0.0, aDYHalf = 0.0;
  if (IsOrthographic())
  {
    aDXHalf = aScale * Elem_t (0.5);
    aDYHalf = aScale * Elem_t (0.5);
  }
  else
  {
    aDXHalf = aZNear * Elem_t (myFOVyTan);
    aDYHalf = aZNear * Elem_t (myFOVyTan);
  }

  if (anAspect > 1.0)
  {
    aDXHalf *= anAspect;
  }
  else
  {
    aDYHalf /= anAspect;
  }

  // sets right of frustum based on aspect ratio
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

  if (myTile.IsValid())
  {
    const Elem_t aDXFull = Elem_t(2) * aDXHalf;
    const Elem_t aDYFull = Elem_t(2) * aDYHalf;
    const Graphic3d_Vec2i anOffset = myTile.OffsetLowerLeft();
    aLeft  = -aDXHalf + aDXFull * static_cast<Elem_t> (anOffset.x())                       / static_cast<Elem_t> (myTile.TotalSize.x());
    aRight = -aDXHalf + aDXFull * static_cast<Elem_t> (anOffset.x() + myTile.TileSize.x()) / static_cast<Elem_t> (myTile.TotalSize.x());
    aBot   = -aDYHalf + aDYFull * static_cast<Elem_t> (anOffset.y())                       / static_cast<Elem_t> (myTile.TotalSize.y());
    aTop   = -aDYHalf + aDYFull * static_cast<Elem_t> (anOffset.y() + myTile.TileSize.y()) / static_cast<Elem_t> (myTile.TotalSize.y());
  }

  switch (myProjType)
  {
    case Projection_Orthographic :
      OrthoProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, theMatrices.MProjection);
      break;

    case Projection_Perspective :
      PerspectiveProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, theMatrices.MProjection);
      break;

    case Projection_MonoLeftEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_True, theMatrices.MProjection);
      theMatrices.LProjection = theMatrices.MProjection;
      break;
    }

    case Projection_MonoRightEye :
    {
      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_False, theMatrices.MProjection);
      theMatrices.RProjection = theMatrices.MProjection;
      break;
    }

    case Projection_Stereo :
    {
      PerspectiveProj (aLeft, aRight, aBot, aTop, aZNear, aZFar, theMatrices.MProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_True,
                     theMatrices.LProjection);

      StereoEyeProj (aLeft, aRight, aBot, aTop,
                     aZNear, aZFar, aIOD, aFocus,
                     Standard_False,
                     theMatrices.RProjection);
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

  LookOrientation (anEye, aCenter, anUp, anAxialScale, theMatrices.Orientation);

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
  myWorldViewProjState.ProjectionState() = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
}

// =======================================================================
// function : InvalidateOrientation
// purpose  :
// =======================================================================
void Graphic3d_Camera::InvalidateOrientation()
{
  myMatricesD.ResetOrientation();
  myMatricesF.ResetOrientation();
  myWorldViewProjState.WorldViewState() = (Standard_Size)Standard_Atomic_Increment (&THE_STATE_COUNTER);
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
bool Graphic3d_Camera::ZFitAll (const Standard_Real theScaleFactor,
                                const Bnd_Box&      theMinMax,
                                const Bnd_Box&      theGraphicBB,
                                Standard_Real&      theZNear,
                                Standard_Real&      theZFar) const
{
  Standard_ASSERT_RAISE (theScaleFactor > 0.0, "Zero or negative scale factor is not allowed.");

  // Method changes zNear and zFar parameters of camera so as to fit graphical structures
  // by their graphical boundaries. It precisely fits min max boundaries of primary application
  // objects (second argument), while it can sacrifice the real graphical boundaries of the
  // scene with infinite or helper objects (third argument) for the sake of perspective projection.
  if (theGraphicBB.IsVoid())
  {
    theZNear = DEFAULT_ZNEAR;
    theZFar  = DEFAULT_ZFAR;
    return false;
  }

  // Measure depth of boundary points from camera eye.
  NCollection_Sequence<gp_Pnt> aPntsToMeasure;

  Standard_Real aGraphicBB[6];
  theGraphicBB.Get (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2], aGraphicBB[3], aGraphicBB[4], aGraphicBB[5]);

  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[1], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[4], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[0], aGraphicBB[4], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[1], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[1], aGraphicBB[5]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[4], aGraphicBB[2]));
  aPntsToMeasure.Append (gp_Pnt (aGraphicBB[3], aGraphicBB[4], aGraphicBB[5]));

  Standard_Boolean isFiniteMinMax = !theMinMax.IsVoid() && !theMinMax.IsWhole();

  if (isFiniteMinMax)
  {
    Standard_Real aMinMax[6];
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

  // Camera eye plane.
  gp_Dir aCamDir = Direction();
  gp_Pnt aCamEye = myEye;
  gp_Pln aCamPln (aCamEye, aCamDir);

  Standard_Real aModelMinDist = RealLast();
  Standard_Real aModelMaxDist = RealFirst();
  Standard_Real aGraphMinDist = RealLast();
  Standard_Real aGraphMaxDist = RealFirst();

  const gp_XYZ& anAxialScale = myAxialScale;

  // Get minimum and maximum distances to the eye plane.
  Standard_Integer aCounter = 0;
  NCollection_Sequence<gp_Pnt>::Iterator aPntIt(aPntsToMeasure);
  for (; aPntIt.More(); aPntIt.Next())
  {
    gp_Pnt aMeasurePnt = aPntIt.Value();

    aMeasurePnt = gp_Pnt (aMeasurePnt.X() * anAxialScale.X(),
                          aMeasurePnt.Y() * anAxialScale.Y(),
                          aMeasurePnt.Z() * anAxialScale.Z());

    Standard_Real aDistance = aCamPln.Distance (aMeasurePnt);

    // Check if the camera is intruded into the scene.
    if (aCamDir.IsOpposite (gp_Vec (aCamEye, aMeasurePnt), M_PI * 0.5))
    {
      aDistance *= -1;
    }

    // The first eight points are from theGraphicBB, the last eight points are from theMinMax (can be absent).
    Standard_Real& aChangeMinDist = aCounter >= 8 ? aModelMinDist : aGraphMinDist;
    Standard_Real& aChangeMaxDist = aCounter >= 8 ? aModelMaxDist : aGraphMaxDist;
    aChangeMinDist = Min (aDistance, aChangeMinDist);
    aChangeMaxDist = Max (aDistance, aChangeMaxDist);
    aCounter++;
  }

  // Compute depth of bounding box center.
  Standard_Real aMidDepth  = (aGraphMinDist + aGraphMaxDist) * 0.5;
  Standard_Real aHalfDepth = (aGraphMaxDist - aGraphMinDist) * 0.5;

  // Compute enlarged or shrank near and far z ranges.
  Standard_Real aZNear  = aMidDepth - aHalfDepth * theScaleFactor;
  Standard_Real aZFar   = aMidDepth + aHalfDepth * theScaleFactor;

  if (!IsOrthographic())
  {
    // Everything is behind the perspective camera.
    if (aZFar < zEpsilon())
    {
      theZNear = DEFAULT_ZNEAR;
      theZFar  = DEFAULT_ZFAR;
      return false;
    }
  }

  //
  // Consider clipping errors due to double to single precision floating-point conversion.
  //

  // Model to view transformation performs translation of points against eye position
  // in three dimensions. Both point coordinate and eye position values are converted from
  // double to single precision floating point numbers producing conversion errors. 
  // Epsilon (Mod) * 3.0 should safely compensate precision error for z coordinate after
  // translation assuming that the:
  // Epsilon (Eye.Mod()) * 3.0 > Epsilon (Eye.X()) + Epsilon (Eye.Y()) + Epsilon (Eye.Z()).
  Standard_Real aEyeConf = 3.0 * zEpsilon (myEye.XYZ().Modulus());

  // Model to view transformation performs rotation of points according to view direction.
  // New z coordinate is computed as a multiplication of point's x, y, z coordinates by the
  // "forward" direction vector's x, y, z coordinates. Both point's and "z" direction vector's
  // values are converted from double to single precision floating point numbers producing
  // conversion errors.
  // Epsilon (Mod) * 6.0 should safely compensate the precision errors for the multiplication
  // of point coordinates by direction vector.
  gp_Pnt aGraphicMin = theGraphicBB.CornerMin();
  gp_Pnt aGraphicMax = theGraphicBB.CornerMax();

  Standard_Real aModelConf = 6.0 * zEpsilon (aGraphicMin.XYZ().Modulus()) +
                             6.0 * zEpsilon (aGraphicMax.XYZ().Modulus());

  // Compensate floating point conversion errors by increasing zNear, zFar to avoid clipping.
  aZNear -= zEpsilon (aZNear) + aEyeConf + aModelConf;
  aZFar  += zEpsilon (aZFar)  + aEyeConf + aModelConf;

  if (!IsOrthographic())
  {
    // For perspective projection, the value of z in normalized device coordinates is non-linear
    // function of eye z coordinate. For fixed-point depth representation resolution of z in
    // model-view space will grow towards zFar plane and its scale depends mostly on how far is zNear
    // against camera's eye. The purpose of the code below is to select most appropriate zNear distance
    // to balance between clipping (less zNear, more chances to observe closely small models without clipping)
    // and resolution of depth. A well applicable criteria to this is a ratio between resolution of z at center
    // of model boundaries and the distance to that center point. The ratio is chosen empirically and validated
    // by tests database. It is considered to be ~0.001 (0.1%) for 24 bit depth buffer, for less depth bitness
    // the zNear will be placed similarly giving lower resolution.
    // Approximation of the formula for respectively large z range is:
    // zNear = [z * (1 + k) / (k * c)],
    // where:
    // z - distance to center of model boundaries;
    // k - chosen ratio, c - capacity of depth buffer;
    // k = 0.001, k * c = 1677.216, (1 + k) / (k * c) ~ 5.97E-4
    //
    // The function uses center of model boundaries computed from "theMinMax" boundaries (instead of using real
    // graphical boundaries of all displayed objects). That means that it can sacrifice resolution of presentation
    // of non primary ("infinite") application graphical objects in favor of better perspective projection of the
    // small applicative objects measured with "theMinMax" values.
    Standard_Real aZRange   = isFiniteMinMax ? aModelMaxDist - aModelMinDist : aGraphMaxDist - aGraphMinDist;
    Standard_Real aZMin     = isFiniteMinMax ? aModelMinDist : aGraphMinDist;
    Standard_Real aZ        = aZMin < 0 ? aZRange / 2.0 : aZRange / 2.0 + aZMin;
    Standard_Real aZNearMin = aZ * 5.97E-4;
    if (aZNear < aZNearMin)
    {
      // Clip zNear according to the minimum value matching the quality.
      aZNear = aZNearMin;
    }
    else
    {
      // Compensate zNear conversion errors for perspective projection.
      aZNear -= aZFar * zEpsilon (aZNear) / (aZFar - zEpsilon (aZNear));
    }

    // Compensate zFar conversion errors for perspective projection.
    aZFar += zEpsilon (aZFar);

    // Ensure that after all the zNear is not a negative value.
    if (aZNear < zEpsilon())
    {
      aZNear = zEpsilon();
    }
  }

  theZNear = aZNear;
  theZFar  = aZFar;
  return true;
}

//=============================================================================
//function : Interpolate
//purpose  :
//=============================================================================
template<>
Standard_EXPORT void NCollection_Lerp<Handle(Graphic3d_Camera)>::Interpolate (const double theT,
                                                                              Handle(Graphic3d_Camera)& theCamera) const
{
  if (Abs (theT - 1.0) < Precision::Confusion())
  {
    // just copy end-point transformation
    theCamera->Copy (myEnd);
    return;
  }

  theCamera->Copy (myStart);
  if (Abs (theT - 0.0) < Precision::Confusion())
  {
    return;
  }

  // apply rotation
  {
    gp_Ax3 aCamStart = cameraToAx3 (*myStart);
    gp_Ax3 aCamEnd   = cameraToAx3 (*myEnd);
    gp_Trsf aTrsfStart, aTrsfEnd;
    aTrsfStart.SetTransformation (aCamStart, gp::XOY());
    aTrsfEnd  .SetTransformation (aCamEnd,   gp::XOY());

    gp_Quaternion aRotStart = aTrsfStart.GetRotation();
    gp_Quaternion aRotEnd   = aTrsfEnd  .GetRotation();
    gp_Quaternion aRotDelta = aRotEnd * aRotStart.Inverted();
    gp_Quaternion aRot = gp_QuaternionNLerp::Interpolate (gp_Quaternion(), aRotDelta, theT);
    gp_Trsf aTrsfRot;
    aTrsfRot.SetRotation (aRot);
    theCamera->Transform (aTrsfRot);
  }

  // apply translation
  {
    gp_XYZ aCenter  = NCollection_Lerp<gp_XYZ>::Interpolate (myStart->Center().XYZ(), myEnd->Center().XYZ(), theT);
    gp_XYZ anEye    = NCollection_Lerp<gp_XYZ>::Interpolate (myStart->Eye().XYZ(),    myEnd->Eye().XYZ(),    theT);
    gp_XYZ anAnchor = aCenter;
    Standard_Real aKc = 0.0;

    const Standard_Real aDeltaCenter = myStart->Center().Distance (myEnd->Center());
    const Standard_Real aDeltaEye    = myStart->Eye()   .Distance (myEnd->Eye());
    if (aDeltaEye <= gp::Resolution())
    {
      anAnchor = anEye;
      aKc = 1.0;
    }
    else if (aDeltaCenter > gp::Resolution())
    {
      aKc = aDeltaCenter / (aDeltaCenter + aDeltaEye);

      const gp_XYZ anAnchorStart = NCollection_Lerp<gp_XYZ>::Interpolate (myStart->Center().XYZ(), myStart->Eye().XYZ(), aKc);
      const gp_XYZ anAnchorEnd   = NCollection_Lerp<gp_XYZ>::Interpolate (myEnd  ->Center().XYZ(), myEnd  ->Eye().XYZ(), aKc);
      anAnchor = NCollection_Lerp<gp_XYZ>::Interpolate (anAnchorStart, anAnchorEnd, theT);
    }

    const gp_Vec        aDirEyeToCenter     = theCamera->Direction();
    const Standard_Real aDistEyeCenterStart = myStart->Eye().Distance (myStart->Center());
    const Standard_Real aDistEyeCenterEnd   = myEnd  ->Eye().Distance (myEnd  ->Center());
    const Standard_Real aDistEyeCenter      = NCollection_Lerp<Standard_Real>::Interpolate (aDistEyeCenterStart, aDistEyeCenterEnd, theT);
    aCenter = anAnchor + aDirEyeToCenter.XYZ() * aDistEyeCenter * aKc;
    anEye   = anAnchor - aDirEyeToCenter.XYZ() * aDistEyeCenter * (1.0 - aKc);

    theCamera->SetCenter (aCenter);
    theCamera->SetEye    (anEye);
  }

  // apply scaling
  if (Abs(myStart->Scale() - myEnd->Scale()) > Precision::Confusion()
   && myStart->IsOrthographic())
  {
    const Standard_Real aScale = NCollection_Lerp<Standard_Real>::Interpolate (myStart->Scale(), myEnd->Scale(), theT);
    theCamera->SetScale (aScale);
  }
}
