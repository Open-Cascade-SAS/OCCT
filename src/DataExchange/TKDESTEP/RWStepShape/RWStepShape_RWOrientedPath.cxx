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
#include "RWStepShape_RWOrientedPath.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedPath.hxx>

RWStepShape_RWOrientedPath::RWStepShape_RWOrientedPath() {}

void RWStepShape_RWOrientedPath::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                 num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepShape_OrientedPath>&  ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "oriented_path"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : edgeList ---
  // --- this field is redefined ---
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->CheckDerived(num, 2, "edge_list", ach, false);

  // --- own field : pathElement ---

  occ::handle<StepShape_EdgeLoop> aPathElement;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "path_element", ach, STANDARD_TYPE(StepShape_EdgeLoop), aPathElement);

  // --- own field : orientation ---

  bool aOrientation;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadBoolean(num, 4, "orientation", ach, aOrientation);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPathElement, aOrientation);
}

void RWStepShape_RWOrientedPath::WriteStep(StepData_StepWriter&                  SW,
                                           const occ::handle<StepShape_OrientedPath>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field edgeList ---

  SW.SendDerived();

  // --- own field : pathElement ---

  SW.Send(ent->PathElement());

  // --- own field : orientation ---

  SW.SendBoolean(ent->Orientation());
}

void RWStepShape_RWOrientedPath::Share(const occ::handle<StepShape_OrientedPath>& ent,
                                       Interface_EntityIterator&             iter) const
{

  iter.GetOneItem(ent->PathElement());
}
