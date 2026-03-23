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

#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Reconstruct.hxx>

#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Shape(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: check unmodified originals — first from incidence storage, then mutation map.
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef != nullptr && !aDef->IsModified)
  {
    const TopoDS_Shape* anOrig = myGraph->myData->myIncStorage.OriginalShapes.Seek(theNode);
    if (anOrig != nullptr)
      return *anOrig;
    anOrig = myGraph->myData->myMutationOriginals.Seek(theNode);
    if (anOrig != nullptr)
      return *anOrig;
  }

  // Check mutable cache under shared lock.
  {
    std::shared_lock<std::shared_mutex> aReadLock(myGraph->myData->myCurrentShapesMutex);
    const TopoDS_Shape* aCached = myGraph->myData->myCurrentShapes.Seek(theNode);
    if (aCached != nullptr)
      return *aCached;
  }

  // Reconstruct from incidence storage (sole source of truth for all entities).
  TopoDS_Shape aReconstructed =
    BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);

  // For modified nodes, apply per-node location if stored.
  if (aDef != nullptr && aDef->IsModified && !aReconstructed.IsNull())
  {
    const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(theNode);
    if (aNodeLoc != nullptr && !aNodeLoc->IsIdentity())
      aReconstructed.Location(*aNodeLoc);
  }

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    if (!myGraph->myData->myCurrentShapes.IsBound(theNode))
    {
      myGraph->myData->myCurrentShapes.Bind(theNode, aReconstructed);
      myGraph->myData->myReconstructedTShapeToDefId.Bind(aReconstructed.TShape().get(), theNode);
    }
  }
  return aReconstructed;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasOriginal(BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myIncStorage.OriginalShapes.IsBound(theNode)
      || myGraph->myData->myMutationOriginals.IsBound(theNode);
}

//=================================================================================================

const TopoDS_Shape& BRepGraph::ShapesView::OriginalOf(BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = myGraph->myData->myIncStorage.OriginalShapes.Seek(theNode);
  if (aShape != nullptr)
    return *aShape;
  aShape = myGraph->myData->myMutationOriginals.Seek(theNode);
  if (aShape != nullptr)
    return *aShape;
  throw Standard_ProgramError("BRepGraph::ShapesView::OriginalOf() -- no original shape.");
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Reconstruct(BRepGraph_NodeId theRoot) const
{
  TopoDS_Shape aShape = BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theRoot);

  // Apply per-node location if stored (e.g. from Transform).
  if (!aShape.IsNull())
  {
    const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(theRoot);
    if (aNodeLoc != nullptr && !aNodeLoc->IsIdentity())
      aShape.Location(*aNodeLoc);
  }
  return aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFace(int theFaceDefIdx) const
{
  BRepGraphInc_Reconstruct::Cache aCache;
  return BRepGraphInc_Reconstruct::FaceWithCache(
    myGraph->myData->myIncStorage, theFaceDefIdx, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFromNode(BRepGraph_NodeId theNode) const
{
  TopoDS_Shape aShape = BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);
  if (aShape.IsNull() || !theNode.IsValid())
    return aShape;

  // Apply per-node location if stored.
  const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(theNode);
  if (aNodeLoc != nullptr && !aNodeLoc->IsIdentity())
    aShape.Location(*aNodeLoc);

  return aShape;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::FindNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return BRepGraph_NodeId();

  const TopoDS_TShape* aTShapeKey = theShape.TShape().get();
  // Check incidence storage first (Build-time shapes), then reconstructed cache.
  const BRepGraph_NodeId* aNodeId = myGraph->myData->myIncStorage.TShapeToNodeId.Seek(aTShapeKey);
  if (aNodeId != nullptr)
    return *aNodeId;
  aNodeId = myGraph->myData->myReconstructedTShapeToDefId.Seek(aTShapeKey);
  if (aNodeId != nullptr)
    return *aNodeId;
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::ShapesView::HasNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return false;

  const TopoDS_TShape* aTShapeKey = theShape.TShape().get();
  return myGraph->myData->myIncStorage.TShapeToNodeId.IsBound(aTShapeKey)
      || myGraph->myData->myReconstructedTShapeToDefId.IsBound(aTShapeKey);
}
