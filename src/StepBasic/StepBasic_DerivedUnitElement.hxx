// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepBasic_DerivedUnitElement_HeaderFile
#define _StepBasic_DerivedUnitElement_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class StepBasic_NamedUnit;


class StepBasic_DerivedUnitElement;
DEFINE_STANDARD_HANDLE(StepBasic_DerivedUnitElement, Standard_Transient)

//! Added from StepBasic Rev2 to Rev4
class StepBasic_DerivedUnitElement : public Standard_Transient
{

public:

  
  Standard_EXPORT StepBasic_DerivedUnitElement();
  
  Standard_EXPORT void Init (const Handle(StepBasic_NamedUnit)& aUnit, const Standard_Real aExponent);
  
  Standard_EXPORT void SetUnit (const Handle(StepBasic_NamedUnit)& aUnit);
  
  Standard_EXPORT Handle(StepBasic_NamedUnit) Unit() const;
  
  Standard_EXPORT void SetExponent (const Standard_Real aExponent);
  
  Standard_EXPORT Standard_Real Exponent() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_DerivedUnitElement,Standard_Transient)

protected:




private:


  Handle(StepBasic_NamedUnit) theUnit;
  Standard_Real theExponent;


};







#endif // _StepBasic_DerivedUnitElement_HeaderFile
