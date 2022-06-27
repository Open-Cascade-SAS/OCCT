// Created on: 2015-07-16
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_DatumReferenceCompartment_HeaderFile
#define _StepDimTol_DatumReferenceCompartment_HeaderFile

#include <Standard.hxx>

#include <StepDimTol_GeneralDatumReference.hxx>

class StepDimTol_DatumReferenceCompartment;
DEFINE_STANDARD_HANDLE(StepDimTol_DatumReferenceCompartment, StepDimTol_GeneralDatumReference)
//! Representation of STEP entity DatumReferenceCompartment
class StepDimTol_DatumReferenceCompartment : public StepDimTol_GeneralDatumReference
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumReferenceCompartment();

  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumReferenceCompartment,StepDimTol_GeneralDatumReference)
};
#endif // _StepDimTol_DatumReferenceCompartment_HeaderFile
