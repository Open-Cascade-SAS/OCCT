// Created on : Thu Mar 24 18:30:12 2022
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <StepVisual_TessellatedShapeRepresentationWithAccuracyParameters.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters,
                           StepVisual_TessellatedShapeRepresentation)

//=================================================================================================

StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::
  StepVisual_TessellatedShapeRepresentationWithAccuracyParameters() = default;

//=================================================================================================

void StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::Init(
  const occ::handle<TCollection_HAsciiString>& theRepresentation_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
                                                     theRepresentation_Items,
  const occ::handle<StepRepr_RepresentationContext>& theRepresentation_ContextOfItems,
  const occ::handle<NCollection_HArray1<double>>&    theTessellationAccuracyParameters)
{
  StepVisual_TessellatedShapeRepresentation::Init(theRepresentation_Name,
                                                  theRepresentation_Items,
                                                  theRepresentation_ContextOfItems);

  myTessellationAccuracyParameters = theTessellationAccuracyParameters;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>>
  StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::TessellationAccuracyParameters()
    const
{
  return myTessellationAccuracyParameters;
}

//=================================================================================================

void StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::
  SetTessellationAccuracyParameters(
    const occ::handle<NCollection_HArray1<double>>& theTessellationAccuracyParameters)
{
  myTessellationAccuracyParameters = theTessellationAccuracyParameters;
}

//=================================================================================================

int StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::
  NbTessellationAccuracyParameters() const
{
  if (myTessellationAccuracyParameters.IsNull())
  {
    return 0;
  }
  return myTessellationAccuracyParameters->Length();
}

//=================================================================================================

const double& StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::
  TessellationAccuracyParametersValue(const int theNum) const
{
  return myTessellationAccuracyParameters->Value(theNum);
}
