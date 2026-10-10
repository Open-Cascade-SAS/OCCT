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

RWStepDimTol_RWGeoTolAndUneqDisGeoTol::RWStepDimTol_RWGeoTolAndUneqDisGeoTol() = default;

//=================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&           data,
  const int                                             num,
  occ::handle<Interface_Check>&                         ach,
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::ReadStep<false, false, false>(data, num, ach, ent);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                  SW,
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::WriteStep<false, false, false>(SW, ent);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndUneqDisGeoTol>& ent,
  Interface_EntityIterator&                             iter) const
{
  RWStepDimTol_UnequalTolerance::Share<false, false, false>(ent, iter);
}
