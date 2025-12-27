// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Interface_EntityIterator.hxx>
#include "RWStepGeom_RWEvaluatedDegeneratePcurve.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_EvaluatedDegeneratePcurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>

RWStepGeom_RWEvaluatedDegeneratePcurve::RWStepGeom_RWEvaluatedDegeneratePcurve() {}

void RWStepGeom_RWEvaluatedDegeneratePcurve::ReadStep(
  const occ::handle<StepData_StepReaderData>&            data,
  const int                            num,
  occ::handle<Interface_Check>&                          ach,
  const occ::handle<StepGeom_EvaluatedDegeneratePcurve>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "evaluated_degenerate_pcurve"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : basisSurface ---

  occ::handle<StepGeom_Surface> aBasisSurface;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

  // --- inherited field : referenceToCurve ---

  occ::handle<StepRepr_DefinitionalRepresentation> aReferenceToCurve;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "reference_to_curve",
                   ach,
                   STANDARD_TYPE(StepRepr_DefinitionalRepresentation),
                   aReferenceToCurve);

  // --- own field : equivalentPoint ---

  occ::handle<StepGeom_CartesianPoint> aEquivalentPoint;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadEntity(num,
                   4,
                   "equivalent_point",
                   ach,
                   STANDARD_TYPE(StepGeom_CartesianPoint),
                   aEquivalentPoint);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBasisSurface, aReferenceToCurve, aEquivalentPoint);
}

void RWStepGeom_RWEvaluatedDegeneratePcurve::WriteStep(
  StepData_StepWriter&                              SW,
  const occ::handle<StepGeom_EvaluatedDegeneratePcurve>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field basisSurface ---

  SW.Send(ent->BasisSurface());

  // --- inherited field referenceToCurve ---

  SW.Send(ent->ReferenceToCurve());

  // --- own field : equivalentPoint ---

  SW.Send(ent->EquivalentPoint());
}

void RWStepGeom_RWEvaluatedDegeneratePcurve::Share(
  const occ::handle<StepGeom_EvaluatedDegeneratePcurve>& ent,
  Interface_EntityIterator&                         iter) const
{

  iter.GetOneItem(ent->BasisSurface());

  iter.GetOneItem(ent->ReferenceToCurve());

  iter.GetOneItem(ent->EquivalentPoint());
}
