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

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters, StepVisual_TessellatedShapeRepresentation)

//=======================================================================
//function : StepVisual_TessellatedShapeRepresentationWithAccuracyParameters
//purpose  : 
//=======================================================================

StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::StepVisual_TessellatedShapeRepresentationWithAccuracyParameters ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::Init (const Handle(TCollection_HAsciiString)& theRepresentation_Name,
                                                                            const Handle(StepRepr_HArray1OfRepresentationItem)& theRepresentation_Items,
                                                                            const Handle(StepRepr_RepresentationContext)& theRepresentation_ContextOfItems,
                                                                            const Handle(TColStd_HArray1OfReal)& theTessellationAccuracyParameters)
{
  StepVisual_TessellatedShapeRepresentation::Init(theRepresentation_Name,
                                                  theRepresentation_Items,
                                                  theRepresentation_ContextOfItems);

  myTessellationAccuracyParameters = theTessellationAccuracyParameters;
}

//=======================================================================
//function : TessellationAccuracyParameters
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::TessellationAccuracyParameters () const
{
  return myTessellationAccuracyParameters;
}

//=======================================================================
//function : SetTessellationAccuracyParameters
//purpose  : 
//=======================================================================

void StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::SetTessellationAccuracyParameters(const Handle(TColStd_HArray1OfReal)& theTessellationAccuracyParameters)
{
  myTessellationAccuracyParameters = theTessellationAccuracyParameters;
}


//=======================================================================
//function : NbTessellationAccuracyParameters
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::NbTessellationAccuracyParameters() const
{
  if (myTessellationAccuracyParameters.IsNull())
  {
    return 0;
  }
  return myTessellationAccuracyParameters->Length();
}


//=======================================================================
//function : TessellationAccuracyParametersValue
//purpose  : 
//=======================================================================

const Standard_Real& StepVisual_TessellatedShapeRepresentationWithAccuracyParameters::TessellationAccuracyParametersValue(const Standard_Integer theNum) const
{
  return myTessellationAccuracyParameters->Value(theNum);
}
