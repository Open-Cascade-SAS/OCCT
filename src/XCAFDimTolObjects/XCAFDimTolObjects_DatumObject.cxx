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


IMPLEMENT_STANDARD_RTTIEXT(XCAFDimTolObjects_DatumObject,Standard_Transient)

//=======================================================================
//function : XCAFDimTolObjects_DatumObject
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject()
{
  myIsDTarget = Standard_False;
  myIsValidDT = Standard_False;
  myHasPlane = Standard_False;
  myHasPnt = Standard_False;
  myHasPntText = Standard_False;
}

//=======================================================================
//function : XCAFDimTolObjects_DatumObject
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject(const Handle(XCAFDimTolObjects_DatumObject)& theObj)
{
  myName = theObj->myName;
  myModifiers = theObj->myModifiers;
  myModifierWithValue = theObj->myModifierWithValue;
  myValueOfModifier = theObj->myValueOfModifier;
  myDatumTarget = theObj->myDatumTarget;
  myIsDTarget = theObj->myIsDTarget;
  myIsValidDT = theObj->myIsValidDT;
  myAxis = theObj->myAxis;
  myDTargetType = theObj->myDTargetType;
  myPlane = theObj->myPlane;
  myPnt= theObj->myPnt;
  myPntText= theObj->myPntText;
  myHasPlane = theObj->myHasPlane;
  myHasPnt = theObj->myHasPnt;
  myHasPntText = theObj->myHasPntText;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDimTolObjects_DatumObject::GetName() const
{
  if(myName.IsNull())
    return new TCollection_HAsciiString();
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetName(const Handle(TCollection_HAsciiString)& theName)
{
  myName = theName;
}

//=======================================================================
//function : GetModifiers
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DatumModifiersSequence XCAFDimTolObjects_DatumObject::GetModifiers() const
{
  return myModifiers;
}

//=======================================================================
//function : SetModifiers
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetModifiers(const XCAFDimTolObjects_DatumModifiersSequence& theModifiers)
{
  myModifiers = theModifiers;
}

//=======================================================================
//function : SetModifierWithValue
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetModifierWithValue(const XCAFDimTolObjects_DatumModifWithValue theModifier, const Standard_Real theValue)
{
  myModifierWithValue = theModifier;
  myValueOfModifier = theValue;
}

//=======================================================================
//function : GetModifierWithValue
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::GetModifierWithValue(XCAFDimTolObjects_DatumModifWithValue& theModifier, Standard_Real& theValue) const
{
  theModifier = myModifierWithValue;
  theValue = myValueOfModifier;
}
  
//=======================================================================
//function : AddModifier
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::AddModifier(const XCAFDimTolObjects_DatumSingleModif theModifier)
{
  myModifiers.Append(theModifier);
}

//=======================================================================
//function : GetDatumTarget
//purpose  : 
//=======================================================================

TopoDS_Shape XCAFDimTolObjects_DatumObject::GetDatumTarget()  const
{
  return myDatumTarget;
}

//=======================================================================
//function : SetDatumTarget
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTarget (const TopoDS_Shape& theShape) 
{
  myDatumTarget = theShape;
}
  
//=======================================================================
//function : GetPosition
//purpose  : 
//=======================================================================

Standard_Integer XCAFDimTolObjects_DatumObject::GetPosition() const
{
  return myPosition;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetPosition(const Standard_Integer thePosition)
{
  myPosition = thePosition;
}

//=======================================================================
//function : IsDatumTarget
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_DatumObject::IsDatumTarget() const
{
  return myIsDTarget;
}

//=======================================================================
//function : IsDatumTarget
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::IsDatumTarget(const Standard_Boolean theIsDT)
{
  myIsDTarget = theIsDT;
}

//=======================================================================
//function : GetDatumTargetType
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DatumTargetType XCAFDimTolObjects_DatumObject::GetDatumTargetType() const
{
  return myDTargetType;
}

//=======================================================================
//function : SetDatumTargetType
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetType(const XCAFDimTolObjects_DatumTargetType theType)
{
  myDTargetType = theType;
}

//=======================================================================
//function : GetDatumTargetAxis
//purpose  : 
//=======================================================================

gp_Ax2 XCAFDimTolObjects_DatumObject::GetDatumTargetAxis() const
{
  return myAxis;
}

//=======================================================================
//function : SetDatumTargetAxis
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetAxis(const gp_Ax2& theAxis)
{
  myAxis = theAxis;
  myIsValidDT = Standard_True;
}

//=======================================================================
//function : GetDatumTargetLength
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_DatumObject::GetDatumTargetLength() const
{
  return myLength;
}

//=======================================================================
//function : SetDatumTargetLength
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetLength(const Standard_Real theLength)
{
  myLength = theLength;
  myIsValidDT = Standard_True;
}


//=======================================================================
//function : GetDatumTargetWidth
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_DatumObject::GetDatumTargetWidth() const
{
  return myWidth;
}

//=======================================================================
//function : SetDatumTargetWidth
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetWidth(const Standard_Real theWidth)
{
  myWidth = theWidth;
  myIsValidDT = Standard_True;
}

//=======================================================================
//function : GetDatumTargetNumber
//purpose  : 
//=======================================================================

Standard_Integer XCAFDimTolObjects_DatumObject::GetDatumTargetNumber() const
{
  return myDatumTargetNumber;
}

//=======================================================================
//function : SetDatumTargetNumber
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_DatumObject::SetDatumTargetNumber(const Standard_Integer theNumber)
{
  myDatumTargetNumber = theNumber;
}
