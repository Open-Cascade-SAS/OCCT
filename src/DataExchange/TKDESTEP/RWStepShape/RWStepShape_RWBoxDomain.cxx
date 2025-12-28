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
#include "RWStepShape_RWBoxDomain.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepShape_BoxDomain.hxx>

RWStepShape_RWBoxDomain::RWStepShape_RWBoxDomain() {}

void RWStepShape_RWBoxDomain::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                       const int                                   num,
                                       occ::handle<Interface_Check>&               ach,
                                       const occ::handle<StepShape_BoxDomain>&     ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "box_domain"))
    return;

  // --- own field : corner ---

  occ::handle<StepGeom_CartesianPoint> aCorner;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadEntity(num, 1, "corner", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aCorner);

  // --- own field : xlength ---

  double aXlength;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadReal(num, 2, "xlength", ach, aXlength);

  // --- own field : ylength ---

  double aYlength;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "ylength", ach, aYlength);

  // --- own field : zlength ---

  double aZlength;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "zlength", ach, aZlength);

  //--- Initialisation of the read entity ---

  ent->Init(aCorner, aXlength, aYlength, aZlength);
}

void RWStepShape_RWBoxDomain::WriteStep(StepData_StepWriter&                    SW,
                                        const occ::handle<StepShape_BoxDomain>& ent) const
{

  // --- own field : corner ---

  SW.Send(ent->Corner());

  // --- own field : xlength ---

  SW.Send(ent->Xlength());

  // --- own field : ylength ---

  SW.Send(ent->Ylength());

  // --- own field : zlength ---

  SW.Send(ent->Zlength());
}

void RWStepShape_RWBoxDomain::Share(const occ::handle<StepShape_BoxDomain>& ent,
                                    Interface_EntityIterator&               iter) const
{

  iter.GetOneItem(ent->Corner());
}
