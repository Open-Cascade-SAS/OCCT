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
    const gp_Dir aBackDir = -theCamera.Direction();
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
  myDirection (0.0, 0.0, 1.0),
  myEye (0.0, 0.0, -1500.0),
  myDistance (1500.0),
  myAxialScale (1.0, 1.0, 1.0),
  myProjType (Projection_Orthographic),
  myFOVy (45.0),
  myFOVx (45.0),
  myFOV2d (180.0),
  myFOVyTan (Tan (DTR_HALF * 45.0)),
  myZNear (DEFAULT_ZNEAR),
  myZFar (DEFAULT_ZFAR),
  myAspect (1.0),
  myIsZeroToOneDepth (false),
  myScale (1000.0),
  myZFocus (1.0),
  myZFocusType (FocusType_Relative),
  myIOD (0.05),
  myIODType (IODType_Relative),
  myIsCustomProjMatM (false),
  myIsCustomProjMatLR(false),
  myIsCustomFrustomLR(false)
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
: myUp (0.0, 1.0, 0.0),
  myDirection (0.0, 0.0, 1.0),
  myEye (0.0, 0.0, -1500.0),
  myDistance (1500.0),
  myAxialScale (1.0, 1.0, 1.0),
  myProjType (Projection_Orthographic),
  myFOVy (45.0),
  myFOVx (45.0),
  myFOV2d (180.0),
  myFOVyTan (Tan (DTR_HALF * 45.0)),
  myZNear (DEFAULT_ZNEAR),
  myZFar (DEFAULT_ZFAR),
  myAspect (1.0),
  myIsZeroToOneDepth (false),
  myScale (1000.0),
  myZFocus (1.0),
  myZFocusType (FocusType_Relative),
  myIOD (0.05),
  myIODType (IODType_Relative),
  myIsCustomProjMatM (false),
  myIsCustomProjMatLR(false),
  myIsCustomFrustomLR(false)
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
  SetZeroToOneDepth (theOtherCamera->IsZeroToOneDepth());
  SetProjectionType (theOtherCamera->ProjectionType());
  SetFOVy           (theOtherCamera->FOVy());
  SetFOV2d          (theOtherCamera->FOV2d());
  SetZRange         (theOtherCamera->ZNear(), theOtherCamera->ZFar());
  SetAspect         (theOtherCamera->Aspect());
  SetScale          (theOtherCamera->Scale());
  SetZFocus         (theOtherCamera->ZFocusType(), theOtherCamera->ZFocus());
  SetIOD            (theOtherCamera->GetIODType(), theOtherCamera->IOD());
  SetTile           (theOtherCamera->myTile);

  ResetCustomProjection();
  if (theOtherCamera->IsCustomStereoProjection())
  {
    SetCustomStereoProjection (theOtherCamera->myCustomProjMatL,
                               theOtherCamera->myCustomHeadToEyeMatL,
                               theOtherCamera->myCustomProjMatR,
                               theOtherCamera->myCustomHeadToEyeMatR);
  }
  else if (theOtherCamera->IsCustomStereoFrustum())
  {
    SetCustomStereoFrustums (theOtherCamera->myCustomFrustumL, theOtherCamera->myCustomFrustumR);
  }
  if (theOtherCamera->IsCustomMonoProjection())
  {
    SetCustomMonoProjection (theOtherCamera->myCustomProjMatM);
  }
}

// =======================================================================
// function : CopyOrientationData
// purpose  :
// =======================================================================
void Graphic3d_Camera::CopyOrientationData (const Handle(Graphic3d_Camera)& theOtherCamera)
{
  if (!myEye.IsEqual (theOtherCamera->Eye(), 0.0)
   || !myUp.IsEqual (theOtherCamera->Up(), 0.0)
   || !myDirection.IsEqual (theOtherCamera->Direction(), 0.0)
   ||  myDistance != theOtherCamera->Distance())
  {
    myEye = theOtherCamera->Eye();
    myUp  = theOtherCamera->Up();
    myDirection = theOtherCamera->Direction();
    myDistance = theOtherCamera->Distance();
    InvalidateOrientation();
  }
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
// function : SetIdentityOrientation
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetIdentityOrientation()
{
  SetEyeAndCenter (gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, -1.0));
  SetUp (gp_Dir(0.0, 1.0, 0.0));
}

// =======================================================================
// function : MoveEyeTo
// purpose  :
// =======================================================================
void Graphic3d_Camera::MoveEyeTo (const gp_Pnt& theEye)
{
  if (myEye.IsEqual (theEye, 0.0))
  {
    return;
  }

  myEye = theEye;
  InvalidateOrientation();
}

// =======================================================================
// function : SetEyeAndCenter
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetEyeAndCenter (const gp_Pnt& theEye,
                                        const gp_Pnt& theCenter)
{
  if (Eye()   .IsEqual (theEye,    0.0)
   && Center().IsEqual (theCenter, 0.0))
  {
    return;
  }

  myEye = theEye;
  myDistance = theEye.Distance (theCenter);
  if (myDistance > gp::Resolution())
  {
    myDirection = gp_Dir (theCenter.XYZ() - theEye.XYZ());
  }
  InvalidateOrientation();
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

  const gp_Pnt aCenter = Center();
  myEye = theEye;
  myDistance = myEye.Distance (aCenter);
  if (myDistance > gp::Resolution())
  {
    myDirection = gp_Dir (aCenter.XYZ() - myEye.XYZ());
  }
  InvalidateOrientation();
}

