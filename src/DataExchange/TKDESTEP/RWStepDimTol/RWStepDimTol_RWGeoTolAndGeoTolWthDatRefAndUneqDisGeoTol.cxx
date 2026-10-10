// Created on: 2015-08-11
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

#include "RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.pxx"
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include "RWStepDimTol_UnequalTolerance.pxx"

RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::
  RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol() = default;

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::ReadStep(
  const occ::handle<StepData_StepReaderData>&                             data,
  const int                                                               num,
  occ::handle<Interface_Check>&                                           ach,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::Parameters aParameters;
  if (RWStepDimTol_UnequalTolerance::ReadStep<true, false, false>(data, num, ach, aParameters))
  {
    ent->Init(aParameters.Name,
              aParameters.Description,
              aParameters.Magnitude,
              aParameters.Target,
              aParameters.Datum,
              aParameters.Type,
              aParameters.Unequal);
  }
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::WriteStep(
  StepData_StepWriter&                                                    SW,
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent) const
{
  RWStepDimTol_UnequalTolerance::WriteStep<true, false, false>(SW, ent);
}

//=================================================================================================

void RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol::Share(
  const occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>& ent,
  Interface_EntityIterator&                                               iter) const
{
  RWStepDimTol_UnequalTolerance::Share<true, false>(ent, iter);
}
