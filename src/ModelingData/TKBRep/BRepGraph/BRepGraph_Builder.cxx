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

void BRepGraph_Builder::populateUsagesAndBridgeFields(BRepGraph& theGraph)
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

  // --- Wires: build edge-to-wire reverse index ---
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
  {
    const BRepGraphInc::WireEntity& aWireDef = aStorage.Wires.Value(i);
    for (int e = 0; e < aWireDef.EdgeRefs.Length(); ++e)
    {
      const BRepGraphInc::EdgeRef& aER = aWireDef.EdgeRefs.Value(e);
      BRepGraph_BackRefManager::BindEdgeToWire(theGraph, aER.EdgeIdx, i);
    }
  }

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
  theGraph.myData->myTShapeToDefId.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myOriginalShapes.Clear();
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

  // Phase 2: Copy unified TShape->NodeId and OriginalShapes from incidence storage.
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  theGraph.myData->myTShapeToDefId.ReSize(aStorage.TShapeToNodeId.Extent());
  for (NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>::Iterator
         anIter(aStorage.TShapeToNodeId);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myTShapeToDefId.Bind(anIter.Key(), anIter.Value());
  }
  for (NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator
         anIter(aStorage.OriginalShapes);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myOriginalShapes.Bind(anIter.Key(), anIter.Value());
  }

  // Phase 3: Populate bridge fields on incidence entities (UIDs, PCurves, etc.).
  populateUsagesAndBridgeFields(theGraph);

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
  theGraph.myData->myEdgeToWires.Clear();

  // Re-copy TShape->NodeId and OriginalShapes.
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  theGraph.myData->myTShapeToDefId.Clear();
  theGraph.myData->myTShapeToDefId.ReSize(aStorage.TShapeToNodeId.Extent());
  for (NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>::Iterator
         anIter(aStorage.TShapeToNodeId);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myTShapeToDefId.Bind(anIter.Key(), anIter.Value());
  }
  theGraph.myData->myOriginalShapes.Clear();
  for (NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator
         anIter(aStorage.OriginalShapes);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myOriginalShapes.Bind(anIter.Key(), anIter.Value());
  }

  populateUsagesAndBridgeFields(theGraph);

  theGraph.myData->myIsDone = true;
}
