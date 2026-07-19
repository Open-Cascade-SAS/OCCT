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

#include <StepVisual_ColourRgb.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ColourRgb, StepVisual_ColourSpecification)

StepVisual_ColourRgb::StepVisual_ColourRgb() = default;

void StepVisual_ColourRgb::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                const double                                 aRed,
                                const double                                 aGreen,
                                const double                                 aBlue)
{
  // --- classe own fields ---
  red   = aRed;
  green = aGreen;
  blue  = aBlue;
  // --- classe inherited fields ---
  StepVisual_ColourSpecification::Init(aName);
}

void StepVisual_ColourRgb::SetRed(const double aRed)
{
  red = aRed;
}

double StepVisual_ColourRgb::Red() const
{
  return red;
}

void StepVisual_ColourRgb::SetGreen(const double aGreen)
{
  green = aGreen;
}

double StepVisual_ColourRgb::Green() const
{
  return green;
}

void StepVisual_ColourRgb::SetBlue(const double aBlue)
{
  blue = aBlue;
}

double StepVisual_ColourRgb::Blue() const
{
  return blue;
}
