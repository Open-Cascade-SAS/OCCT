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

#include <XCAFKinObjects_LowOrderPairObjectWithCoupling.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_LowOrderPairObjectWithCoupling, XCAFKinObjects_PairObject)

//=================================================================================================

XCAFKinObjects_LowOrderPairObjectWithCoupling::XCAFKinObjects_LowOrderPairObjectWithCoupling()
{
  myIsRanged   = Standard_False;
  myLowLimit   = -Precision::Infinite();
  myUpperLimit = Precision::Infinite();
  myParams     = NULL;
}

//=================================================================================================

XCAFKinObjects_LowOrderPairObjectWithCoupling::XCAFKinObjects_LowOrderPairObjectWithCoupling(
  const Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling)& theObj)
{
  SetName(theObj->Name());
  SetType(theObj->Type());
  SetFirstTransformation(theObj->FirstTransformation());
  SetSecondTransformation(theObj->SecondTransformation());
  myIsRanged   = theObj->HasLimits();
  myLowLimit   = theObj->LowLimit();
  myUpperLimit = theObj->UpperLimit();
  myParams     = theObj->GetAllParams();
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetType(const XCAFKinObjects_PairType theType)
{
  XCAFKinObjects_PairObject::SetType(theType);
  if (theType == XCAFKinObjects_PairType_Gear)
    myParams = new TColStd_HArray1OfReal(1, 5);
  else
    myParams = new TColStd_HArray1OfReal(1, 1);
}

//=================================================================================================

Handle(TColStd_HArray1OfReal) XCAFKinObjects_LowOrderPairObjectWithCoupling::GetAllLimits() const
{
  Handle(TColStd_HArray1OfReal) aLimitArray;
  if (!HasLimits())
  {
    return aLimitArray;
  }
  aLimitArray                 = new TColStd_HArray1OfReal(1, 2);
  aLimitArray->ChangeValue(1) = myLowLimit;
  aLimitArray->ChangeValue(2) = myUpperLimit;
  return aLimitArray;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetPitch(const Standard_Real thePitch)
{
  if (Type() == XCAFKinObjects_PairType_Screw)
  {
    myParams->ChangeFirst() = thePitch;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::Pitch() const
{
  if (Type() == XCAFKinObjects_PairType_Screw)
  {
    return myParams->First();
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetPinionRadius(const Standard_Real theRadius)
{
  if (Type() == XCAFKinObjects_PairType_RackAndPinion
      || Type() == XCAFKinObjects_PairType_LinearFlexibleAndPinion)
  {
    myParams->ChangeFirst() = theRadius;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::PinionRadius() const
{
  if (Type() == XCAFKinObjects_PairType_RackAndPinion
      || Type() == XCAFKinObjects_PairType_LinearFlexibleAndPinion)
  {
    return myParams->First();
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetRadiusFirstLink(
  const Standard_Real theRadius)
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    myParams->ChangeFirst() = theRadius;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::RadiusFirstLink() const
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    return myParams->First();
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetRadiusSecondLink(
  const Standard_Real theRadius)
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    myParams->ChangeValue(2) = theRadius;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::RadiusSecondLink() const
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    return myParams->Value(2);
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetBevel(const Standard_Real theBevel)
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    myParams->ChangeValue(3) = theBevel;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::Bevel() const
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    return myParams->Value(3);
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetHelicalAngle(const Standard_Real theAngle)
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    myParams->ChangeValue(4) = theAngle;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::HelicalAngle() const
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    return myParams->Value(4);
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetGearRatio(const Standard_Real theGearRatio)
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    myParams->ChangeValue(5) = theGearRatio;
  }
}

//=================================================================================================

Standard_Real XCAFKinObjects_LowOrderPairObjectWithCoupling::GearRatio() const
{
  if (Type() == XCAFKinObjects_PairType_Gear)
  {
    return myParams->Value(5);
  }
  return 0;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetAllParams(
  const Handle(TColStd_HArray1OfReal)& theParams)
{
  if (theParams->Length() == myParams->Length())
  {
    myParams = theParams;
  }
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_LowOrderPairObjectWithCoupling::HasLimits() const
{
  return myIsRanged;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObjectWithCoupling::SetAllLimits(
  const Handle(TColStd_HArray1OfReal)& theLimits)
{
  if (theLimits->Length() == 2)
  {
    myIsRanged   = Standard_True;
    myLowLimit   = theLimits->Value(1);
    myUpperLimit = theLimits->Value(2);
  }
}
