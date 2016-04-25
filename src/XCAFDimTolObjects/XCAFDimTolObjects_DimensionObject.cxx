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

#include <XCAFDimTolObjects_DimensionObject.hxx>

#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_DimensionObject,Standard_Transient)

//=======================================================================
//function : XCAFDimTolObjects_DimensionObject
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject()
{
  myHasPlane = Standard_False;
  myHasPntText = Standard_False;
  myHasPoint1 = Standard_False;
  myHasPoint2 = Standard_False;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject(const Handle(XCAFDimTolObjects_DimensionObject)& theObj)
{
  myType = theObj->myType;
  myVal = theObj->myVal;
  myQualifier = theObj->myQualifier;
  myIsHole = theObj->myIsHole;
  myFormVariance = theObj->myFormVariance;
  myGrade = theObj->myGrade;
  myL = theObj->myL;
  myR = theObj->myR;
  myModifiers = theObj->myModifiers;
  myPath = theObj->myPath;
  myDir = theObj->myDir;
  myHasPoint1 = theObj->myHasPoint1;
  myPnt1 = theObj->myPnt1;
  myHasPoint2 = theObj->myHasPoint2;
  myPnt2 = theObj->myPnt2;
  myPntText= theObj->myPntText;
  myHasPlane = theObj->myHasPlane;
  myPlane = theObj->myPlane;
  myHasPntText = theObj->myHasPntText;
}

//=======================================================================
//function : SetQualifier
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetQualifier (const XCAFDimTolObjects_DimensionQualifier theQualifier)
{
  myQualifier = theQualifier;
}

//=======================================================================
//function : GetQualifier
//purpose  : 
//=======================================================================
XCAFDimTolObjects_DimensionQualifier XCAFDimTolObjects_DimensionObject::GetQualifier()  const
{
  return myQualifier;
}

//=======================================================================
//function : HasQualifier
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::HasQualifier()  const
{
  return (myQualifier != XCAFDimTolObjects_DimensionQualifier_None);
}

//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetType (const XCAFDimTolObjects_DimensionType theType)
{
  myType = theType;
}

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================
XCAFDimTolObjects_DimensionType XCAFDimTolObjects_DimensionObject::GetType()  const
{
  return myType;
}
  
//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Real XCAFDimTolObjects_DimensionObject::GetValue ()  const
{
  if (myVal.IsNull())
    return 0;

  // Simple value or value with Plus_Minus_Tolerance
  if (myVal->Length() == 1 || myVal->Length() == 3) {
    return myVal->Value(1);
  }
  // Range
  if (myVal->Length() == 2) {
    return (myVal->Value(1) + myVal->Value(2)) / 2;
  }
  return 0;
}

//=======================================================================
//function : GetValues
//purpose  : 
//=======================================================================
Handle(TColStd_HArray1OfReal) XCAFDimTolObjects_DimensionObject::GetValues ()  const
{
  return myVal;
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetValue (const Standard_Real theValue)
{
  myVal = new TColStd_HArray1OfReal(1, 1);
  myVal->SetValue(1,theValue);
}

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetValues (const Handle(TColStd_HArray1OfReal)& theValue)
{
  myVal = theValue;
}
  
//=======================================================================
//function : IsDimWithRange
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithRange()  const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
    return Standard_True;
  return Standard_False;
}
  
//=======================================================================
//function : SetUpperBound
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetUpperBound (const Standard_Real theUpperBound)
{
  if(!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(2, theUpperBound);
  else
  {
    myVal = new TColStd_HArray1OfReal(1, 2);
    myVal->SetValue(1, theUpperBound);
    myVal->SetValue(2, theUpperBound);
  }
}
  
//=======================================================================
//function : SetLowerBound
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetLowerBound (const Standard_Real theLowerBound)
{
  if(!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(1, theLowerBound);
  else
  {
    myVal = new TColStd_HArray1OfReal(1, 2);
    myVal->SetValue(2, theLowerBound);
    myVal->SetValue(1, theLowerBound);
  }
}
  
//=======================================================================
//function : GetUpperBound
//purpose  : 
//=======================================================================
Standard_Real XCAFDimTolObjects_DimensionObject::GetUpperBound ()  const
{
  if(!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(2);
  }
  return 0;
}
  
//=======================================================================
//function : GetLowerBound
//purpose  : 
//=======================================================================
Standard_Real XCAFDimTolObjects_DimensionObject::GetLowerBound ()  const
{
  if(!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(1);
  }
  return 0;
}
  
//=======================================================================
//function : IsDimWithPlusMinusTolerance
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithPlusMinusTolerance()  const
{
  return (!myVal.IsNull() && myVal->Length() == 3);
}
  
//=======================================================================
//function : SetUpperTolValue
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::SetUpperTolValue (const Standard_Real theUperTolValue)
{
  if(!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(3, theUperTolValue);
    return Standard_True;
  }
  else if(!myVal.IsNull() && myVal->Length() == 1)
  {
    Standard_Real v = myVal->Value(1);
    myVal = new TColStd_HArray1OfReal(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theUperTolValue);
    myVal->SetValue(3, theUperTolValue);
    return Standard_True;
  }
  return Standard_False;
}
  
//=======================================================================
//function : SetLowerTolValue
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::SetLowerTolValue (const Standard_Real theLowerTolValue)
{
  if(!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(2, theLowerTolValue);
    return Standard_True;
  }
  else if(!myVal.IsNull() && myVal->Length() == 1)
  {
    Standard_Real v = myVal->Value(1);
    myVal = new TColStd_HArray1OfReal(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theLowerTolValue);
    myVal->SetValue(3, theLowerTolValue);
    return Standard_True;
  }
  return Standard_False;
}
  
//=======================================================================
//function : GetUpperTolValue
//purpose  : 
//=======================================================================
Standard_Real XCAFDimTolObjects_DimensionObject::GetUpperTolValue ()  const
{
  if(!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(3);
  }
  return 0;
}
  
//=======================================================================
//function : GetLowerTolValue
//purpose  : 
//=======================================================================
Standard_Real XCAFDimTolObjects_DimensionObject::GetLowerTolValue ()  const
{
  if(!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(2);
  }
  return 0;
}
  
//=======================================================================
//function : IsDimWithClassOfTolerance
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithClassOfTolerance()  const
{
  return (myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None);
}
  
//=======================================================================
//function : SetClassOfTolerance
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetClassOfTolerance (const Standard_Boolean theHole,
  const XCAFDimTolObjects_DimensionFormVariance theFormVariance,
  const XCAFDimTolObjects_DimensionGrade theGrade)
{
  myIsHole = theHole;
  myFormVariance = theFormVariance;
  myGrade = theGrade;
}
  
//=======================================================================
//function : GetClassOfTolerance
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::GetClassOfTolerance (Standard_Boolean& theHole,
  XCAFDimTolObjects_DimensionFormVariance& theFormVariance,
  XCAFDimTolObjects_DimensionGrade& theGrade)  const
{
  theFormVariance = myFormVariance;
  if(myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    theHole = myIsHole;
    theGrade = myGrade;
    return Standard_True;
  }
  return Standard_False;
}
  
//=======================================================================
//function : SetNbOfDecimalPlaces
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetNbOfDecimalPlaces (const Standard_Integer theL, const Standard_Integer theR)
{
  myL = theL;
  myR = theR;
}
  
//=======================================================================
//function : GetNbOfDecimalPlaces
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::GetNbOfDecimalPlaces (Standard_Integer& theL, Standard_Integer& theR)  const
{
  theL = myL;
  theR = myR;
}
  
//=======================================================================
//function : GetModifiers
//purpose  : 
//=======================================================================
XCAFDimTolObjects_DimensionModifiersSequence XCAFDimTolObjects_DimensionObject::GetModifiers ()  const
{
  return myModifiers;
}
  
//=======================================================================
//function : SetModifiers
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetModifiers (const XCAFDimTolObjects_DimensionModifiersSequence& theModifiers)
{
  myModifiers = theModifiers;
}
  
//=======================================================================
//function : AddModifier
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::AddModifier (const XCAFDimTolObjects_DimensionModif theModifier)
{
  myModifiers.Append(theModifier);
}
  
//=======================================================================
//function : GetPath
//purpose  : 
//=======================================================================
TopoDS_Edge XCAFDimTolObjects_DimensionObject::GetPath ()  const
{
    return myPath;
}
  
//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::SetPath (const TopoDS_Edge& thePath)
{
  if(!thePath.IsNull())
  {
    myPath = thePath;
  }
}
  
//=======================================================================
//function : GetDirection
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::GetDirection (gp_Dir& theDir)  const
{
  theDir = myDir;
  return Standard_True;
}
  
//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDimTolObjects_DimensionObject::SetDirection (const gp_Dir& theDir)
{
  myDir = theDir;
  return Standard_True;
}

//=======================================================================
//function : RemoveDescription
//purpose  : 
//=======================================================================
void XCAFDimTolObjects_DimensionObject::RemoveDescription(const Standard_Integer theNumber)
{
  if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
    return;
  NCollection_Vector<Handle(TCollection_HAsciiString)> aDescriptions;
  NCollection_Vector<Handle(TCollection_HAsciiString)> aDescriptionNames;
  for (Standard_Integer i = aDescriptions.Lower(); i < theNumber; i++) {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  for (Standard_Integer i = theNumber + 1; i <= aDescriptions.Upper(); i++) {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  myDescriptions = aDescriptions;
  myDescriptionNames = aDescriptionNames;
}
