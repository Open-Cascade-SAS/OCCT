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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>

//=================================================================================================

int BRepGraph_BackRefManager::AddRelEdge(BRepGraph&          theGraph,
                                         BRepGraph_NodeId    theFrom,
                                         BRepGraph_NodeId    theTo,
                                         BRepGraph_RelEdge::Kind theKind)
{
  BRepGraph_RelEdge anEdge;
  anEdge.RelKind = theKind;
  anEdge.Source  = theFrom;
  anEdge.Target  = theTo;

  theGraph.myData->myOutRelEdges.TryBind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  theGraph.myData->myInRelEdges.TryBind(theTo, NCollection_Vector<BRepGraph_RelEdge>());

  NCollection_Vector<BRepGraph_RelEdge>& anOutVec =
    theGraph.myData->myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  theGraph.myData->myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph_BackRefManager::RemoveRelEdges(BRepGraph&          theGraph,
                                              BRepGraph_NodeId    theFrom,
                                              BRepGraph_NodeId    theTo,
                                              BRepGraph_RelEdge::Kind theKind)
{
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec =
    theGraph.myData->myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Target == theTo)
      {
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  NCollection_Vector<BRepGraph_RelEdge>* anInVec =
    theGraph.myData->myInRelEdges.ChangeSeek(theTo);
  if (anInVec != nullptr)
  {
    for (int anIdx = anInVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anInVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Source == theFrom)
      {
        if (anIdx < anInVec->Length() - 1)
          anInVec->ChangeValue(anIdx) = anInVec->Value(anInVec->Length() - 1);
        anInVec->EraseLast();
      }
    }
  }
}

//=================================================================================================

void BRepGraph_BackRefManager::ClearRelEdges(BRepGraph&       theGraph,
                                             BRepGraph_NodeId theNode)
{
  theGraph.myData->myOutRelEdges.UnBind(theNode);
  theGraph.myData->myInRelEdges.UnBind(theNode);
}

//=================================================================================================

void BRepGraph_BackRefManager::ClearAll(BRepGraph& theGraph)
{
  // Clear incidence reverse indices (edge-to-wire, etc.).
  theGraph.myData->myIncStorage.ReverseIdx.Clear();

  // Clear relation edge maps.
  theGraph.myData->myOutRelEdges.Clear();
  theGraph.myData->myInRelEdges.Clear();
}

//=================================================================================================

void BRepGraph_BackRefManager::RebuildAll(BRepGraph& theGraph)
{
  // Rebuild incidence reverse index after mutation.
  theGraph.myData->myIncStorage.BuildReverseIndex();
}
