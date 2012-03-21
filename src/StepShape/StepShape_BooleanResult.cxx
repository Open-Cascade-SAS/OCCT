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

#include <StepShape_BooleanResult.ixx>


StepShape_BooleanResult::StepShape_BooleanResult ()  {}

void StepShape_BooleanResult::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_BooleanResult::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepShape_BooleanOperator aOperator,
	const StepShape_BooleanOperand& aFirstOperand,
	const StepShape_BooleanOperand& aSecondOperand)
{
	// --- classe own fields ---
	anOperator = aOperator;
	firstOperand = aFirstOperand;
	secondOperand = aSecondOperand;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_BooleanResult::SetOperator(const StepShape_BooleanOperator aOperator)
{
	anOperator = aOperator;
}

StepShape_BooleanOperator StepShape_BooleanResult::Operator() const
{
	return anOperator;
}

void StepShape_BooleanResult::SetFirstOperand(const StepShape_BooleanOperand& aFirstOperand)
{
	firstOperand = aFirstOperand;
}

StepShape_BooleanOperand StepShape_BooleanResult::FirstOperand() const
{
	return firstOperand;
}

void StepShape_BooleanResult::SetSecondOperand(const StepShape_BooleanOperand& aSecondOperand)
{
	secondOperand = aSecondOperand;
}

StepShape_BooleanOperand StepShape_BooleanResult::SecondOperand() const
{
	return secondOperand;
}
