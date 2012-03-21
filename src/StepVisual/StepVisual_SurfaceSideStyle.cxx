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

#include <StepVisual_SurfaceSideStyle.ixx>


StepVisual_SurfaceSideStyle::StepVisual_SurfaceSideStyle ()  {}

void StepVisual_SurfaceSideStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles)
{
	// --- classe own fields ---
	name = aName;
	styles = aStyles;
}


void StepVisual_SurfaceSideStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_SurfaceSideStyle::Name() const
{
	return name;
}

void StepVisual_SurfaceSideStyle::SetStyles(const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfSurfaceStyleElementSelect) StepVisual_SurfaceSideStyle::Styles() const
{
	return styles;
}

StepVisual_SurfaceStyleElementSelect StepVisual_SurfaceSideStyle::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_SurfaceSideStyle::NbStyles () const
{
	return styles->Length();
}