// =======================================================================
// function : SetCenter
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCenter (const gp_Pnt& theCenter)
{
  const Standard_Real aDistance = myEye.Distance (theCenter);
  if (myDistance == aDistance)
  {
    return;
  }

  myDistance = aDistance;
  if (myDistance > gp::Resolution())
  {
    myDirection = gp_Dir (theCenter.XYZ() - myEye.XYZ());
  }
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
  if (myDistance == theDistance)
  {
    return;
  }

  const gp_Pnt aCenter = Center();
  myDistance = theDistance;
  myEye = aCenter.XYZ() - myDirection.XYZ() * myDistance;
  InvalidateOrientation();
}

// =======================================================================
// function : SetDirectionFromEye
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDirectionFromEye (const gp_Dir& theDir)
{
  if (myDirection.IsEqual (theDir, 0.0))
  {
    return;
  }

  myDirection = theDir;
  InvalidateOrientation();
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetDirection (const gp_Dir& theDir)
{
  if (myDirection.IsEqual (theDir, 0.0))
  {
    return;
  }

  const gp_Pnt aCenter = Center();
  myDirection = theDir;
  myEye = aCenter.XYZ() - theDir.XYZ() * myDistance;
  InvalidateOrientation();
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
  myFOVx = theFOVy * myAspect;
  myFOVyTan = Tan(DTR_HALF * myFOVy);

  InvalidateProjection();
}

// =======================================================================
// function : SetFOV2d
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetFOV2d (const Standard_Real theFOV)
{
  if (FOV2d() == theFOV)
  {
    return;
  }

  myFOV2d = theFOV;
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
  myFOVx = myFOVy * theAspect;

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

  myUp .Transform (theTrsf);
  myDirection.Transform (theTrsf);
  myEye.Transform (theTrsf);
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

  Standard_Real aHScaleHor = 0.0, aHScaleVer = 0.0;
  if (Aspect() >= 1.0)
  {
    aHScaleHor = Scale() * 0.5 * Aspect();
    aHScaleVer = Scale() * 0.5;
  }
  else
  {
    aHScaleHor = Scale() * 0.5;
    aHScaleVer = Scale() * 0.5 / Aspect();
  }

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
// function : ResetCustomProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::ResetCustomProjection()
{
  if (myIsCustomFrustomLR
   || myIsCustomProjMatLR
   || myIsCustomProjMatM)
  {
    myIsCustomFrustomLR = false;
    myIsCustomProjMatLR = false;
    myIsCustomProjMatM  = false;
    InvalidateProjection();
  }
}

// =======================================================================
// function : StereoProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::StereoProjection (Graphic3d_Mat4d& theProjL,
                                         Graphic3d_Mat4d& theHeadToEyeL,
                                         Graphic3d_Mat4d& theProjR,
                                         Graphic3d_Mat4d& theHeadToEyeR) const
{
  stereoProjection (theProjL, theHeadToEyeL, theProjR, theHeadToEyeR);
}

// =======================================================================
// function : StereoProjectionF
// purpose  :
// =======================================================================
void Graphic3d_Camera::StereoProjectionF (Graphic3d_Mat4& theProjL,
                                          Graphic3d_Mat4& theHeadToEyeL,
                                          Graphic3d_Mat4& theProjR,
                                          Graphic3d_Mat4& theHeadToEyeR) const
{
  stereoProjection (theProjL, theHeadToEyeL, theProjR, theHeadToEyeR);
}

// =======================================================================
// function : stereoProjection
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::stereoProjection (NCollection_Mat4<Elem_t>& theProjL,
                                         NCollection_Mat4<Elem_t>& theHeadToEyeL,
                                         NCollection_Mat4<Elem_t>& theProjR,
                                         NCollection_Mat4<Elem_t>& theHeadToEyeR) const
{
  if (myIsCustomProjMatLR)
  {
    theProjL     .ConvertFrom (myCustomProjMatL);
    theHeadToEyeL.ConvertFrom (myCustomHeadToEyeMatL);
    theProjR     .ConvertFrom (myCustomProjMatR);
    theHeadToEyeR.ConvertFrom (myCustomHeadToEyeMatR);
    return;
  }

  NCollection_Mat4<Elem_t> aDummy;
  computeProjection (aDummy, theProjL, theProjR, false);

  const Standard_Real aIOD = myIODType == IODType_Relative
                           ? myIOD * Distance()
                           : myIOD;
  if (aIOD != 0.0)
  {
    // X translation to cancel parallax
    theHeadToEyeL.InitIdentity();
    theHeadToEyeL.SetColumn (3, NCollection_Vec3<Elem_t> (Elem_t ( 0.5 * aIOD), Elem_t (0.0), Elem_t (0.0)));
    theHeadToEyeR.InitIdentity();
    theHeadToEyeR.SetColumn (3, NCollection_Vec3<Elem_t> (Elem_t (-0.5 * aIOD), Elem_t (0.0), Elem_t (0.0)));
  }
}

// =======================================================================
// function : SetCustomStereoFrustums
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCustomStereoFrustums (const Aspect_FrustumLRBT<Standard_Real>& theFrustumL,
                                                const Aspect_FrustumLRBT<Standard_Real>& theFrustumR)
{
  myCustomFrustumL = theFrustumL;
  myCustomFrustumR = theFrustumR;
  myIsCustomFrustomLR = true;
  myIsCustomProjMatLR = false;
  InvalidateProjection();
}

// =======================================================================
// function : SetCustomStereoProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCustomStereoProjection (const Graphic3d_Mat4d& theProjL,
                                                  const Graphic3d_Mat4d& theHeadToEyeL,
                                                  const Graphic3d_Mat4d& theProjR,
                                                  const Graphic3d_Mat4d& theHeadToEyeR)
{
  myCustomProjMatL = theProjL;
  myCustomProjMatR = theProjR;
  myCustomHeadToEyeMatL = theHeadToEyeL;
  myCustomHeadToEyeMatR = theHeadToEyeR;
  myIsCustomProjMatLR = true;
  myIsCustomFrustomLR = false;
  InvalidateProjection();
}

