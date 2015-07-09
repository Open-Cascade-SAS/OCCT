// Created on: 2015-08-06
// Created by: Ilya Novikov
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#ifndef _XCAFDimTolObjects_DatumObject_HeaderFile
#define _XCAFDimTolObjects_DatumObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <Standard_Boolean.hxx>

class XCAFDimTolObjects_DatumObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_DatumObject, Standard_Transient)

//! object to store datum
class XCAFDimTolObjects_DatumObject : public Standard_Transient
{

public:

  Standard_EXPORT XCAFDimTolObjects_DatumObject();
  
  Standard_EXPORT XCAFDimTolObjects_DatumObject(const Handle(XCAFDimTolObjects_DatumObject)& theObj);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& theTag);
  
  Standard_EXPORT XCAFDimTolObjects_DatumModifiersSequence GetModifiers() const;
  
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_DatumModifiersSequence& theModifiers);
  
  Standard_EXPORT void GetModifierWithValue (XCAFDimTolObjects_DatumModifWithValue& theModifier, Standard_Real& theValue) const;
  
  Standard_EXPORT void SetModifierWithValue (const XCAFDimTolObjects_DatumModifWithValue theModifier, const Standard_Real theValue);
  
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_DatumSingleModif theModifier);
  
  Standard_EXPORT TopoDS_Shape GetDatumTarget() const;
  
  Standard_EXPORT void SetDatumTarget (const TopoDS_Shape& theShape);
  
  Standard_EXPORT Standard_Boolean IsDatumTarget() const;


  DEFINE_STANDARD_RTTI(XCAFDimTolObjects_DatumObject,Standard_Transient)

private: 

  Handle(TCollection_HAsciiString) myName;
  XCAFDimTolObjects_DatumModifiersSequence myModifiers;
  XCAFDimTolObjects_DatumModifWithValue myModifierWithValue;
  Standard_Real myValueOfModifier;
  TopoDS_Shape myDatumTarget;

};

#endif // _XCAFDimTolObjects_DatumObject_HeaderFile
