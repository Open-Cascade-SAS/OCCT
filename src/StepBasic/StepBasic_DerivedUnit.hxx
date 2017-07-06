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

#ifndef _StepBasic_DerivedUnit_HeaderFile
#define _StepBasic_DerivedUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_HArray1OfDerivedUnitElement.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class StepBasic_DerivedUnitElement;


class StepBasic_DerivedUnit;
DEFINE_STANDARD_HANDLE(StepBasic_DerivedUnit, Standard_Transient)

//! Added from StepBasic Rev2 to Rev4
class StepBasic_DerivedUnit : public Standard_Transient
{

public:

  
  Standard_EXPORT StepBasic_DerivedUnit();
  
  Standard_EXPORT void Init (const Handle(StepBasic_HArray1OfDerivedUnitElement)& elements);
  
  Standard_EXPORT void SetElements (const Handle(StepBasic_HArray1OfDerivedUnitElement)& elements);
  
  Standard_EXPORT Handle(StepBasic_HArray1OfDerivedUnitElement) Elements() const;
  
  Standard_EXPORT Standard_Integer NbElements() const;
  
  Standard_EXPORT Handle(StepBasic_DerivedUnitElement) ElementsValue (const Standard_Integer num) const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_DerivedUnit,Standard_Transient)

protected:




private:


  Handle(StepBasic_HArray1OfDerivedUnitElement) theElements;


};







#endif // _StepBasic_DerivedUnit_HeaderFile