// =======================================================================
// function : SetCustomMonoProjection
// purpose  :
// =======================================================================
void Graphic3d_Camera::SetCustomMonoProjection (const Graphic3d_Mat4d& theProj)
{
  myCustomProjMatM = theProj;
  myIsCustomProjMatM = true;
  InvalidateProjection();
}

// =======================================================================
// function : computeProjection
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::computeProjection (NCollection_Mat4<Elem_t>& theProjM,
                                          NCollection_Mat4<Elem_t>& theProjL,
                                          NCollection_Mat4<Elem_t>& theProjR,
                                          bool theToAddHeadToEye) const
{
  theProjM.InitIdentity();
  theProjL.InitIdentity();
  theProjR.InitIdentity();

  // sets top of frustum based on FOVy and near clipping plane
  Elem_t aScale   = static_cast<Elem_t> (myScale);
  Elem_t aZNear   = static_cast<Elem_t> (myZNear);
  Elem_t aZFar    = static_cast<Elem_t> (myZFar);
  Elem_t anAspect = static_cast<Elem_t> (myAspect);
  Elem_t aDXHalf = 0.0, aDYHalf = 0.0;
  if (IsOrthographic())
  {
    aDXHalf = aDYHalf = aScale * Elem_t (0.5);
  }
  else
  {
    aDXHalf = aDYHalf = aZNear * Elem_t (myFOVyTan);
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
  Aspect_FrustumLRBT<Elem_t> anLRBT;
  anLRBT.Left   = -aDXHalf;
  anLRBT.Right  =  aDXHalf;
  anLRBT.Bottom = -aDYHalf;
  anLRBT.Top    =  aDYHalf;

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
    anLRBT.Left   = -aDXHalf + aDXFull * static_cast<Elem_t> (anOffset.x())                       / static_cast<Elem_t> (myTile.TotalSize.x());
    anLRBT.Right  = -aDXHalf + aDXFull * static_cast<Elem_t> (anOffset.x() + myTile.TileSize.x()) / static_cast<Elem_t> (myTile.TotalSize.x());
    anLRBT.Bottom = -aDYHalf + aDYFull * static_cast<Elem_t> (anOffset.y())                       / static_cast<Elem_t> (myTile.TotalSize.y());
    anLRBT.Top    = -aDYHalf + aDYFull * static_cast<Elem_t> (anOffset.y() + myTile.TileSize.y()) / static_cast<Elem_t> (myTile.TotalSize.y());
  }

  if (myIsCustomProjMatM)
  {
    theProjM.ConvertFrom (myCustomProjMatM);
  }
  switch (myProjType)
  {
    case Projection_Orthographic:
    {
      if (!myIsCustomProjMatM)
      {
        orthoProj (theProjM, anLRBT, aZNear, aZFar);
      }
      break;
    }
    case Projection_Perspective:
    {
      if (!myIsCustomProjMatM)
      {
        perspectiveProj (theProjM, anLRBT, aZNear, aZFar);
      }
      break;
    }
    case Projection_MonoLeftEye:
    case Projection_MonoRightEye:
    case Projection_Stereo:
    {
      if (!myIsCustomProjMatM)
      {
        perspectiveProj (theProjM, anLRBT, aZNear, aZFar);
      }
      if (myIsCustomProjMatLR)
      {
        if (theToAddHeadToEye)
        {
          theProjL.ConvertFrom (myCustomProjMatL * myCustomHeadToEyeMatL);
          theProjR.ConvertFrom (myCustomProjMatR * myCustomHeadToEyeMatR);
        }
        else
        {
          theProjL.ConvertFrom (myCustomProjMatL);
          theProjR.ConvertFrom (myCustomProjMatR);
        }
      }
      else if (myIsCustomFrustomLR)
      {
        anLRBT = Aspect_FrustumLRBT<Elem_t> (myCustomFrustumL).Multiplied (aZNear);
        perspectiveProj (theProjL, anLRBT, aZNear, aZFar);

        anLRBT = Aspect_FrustumLRBT<Elem_t> (myCustomFrustumR).Multiplied (aZNear);
        perspectiveProj (theProjR, anLRBT, aZNear, aZFar);
      }
      else
      {
        stereoEyeProj (theProjL,
                       anLRBT, aZNear, aZFar, aIOD, aFocus,
                       Aspect_Eye_Left);
        stereoEyeProj (theProjR,
                       anLRBT, aZNear, aZFar, aIOD, aFocus,
                       Aspect_Eye_Right);
      }

      if (theToAddHeadToEye
      && !myIsCustomProjMatLR
      &&  aIOD != Elem_t (0.0))
      {
        // X translation to cancel parallax
        theProjL.Translate (NCollection_Vec3<Elem_t> (Elem_t ( 0.5) * aIOD, Elem_t (0.0), Elem_t (0.0)));
        theProjR.Translate (NCollection_Vec3<Elem_t> (Elem_t (-0.5) * aIOD, Elem_t (0.0), Elem_t (0.0)));
      }
      break;
    }
  }
  if (myProjType == Projection_MonoLeftEye)
  {
    theProjM = theProjL;
  }
  else if (myProjType == Projection_MonoRightEye)
  {
    theProjM = theProjR;
  }
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

  NCollection_Vec3<Elem_t> aViewDir (static_cast<Elem_t> (myDirection.X()),
                                     static_cast<Elem_t> (myDirection.Y()),
                                     static_cast<Elem_t> (myDirection.Z()));

  NCollection_Vec3<Elem_t> anUp (static_cast<Elem_t> (myUp.X()),
                                 static_cast<Elem_t> (myUp.Y()),
                                 static_cast<Elem_t> (myUp.Z()));

  NCollection_Vec3<Elem_t> anAxialScale (static_cast<Elem_t> (myAxialScale.X()),
                                         static_cast<Elem_t> (myAxialScale.Y()),
                                         static_cast<Elem_t> (myAxialScale.Z()));

  LookOrientation (anEye, aViewDir, anUp, anAxialScale, theMatrices.Orientation);

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
// function : orthoProj
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::orthoProj (NCollection_Mat4<Elem_t>& theOutMx,
                                  const Aspect_FrustumLRBT<Elem_t>& theLRBT,
                                  const Elem_t theNear,
                                  const Elem_t theFar) const
{
  // row 0
  theOutMx.ChangeValue (0, 0) = Elem_t (2.0) / (theLRBT.Right - theLRBT.Left);
  theOutMx.ChangeValue (0, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (0, 2) = Elem_t (0.0);
  theOutMx.ChangeValue (0, 3) = - (theLRBT.Right + theLRBT.Left) / (theLRBT.Right - theLRBT.Left);

  // row 1
  theOutMx.ChangeValue (1, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 1) = Elem_t (2.0) / (theLRBT.Top - theLRBT.Bottom);
  theOutMx.ChangeValue (1, 2) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 3) = - (theLRBT.Top + theLRBT.Bottom) / (theLRBT.Top - theLRBT.Bottom);

  // row 2
  theOutMx.ChangeValue (2, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 1) = Elem_t (0.0);
  if (myIsZeroToOneDepth)
  {
    theOutMx.ChangeValue (2, 2) = Elem_t (-1.0) / (theFar - theNear);
    theOutMx.ChangeValue (2, 3) = -theNear / (theFar - theNear);
  }
  else
  {
    theOutMx.ChangeValue (2, 2) = Elem_t (-2.0) / (theFar - theNear);
    theOutMx.ChangeValue (2, 3) = - (theFar + theNear) / (theFar - theNear);
  }

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
void Graphic3d_Camera::perspectiveProj (NCollection_Mat4<Elem_t>& theOutMx,
                                        const Aspect_FrustumLRBT<Elem_t>& theLRBT,
                                        const Elem_t theNear,
                                        const Elem_t theFar) const
{
  // column 0
  theOutMx.ChangeValue (0, 0) = (Elem_t (2.0) * theNear) / (theLRBT.Right - theLRBT.Left);
  theOutMx.ChangeValue (1, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (2, 0) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 0) = Elem_t (0.0);

  // column 1
  theOutMx.ChangeValue (0, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 1) = (Elem_t (2.0) * theNear) / (theLRBT.Top - theLRBT.Bottom);
  theOutMx.ChangeValue (2, 1) = Elem_t (0.0);
  theOutMx.ChangeValue (3, 1) = Elem_t (0.0);

  // column 2
  theOutMx.ChangeValue (0, 2) = (theLRBT.Right + theLRBT.Left) / (theLRBT.Right - theLRBT.Left);
  theOutMx.ChangeValue (1, 2) = (theLRBT.Top + theLRBT.Bottom) / (theLRBT.Top - theLRBT.Bottom);
  if (myIsZeroToOneDepth)
  {
    theOutMx.ChangeValue (2, 2) = theFar / (theNear - theFar);
  }
  else
  {
    theOutMx.ChangeValue (2, 2) = -(theFar + theNear) / (theFar - theNear);
  }
  theOutMx.ChangeValue (3, 2) = Elem_t (-1.0);

  // column 3
  theOutMx.ChangeValue (0, 3) = Elem_t (0.0);
  theOutMx.ChangeValue (1, 3) = Elem_t (0.0);
  if (myIsZeroToOneDepth)
  {
    theOutMx.ChangeValue (2, 3) = -(theFar * theNear) / (theFar - theNear);
  }
  else
  {
    theOutMx.ChangeValue (2, 3) = -(Elem_t (2.0) * theFar * theNear) / (theFar - theNear);
  }
  theOutMx.ChangeValue (3, 3) = Elem_t (0.0);
}

// =======================================================================
// function : StereoEyeProj
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::stereoEyeProj (NCollection_Mat4<Elem_t>& theOutMx,
                                      const Aspect_FrustumLRBT<Elem_t>& theLRBT,
                                      const Elem_t theNear,
                                      const Elem_t theFar,
                                      const Elem_t theIOD,
                                      const Elem_t theZFocus,
                                      const Aspect_Eye theEyeIndex) const
{
  Elem_t aDx = theEyeIndex == Aspect_Eye_Left ? Elem_t (0.5) * theIOD : Elem_t (-0.5) * theIOD;
  Elem_t aDXStereoShift = aDx * theNear / theZFocus;

  // construct eye projection matrix
  Aspect_FrustumLRBT<Elem_t> aLRBT = theLRBT;
  aLRBT.Left  = theLRBT.Left  + aDXStereoShift;
  aLRBT.Right = theLRBT.Right + aDXStereoShift;
  perspectiveProj (theOutMx, aLRBT, theNear, theFar);
}

// =======================================================================
// function : LookOrientation
// purpose  :
// =======================================================================
template <typename Elem_t>
void Graphic3d_Camera::LookOrientation (const NCollection_Vec3<Elem_t>& theEye,
                                        const NCollection_Vec3<Elem_t>& theFwdDir,
                                        const NCollection_Vec3<Elem_t>& theUpDir,
                                        const NCollection_Vec3<Elem_t>& theAxialScale,
                                        NCollection_Mat4<Elem_t>& theOutMx)
{
  NCollection_Vec3<Elem_t> aForward = theFwdDir;
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

// =======================================================================
// function : FitMinMax
// purpose  :
// =======================================================================
bool Graphic3d_Camera::FitMinMax (const Bnd_Box& theBox,
                                  const Standard_Real theResolution,
                                  const bool theToEnlargeIfLine)
{
  // Check bounding box for validness
  if (theBox.IsVoid())
  {
    return false; // bounding box is out of bounds...
  }

  // Apply "axial scaling" to the bounding points.
  // It is not the best approach to make this scaling as a part of fit all operation,
  // but the axial scale is integrated into camera orientation matrix and the other
  // option is to perform frustum plane adjustment algorithm in view camera space,
  // which will lead to a number of additional world-view space conversions and
  // loosing precision as well.
  const gp_Pnt aBndMin = theBox.CornerMin().XYZ().Multiplied (myAxialScale);
  const gp_Pnt aBndMax = theBox.CornerMax().XYZ().Multiplied (myAxialScale);
  if (aBndMax.IsEqual (aBndMin, RealEpsilon()))
  {
    return false; // nothing to fit all
  }

  // Prepare camera frustum planes.
  gp_Pln aFrustumPlaneArray[6];
  NCollection_Array1<gp_Pln> aFrustumPlane (aFrustumPlaneArray[0], 1, 6);
  Frustum (aFrustumPlane[1], aFrustumPlane[2], aFrustumPlane[3],
           aFrustumPlane[4], aFrustumPlane[5], aFrustumPlane[6]);

  // Prepare camera up, side, direction vectors.
  const gp_Dir aCamUp  = OrthogonalizedUp();
  const gp_Dir aCamDir = Direction();
  const gp_Dir aCamSide = aCamDir ^ aCamUp;

  // Prepare scene bounding box parameters.
  const gp_Pnt aBndCenter = (aBndMin.XYZ() + aBndMax.XYZ()) / 2.0;

  gp_Pnt aBndCornerArray[8];
  NCollection_Array1<gp_Pnt> aBndCorner (aBndCornerArray[0], 1, 8);
  aBndCorner[1].SetCoord (aBndMin.X(), aBndMin.Y(), aBndMin.Z());
  aBndCorner[2].SetCoord (aBndMin.X(), aBndMin.Y(), aBndMax.Z());
  aBndCorner[3].SetCoord (aBndMin.X(), aBndMax.Y(), aBndMin.Z());
  aBndCorner[4].SetCoord (aBndMin.X(), aBndMax.Y(), aBndMax.Z());
  aBndCorner[5].SetCoord (aBndMax.X(), aBndMin.Y(), aBndMin.Z());
  aBndCorner[6].SetCoord (aBndMax.X(), aBndMin.Y(), aBndMax.Z());
  aBndCorner[7].SetCoord (aBndMax.X(), aBndMax.Y(), aBndMin.Z());
  aBndCorner[8].SetCoord (aBndMax.X(), aBndMax.Y(), aBndMax.Z());

  // Perspective-correct camera projection vector, matching the bounding box is determined geometrically.
  // Knowing the initial shape of a frustum it is possible to match it to a bounding box.
  // Then, knowing the relation of camera projection vector to the frustum shape it is possible to
  // set up perspective-correct camera projection matching the bounding box.
  // These steps support non-asymmetric transformations of view-projection space provided by camera.
  // The zooming can be done by calculating view plane size matching the bounding box at center of
  // the bounding box. The only limitation here is that the scale of camera should define size of
  // its view plane passing through the camera center, and the center of camera should be on the
  // same line with the center of bounding box.

  // The following method is applied:
  // 1) Determine normalized asymmetry of camera projection vector by frustum planes.
  // 2) Determine new location of frustum planes, "matching" the bounding box.
  // 3) Determine new camera projection vector using the normalized asymmetry.
  // 4) Determine new zooming in view space.

  // 1. Determine normalized projection asymmetry (if any).
  Standard_Real anAssymX = Tan (( aCamSide).Angle (aFrustumPlane[1].Axis().Direction()))
                         - Tan ((-aCamSide).Angle (aFrustumPlane[2].Axis().Direction()));
  Standard_Real anAssymY = Tan (( aCamUp)  .Angle (aFrustumPlane[3].Axis().Direction()))
                         - Tan ((-aCamUp)  .Angle (aFrustumPlane[4].Axis().Direction()));

  // 2. Determine how far should be the frustum planes placed from center
  //    of bounding box, in order to match the bounding box closely.
  Standard_Real aFitDistanceArray[6];
  NCollection_Array1<Standard_Real> aFitDistance (aFitDistanceArray[0], 1, 6);
  aFitDistance.Init (0.0);
  for (Standard_Integer anI = aFrustumPlane.Lower(); anI <= aFrustumPlane.Upper(); ++anI)
  {
    // Measure distances from center of bounding box to its corners towards the frustum plane.
    const gp_Dir& aPlaneN = aFrustumPlane[anI].Axis().Direction();

    Standard_Real& aFitDist = aFitDistance[anI];
    for (Standard_Integer aJ = aBndCorner.Lower(); aJ <= aBndCorner.Upper(); ++aJ)
    {
      aFitDist = Max (aFitDist, gp_Vec (aBndCenter, aBndCorner[aJ]).Dot (aPlaneN));
    }
  }
  // The center of camera is placed on the same line with center of bounding box.
  // The view plane section crosses the bounding box at its center.
  // To compute view plane size, evaluate coefficients converting "point -> plane distance"
  // into view section size between the point and the frustum plane.
  //       proj
  //       /|\   right half of frame     //
  //        |                           //
  //  point o<--  distance * coeff  -->//---- (view plane section)
  //         \                        //
  //      (distance)                 //
  //                ~               //
  //                 (distance)    //
  //                           \/\//
  //                            \//
  //                            //
  //                      (frustum plane)
  aFitDistance[1] *= Sqrt(1 + Pow (Tan (  aCamSide .Angle (aFrustumPlane[1].Axis().Direction())), 2.0));
  aFitDistance[2] *= Sqrt(1 + Pow (Tan ((-aCamSide).Angle (aFrustumPlane[2].Axis().Direction())), 2.0));
  aFitDistance[3] *= Sqrt(1 + Pow (Tan (  aCamUp   .Angle (aFrustumPlane[3].Axis().Direction())), 2.0));
  aFitDistance[4] *= Sqrt(1 + Pow (Tan ((-aCamUp)  .Angle (aFrustumPlane[4].Axis().Direction())), 2.0));
  aFitDistance[5] *= Sqrt(1 + Pow (Tan (  aCamDir  .Angle (aFrustumPlane[5].Axis().Direction())), 2.0));
  aFitDistance[6] *= Sqrt(1 + Pow (Tan ((-aCamDir) .Angle (aFrustumPlane[6].Axis().Direction())), 2.0));

  Standard_Real aViewSizeXv = aFitDistance[1] + aFitDistance[2];
  Standard_Real aViewSizeYv = aFitDistance[3] + aFitDistance[4];
  Standard_Real aViewSizeZv = aFitDistance[5] + aFitDistance[6];

  // 3. Place center of camera on the same line with center of bounding
  //    box applying corresponding projection asymmetry (if any).
  Standard_Real anAssymXv = anAssymX * aViewSizeXv * 0.5;
  Standard_Real anAssymYv = anAssymY * aViewSizeYv * 0.5;
  Standard_Real anOffsetXv = (aFitDistance[2] - aFitDistance[1]) * 0.5 + anAssymXv;
  Standard_Real anOffsetYv = (aFitDistance[4] - aFitDistance[3]) * 0.5 + anAssymYv;
  gp_Vec aTranslateSide = gp_Vec (aCamSide) * anOffsetXv;
  gp_Vec aTranslateUp   = gp_Vec (aCamUp)   * anOffsetYv;
  gp_Pnt aCamNewCenter  = aBndCenter.Translated (aTranslateSide).Translated (aTranslateUp);

  gp_Trsf aCenterTrsf;
  aCenterTrsf.SetTranslation (Center(), aCamNewCenter);
  Transform (aCenterTrsf);
  SetDistance (aFitDistance[6] + aFitDistance[5]);

  if (aViewSizeXv < theResolution
   && aViewSizeYv < theResolution)
  {
    // Bounding box collapses to a point or thin line going in depth of the screen
    if (aViewSizeXv < theResolution || !theToEnlargeIfLine)
    {
      return false; // This is just one point or line and zooming has no effect.
    }

    // Looking along line and "theToEnlargeIfLine" is requested.
    // Fit view to see whole scene on rotation.
    aViewSizeXv = aViewSizeZv;
    aViewSizeYv = aViewSizeZv;
  }

  const Standard_Real anAspect = Aspect();
  if (anAspect > 1.0)
  {
    SetScale (Max (aViewSizeXv / anAspect, aViewSizeYv));
  }
  else
  {
    SetScale (Max (aViewSizeXv, aViewSizeYv * anAspect));
  }
  return true;
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
    gp_Vec aVecToMeasurePnt (aCamEye, aMeasurePnt);
    if (aVecToMeasurePnt.Magnitude() > gp::Resolution()
     && aCamDir.IsOpposite (aVecToMeasurePnt, M_PI * 0.5))
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
      if (aZFar < aZNear)
      {
        aZFar = aZNear;
      }
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
    Standard_ASSERT_RAISE (aZFar > aZNear, "ZFar should be greater than ZNear");
  }

  theZNear = aZNear;
  theZFar  = aZFar;
  Standard_ASSERT_RAISE (aZFar > aZNear, "ZFar should be greater than ZNear");
  return true;
}

//=============================================================================
//function : Interpolate
//purpose  :
//=============================================================================
void Graphic3d_Camera::Interpolate (const Handle(Graphic3d_Camera)& theStart,
                                    const Handle(Graphic3d_Camera)& theEnd,
                                    const double theT,
                                    Handle(Graphic3d_Camera)& theCamera)
{
  if (Abs (theT - 1.0) < Precision::Confusion())
  {
    // just copy end-point transformation
    theCamera->Copy (theEnd);
    return;
  }

  theCamera->Copy (theStart);
  if (Abs (theT - 0.0) < Precision::Confusion())
  {
    return;
  }

  // apply rotation
  {
    gp_Ax3 aCamStart = cameraToAx3 (*theStart);
    gp_Ax3 aCamEnd   = cameraToAx3 (*theEnd);
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
    gp_XYZ aCenter  = NCollection_Lerp<gp_XYZ>::Interpolate (theStart->Center().XYZ(), theEnd->Center().XYZ(), theT);
    gp_XYZ anEye    = NCollection_Lerp<gp_XYZ>::Interpolate (theStart->Eye().XYZ(),    theEnd->Eye().XYZ(),    theT);
    gp_XYZ anAnchor = aCenter;
    Standard_Real aKc = 0.0;

    const Standard_Real aDeltaCenter = theStart->Center().Distance (theEnd->Center());
    const Standard_Real aDeltaEye    = theStart->Eye()   .Distance (theEnd->Eye());
    if (aDeltaEye <= gp::Resolution())
    {
      anAnchor = anEye;
      aKc = 1.0;
    }
    else if (aDeltaCenter > gp::Resolution())
    {
      aKc = aDeltaCenter / (aDeltaCenter + aDeltaEye);

      const gp_XYZ anAnchorStart = NCollection_Lerp<gp_XYZ>::Interpolate (theStart->Center().XYZ(), theStart->Eye().XYZ(), aKc);
      const gp_XYZ anAnchorEnd   = NCollection_Lerp<gp_XYZ>::Interpolate (theEnd  ->Center().XYZ(), theEnd  ->Eye().XYZ(), aKc);
      anAnchor = NCollection_Lerp<gp_XYZ>::Interpolate (anAnchorStart, anAnchorEnd, theT);
    }

    const gp_Vec        aDirEyeToCenter     = theCamera->Direction();
    const Standard_Real aDistEyeCenterStart = theStart->Eye().Distance (theStart->Center());
    const Standard_Real aDistEyeCenterEnd   = theEnd  ->Eye().Distance (theEnd  ->Center());
    const Standard_Real aDistEyeCenter      = NCollection_Lerp<Standard_Real>::Interpolate (aDistEyeCenterStart, aDistEyeCenterEnd, theT);
    aCenter = anAnchor + aDirEyeToCenter.XYZ() * aDistEyeCenter * aKc;
    anEye   = anAnchor - aDirEyeToCenter.XYZ() * aDistEyeCenter * (1.0 - aKc);

    theCamera->SetEyeAndCenter (anEye, aCenter);
  }

  // apply scaling
  if (Abs(theStart->Scale() - theEnd->Scale()) > Precision::Confusion()
   && theStart->IsOrthographic())
  {
    const Standard_Real aScale = NCollection_Lerp<Standard_Real>::Interpolate (theStart->Scale(), theEnd->Scale(), theT);
    theCamera->SetScale (aScale);
  }
}

//=======================================================================
//function : FrustumPoints
//purpose  :
//=======================================================================
void Graphic3d_Camera::FrustumPoints (NCollection_Array1<Graphic3d_Vec3d>& thePoints,
                                      const Graphic3d_Mat4d& theModelWorld) const
{
  if (thePoints.Length() != FrustumVerticesNB)
  {
    thePoints.Resize (0, FrustumVerticesNB, Standard_False);
  }

  const Graphic3d_Mat4d& aProjectionMat = ProjectionMatrix();
  const Graphic3d_Mat4d aWorldViewMat = OrientationMatrix() * theModelWorld;

  Standard_Real nLeft = 0.0, nRight = 0.0, nTop = 0.0, nBottom = 0.0;
  Standard_Real fLeft = 0.0, fRight = 0.0, fTop = 0.0, fBottom = 0.0;
  Standard_Real aNear = myZNear, aFar = myZFar;
  if (!IsOrthographic())
  {
    // handle perspective projection
    // Near plane
    nLeft   = aNear * (aProjectionMat.GetValue (0, 2) - 1.0) / aProjectionMat.GetValue (0, 0);
    nRight  = aNear * (aProjectionMat.GetValue (0, 2) + 1.0) / aProjectionMat.GetValue (0, 0);
    nTop    = aNear * (aProjectionMat.GetValue (1, 2) + 1.0) / aProjectionMat.GetValue (1, 1);
    nBottom = aNear * (aProjectionMat.GetValue (1, 2) - 1.0) / aProjectionMat.GetValue (1, 1);
    // Far plane
    fLeft   = aFar  * (aProjectionMat.GetValue (0, 2) - 1.0) / aProjectionMat.GetValue (0, 0);
    fRight  = aFar  * (aProjectionMat.GetValue (0, 2) + 1.0) / aProjectionMat.GetValue (0, 0);
    fTop    = aFar  * (aProjectionMat.GetValue (1, 2) + 1.0) / aProjectionMat.GetValue (1, 1);
    fBottom = aFar  * (aProjectionMat.GetValue (1, 2) - 1.0) / aProjectionMat.GetValue (1, 1);
  }
  else
  {
    // handle orthographic projection
    // Near plane
    nLeft   = ( 1.0 + aProjectionMat.GetValue (0, 3)) / (-aProjectionMat.GetValue (0, 0));
    fLeft   = nLeft;
    nRight  = ( 1.0 - aProjectionMat.GetValue (0, 3)) /   aProjectionMat.GetValue (0, 0);
    fRight  = nRight;
    nTop    = ( 1.0 - aProjectionMat.GetValue (1, 3)) /   aProjectionMat.GetValue (1, 1);
    fTop    = nTop;
    nBottom = (-1.0 - aProjectionMat.GetValue (1, 3)) /   aProjectionMat.GetValue (1, 1);
    fBottom = nBottom;
  }

  Graphic3d_Vec4d aLeftTopNear     (nLeft,  nTop,    -aNear, 1.0), aRightBottomFar (fRight, fBottom, -aFar, 1.0);
  Graphic3d_Vec4d aLeftBottomNear  (nLeft,  nBottom, -aNear, 1.0), aRightTopFar    (fRight, fTop,    -aFar, 1.0);
  Graphic3d_Vec4d aRightBottomNear (nRight, nBottom, -aNear, 1.0), aLeftTopFar     (fLeft,  fTop,    -aFar, 1.0);
  Graphic3d_Vec4d aRightTopNear    (nRight, nTop,    -aNear, 1.0), aLeftBottomFar  (fLeft,  fBottom, -aFar, 1.0);

  Graphic3d_Mat4d anInvWorldView;
  aWorldViewMat.Inverted (anInvWorldView);

  Graphic3d_Vec4d aTmpPnt;
  aTmpPnt = anInvWorldView * aLeftTopNear;
  thePoints.SetValue (FrustumVert_LeftTopNear,     aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aRightBottomFar;
  thePoints.SetValue (FrustumVert_RightBottomFar,  aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aLeftBottomNear;
  thePoints.SetValue (FrustumVert_LeftBottomNear,  aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aRightTopFar;
  thePoints.SetValue (FrustumVert_RightTopFar,     aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aRightBottomNear;
  thePoints.SetValue (FrustumVert_RightBottomNear, aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aLeftTopFar;
  thePoints.SetValue (FrustumVert_LeftTopFar,      aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aRightTopNear;
  thePoints.SetValue (FrustumVert_RightTopNear,    aTmpPnt.xyz() / aTmpPnt.w());
  aTmpPnt = anInvWorldView * aLeftBottomFar;
  thePoints.SetValue (FrustumVert_LeftBottomFar,   aTmpPnt.xyz() / aTmpPnt.w());
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_Camera::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myUp)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDirection)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myEye)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDistance)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAxialScale)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myProjType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFOVy)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZNear)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZFar)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myScale)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZFocus)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZFocusType)
  
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIOD)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIODType)
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myTile)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myMatricesD)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myMatricesF)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myWorldViewProjState)
}
