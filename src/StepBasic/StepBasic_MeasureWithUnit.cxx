// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_MeasureWithUnit.ixx>


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
