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

#include <TColgp_HArray1OfPnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_DimensionObject, Standard_Transient)

//=================================================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject()
{
  myHasPlane   = Standard_False;
  myHasPntText = Standard_False;
  myHasPoint1  = Standard_False;
  myHasPoint2  = Standard_False;
}

//=================================================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject(
  const Handle(XCAFDimTolObjects_DimensionObject)& theObj)
{
  myType             = theObj->myType;
  myVal              = theObj->myVal;
  myQualifier        = theObj->myQualifier;
  myAngularQualifier = theObj->myAngularQualifier;
  myIsHole           = theObj->myIsHole;
  myFormVariance     = theObj->myFormVariance;
  myGrade            = theObj->myGrade;
  myL                = theObj->myL;
  myR                = theObj->myR;
  myModifiers        = theObj->myModifiers;
  myPath             = theObj->myPath;
  myDir              = theObj->myDir;
  myHasPoint1        = theObj->myHasPoint1;
  myPnt1             = theObj->myPnt1;
  myHasPoint2        = theObj->myHasPoint2;
  myPnt2             = theObj->myPnt2;
  myPntText          = theObj->myPntText;
  myHasPlane         = theObj->myHasPlane;
  myPlane            = theObj->myPlane;
  myHasPntText       = theObj->myHasPntText;
  mySemanticName     = theObj->mySemanticName;
  myPresentation     = theObj->myPresentation;
  myPresentationName = theObj->myPresentationName;
  for (int i = 0; i < theObj->myDescriptions.Length(); i++)
  {
    myDescriptions.Append(theObj->myDescriptions(i));
  }
  for (int i = 0; i < theObj->myDescriptionNames.Length(); i++)
  {
    myDescriptionNames.Append(theObj->myDescriptionNames(i));
  }
}

//=================================================================================================

