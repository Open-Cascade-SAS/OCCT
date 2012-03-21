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

#include <StepVisual_FillAreaStyle.ixx>


StepVisual_FillAreaStyle::StepVisual_FillAreaStyle ()  {}

void StepVisual_FillAreaStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfFillStyleSelect)& aFillStyles)
{
	// --- classe own fields ---
	name = aName;
	fillStyles = aFillStyles;
}


void StepVisual_FillAreaStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_FillAreaStyle::Name() const
{
	return name;
}

void StepVisual_FillAreaStyle::SetFillStyles(const Handle(StepVisual_HArray1OfFillStyleSelect)& aFillStyles)
{
	fillStyles = aFillStyles;
}

Handle(StepVisual_HArray1OfFillStyleSelect) StepVisual_FillAreaStyle::FillStyles() const
{
	return fillStyles;
}

StepVisual_FillStyleSelect StepVisual_FillAreaStyle::FillStylesValue(const Standard_Integer num) const
{
	return fillStyles->Value(num);
}

Standard_Integer StepVisual_FillAreaStyle::NbFillStyles () const
{
	return fillStyles->Length();
}
