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
#include "RWStepShape_RWBoxedHalfSpace.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Surface.hxx>
#include <StepShape_BoxDomain.hxx>
#include <StepShape_BoxedHalfSpace.hxx>

RWStepShape_RWBoxedHalfSpace::RWStepShape_RWBoxedHalfSpace() {}

void RWStepShape_RWBoxedHalfSpace::ReadStep(const occ::handle<StepData_StepReaderData>&  data,
                                            const int                  num,
                                            occ::handle<Interface_Check>&                ach,
                                            const occ::handle<StepShape_BoxedHalfSpace>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "boxed_half_space"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : baseSurface ---

  occ::handle<StepGeom_Surface> aBaseSurface;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "base_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBaseSurface);

  // --- inherited field : agreementFlag ---

  bool aAgreementFlag;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadBoolean(num, 3, "agreement_flag", ach, aAgreementFlag);

  // --- own field : enclosure ---

  occ::handle<StepShape_BoxDomain> aEnclosure;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadEntity(num, 4, "enclosure", ach, STANDARD_TYPE(StepShape_BoxDomain), aEnclosure);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBaseSurface, aAgreementFlag, aEnclosure);
}

void RWStepShape_RWBoxedHalfSpace::WriteStep(StepData_StepWriter&                    SW,
                                             const occ::handle<StepShape_BoxedHalfSpace>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field baseSurface ---

  SW.Send(ent->BaseSurface());

  // --- inherited field agreementFlag ---

  SW.SendBoolean(ent->AgreementFlag());

  // --- own field : enclosure ---

  SW.Send(ent->Enclosure());
}

void RWStepShape_RWBoxedHalfSpace::Share(const occ::handle<StepShape_BoxedHalfSpace>& ent,
                                         Interface_EntityIterator&               iter) const
{

  iter.GetOneItem(ent->BaseSurface());

  iter.GetOneItem(ent->Enclosure());
}
