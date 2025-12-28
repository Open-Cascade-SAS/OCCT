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

#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_FillStyleSelect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_FillAreaStyle, Standard_Transient)

StepVisual_FillAreaStyle::StepVisual_FillAreaStyle() = default;

void StepVisual_FillAreaStyle::Init(
  const occ::handle<TCollection_HAsciiString>&                        aName,
  const occ::handle<NCollection_HArray1<StepVisual_FillStyleSelect>>& aFillStyles)
{
  // --- classe own fields ---
  name       = aName;
  fillStyles = aFillStyles;
}

void StepVisual_FillAreaStyle::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepVisual_FillAreaStyle::Name() const
{
  return name;
}

void StepVisual_FillAreaStyle::SetFillStyles(
  const occ::handle<NCollection_HArray1<StepVisual_FillStyleSelect>>& aFillStyles)
{
  fillStyles = aFillStyles;
}

occ::handle<NCollection_HArray1<StepVisual_FillStyleSelect>> StepVisual_FillAreaStyle::FillStyles()
  const
{
  return fillStyles;
}

StepVisual_FillStyleSelect StepVisual_FillAreaStyle::FillStylesValue(const int num) const
{
  return fillStyles->Value(num);
}

int StepVisual_FillAreaStyle::NbFillStyles() const
{
  return (fillStyles.IsNull() ? 0 : fillStyles->Length());
}
