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
#include "RWStepShape_RWEdge.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_Vertex.hxx>

RWStepShape_RWEdge::RWStepShape_RWEdge() {}

void RWStepShape_RWEdge::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                  const int                                   num,
                                  occ::handle<Interface_Check>&               ach,
                                  const occ::handle<StepShape_Edge>&          ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "edge"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : edgeStart ---

  occ::handle<StepShape_Vertex> aEdgeStart;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "edge_start", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeStart);

  // --- own field : edgeEnd ---

  occ::handle<StepShape_Vertex> aEdgeEnd;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "edge_end", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeEnd);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aEdgeStart, aEdgeEnd);
}

void RWStepShape_RWEdge::WriteStep(StepData_StepWriter&               SW,
                                   const occ::handle<StepShape_Edge>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : edgeStart ---

  SW.Send(ent->EdgeStart());

  // --- own field : edgeEnd ---

  SW.Send(ent->EdgeEnd());
}

void RWStepShape_RWEdge::Share(const occ::handle<StepShape_Edge>& ent,
                               Interface_EntityIterator&          iter) const
{

  iter.GetOneItem(ent->EdgeStart());

  iter.GetOneItem(ent->EdgeEnd());
}
