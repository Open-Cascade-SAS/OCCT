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

#ifndef _StepVisual_TessellatedShapeRepresentationWithAccuracyParameters_HeaderFile_
#define _StepVisual_TessellatedShapeRepresentationWithAccuracyParameters_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepVisual_TessellatedShapeRepresentation.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters,
                       StepVisual_TessellatedShapeRepresentation)

//! Representation of STEP entity TessellatedShapeRepresentationWithAccuracyParameters
class StepVisual_TessellatedShapeRepresentationWithAccuracyParameters
    : public StepVisual_TessellatedShapeRepresentation
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedShapeRepresentationWithAccuracyParameters();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&             theRepresentation_Name,
    const Handle(StepRepr_HArray1OfRepresentationItem)& theRepresentation_Items,
    const Handle(StepRepr_RepresentationContext)&       theRepresentation_ContextOfItems,
    const Handle(TColStd_HArray1OfReal)&                theTessellationAccuracyParameters);

  //! Returns field TessellationAccuracyParameters
  Standard_EXPORT Handle(TColStd_HArray1OfReal) TessellationAccuracyParameters() const;

  //! Sets field TessellationAccuracyParameters
  Standard_EXPORT void SetTessellationAccuracyParameters(
    const Handle(TColStd_HArray1OfReal)& theTessellationAccuracyParameters);

  //! Returns number of TessellationAccuracyParameters
  Standard_EXPORT Standard_Integer NbTessellationAccuracyParameters() const;

  //! Returns value of TessellationAccuracyParameters by its num
  Standard_EXPORT const Standard_Real& TessellationAccuracyParametersValue(
    const Standard_Integer theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters,
                          StepVisual_TessellatedShapeRepresentation)

private:
  Handle(TColStd_HArray1OfReal) myTessellationAccuracyParameters;
};

#endif // _StepVisual_TessellatedShapeRepresentationWithAccuracyParameters_HeaderFile_
