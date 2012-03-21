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

#include <StepRepr_ShapeAspect.ixx>


StepRepr_ShapeAspect::StepRepr_ShapeAspect ()  {}

void StepRepr_ShapeAspect::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepRepr_ProductDefinitionShape)& aOfShape,
	const StepData_Logical aProductDefinitional)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	ofShape = aOfShape;
	productDefinitional = aProductDefinitional;
}


void StepRepr_ShapeAspect::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_ShapeAspect::Name() const
{
	return name;
}

void StepRepr_ShapeAspect::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_ShapeAspect::Description() const
{
	return description;
}

void StepRepr_ShapeAspect::SetOfShape(const Handle(StepRepr_ProductDefinitionShape)& aOfShape)
{
	ofShape = aOfShape;
}

Handle(StepRepr_ProductDefinitionShape) StepRepr_ShapeAspect::OfShape() const
{
	return ofShape;
}

void StepRepr_ShapeAspect::SetProductDefinitional(const StepData_Logical aProductDefinitional)
{
	productDefinitional = aProductDefinitional;
}

StepData_Logical StepRepr_ShapeAspect::ProductDefinitional() const
{
	return productDefinitional;
}
