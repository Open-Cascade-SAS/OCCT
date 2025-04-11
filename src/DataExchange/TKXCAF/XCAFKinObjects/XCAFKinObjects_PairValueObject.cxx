// Created on: 2020-03-27
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

#include <gp_Ax2.hxx>
#include <XCAFKinObjects_PairValueObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_PairValueObject, Standard_Transient)

//=================================================================================================

XCAFKinObjects_PairValueObject::XCAFKinObjects_PairValueObject()
{
  myValues = NULL;
}

//=================================================================================================

XCAFKinObjects_PairValueObject::XCAFKinObjects_PairValueObject(
  const Handle(XCAFKinObjects_PairValueObject)& theObj)
{
  myType   = theObj->myType;
  myValues = theObj->myValues;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetType(const XCAFKinObjects_PairType theType)
{
  myType                     = theType;
  Standard_Integer aNbParams = 0;
  switch (myType)
  {
    case XCAFKinObjects_PairType_Revolute:
      aNbParams = 1;
      break;
    case XCAFKinObjects_PairType_Prismatic:
      aNbParams = 1;
      break;
    case XCAFKinObjects_PairType_Cylindrical:
      aNbParams = 2;
      break;
    case XCAFKinObjects_PairType_Universal:
    case XCAFKinObjects_PairType_Homokinetic:
      aNbParams = 2;
      break;
    case XCAFKinObjects_PairType_SphericalWithPin:
    case XCAFKinObjects_PairType_Spherical:
      aNbParams = 3;
      break;
    case XCAFKinObjects_PairType_Planar:
      aNbParams = 3;
      break;
    case XCAFKinObjects_PairType_Unconstrained:
      aNbParams = 9;
      break;
    case XCAFKinObjects_PairType_Screw:
      aNbParams = 1;
      break;
    case XCAFKinObjects_PairType_RackAndPinion:
      aNbParams = 1;
      break;
    case XCAFKinObjects_PairType_Gear:
      aNbParams = 1;
      break;
    case XCAFKinObjects_PairType_PointOnSurface:
      aNbParams = 5;
      break;
    case XCAFKinObjects_PairType_SlidingSurface:
      aNbParams = 5;
      break;
    case XCAFKinObjects_PairType_RollingSurface:
      aNbParams = 3;
      break;
    case XCAFKinObjects_PairType_PointOnPlanarCurve:
      aNbParams = 4;
      break;
    case XCAFKinObjects_PairType_SlidingCurve:
      aNbParams = 2;
      break;
    case XCAFKinObjects_PairType_RollingCurve:
      aNbParams = 1;
      break;
    default:
      break;
  }

  if (aNbParams == 0)
    myValues = NULL;
  else
    myValues = new TColStd_HArray1OfReal(1, aNbParams);
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetFirstRotation(const Standard_Real theRotation)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Revolute:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_Cylindrical:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_Universal:
    case XCAFKinObjects_PairType_Homokinetic:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_Planar:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_Screw:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_Gear:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_SlidingSurface:
      myValues->ChangeValue(1) = theRotation;
      break;
    case XCAFKinObjects_PairType_RollingSurface:
      myValues->ChangeValue(1) = theRotation;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetFirstRotation() const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Revolute:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Cylindrical:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Universal:
    case XCAFKinObjects_PairType_Homokinetic:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Planar:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Screw:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Gear:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_SlidingSurface:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_RollingSurface:
      return myValues->Value(1);
    default:
      break;
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetSecondRotation(const Standard_Real theRotation)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Universal:
    case XCAFKinObjects_PairType_Homokinetic:
      myValues->ChangeValue(2) = theRotation;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetSecondRotation() const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Universal:
    case XCAFKinObjects_PairType_Homokinetic:
      return myValues->Value(2);
    default:
      break;
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetFirstTranslation(const Standard_Real theTranslation)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Prismatic:
      myValues->ChangeValue(1) = theTranslation;
      break;
    case XCAFKinObjects_PairType_Cylindrical:
      myValues->ChangeValue(2) = theTranslation;
      break;
    case XCAFKinObjects_PairType_Planar:
      myValues->ChangeValue(2) = theTranslation;
      break;
    case XCAFKinObjects_PairType_RackAndPinion:
      myValues->ChangeValue(1) = theTranslation;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetFirstTranslation() const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_Prismatic:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_Cylindrical:
      return myValues->Value(2);
    case XCAFKinObjects_PairType_Planar:
      return myValues->Value(2);
    case XCAFKinObjects_PairType_RackAndPinion:
      return myValues->Value(1);
    default:
      break;
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetSecondTranslation(const Standard_Real theTranslation)
{
  if (myType == XCAFKinObjects_PairType_Planar)
  {
    myValues->ChangeValue(3) = theTranslation;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetSecondTranslation() const
{
  if (myType == XCAFKinObjects_PairType_Planar)
  {
    return myValues->Value(3);
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetFirstPointOnSurface(const Standard_Real theU,
                                                            const Standard_Real theV)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_PointOnSurface:
      myValues->ChangeValue(1) = theU;
      myValues->ChangeValue(2) = theV;
      break;
    case XCAFKinObjects_PairType_SlidingSurface:
      myValues->ChangeValue(2) = theU;
      myValues->ChangeValue(3) = theV;
      break;
    case XCAFKinObjects_PairType_RollingSurface:
      myValues->ChangeValue(2) = theU;
      myValues->ChangeValue(3) = theV;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_PairValueObject::GetFirstPointOnSurface(Standard_Real& theU,
                                                                        Standard_Real& theV) const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_PointOnSurface:
      theU = myValues->Value(1);
      theV = myValues->Value(2);
      return Standard_True;
    case XCAFKinObjects_PairType_SlidingSurface:
      theU = myValues->Value(2);
      theV = myValues->Value(3);
      return Standard_True;
    case XCAFKinObjects_PairType_RollingSurface:
      theU = myValues->Value(2);
      theV = myValues->Value(3);
      return Standard_True;
    default:
      break;
  }
  return Standard_False;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetSecondPointOnSurface(const Standard_Real theU,
                                                             const Standard_Real theV)
{
  if (myType != XCAFKinObjects_PairType_SlidingSurface)
    return;
  myValues->ChangeValue(4) = theU;
  myValues->ChangeValue(5) = theV;
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_PairValueObject::GetSecondPointOnSurface(Standard_Real& theU,
                                                                         Standard_Real& theV) const
{
  if (myType != XCAFKinObjects_PairType_SlidingSurface)
    return Standard_False;
  theU = myValues->Value(4);
  theV = myValues->Value(5);
  return Standard_True;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetFirstPointOnCurve(const Standard_Real theT)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_PointOnPlanarCurve:
      myValues->ChangeValue(1) = theT;
      break;
    case XCAFKinObjects_PairType_SlidingCurve:
      myValues->ChangeValue(1) = theT;
      break;
    case XCAFKinObjects_PairType_RollingCurve:
      myValues->ChangeValue(1) = theT;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetFirstPointOnCurve() const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_PointOnPlanarCurve:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_SlidingCurve:
      return myValues->Value(1);
    case XCAFKinObjects_PairType_RollingCurve:
      return myValues->Value(1);
    default:
      break;
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetSecondPointOnCurve(const Standard_Real theT)
{
  if (myType == XCAFKinObjects_PairType_SlidingCurve)
  {
    myValues->ChangeValue(2) = theT;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_PairValueObject::GetSecondPointOnCurve() const
{
  if (myType == XCAFKinObjects_PairType_SlidingCurve)
    return myValues->Value(2);
  return 0;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetYPR(const Standard_Real theYaw,
                                            const Standard_Real thePitch,
                                            const Standard_Real theRoll)
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_SphericalWithPin:
    case XCAFKinObjects_PairType_Spherical:
      myValues->ChangeValue(1) = theYaw;
      myValues->ChangeValue(2) = thePitch;
      myValues->ChangeValue(3) = theRoll;
      break;
    case XCAFKinObjects_PairType_PointOnSurface:
      myValues->ChangeValue(3) = theYaw;
      myValues->ChangeValue(4) = thePitch;
      myValues->ChangeValue(5) = theRoll;
      break;
    case XCAFKinObjects_PairType_PointOnPlanarCurve:
      myValues->ChangeValue(2) = theYaw;
      myValues->ChangeValue(3) = thePitch;
      myValues->ChangeValue(4) = theRoll;
      break;
    default:
      break;
  }
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_PairValueObject::GetYPR(Standard_Real& theYaw,
                                                        Standard_Real& thePitch,
                                                        Standard_Real& theRoll) const
{
  switch (myType)
  {
    case XCAFKinObjects_PairType_SphericalWithPin:
    case XCAFKinObjects_PairType_Spherical:
      theYaw   = myValues->Value(1);
      thePitch = myValues->Value(2);
      theRoll  = myValues->Value(3);
      return Standard_True;
    case XCAFKinObjects_PairType_PointOnSurface:
      theYaw   = myValues->Value(3);
      thePitch = myValues->Value(4);
      theRoll  = myValues->Value(5);
      return Standard_True;
    case XCAFKinObjects_PairType_PointOnPlanarCurve:
      theYaw   = myValues->Value(2);
      thePitch = myValues->Value(3);
      theRoll  = myValues->Value(4);
      return Standard_True;
    default:
      break;
  }
  return Standard_False;
}

//=================================================================================================

void XCAFKinObjects_PairValueObject::SetTransformation(const gp_Ax2& theTrsf)
{
  if (myType != XCAFKinObjects_PairType_Unconstrained)
    return;

  myValues->ChangeValue(1) = theTrsf.Location().X();
  myValues->ChangeValue(2) = theTrsf.Location().Y();
  myValues->ChangeValue(3) = theTrsf.Location().Z();
  myValues->ChangeValue(4) = theTrsf.Direction().X();
  myValues->ChangeValue(5) = theTrsf.Direction().Y();
  myValues->ChangeValue(6) = theTrsf.Direction().Z();
  myValues->ChangeValue(7) = theTrsf.XDirection().X();
  myValues->ChangeValue(8) = theTrsf.XDirection().Y();
  myValues->ChangeValue(9) = theTrsf.XDirection().Z();
}

//=================================================================================================

gp_Ax2 XCAFKinObjects_PairValueObject::GetTransformation()
{
  if (myType != XCAFKinObjects_PairType_Unconstrained)
    return gp_Ax2();

  gp_Pnt aLoc(myValues->Value(1), myValues->Value(2), myValues->Value(3));
  gp_Dir aDir(myValues->Value(4), myValues->Value(5), myValues->Value(6));
  gp_Dir aXDir(myValues->Value(7), myValues->Value(8), myValues->Value(9));
  gp_Ax2 aResult = gp_Ax2(aLoc, aDir, aXDir);
  return aResult;
}
