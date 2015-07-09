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


#ifndef _XCAFDimTolObjects_GeomToleranceObject_HeaderFile
#define _XCAFDimTolObjects_GeomToleranceObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDimTolObjects_GeomToleranceObjectSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>
#include <Standard_Real.hxx>
#include <XCAFDimTolObjects_GeomToleranceMatReqModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceZoneModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceModifiersSequence.hxx>
#include <Standard_Transient.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>

class XCAFDimTolObjects_GeomToleranceObject;
DEFINE_STANDARD_HANDLE(XCAFDimTolObjects_GeomToleranceObject, Standard_Transient)

//! attribute to store dimension and tolerance
class XCAFDimTolObjects_GeomToleranceObject : public Standard_Transient
{

public:
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject();
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceObject(const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObj);
  
  Standard_EXPORT void SetType (const XCAFDimTolObjects_GeomToleranceType theType);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceType GetType() const;
  
  Standard_EXPORT void SetTypeOfValue (const XCAFDimTolObjects_GeomToleranceTypeValue theTypeOfValue);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceTypeValue GetTypeOfValue() const;
  
  Standard_EXPORT void SetValue (const Standard_Real theValue);
  
  Standard_EXPORT Standard_Real GetValue() const;
  
  Standard_EXPORT void SetMaterialRequirementModifier (const XCAFDimTolObjects_GeomToleranceMatReqModif theMatReqModif);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceMatReqModif GetMaterialRequirementModifier() const;
  
  Standard_EXPORT void SetZoneModifier (const XCAFDimTolObjects_GeomToleranceZoneModif theZoneModif);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceZoneModif GetZoneModifier() const;
  
  Standard_EXPORT void SetValueOfZoneModifier (const Standard_Real theValue);
  
  Standard_EXPORT Standard_Real GetValueOfZoneModifier() const;
  
  Standard_EXPORT void SetModifiers (const XCAFDimTolObjects_GeomToleranceModifiersSequence& theModifiers);
  
  Standard_EXPORT void AddModifier (const XCAFDimTolObjects_GeomToleranceModif theModifier);
  
  Standard_EXPORT XCAFDimTolObjects_GeomToleranceModifiersSequence GetModifiers() const;
  
  Standard_EXPORT void SetMaxValueModifier (const Standard_Real theModifier);
  
  Standard_EXPORT Standard_Real GetMaxValueModifier() const;

  DEFINE_STANDARD_RTTI(XCAFDimTolObjects_GeomToleranceObject,Standard_Transient)

private: 

  XCAFDimTolObjects_GeomToleranceType myType;
  XCAFDimTolObjects_GeomToleranceTypeValue myTypeOfValue;
  Standard_Real myValue;
  XCAFDimTolObjects_GeomToleranceMatReqModif myMatReqModif;
  XCAFDimTolObjects_GeomToleranceZoneModif myZoneModif;
  Standard_Real myValueOfZoneModif;
  XCAFDimTolObjects_GeomToleranceModifiersSequence myModifiers;
  Standard_Real myMaxValueModif;


};







#endif // _XCAFDimTolObjects_GeomToleranceObject_HeaderFile
