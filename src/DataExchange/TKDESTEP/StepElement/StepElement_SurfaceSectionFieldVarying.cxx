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

#include <StepElement_SurfaceSectionFieldVarying.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_SurfaceSectionFieldVarying, StepElement_SurfaceSectionField)

//=================================================================================================

StepElement_SurfaceSectionFieldVarying::StepElement_SurfaceSectionFieldVarying() {}

//=================================================================================================

void StepElement_SurfaceSectionFieldVarying::Init(
  const occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>& aDefinitions,
  const bool aAdditionalNodeValues)
{
  // StepElement_SurfaceSectionField::Init();

  theDefinitions = aDefinitions;

  theAdditionalNodeValues = aAdditionalNodeValues;
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>
  StepElement_SurfaceSectionFieldVarying::Definitions() const
{
  return theDefinitions;
}

//=================================================================================================

void StepElement_SurfaceSectionFieldVarying::SetDefinitions(
  const occ::handle<NCollection_HArray1<occ::handle<StepElement_SurfaceSection>>>& aDefinitions)
{
  theDefinitions = aDefinitions;
}

//=================================================================================================

bool StepElement_SurfaceSectionFieldVarying::AdditionalNodeValues() const
{
  return theAdditionalNodeValues;
}

//=================================================================================================

void StepElement_SurfaceSectionFieldVarying::SetAdditionalNodeValues(
  const bool aAdditionalNodeValues)
{
  theAdditionalNodeValues = aAdditionalNodeValues;
}
