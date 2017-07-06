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


#include <Standard_Type.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_Unit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_MeasureWithUnit,Standard_Transient)

//=======================================================================
//function : StepBasic_MeasureWithUnit
//purpose  : 
//=======================================================================
StepBasic_MeasureWithUnit::StepBasic_MeasureWithUnit ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit &aUnitComponent)
{
	// --- classe own fields ---
	valueComponent = aValueComponent;
	unitComponent = aUnitComponent;
}


//=======================================================================
//function : SetValueComponent
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::SetValueComponent(const Standard_Real aValueComponent)
{
  if (valueComponent.IsNull()) valueComponent = new StepBasic_MeasureValueMember;
  valueComponent->SetReal(aValueComponent);
}

//=======================================================================
//function : ValueComponent
//purpose  : 
//=======================================================================

Standard_Real StepBasic_MeasureWithUnit::ValueComponent() const
{
  return (valueComponent.IsNull() ? 0.0 : valueComponent->Real());
}

//=======================================================================
//function : ValueComponentMember
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureValueMember)  StepBasic_MeasureWithUnit::ValueComponentMember () const
{  return valueComponent;  }

//=======================================================================
//function : SetValueComponentMember
//purpose  : 
//=======================================================================

void  StepBasic_MeasureWithUnit::SetValueComponentMember (const Handle(StepBasic_MeasureValueMember)& val)
{  valueComponent = val;  }

//=======================================================================
//function : SetUnitComponent
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::SetUnitComponent(const StepBasic_Unit& aUnitComponent)
{
	unitComponent = aUnitComponent;
}

//=======================================================================
//function : UnitComponent
//purpose  : 
//=======================================================================

StepBasic_Unit StepBasic_MeasureWithUnit::UnitComponent() const
{
	return unitComponent;
}
