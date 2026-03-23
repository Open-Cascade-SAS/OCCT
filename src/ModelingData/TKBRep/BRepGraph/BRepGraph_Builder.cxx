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

#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TVertex.hxx>
#include <TopoDS.hxx>

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

  // --- Vertex point representations (post-pass) ---
  NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef;
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
  {
    const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.Value(i);
    if (!anEdge.Curve3d.IsNull())
      aCurveToEdgeDef.TryBind(anEdge.Curve3d.get(), anEdge.Id);
  }

  NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDef;
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
  {
    const BRepGraphInc::FaceEntity& aFace = aStorage.Faces.Value(i);
    if (!aFace.Surface.IsNull())
      aSurfToFaceDef.TryBind(aFace.Surface.get(), aFace.Id);
  }

  for (int aVtxIdx = 0; aVtxIdx < aStorage.Vertices.Length(); ++aVtxIdx)
  {
    BRepGraphInc::VertexEntity& aVtxDef = aStorage.Vertices.ChangeValue(aVtxIdx);
    const TopoDS_Shape* aVtxShape = aStorage.OriginalShapes.Seek(aVtxDef.Id);
    if (aVtxShape == nullptr || aVtxShape->IsNull())
      continue;
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(*aVtxShape);
    const Handle(BRep_TVertex)& aTVertex =
      Handle(BRep_TVertex)::DownCast(aVertex.TShape());
    if (aTVertex.IsNull())
      continue;

    for (const Handle(BRep_PointRepresentation)& aPtRep : aTVertex->Points())
    {
      if (aPtRep.IsNull())
        continue;
      if (const Handle(BRep_PointOnCurve) aPOC = Handle(BRep_PointOnCurve)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* anEdgeId = aCurveToEdgeDef.Seek(aPOC->Curve().get());
        if (anEdgeId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnCurveEntry anEntry;
        anEntry.Parameter = aPOC->Parameter();
        anEntry.EdgeDefId = *anEdgeId;
        aVtxDef.PointsOnCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnCurveOnSurface) aPOCS =
                 Handle(BRep_PointOnCurveOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOCS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnPCurveEntry anEntry;
        anEntry.Parameter = aPOCS->Parameter();
        anEntry.FaceDefId = *aFaceId;
        aVtxDef.PointsOnPCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnSurface) aPOS =
                 Handle(BRep_PointOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnSurfaceEntry anEntry;
        anEntry.ParameterU = aPOS->Parameter();
        anEntry.ParameterV = aPOS->Parameter2();
        anEntry.FaceDefId  = *aFaceId;
        aVtxDef.PointsOnSurface.Append(anEntry);
      }
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

  // Clear vertex point representation bridge fields before re-populating.
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
  {
    BRepGraphInc::VertexEntity& aVtx = aStorage.Vertices.ChangeValue(i);
    aVtx.PointsOnCurve.Clear();
    aVtx.PointsOnSurface.Clear();
    aVtx.PointsOnPCurve.Clear();
  }
  // Re-copy TShape->NodeId and OriginalShapes.
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
