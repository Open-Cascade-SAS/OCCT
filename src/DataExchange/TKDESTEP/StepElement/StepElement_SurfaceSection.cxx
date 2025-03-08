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

#include <StepElement_SurfaceSection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_SurfaceSection, Standard_Transient)

//=================================================================================================

StepElement_SurfaceSection::StepElement_SurfaceSection() {}

//=================================================================================================

void StepElement_SurfaceSection::Init(
  const StepElement_MeasureOrUnspecifiedValue& aOffset,
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMass,
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMassOffset)
{

  theOffset = aOffset;

  theNonStructuralMass = aNonStructuralMass;

  theNonStructuralMassOffset = aNonStructuralMassOffset;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::Offset() const
{
  return theOffset;
}

//=================================================================================================

void StepElement_SurfaceSection::SetOffset(const StepElement_MeasureOrUnspecifiedValue& aOffset)
{
  theOffset = aOffset;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::NonStructuralMass() const
{
  return theNonStructuralMass;
}

//=================================================================================================

void StepElement_SurfaceSection::SetNonStructuralMass(
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMass)
{
  theNonStructuralMass = aNonStructuralMass;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::NonStructuralMassOffset() const
{
  return theNonStructuralMassOffset;
}

//=================================================================================================

void StepElement_SurfaceSection::SetNonStructuralMassOffset(
  const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMassOffset)
{
  theNonStructuralMassOffset = aNonStructuralMassOffset;
}
