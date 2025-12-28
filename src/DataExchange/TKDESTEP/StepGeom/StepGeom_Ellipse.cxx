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

#include <StepGeom_Ellipse.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Ellipse, StepGeom_Conic)

StepGeom_Ellipse::StepGeom_Ellipse() = default;

void StepGeom_Ellipse::Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const StepGeom_Axis2Placement&               aPosition,
                            const double                                 aSemiAxis1,
                            const double                                 aSemiAxis2)
{
  // --- classe own fields ---
  semiAxis1 = aSemiAxis1;
  semiAxis2 = aSemiAxis2;
  // --- classe inherited fields ---
  StepGeom_Conic::Init(aName, aPosition);
}

void StepGeom_Ellipse::SetSemiAxis1(const double aSemiAxis1)
{
  semiAxis1 = aSemiAxis1;
}

double StepGeom_Ellipse::SemiAxis1() const
{
  return semiAxis1;
}

void StepGeom_Ellipse::SetSemiAxis2(const double aSemiAxis2)
{
  semiAxis2 = aSemiAxis2;
}

double StepGeom_Ellipse::SemiAxis2() const
{
  return semiAxis2;
}
