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

#include <XCAFKinObjects_LowOrderPairObject.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_LowOrderPairObject, XCAFKinObjects_PairObject)

//=================================================================================================

XCAFKinObjects_LowOrderPairObject::XCAFKinObjects_LowOrderPairObject()
{
  myMinRotationX    = -Precision::Infinite();
  myMaxRotationX    = Precision::Infinite();
  myMinRotationY    = -Precision::Infinite();
  myMaxRotationY    = Precision::Infinite();
  myMinRotationZ    = -Precision::Infinite();
  myMaxRotationZ    = Precision::Infinite();
  myMinTranslationX = -Precision::Infinite();
  myMaxTranslationX = Precision::Infinite();
  myMinTranslationY = -Precision::Infinite();
  myMaxTranslationY = Precision::Infinite();
  myMinTranslationZ = -Precision::Infinite();
  myMaxTranslationZ = Precision::Infinite();
  myIsRanged        = Standard_False;
}

//=================================================================================================

XCAFKinObjects_LowOrderPairObject::XCAFKinObjects_LowOrderPairObject(
  const Handle(XCAFKinObjects_LowOrderPairObject)& theObj)
{
  SetName(theObj->Name());
  SetType(theObj->Type());
  SetFirstTransformation(theObj->FirstTransformation());
  SetSecondTransformation(theObj->SecondTransformation());
  myMinRotationX    = theObj->myMinRotationX;
  myMaxRotationX    = theObj->myMaxRotationX;
  myMinRotationY    = theObj->myMinRotationY;
  myMaxRotationY    = theObj->myMaxRotationY;
  myMinRotationZ    = theObj->myMinRotationZ;
  myMaxRotationZ    = theObj->myMaxRotationZ;
  myMinTranslationX = theObj->myMinTranslationX;
  myMaxTranslationX = theObj->myMaxTranslationX;
  myMinTranslationY = theObj->myMinTranslationY;
  myMaxTranslationY = theObj->myMaxTranslationY;
  myMinTranslationZ = theObj->myMinTranslationZ;
  myMaxTranslationZ = theObj->myMaxTranslationZ;
  myIsRanged        = theObj->HasLimits();
}

//=================================================================================================

Handle(TColStd_HArray1OfReal) XCAFKinObjects_LowOrderPairObject::GetAllLimits() const
{
  Handle(TColStd_HArray1OfReal) aLimitArray;
  if (!HasLimits())
    return aLimitArray;
  aLimitArray                  = new TColStd_HArray1OfReal(1, 12);
  aLimitArray->ChangeValue(1)  = myMinRotationX;
  aLimitArray->ChangeValue(2)  = myMaxRotationX;
  aLimitArray->ChangeValue(3)  = myMinRotationY;
  aLimitArray->ChangeValue(4)  = myMaxRotationY;
  aLimitArray->ChangeValue(5)  = myMinRotationZ;
  aLimitArray->ChangeValue(6)  = myMaxRotationZ;
  aLimitArray->ChangeValue(7)  = myMinTranslationX;
  aLimitArray->ChangeValue(8)  = myMaxTranslationX;
  aLimitArray->ChangeValue(9)  = myMinTranslationY;
  aLimitArray->ChangeValue(10) = myMaxTranslationY;
  aLimitArray->ChangeValue(11) = myMinTranslationZ;
  aLimitArray->ChangeValue(12) = myMaxTranslationZ;
  return aLimitArray;
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_LowOrderPairObject::HasLimits() const
{
  return myIsRanged;
}

//=================================================================================================

void XCAFKinObjects_LowOrderPairObject::SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
{
  if (theLimits->Length() != 12)
    return;
  myMinRotationX    = theLimits->Value(1);
  myMaxRotationX    = theLimits->Value(2);
  myMinRotationY    = theLimits->Value(3);
  myMaxRotationY    = theLimits->Value(4);
  myMinRotationZ    = theLimits->Value(5);
  myMaxRotationZ    = theLimits->Value(6);
  myMinTranslationX = theLimits->Value(7);
  myMaxTranslationX = theLimits->Value(8);
  myMinTranslationY = theLimits->Value(9);
  myMaxTranslationY = theLimits->Value(10);
  myMinTranslationZ = theLimits->Value(11);
  myMaxTranslationZ = theLimits->Value(12);
  myIsRanged        = Standard_True;
}
