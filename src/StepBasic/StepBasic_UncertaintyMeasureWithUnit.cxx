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

#include <StepBasic_UncertaintyMeasureWithUnit.ixx>


//=======================================================================
//function : StepBasic_UncertaintyMeasureWithUnit
//purpose  : 
//=======================================================================

StepBasic_UncertaintyMeasureWithUnit::StepBasic_UncertaintyMeasureWithUnit ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit& aUnitComponent)
{

	StepBasic_MeasureWithUnit::Init(aValueComponent, aUnitComponent);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit& aUnitComponent,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	// --- classe inherited fields ---
	StepBasic_MeasureWithUnit::Init(aValueComponent, aUnitComponent);
}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_UncertaintyMeasureWithUnit::Name() const
{
	return name;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_UncertaintyMeasureWithUnit::Description() const
{
	return description;
}
