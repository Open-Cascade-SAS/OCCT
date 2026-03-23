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
#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>


//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraph_Data&       aData    = *theGraph.myData;
  BRepGraphInc_Storage& aStorage = aData.myIncStorage;

  if (!aStorage.IsDone)
    return;

  // --- Allocate UIDs for all definitions ---
  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
    theGraph.allocateUID(aStorage.Vertices.Value(i).Id);
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
    theGraph.allocateUID(aStorage.Edges.Value(i).Id);
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
    theGraph.allocateUID(aStorage.Wires.Value(i).Id);
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
    theGraph.allocateUID(aStorage.Faces.Value(i).Id);
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
    theGraph.allocateUID(aStorage.Shells.Value(i).Id);
  for (int i = 0; i < aStorage.Solids.Length(); ++i)
    theGraph.allocateUID(aStorage.Solids.Value(i).Id);
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
    theGraph.allocateUID(aStorage.Compounds.Value(i).Id);
  for (int i = 0; i < aStorage.CompSolids.Length(); ++i)
    theGraph.allocateUID(aStorage.CompSolids.Value(i).Id);

}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  // Phase 0: Clear all storage.
  BRepGraph_BackRefManager::ClearAll(theGraph);

  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->mySolidUIDs.Clear();
  theGraph.myData->myShellUIDs.Clear();
  theGraph.myData->myFaceUIDs.Clear();
  theGraph.myData->myWireUIDs.Clear();
  theGraph.myData->myEdgeUIDs.Clear();
  theGraph.myData->myVertexUIDs.Clear();
  theGraph.myData->myCompoundUIDs.Clear();
  theGraph.myData->myCompSolidUIDs.Clear();
  theGraph.myData->myReconstructedTShapeToDefId.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myMutationOriginals.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myIsDone = false;

  if (theShape.IsNull())
    return;

  // Phase 1: Populate incidence-table storage (sole source of truth).
  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage, theShape, theParallel);
  if (!theGraph.myData->myIncStorage.IsDone)
  {
    theGraph.myData->myIncStorage.Clear();
    return;
  }

  // Phase 2: Allocate UIDs for all incidence entities.
  populateUIDs(theGraph);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  if (theShape.IsNull())
    return;

  // Extend incidence storage incrementally.
  BRepGraphInc_Populate::Append(theGraph.myData->myIncStorage, theShape, theParallel);

  // Clear UID stores.
  theGraph.myData->mySolidUIDs.Clear();
  theGraph.myData->myShellUIDs.Clear();
  theGraph.myData->myFaceUIDs.Clear();
  theGraph.myData->myWireUIDs.Clear();
  theGraph.myData->myEdgeUIDs.Clear();
  theGraph.myData->myVertexUIDs.Clear();
  theGraph.myData->myCompoundUIDs.Clear();
  theGraph.myData->myCompSolidUIDs.Clear();
  // Clear reconstruction caches (stale after append).
  theGraph.myData->myReconstructedTShapeToDefId.Clear();
  theGraph.myData->myMutationOriginals.Clear();

  populateUIDs(theGraph);

  theGraph.myData->myIsDone = true;
}
