// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepElement_CurveElementSectionDefinition.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_CurveElementSectionDefinition, Standard_Transient)

//=================================================================================================

StepElement_CurveElementSectionDefinition::StepElement_CurveElementSectionDefinition() = default;

//=================================================================================================

void StepElement_CurveElementSectionDefinition::Init(
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const double                                 aSectionAngle)
{

  theDescription = aDescription;

  theSectionAngle = aSectionAngle;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepElement_CurveElementSectionDefinition::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepElement_CurveElementSectionDefinition::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

double StepElement_CurveElementSectionDefinition::SectionAngle() const
{
  return theSectionAngle;
}

//=================================================================================================

void StepElement_CurveElementSectionDefinition::SetSectionAngle(const double aSectionAngle)
{
  theSectionAngle = aSectionAngle;
}
