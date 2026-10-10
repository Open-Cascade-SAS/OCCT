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

#include "RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol.pxx"
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol.hxx>
#include "RWStepDimTol_UnequalTolerance.pxx"

//==================================================================================================

RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol::
  RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol() = default;

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                            theData,
  const int                                                                              theNum,
  occ::handle<Interface_Check>&                                                          theCheck,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol>& theEntity)
  const
{
  RWStepDimTol_UnequalTolerance::Parameters aParameters;
  if (RWStepDimTol_UnequalTolerance::ReadStep<true, true, false>(theData,
                                                                 theNum,
                                                                 theCheck,
                                                                 aParameters))
  {
    theEntity->Init(aParameters.Name,
                    aParameters.Description,
                    aParameters.Magnitude,
                    aParameters.Target,
                    aParameters.Datum,
                    aParameters.Modifiers,
                    aParameters.Type,
                    aParameters.Unequal);
  }
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                                                   theWriter,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol>& theEntity)
  const
{
  RWStepDimTol_UnequalTolerance::WriteStep<true, true, false>(theWriter, theEntity);
}

//==================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthModAndUneqDisGeoTol>& theEntity,
  Interface_EntityIterator& theIterator) const
{
  RWStepDimTol_UnequalTolerance::Share<true, false>(theEntity, theIterator);
}
