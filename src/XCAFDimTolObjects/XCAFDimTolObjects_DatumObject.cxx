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


//=======================================================================
//function : XCAFDimTolObjects_DatumObject
//purpose  : 
//=======================================================================

XCAFDimTolObjects_DatumObject::XCAFDimTolObjects_DatumObject()
{
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
//function : IsDatumTarget
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_DatumObject::IsDatumTarget()  const
{
  return !myDatumTarget.IsNull();
}
