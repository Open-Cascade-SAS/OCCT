// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_MeasureWithUnit_HeaderFile
#define _StepBasic_MeasureWithUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Unit.hxx>
#include <Standard_Transient.hxx>
class StepBasic_MeasureValueMember;


class StepBasic_MeasureWithUnit;
DEFINE_STANDARD_HANDLE(StepBasic_MeasureWithUnit, Standard_Transient)


class StepBasic_MeasureWithUnit : public Standard_Transient
{

public:

  
  //! Returns a MeasureWithUnit
  Standard_EXPORT StepBasic_MeasureWithUnit();
  
  Standard_EXPORT void Init (const Handle(StepBasic_MeasureValueMember)& aValueComponent, const StepBasic_Unit& aUnitComponent);
  
  Standard_EXPORT void SetValueComponent (const Standard_Real aValueComponent);
  
  Standard_EXPORT Standard_Real ValueComponent() const;
  
  Standard_EXPORT Handle(StepBasic_MeasureValueMember) ValueComponentMember() const;
  
  Standard_EXPORT void SetValueComponentMember (const Handle(StepBasic_MeasureValueMember)& val);
  
  Standard_EXPORT void SetUnitComponent (const StepBasic_Unit& aUnitComponent);
  
  Standard_EXPORT StepBasic_Unit UnitComponent() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_MeasureWithUnit,Standard_Transient)

protected:




private:


  Handle(StepBasic_MeasureValueMember) valueComponent;
  StepBasic_Unit unitComponent;


};







#endif // _StepBasic_MeasureWithUnit_HeaderFile
