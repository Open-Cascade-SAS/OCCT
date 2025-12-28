// Created on: 2015-07-13
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

#ifndef _StepDimTol_RunoutZoneDefinition_HeaderFile
#define _StepDimTol_RunoutZoneDefinition_HeaderFile

#include <Standard.hxx>

#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepDimTol_ToleranceZoneDefinition.hxx>
#include <Standard_Integer.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity ToleranceZoneDefinition
class StepDimTol_RunoutZoneDefinition : public StepDimTol_ToleranceZoneDefinition
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_RunoutZoneDefinition();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepDimTol_ToleranceZone>&                               theZone,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>>& theBoundaries,
    const occ::handle<StepDimTol_RunoutZoneOrientation>&                       theOrientation);

  //! Returns field Orientation
  inline occ::handle<StepDimTol_RunoutZoneOrientation> Orientation() const { return myOrientation; }

  //! Set field Orientation
  inline void SetOrientation(const occ::handle<StepDimTol_RunoutZoneOrientation>& theOrientation)
  {
    myOrientation = theOrientation;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_RunoutZoneDefinition, StepDimTol_ToleranceZoneDefinition)

private:
  occ::handle<StepDimTol_RunoutZoneOrientation> myOrientation;
};
#endif // _StepDimTol_RunoutToleranceZone_HeaderFile
