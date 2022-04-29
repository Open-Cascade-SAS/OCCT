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

#include <XCAFDimTolObjects_GeomToleranceObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_GeomToleranceObject,Standard_Transient)

//=======================================================================
//function : XCAFDimTolObjects_GeomTolerance
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceObject::XCAFDimTolObjects_GeomToleranceObject()
{
  myHasAxis = Standard_False;
  myHasPlane = Standard_False;
  myHasPnt = Standard_False;
  myHasPntText = Standard_False;
  myAffectedPlaneType = XCAFDimTolObjects_ToleranceZoneAffectedPlane_None;
}

//=======================================================================
//function : XCAFDimTolObjects_GeomTolerance
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceObject::XCAFDimTolObjects_GeomToleranceObject(const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObj)
{
  myType = theObj->myType;
  myTypeOfValue = theObj->myTypeOfValue;
  myValue = theObj->myValue;
  myMatReqModif = theObj->myMatReqModif;
  myZoneModif = theObj->myZoneModif;
  myValueOfZoneModif = theObj->myValueOfZoneModif;
  myModifiers = theObj->myModifiers;
  myMaxValueModif = theObj->myMaxValueModif;
  myAxis = theObj->myAxis;
  myHasAxis = theObj->myHasAxis;
  myPlane = theObj->myPlane;
  myPnt= theObj->myPnt;
  myPntText= theObj->myPntText;
  myHasPlane = theObj->myHasPlane;
  myHasPnt = theObj->myHasPnt;
  myHasPntText = theObj->myHasPntText;
  mySemanticName = theObj->mySemanticName;
  myAffectedPlaneType = theObj->myAffectedPlaneType;
  myAffectedPlane = theObj->myAffectedPlane;
  myPresentation = theObj->myPresentation;
  myPresentationName = theObj->myPresentationName;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDimTolObjects_GeomToleranceObject::GetSemanticName() const
{
  return mySemanticName;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetSemanticName(const Handle(TCollection_HAsciiString)& theName)
{
  mySemanticName = theName;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetType (const XCAFDimTolObjects_GeomToleranceType theType) 
{
  myType = theType;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceType XCAFDimTolObjects_GeomToleranceObject::GetType()  const
{
  return myType;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetTypeOfValue (const XCAFDimTolObjects_GeomToleranceTypeValue theTypeOfValue) 
{
  myTypeOfValue = theTypeOfValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceTypeValue XCAFDimTolObjects_GeomToleranceObject::GetTypeOfValue()  const
{
  return myTypeOfValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetValue (const Standard_Real theValue) 
{
  myValue = theValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetValue()  const
{
  return myValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetMaterialRequirementModifier (const XCAFDimTolObjects_GeomToleranceMatReqModif theMatReqModif) 
{
  myMatReqModif = theMatReqModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceMatReqModif XCAFDimTolObjects_GeomToleranceObject::GetMaterialRequirementModifier()  const
{
  return myMatReqModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetZoneModifier (const XCAFDimTolObjects_GeomToleranceZoneModif theZoneModif) 
{
  myZoneModif = theZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceZoneModif XCAFDimTolObjects_GeomToleranceObject::GetZoneModifier()  const
{
  return myZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetValueOfZoneModifier (const Standard_Real theValue) 
{
  myValueOfZoneModif = theValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetValueOfZoneModifier()  const
{
  return myValueOfZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetModifiers (const XCAFDimTolObjects_GeomToleranceModifiersSequence& theModifiers) 
{
  myModifiers = theModifiers;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::AddModifier (const XCAFDimTolObjects_GeomToleranceModif theModifier) 
{
  myModifiers.Append(theModifier);
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceModifiersSequence XCAFDimTolObjects_GeomToleranceObject::GetModifiers()  const
{
  return myModifiers;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetMaxValueModifier (const Standard_Real theModifier) 
{
  myMaxValueModif = theModifier;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetMaxValueModifier()  const
{
  return myMaxValueModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetAxis(const gp_Ax2& theAxis)
{
  myAxis = theAxis;
  myHasAxis = Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

gp_Ax2 XCAFDimTolObjects_GeomToleranceObject::GetAxis()  const
{
  return myAxis;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_GeomToleranceObject::HasAxis () const 
{
  return myHasAxis;
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void XCAFDimTolObjects_GeomToleranceObject::DumpJson (Standard_OStream& theOStream,
                                                      Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTypeOfValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myValue)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMatReqModif)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZoneModif)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myValueOfZoneModif)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMaxValueModif)
  if (myHasAxis)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAxis)
  }

  if (myHasPlane)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPlane)
  }

  if (myHasPnt)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPnt)
  }

  if (myHasPntText)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPntText)
  }

  if (!myPresentation.IsNull())
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPresentation)
  }

  if (!mySemanticName.IsNull())
  {
    Standard_CString aSemanticName = mySemanticName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING (theOStream, aSemanticName)
  }
  if (!myPresentationName.IsNull())
  {
    Standard_CString aPresentationName = myPresentationName->ToCString();
    OCCT_DUMP_FIELD_VALUE_STRING (theOStream, aPresentationName)
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAffectedPlane)

  for (XCAFDimTolObjects_GeomToleranceModifiersSequence::Iterator aModifIt (myModifiers); aModifIt.More(); aModifIt.Next())
  {
    XCAFDimTolObjects_GeomToleranceModif aModifier = aModifIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, aModifier)
  }
}
