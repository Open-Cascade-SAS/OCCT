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
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleElementSelect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceSideStyle,Standard_Transient)

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
  return (styles.IsNull()) ? 0 : styles->Length();
}
