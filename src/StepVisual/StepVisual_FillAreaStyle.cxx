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
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_FillStyleSelect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_FillAreaStyle,Standard_Transient)

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
  return (fillStyles.IsNull() ? 0 : fillStyles->Length());
}
