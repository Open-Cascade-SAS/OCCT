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

#include <XCAFDimTolObjects_DatumObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_DatumObject, Standard_Transient)

//=================================================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject()
{
  myIsDTarget  = false;
  myIsValidDT  = false;
  myHasPlane   = false;
  myHasPnt     = false;
  myHasPntText = false;
}

//=================================================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject(
  const occ::handle<XCAFDimTolObjects_DatumObject>& theObj)
{
  myName              = theObj->myName;
  myModifiers         = theObj->myModifiers;
  myModifierWithValue = theObj->myModifierWithValue;
  myValueOfModifier   = theObj->myValueOfModifier;
  myDatumTarget       = theObj->myDatumTarget;
  myPosition          = theObj->myPosition;
  myIsDTarget         = theObj->myIsDTarget;
  myIsValidDT         = theObj->myIsValidDT;
  myDTargetType       = theObj->myDTargetType;
  myLength            = theObj->myLength;
  myWidth             = theObj->myWidth;
  myDatumTargetNumber = theObj->myDatumTargetNumber;
  myAxis              = theObj->myAxis;
  myPlane             = theObj->myPlane;
  myPnt               = theObj->myPnt;
  myPntText           = theObj->myPntText;
  myHasPlane          = theObj->myHasPlane;
  myHasPnt            = theObj->myHasPnt;
  myHasPntText        = theObj->myHasPntText;
  myPresentation      = theObj->myPresentation;
  mySemanticName      = theObj->mySemanticName;
  myPresentationName  = theObj->myPresentationName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> XCAFDimTolObjects_DatumObject::GetSemanticName() const
{
  return mySemanticName;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetSemanticName(const occ::handle<TCollection_HAsciiString>& theName)
{
  mySemanticName = theName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> XCAFDimTolObjects_DatumObject::GetName() const
{
  if (myName.IsNull())
    return new TCollection_HAsciiString();
  return myName;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetName(const occ::handle<TCollection_HAsciiString>& theName)
{
  myName = theName;
}

//=================================================================================================

NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif> XCAFDimTolObjects_DatumObject::GetModifiers() const
{
  return myModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetModifiers(
  const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>& theModifiers)
{
  myModifiers = theModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetModifierWithValue(
  const XCAFDimTolObjects_DatumModifWithValue theModifier,
  const double                         theValue)
{
  myModifierWithValue = theModifier;
  myValueOfModifier   = theValue;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::GetModifierWithValue(
  XCAFDimTolObjects_DatumModifWithValue& theModifier,
  double&                         theValue) const
{
  theModifier = myModifierWithValue;
  theValue    = myValueOfModifier;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::AddModifier(
  const XCAFDimTolObjects_DatumSingleModif theModifier)
{
  myModifiers.Append(theModifier);
}

//=================================================================================================

TopoDS_Shape XCAFDimTolObjects_DatumObject::GetDatumTarget() const
{
  return myDatumTarget;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTarget(const TopoDS_Shape& theShape)
{
  myDatumTarget = theShape;
}

//=================================================================================================

int XCAFDimTolObjects_DatumObject::GetPosition() const
{
  return myPosition;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetPosition(const int thePosition)
{
  myPosition = thePosition;
}

//=================================================================================================

bool XCAFDimTolObjects_DatumObject::IsDatumTarget() const
{
  return myIsDTarget;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::IsDatumTarget(const bool theIsDT)
{
  myIsDTarget = theIsDT;
}

//=================================================================================================

XCAFDimTolObjects_DatumTargetType XCAFDimTolObjects_DatumObject::GetDatumTargetType() const
{
  return myDTargetType;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetType(
  const XCAFDimTolObjects_DatumTargetType theType)
{
  myDTargetType = theType;
}

//=================================================================================================

gp_Ax2 XCAFDimTolObjects_DatumObject::GetDatumTargetAxis() const
{
  return myAxis;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetAxis(const gp_Ax2& theAxis)
{
  myAxis      = theAxis;
  myIsValidDT = true;
}

//=================================================================================================

double XCAFDimTolObjects_DatumObject::GetDatumTargetLength() const
{
  return myLength;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetLength(const double theLength)
{
  myLength    = theLength;
  myIsValidDT = true;
}

//=================================================================================================

double XCAFDimTolObjects_DatumObject::GetDatumTargetWidth() const
{
  return myWidth;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetWidth(const double theWidth)
{
  myWidth     = theWidth;
  myIsValidDT = true;
}

//=================================================================================================

int XCAFDimTolObjects_DatumObject::GetDatumTargetNumber() const
{
  return myDatumTargetNumber;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetNumber(const int theNumber)
{
  myDatumTargetNumber = theNumber;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::DumpJson(Standard_OStream& theOStream,
                                             int  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  if (!myName.IsNull())
  {
    const char* aDatumName = myName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aDatumName)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myModifierWithValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValueOfModifier)

  if (!myDatumTarget.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myDatumTarget)
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDTarget)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsValidDT)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDTargetType)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myAxis)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLength)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWidth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDatumTargetNumber)

  if (myHasPlane)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPlane)
  }

  if (myHasPnt)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPnt)
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

  for (NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>::Iterator aModifIt(myModifiers); aModifIt.More();
       aModifIt.Next())
  {
    XCAFDimTolObjects_DatumSingleModif aModifier = aModifIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aModifier)
  }
}
