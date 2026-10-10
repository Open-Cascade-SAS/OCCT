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

RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::
  RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol() = default;

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&                             data,
  const int                                                               num,
  occ::handle<Interface_Check>&                                           ach,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::ReadStep<false, true, true>(data, num, ach, ent);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                                    SW,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::WriteStep<false, true, true>(SW, ent);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTolAndUneqDisGeoTol>& ent,
  Interface_EntityIterator&                                               iter) const
{
  RWStepDimTol_UnequalTolerance::Share<false, true, true>(ent, iter);
}