Handle(TCollection_HAsciiString) XCAFDimTolObjects_DimensionObject::GetSemanticName() const
{
  return mySemanticName;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetSemanticName(
  const Handle(TCollection_HAsciiString)& theName)
{
  mySemanticName = theName;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetQualifier(
  const XCAFDimTolObjects_DimensionQualifier theQualifier)
{
  myQualifier = theQualifier;
}

//=================================================================================================

XCAFDimTolObjects_DimensionQualifier XCAFDimTolObjects_DimensionObject::GetQualifier() const
{
  return myQualifier;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::HasQualifier() const
{
  return (myQualifier != XCAFDimTolObjects_DimensionQualifier_None);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetAngularQualifier(
  const XCAFDimTolObjects_AngularQualifier theAngularQualifier)
{
  myAngularQualifier = theAngularQualifier;
}

//=================================================================================================

XCAFDimTolObjects_AngularQualifier XCAFDimTolObjects_DimensionObject::GetAngularQualifier() const
{
  return myAngularQualifier;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::HasAngularQualifier() const
{
  return (myAngularQualifier != XCAFDimTolObjects_AngularQualifier_None);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetType(const XCAFDimTolObjects_DimensionType theType)
{
  myType = theType;
}

//=================================================================================================

XCAFDimTolObjects_DimensionType XCAFDimTolObjects_DimensionObject::GetType() const
{
  return myType;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DimensionObject::GetValue() const
{
  if (myVal.IsNull())
    return 0;

  // Simple value or value with Plus_Minus_Tolerance
  if (myVal->Length() == 1 || myVal->Length() == 3)
  {
    return myVal->Value(1);
  }
  // Range
  if (myVal->Length() == 2)
  {
    return (myVal->Value(1) + myVal->Value(2)) / 2;
  }
  return 0;
}

//=================================================================================================

Handle(TColStd_HArray1OfReal) XCAFDimTolObjects_DimensionObject::GetValues() const
{
  return myVal;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetValue(const Standard_Real theValue)
{
  myVal = new TColStd_HArray1OfReal(1, 1);
  myVal->SetValue(1, theValue);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetValues(const Handle(TColStd_HArray1OfReal)& theValue)
{
  myVal = theValue;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithRange() const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
    return Standard_True;
  return Standard_False;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetUpperBound(const Standard_Real theUpperBound)
{
  if (!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(2, theUpperBound);
  else
  {
    myVal = new TColStd_HArray1OfReal(1, 2);
    myVal->SetValue(1, theUpperBound);
    myVal->SetValue(2, theUpperBound);
  }
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetLowerBound(const Standard_Real theLowerBound)
{
  if (!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(1, theLowerBound);
  else
  {
    myVal = new TColStd_HArray1OfReal(1, 2);
    myVal->SetValue(2, theLowerBound);
    myVal->SetValue(1, theLowerBound);
  }
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DimensionObject::GetUpperBound() const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(2);
  }
  return 0;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DimensionObject::GetLowerBound() const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(1);
  }
  return 0;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithPlusMinusTolerance() const
{
  return (!myVal.IsNull() && myVal->Length() == 3);
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::SetUpperTolValue(
  const Standard_Real theUperTolValue)
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(3, theUperTolValue);
    return Standard_True;
  }
  else if (!myVal.IsNull() && myVal->Length() == 1)
  {
    Standard_Real v = myVal->Value(1);
    myVal           = new TColStd_HArray1OfReal(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theUperTolValue);
    myVal->SetValue(3, theUperTolValue);
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::SetLowerTolValue(
  const Standard_Real theLowerTolValue)
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(2, theLowerTolValue);
    return Standard_True;
  }
  else if (!myVal.IsNull() && myVal->Length() == 1)
  {
    Standard_Real v = myVal->Value(1);
    myVal           = new TColStd_HArray1OfReal(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theLowerTolValue);
    myVal->SetValue(3, theLowerTolValue);
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DimensionObject::GetUpperTolValue() const
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(3);
  }
  return 0;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DimensionObject::GetLowerTolValue() const
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(2);
  }
  return 0;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::IsDimWithClassOfTolerance() const
{
  return (myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetClassOfTolerance(
  const Standard_Boolean                        theHole,
  const XCAFDimTolObjects_DimensionFormVariance theFormVariance,
  const XCAFDimTolObjects_DimensionGrade        theGrade)
{
  myIsHole       = theHole;
  myFormVariance = theFormVariance;
  myGrade        = theGrade;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::GetClassOfTolerance(
  Standard_Boolean&                        theHole,
  XCAFDimTolObjects_DimensionFormVariance& theFormVariance,
  XCAFDimTolObjects_DimensionGrade&        theGrade) const
{
  theFormVariance = myFormVariance;
  if (myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    theHole  = myIsHole;
    theGrade = myGrade;
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetNbOfDecimalPlaces(const Standard_Integer theL,
                                                             const Standard_Integer theR)
{
  myL = theL;
  myR = theR;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::GetNbOfDecimalPlaces(Standard_Integer& theL,
                                                             Standard_Integer& theR) const
{
  theL = myL;
  theR = myR;
}

//=================================================================================================

XCAFDimTolObjects_DimensionModifiersSequence XCAFDimTolObjects_DimensionObject::GetModifiers() const
{
  return myModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetModifiers(
  const XCAFDimTolObjects_DimensionModifiersSequence& theModifiers)
{
  myModifiers = theModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::AddModifier(
  const XCAFDimTolObjects_DimensionModif theModifier)
{
  myModifiers.Append(theModifier);
}

//=================================================================================================

TopoDS_Edge XCAFDimTolObjects_DimensionObject::GetPath() const
{
  return myPath;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetPath(const TopoDS_Edge& thePath)
{
  if (!thePath.IsNull())
  {
    myPath = thePath;
  }
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::GetDirection(gp_Dir& theDir) const
{
  theDir = myDir;
  return Standard_True;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DimensionObject::SetDirection(const gp_Dir& theDir)
{
  myDir = theDir;
  return Standard_True;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::RemoveDescription(const Standard_Integer theNumber)
{
  if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
    return;
  NCollection_Vector<Handle(TCollection_HAsciiString)> aDescriptions;
  NCollection_Vector<Handle(TCollection_HAsciiString)> aDescriptionNames;
  for (Standard_Integer i = aDescriptions.Lower(); i < theNumber; i++)
  {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  for (Standard_Integer i = theNumber + 1; i <= aDescriptions.Upper(); i++)
  {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  myDescriptions     = aDescriptions;
  myDescriptionNames = aDescriptionNames;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::DumpJson(Standard_OStream& theOStream,
                                                 Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myType)

  if (!myVal.IsNull())
  {
    for (Standard_Integer anId = myVal->Lower(); anId <= myVal->Upper(); anId++)
    {
      Standard_Real aValue = myVal->Value(anId);
      OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
    }
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myQualifier)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsHole)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFormVariance)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGrade)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myL)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myR)

  if (!myPath.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPath)
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myDir)
  if (myHasPoint1)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPnt1)
  }

  if (myHasPoint2)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPnt2)
  }

  if (myHasPlane)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPlane)
  }

  if (myHasPntText)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPntText)
  }

  if (!myPresentation.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPresentation)
  }

  if (!mySemanticName.IsNull())
  {
    Standard_CString aSemanticName = mySemanticName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aSemanticName)
  }
  if (!myPresentationName.IsNull())
  {
    Standard_CString aPresentationName = myPresentationName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aPresentationName)
  }

  for (NCollection_Vector<Handle(TCollection_HAsciiString)>::Iterator aDescIt(myDescriptions);
       aDescIt.More();
       aDescIt.Next())
  {
    if (aDescIt.Value().IsNull())
      continue;
    Standard_CString aDescription = aDescIt.Value()->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDescription)
  }

  for (NCollection_Vector<Handle(TCollection_HAsciiString)>::Iterator aDescNameIt(
         myDescriptionNames);
       aDescNameIt.More();
       aDescNameIt.Next())
  {
    if (aDescNameIt.Value().IsNull())
      continue;
    Standard_CString aDescriptionName = aDescNameIt.Value()->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDescriptionName)
  }

  for (XCAFDimTolObjects_DimensionModifiersSequence::Iterator aModifIt(myModifiers);
       aModifIt.More();
       aModifIt.Next())
  {
    XCAFDimTolObjects_DimensionModif aModifier = aModifIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aModifier)
  }
}
