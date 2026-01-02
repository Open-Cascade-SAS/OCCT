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

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_DimensionObject, Standard_Transient)

//=================================================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject()
{
  myHasPlane       = false;
  myHasPntText     = false;
  myHasConnection1 = false;
  myHasConnection2 = false;
}

//=================================================================================================

XCAFDimTolObjects_DimensionObject::XCAFDimTolObjects_DimensionObject(
  const occ::handle<XCAFDimTolObjects_DimensionObject>& theObj)
{
  myType               = theObj->myType;
  myVal                = theObj->myVal;
  myQualifier          = theObj->myQualifier;
  myAngularQualifier   = theObj->myAngularQualifier;
  myIsHole             = theObj->myIsHole;
  myFormVariance       = theObj->myFormVariance;
  myGrade              = theObj->myGrade;
  myL                  = theObj->myL;
  myR                  = theObj->myR;
  myModifiers          = theObj->myModifiers;
  myPath               = theObj->myPath;
  myDir                = theObj->myDir;
  myHasConnection1     = theObj->myHasConnection1;
  myHasConnection2     = theObj->myHasConnection2;
  myConnection1        = theObj->myConnection1;
  myConnection2        = theObj->myConnection2;
  myConnectionName1    = theObj->myConnectionName1;
  myConnectionName2    = theObj->myConnectionName2;
  myConnectionIsPoint1 = theObj->myConnectionIsPoint1;
  myConnectionIsPoint2 = theObj->myConnectionIsPoint2;
  myPntText            = theObj->myPntText;
  myHasPlane           = theObj->myHasPlane;
  myPlane              = theObj->myPlane;
  myHasPntText         = theObj->myHasPntText;
  mySemanticName       = theObj->mySemanticName;
  myPresentation       = theObj->myPresentation;
  myPresentationName   = theObj->myPresentationName;
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

occ::handle<TCollection_HAsciiString> XCAFDimTolObjects_DimensionObject::GetSemanticName() const
{
  return mySemanticName;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetSemanticName(
  const occ::handle<TCollection_HAsciiString>& theName)
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

bool XCAFDimTolObjects_DimensionObject::HasQualifier() const
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

bool XCAFDimTolObjects_DimensionObject::HasAngularQualifier() const
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

double XCAFDimTolObjects_DimensionObject::GetValue() const
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

occ::handle<NCollection_HArray1<double>> XCAFDimTolObjects_DimensionObject::GetValues() const
{
  return myVal;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetValue(const double theValue)
{
  myVal = new NCollection_HArray1<double>(1, 1);
  myVal->SetValue(1, theValue);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetValues(
  const occ::handle<NCollection_HArray1<double>>& theValue)
{
  myVal = theValue;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::IsDimWithRange() const
{
  return !myVal.IsNull() && myVal->Length() == 2;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetUpperBound(const double theUpperBound)
{
  if (!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(2, theUpperBound);
  else
  {
    myVal = new NCollection_HArray1<double>(1, 2);
    myVal->SetValue(1, theUpperBound);
    myVal->SetValue(2, theUpperBound);
  }
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetLowerBound(const double theLowerBound)
{
  if (!myVal.IsNull() && myVal->Length() > 1)
    myVal->SetValue(1, theLowerBound);
  else
  {
    myVal = new NCollection_HArray1<double>(1, 2);
    myVal->SetValue(2, theLowerBound);
    myVal->SetValue(1, theLowerBound);
  }
}

//=================================================================================================

double XCAFDimTolObjects_DimensionObject::GetUpperBound() const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(2);
  }
  return 0;
}

//=================================================================================================

double XCAFDimTolObjects_DimensionObject::GetLowerBound() const
{
  if (!myVal.IsNull() && myVal->Length() == 2)
  {
    return myVal->Value(1);
  }
  return 0;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::IsDimWithPlusMinusTolerance() const
{
  return (!myVal.IsNull() && myVal->Length() == 3);
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::SetUpperTolValue(const double theUperTolValue)
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(3, theUperTolValue);
    return true;
  }
  else if (!myVal.IsNull() && myVal->Length() == 1)
  {
    double v = myVal->Value(1);
    myVal    = new NCollection_HArray1<double>(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theUperTolValue);
    myVal->SetValue(3, theUperTolValue);
    return true;
  }
  return false;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::SetLowerTolValue(const double theLowerTolValue)
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    myVal->SetValue(2, theLowerTolValue);
    return true;
  }
  else if (!myVal.IsNull() && myVal->Length() == 1)
  {
    double v = myVal->Value(1);
    myVal    = new NCollection_HArray1<double>(1, 3);
    myVal->SetValue(1, v);
    myVal->SetValue(2, theLowerTolValue);
    myVal->SetValue(3, theLowerTolValue);
    return true;
  }
  return false;
}

//=================================================================================================

double XCAFDimTolObjects_DimensionObject::GetUpperTolValue() const
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(3);
  }
  return 0;
}

//=================================================================================================

double XCAFDimTolObjects_DimensionObject::GetLowerTolValue() const
{
  if (!myVal.IsNull() && myVal->Length() == 3)
  {
    return myVal->Value(2);
  }
  return 0;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::IsDimWithClassOfTolerance() const
{
  return (myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None);
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetClassOfTolerance(
  const bool                                    theHole,
  const XCAFDimTolObjects_DimensionFormVariance theFormVariance,
  const XCAFDimTolObjects_DimensionGrade        theGrade)
{
  myIsHole       = theHole;
  myFormVariance = theFormVariance;
  myGrade        = theGrade;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::GetClassOfTolerance(
  bool&                                    theHole,
  XCAFDimTolObjects_DimensionFormVariance& theFormVariance,
  XCAFDimTolObjects_DimensionGrade&        theGrade) const
{
  theFormVariance = myFormVariance;
  if (myFormVariance != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    theHole  = myIsHole;
    theGrade = myGrade;
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetNbOfDecimalPlaces(const int theL, const int theR)
{
  myL = theL;
  myR = theR;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::GetNbOfDecimalPlaces(int& theL, int& theR) const
{
  theL = myL;
  theR = myR;
}

//=================================================================================================

NCollection_Sequence<XCAFDimTolObjects_DimensionModif> XCAFDimTolObjects_DimensionObject::
  GetModifiers() const
{
  return myModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::SetModifiers(
  const NCollection_Sequence<XCAFDimTolObjects_DimensionModif>& theModifiers)
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

bool XCAFDimTolObjects_DimensionObject::GetDirection(gp_Dir& theDir) const
{
  theDir = myDir;
  return true;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::SetDirection(const gp_Dir& theDir)
{
  myDir = theDir;
  return true;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::RemoveDescription(const int theNumber)
{
  if (theNumber < myDescriptions.Lower() || theNumber > myDescriptions.Upper())
    return;
  NCollection_Vector<occ::handle<TCollection_HAsciiString>> aDescriptions;
  NCollection_Vector<occ::handle<TCollection_HAsciiString>> aDescriptionNames;
  for (int i = aDescriptions.Lower(); i < theNumber; i++)
  {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  for (int i = theNumber + 1; i <= aDescriptions.Upper(); i++)
  {
    aDescriptions.Append(myDescriptions.Value(i));
    aDescriptionNames.Append(myDescriptionNames.Value(i));
  }
  myDescriptions     = aDescriptions;
  myDescriptionNames = aDescriptionNames;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::IsDimensionalLocation(
  const XCAFDimTolObjects_DimensionType theType)
{
  return theType == XCAFDimTolObjects_DimensionType_Location_None
      || theType == XCAFDimTolObjects_DimensionType_Location_CurvedDistance
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter
      || theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner
      || theType == XCAFDimTolObjects_DimensionType_Location_Oriented;
}

//=================================================================================================

bool XCAFDimTolObjects_DimensionObject::IsDimensionalSize(
  const XCAFDimTolObjects_DimensionType theType)
{
  return theType == XCAFDimTolObjects_DimensionType_Size_CurveLength
      || theType == XCAFDimTolObjects_DimensionType_Size_Diameter
      || theType == XCAFDimTolObjects_DimensionType_Size_SphericalDiameter
      || theType == XCAFDimTolObjects_DimensionType_Size_Radius
      || theType == XCAFDimTolObjects_DimensionType_Size_SphericalRadius
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius
      || theType == XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius
      || theType == XCAFDimTolObjects_DimensionType_Size_Thickness;
}

//=================================================================================================

void XCAFDimTolObjects_DimensionObject::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myType)

  if (!myVal.IsNull())
  {
    for (int anId = myVal->Lower(); anId <= myVal->Upper(); anId++)
    {
      double aValue = myVal->Value(anId);
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
  if (myHasConnection1)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myConnection1)
  }

  if (myHasConnection2)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myConnection2)
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
    const char* aSemanticName = mySemanticName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aSemanticName)
  }
  if (!myPresentationName.IsNull())
  {
    const char* aPresentationName = myPresentationName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aPresentationName)
  }

  for (NCollection_Vector<occ::handle<TCollection_HAsciiString>>::Iterator aDescIt(myDescriptions);
       aDescIt.More();
       aDescIt.Next())
  {
    if (aDescIt.Value().IsNull())
      continue;
    const char* aDescription = aDescIt.Value()->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDescription)
  }

  for (NCollection_Vector<occ::handle<TCollection_HAsciiString>>::Iterator aDescNameIt(
         myDescriptionNames);
       aDescNameIt.More();
       aDescNameIt.Next())
  {
    if (aDescNameIt.Value().IsNull())
      continue;
    const char* aDescriptionName = aDescNameIt.Value()->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDescriptionName)
  }

  for (NCollection_Sequence<XCAFDimTolObjects_DimensionModif>::Iterator aModifIt(myModifiers);
       aModifIt.More();
       aModifIt.Next())
  {
    XCAFDimTolObjects_DimensionModif aModifier = aModifIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aModifier)
  }
}
