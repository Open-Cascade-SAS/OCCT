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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_TessellatedShapeRepresentation.hxx>

//! Representation of STEP entity TessellatedShapeRepresentationWithAccuracyParameters
class StepVisual_TessellatedShapeRepresentationWithAccuracyParameters
    : public StepVisual_TessellatedShapeRepresentation
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedShapeRepresentationWithAccuracyParameters();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&             theRepresentation_Name,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& theRepresentation_Items,
    const occ::handle<StepRepr_RepresentationContext>&       theRepresentation_ContextOfItems,
    const occ::handle<NCollection_HArray1<double>>&                theTessellationAccuracyParameters);

  //! Returns field TessellationAccuracyParameters
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> TessellationAccuracyParameters() const;

  //! Sets field TessellationAccuracyParameters
  Standard_EXPORT void SetTessellationAccuracyParameters(
    const occ::handle<NCollection_HArray1<double>>& theTessellationAccuracyParameters);

  //! Returns number of TessellationAccuracyParameters
  Standard_EXPORT int NbTessellationAccuracyParameters() const;

  //! Returns value of TessellationAccuracyParameters by its num
  Standard_EXPORT const double& TessellationAccuracyParametersValue(
    const int theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters,
                          StepVisual_TessellatedShapeRepresentation)

private:
  occ::handle<NCollection_HArray1<double>> myTessellationAccuracyParameters;
};

#endif // _StepVisual_TessellatedShapeRepresentationWithAccuracyParameters_HeaderFile_
