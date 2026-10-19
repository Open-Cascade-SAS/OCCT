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

#include "RWStepDimTol_RWGeoTolAndUneqDisGeoTol.pxx"
#include <StepDimTol_GeoTolAndUneqDisGeoTol.hxx>
#include "RWStepDimTol_UnequalTolerance.pxx"

//==================================================================================================

RWStepDimTol_RWGeoTolAndUneqDisGeoTol::RWStepDimTol_RWGeoTolAndUneqDisGeoTol() = default;

//==================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&           theData,
  const int                                             theNum,
  occ::handle<Interface_Check>&                         theCheck,
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& theEntity) const
{
  RWStepDimTol_UnequalTolerance::Parameters aParameters;
  if (RWStepDimTol_UnequalTolerance::ReadStep<false, false, false>(theData,
                                                                   theNum,
                                                                   theCheck,
                                                                   aParameters))
  {
    theEntity->Init(aParameters.Name,
                    aParameters.Description,
                    aParameters.Magnitude,
                    aParameters.Target,
                    aParameters.Type,
                    aParameters.Unequal);
  }
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                  theWriter,
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& theEntity) const
{
  RWStepDimTol_UnequalTolerance::WriteStep<false, false, false>(theWriter, theEntity);
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& theEntity,
  Interface_EntityIterator&                             theIterator) const
{
  RWStepDimTol_UnequalTolerance::Share<false, false>(theEntity, theIterator);
}
