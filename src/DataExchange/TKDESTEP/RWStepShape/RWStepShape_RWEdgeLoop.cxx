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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include "RWStepShape_RWEdgeLoop.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OrientedEdge.hxx>

RWStepShape_RWEdgeLoop::RWStepShape_RWEdgeLoop() {}

void RWStepShape_RWEdgeLoop::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                      const int                 num,
                                      occ::handle<Interface_Check>&               ach,
                                      const occ::handle<StepShape_EdgeLoop>&      ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "edge_loop"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat0 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : edgeList ---

  occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> aEdgeList;
  occ::handle<StepShape_OrientedEdge>          anent;
  int                        nsub1;
  if (data->ReadSubList(num, 2, "edge_list", ach, nsub1))
  {
    int nb1 = data->NbParams(nsub1);
    aEdgeList            = new NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>(1, nb1);
    for (int i1 = 1; i1 <= nb1; i1++)
    {
      // szv#4:S4163:12Mar99 `bool stat1 =` not needed
      if (data->ReadEntity(nsub1,
                           i1,
                           "oriented_edge",
                           ach,
                           STANDARD_TYPE(StepShape_OrientedEdge),
                           anent))
        aEdgeList->SetValue(i1, anent);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aEdgeList);
}

void RWStepShape_RWEdgeLoop::WriteStep(StepData_StepWriter&              SW,
                                       const occ::handle<StepShape_EdgeLoop>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : edgeList ---

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->NbEdgeList(); i1++)
  {
    SW.Send(ent->EdgeListValue(i1));
  }
  SW.CloseSub();
}

// ============================================================================
// Method : Share
// ============================================================================

void RWStepShape_RWEdgeLoop::Share(const occ::handle<StepShape_EdgeLoop>& ent,
                                   Interface_EntityIterator&         iter) const
{

  int nbElem1 = ent->NbEdgeList();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->EdgeListValue(is1));
  }
}

void RWStepShape_RWEdgeLoop::Check(const occ::handle<StepShape_EdgeLoop>& ent,
                                   const Interface_ShareTool&,
                                   occ::handle<Interface_Check>& ach) const
{
  //  std::cout << "------ calling CheckEdgeLoop ------" << std::endl;
  bool headToTail = true;
  // bool noIdentVtx = true; //szv#4:S4163:12Mar99 unused
  int nbEdg = ent->NbEdgeList();
  if (nbEdg == 0)
  {
    ach->AddFail("Edge loop contains empty edge list");
    return;
  }
  occ::handle<StepShape_OrientedEdge> theOE     = ent->EdgeListValue(1);
  occ::handle<StepShape_Vertex>       theVxFrst = theOE->EdgeStart();
  occ::handle<StepShape_Vertex>       theVxLst  = theOE->EdgeEnd();
  if ((nbEdg == 1) && (theVxFrst != theVxLst))
  {
    ach->AddFail(
      "Edge loop composed of single Edge : Start and End Vertex of edge are not identical");
  }
  for (int i = 2; i <= nbEdg; i++)
  {
    theOE                              = ent->EdgeListValue(i);
    occ::handle<StepShape_Vertex> theVxStrt = theOE->EdgeStart();
    if (theVxStrt != theVxLst)
    {
      headToTail = false;
    }
    theVxLst = theOE->EdgeEnd();
    if (theVxStrt == theVxLst)
    {
      ach->AddWarning("One edge_curve contains identical vertices");
    }
  }
  if (theVxFrst != theVxLst)
  {
    headToTail = false;
  }
  if (!headToTail)
  {
    ach->AddFail("Error : Path does not head to tail");
  }
}
