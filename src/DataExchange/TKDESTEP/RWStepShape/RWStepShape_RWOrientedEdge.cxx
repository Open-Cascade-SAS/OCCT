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
#include "RWStepShape_RWOrientedEdge.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>

RWStepShape_RWOrientedEdge::RWStepShape_RWOrientedEdge() = default;

void RWStepShape_RWOrientedEdge::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                                   num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepShape_OrientedEdge>&  ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 5, ach, "oriented_edge"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : edgeStart ---
  // --- this field is redefined ---
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->CheckDerived(num, 2, "edge_start", ach, false);

  // --- inherited field : edgeEnd ---
  // --- this field is redefined ---
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->CheckDerived(num, 3, "edge_end", ach, false);

  // --- own field : edgeElement ---

  occ::handle<StepShape_Edge> aEdgeElement;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadEntity(num, 4, "edge_element", ach, STANDARD_TYPE(StepShape_Edge), aEdgeElement);

  // --- own field : orientation ---

  bool aOrientation;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadBoolean(num, 5, "orientation", ach, aOrientation);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aEdgeElement, aOrientation);
}

void RWStepShape_RWOrientedEdge::WriteStep(StepData_StepWriter&                       SW,
                                           const occ::handle<StepShape_OrientedEdge>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field edgeStart ---

  SW.SendDerived();

  // --- inherited field edgeEnd ---

  SW.SendDerived();

  // --- own field : edgeElement ---

  SW.Send(ent->EdgeElement());

  // --- own field : orientation ---

  SW.SendBoolean(ent->Orientation());
}

void RWStepShape_RWOrientedEdge::Share(const occ::handle<StepShape_OrientedEdge>& ent,
                                       Interface_EntityIterator&                  iter) const
{

  iter.GetOneItem(ent->EdgeElement());
}
