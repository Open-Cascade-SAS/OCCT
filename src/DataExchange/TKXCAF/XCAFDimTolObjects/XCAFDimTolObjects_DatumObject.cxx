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
  myIsDTarget  = Standard_False;
  myIsValidDT  = Standard_False;
  myHasPlane   = Standard_False;
  myHasPnt     = Standard_False;
  myHasPntText = Standard_False;
}

//=================================================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject(
  const Handle(XCAFDimTolObjects_DatumObject)& theObj)
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

Handle(TCollection_HAsciiString) XCAFDimTolObjects_DatumObject::GetSemanticName() const
{
  return mySemanticName;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetSemanticName(const Handle(TCollection_HAsciiString)& theName)
{
  mySemanticName = theName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) XCAFDimTolObjects_DatumObject::GetName() const
{
  if (myName.IsNull())
    return new TCollection_HAsciiString();
  return myName;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetName(const Handle(TCollection_HAsciiString)& theName)
{
  myName = theName;
}

//=================================================================================================

XCAFDimTolObjects_DatumModifiersSequence XCAFDimTolObjects_DatumObject::GetModifiers() const
{
  return myModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetModifiers(
  const XCAFDimTolObjects_DatumModifiersSequence& theModifiers)
{
  myModifiers = theModifiers;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetModifierWithValue(
  const XCAFDimTolObjects_DatumModifWithValue theModifier,
  const Standard_Real                         theValue)
{
  myModifierWithValue = theModifier;
  myValueOfModifier   = theValue;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::GetModifierWithValue(
  XCAFDimTolObjects_DatumModifWithValue& theModifier,
  Standard_Real&                         theValue) const
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

Standard_Integer XCAFDimTolObjects_DatumObject::GetPosition() const
{
  return myPosition;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetPosition(const Standard_Integer thePosition)
{
  myPosition = thePosition;
}

//=================================================================================================

Standard_Boolean XCAFDimTolObjects_DatumObject::IsDatumTarget() const
{
  return myIsDTarget;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::IsDatumTarget(const Standard_Boolean theIsDT)
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
  myIsValidDT = Standard_True;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DatumObject::GetDatumTargetLength() const
{
  return myLength;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetLength(const Standard_Real theLength)
{
  myLength    = theLength;
  myIsValidDT = Standard_True;
}

//=================================================================================================

Standard_Real XCAFDimTolObjects_DatumObject::GetDatumTargetWidth() const
{
  return myWidth;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetWidth(const Standard_Real theWidth)
{
  myWidth     = theWidth;
  myIsValidDT = Standard_True;
}

//=================================================================================================

Standard_Integer XCAFDimTolObjects_DatumObject::GetDatumTargetNumber() const
{
  return myDatumTargetNumber;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetNumber(const Standard_Integer theNumber)
{
  myDatumTargetNumber = theNumber;
}

//=================================================================================================

void XCAFDimTolObjects_DatumObject::DumpJson(Standard_OStream& theOStream,
                                             Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  if (!myName.IsNull())
  {
    Standard_CString aDatumName = myName->ToCString();
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
    Standard_CString aSemanticName = mySemanticName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aSemanticName)
  }
  if (!myPresentationName.IsNull())
  {
    Standard_CString aPresentationName = myPresentationName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aPresentationName)
  }

  for (XCAFDimTolObjects_DatumModifiersSequence::Iterator aModifIt(myModifiers); aModifIt.More();
       aModifIt.Next())
  {
    XCAFDimTolObjects_DatumSingleModif aModifier = aModifIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aModifier)
  }
}
