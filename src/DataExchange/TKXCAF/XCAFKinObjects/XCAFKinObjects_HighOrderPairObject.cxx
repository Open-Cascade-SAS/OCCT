// Created on: 2020-03-16
// Created by: Irina KRYLOVA
// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <XCAFKinObjects_HighOrderPairObject.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_HighOrderPairObject, XCAFKinObjects_PairObject)

//=================================================================================================

XCAFKinObjects_HighOrderPairObject::XCAFKinObjects_HighOrderPairObject()
{
  myLimits   = nullptr;
  myIsRanged = Standard_False;
}

//=================================================================================================

XCAFKinObjects_HighOrderPairObject::XCAFKinObjects_HighOrderPairObject(
  const Handle(XCAFKinObjects_HighOrderPairObject)& theObj)
{
  SetName(theObj->Name());
  SetType(theObj->Type());
  SetFirstTransformation(theObj->FirstTransformation());
  SetSecondTransformation(theObj->SecondTransformation());
  myOrientation = theObj->myOrientation;
  myLimits      = theObj->myLimits;
  myGeom        = theObj->myGeom;
  myIsRanged    = theObj->HasLimits();
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetType(const XCAFKinObjects_PairType theType)
{
  XCAFKinObjects_PairObject::SetType(theType);
  switch (theType)
  {
    case XCAFKinObjects_PairType_PointOnSurface: {
      myLimits = new TColStd_HArray1OfReal(1, 6);
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 1);
      break;
    }
    case XCAFKinObjects_PairType_SlidingSurface: {
      myLimits = new TColStd_HArray1OfReal(1, 2);
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
      break;
    }
    case XCAFKinObjects_PairType_RollingSurface: {
      myLimits = new TColStd_HArray1OfReal(1, 2);
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
      break;
    }
    case XCAFKinObjects_PairType_PointOnPlanarCurve: {
      myLimits = new TColStd_HArray1OfReal(1, 6);
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 1);
      break;
    }
    case XCAFKinObjects_PairType_SlidingCurve: {
      myLimits = nullptr;
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
      break;
    }
    case XCAFKinObjects_PairType_RollingCurve: {
      myLimits = nullptr;
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 2);
      break;
    }
    case XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve: {
      myLimits = nullptr;
      myGeom   = NCollection_Array1<Handle(Geom_Geometry)>(1, 1);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetAllLimits(
  const Handle(TColStd_HArray1OfReal)& theLimits)
{
  Standard_Integer aNbLimits = 0;
  switch (Type())
  {
    case XCAFKinObjects_PairType_PointOnSurface: {
      aNbLimits = 6;
      break;
    }
    case XCAFKinObjects_PairType_SlidingSurface: {
      aNbLimits = 2;
      break;
    }
    case XCAFKinObjects_PairType_RollingSurface: {
      aNbLimits = 2;
      break;
    }
    case XCAFKinObjects_PairType_PointOnPlanarCurve: {
      aNbLimits = 6;
      break;
    }
    default:
      break;
  }

  if (theLimits->Length() == aNbLimits)
  {
    myLimits   = theLimits;
    myIsRanged = Standard_True;
  }
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_HighOrderPairObject::HasLimits() const
{
  return myIsRanged;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetRotationLowLimit(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
  {
    myLimits->ChangeValue(1) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::RotationLowLimit()
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    return myLimits->Value(1);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetRotationUpperLimit(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
  {
    myLimits->ChangeValue(2) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::RotationUpperLimit()
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    return myLimits->Value(2);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetLowLimitYaw(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(1) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::LowLimitYaw()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(1);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetUpperLimitYaw(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(2) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::UpperLimitYaw()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(2);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetLowLimitRoll(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(3) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::LowLimitRoll()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(3);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetUpperLimitRoll(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(4) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::UpperLimitRoll()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(4);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetLowLimitPitch(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(5) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::LowLimitPitch()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(5);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetUpperLimitPitch(const Standard_Real theLimit)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
  {
    myLimits->ChangeValue(6) = theLimit;
    myIsRanged               = Standard_True;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_HighOrderPairObject::UpperLimitPitch()
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_PointOnSurface)
    return myLimits->Value(6);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
    myGeom.ChangeFirst() = theCurve;
}

//=================================================================================================

Handle(Geom_Curve) XCAFKinObjects_HighOrderPairObject::Curve() const
{
  if (Type() == XCAFKinObjects_PairType_PointOnPlanarCurve
      || Type() == XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.First());
  return nullptr;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetFirstCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinObjects_PairType_SlidingCurve
      || Type() == XCAFKinObjects_PairType_RollingCurve)
    myGeom.ChangeFirst() = theCurve;
}

//=================================================================================================

Handle(Geom_Curve) XCAFKinObjects_HighOrderPairObject::FirstCurve() const
{
  if (Type() == XCAFKinObjects_PairType_SlidingCurve
      || Type() == XCAFKinObjects_PairType_RollingCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.First());
  return nullptr;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetSecondCurve(const Handle(Geom_Curve)& theCurve)
{
  if (Type() == XCAFKinObjects_PairType_SlidingCurve
      || Type() == XCAFKinObjects_PairType_RollingCurve)
    myGeom.ChangeLast() = theCurve;
}

//=================================================================================================

Handle(Geom_Curve) XCAFKinObjects_HighOrderPairObject::SecondCurve() const
{
  if (Type() == XCAFKinObjects_PairType_SlidingCurve
      || Type() == XCAFKinObjects_PairType_RollingCurve)
    return Handle(Geom_Curve)::DownCast(myGeom.Last());
  return nullptr;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinObjects_PairType_PointOnSurface)
    myGeom.ChangeFirst() = theSurface;
}

//=================================================================================================

Handle(Geom_Surface) XCAFKinObjects_HighOrderPairObject::Surface() const
{
  if (Type() == XCAFKinObjects_PairType_PointOnSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.First());
  return nullptr;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetFirstSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    myGeom.ChangeFirst() = theSurface;
}

//=================================================================================================

Handle(Geom_Surface) XCAFKinObjects_HighOrderPairObject::FirstSurface() const
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.First());
  return nullptr;
}

//=================================================================================================

void XCAFKinObjects_HighOrderPairObject::SetSecondSurface(const Handle(Geom_Surface)& theSurface)
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    myGeom.ChangeLast() = theSurface;
}

//=================================================================================================

Handle(Geom_Surface) XCAFKinObjects_HighOrderPairObject::SecondSurface() const
{
  if (Type() == XCAFKinObjects_PairType_SlidingSurface
      || Type() == XCAFKinObjects_PairType_RollingSurface)
    return Handle(Geom_Surface)::DownCast(myGeom.Last());
  return nullptr;
}
