// Copyright (c) 2026 OPEN CASCADE SAS
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

#include "RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol.pxx"
#include <StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol.hxx>
#include "RWStepDimTol_UnequalTolerance.pxx"

//==================================================================================================

RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::
  RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol() = default;

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&                             theData,
  const int                                                               theNum,
  occ::handle<Interface_Check>&                                           theCheck,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& theEntity) const
{
  RWStepDimTol_UnequalTolerance::Parameters aParameters;
  if (RWStepDimTol_UnequalTolerance::ReadStep<false, true, true>(theData,
                                                                 theNum,
                                                                 theCheck,
                                                                 aParameters))
  {
    theEntity->Init(aParameters.Name,
                    aParameters.Description,
                    aParameters.Magnitude,
                    aParameters.Target,
                    aParameters.Modifiers,
                    aParameters.Maximum,
                    aParameters.Type,
                    aParameters.Unequal);
  }
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                                    theWriter,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& theEntity) const
{
  RWStepDimTol_UnequalTolerance::WriteStep<false, true, true>(theWriter, theEntity);
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& theEntity,
  Interface_EntityIterator&                                               theIterator) const
{
  RWStepDimTol_UnequalTolerance::Share<false, true>(theEntity, theIterator);
}
