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
#include <BRepGraph_Reconstruct.hxx>
#include <BRepGraphInc_Reconstruct.hxx>

#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Shape(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: check unmodified originals (read-only map, no lock needed).
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef != nullptr && !aDef->IsModified)
  {
    const TopoDS_Shape* anOrig = myGraph->myData->myOriginalShapes.Seek(theNode);
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

  // Reconstruct: use incidence-table when unmodified, legacy for mutated nodes.
  // Mutations via MutView/BuilderView update legacy Def stores but do not yet
  // propagate to incidence storage, so modified nodes must use legacy reconstruct
  // until incidence becomes the sole mutable store (Steps 3-4 of migration plan).
  TopoDS_Shape aReconstructed;
  if (aDef != nullptr && aDef->IsModified)
    aReconstructed = BRepGraph_Reconstruct::Node(*myGraph, theNode);
  else
    aReconstructed = BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    if (!myGraph->myData->myCurrentShapes.IsBound(theNode))
    {
      myGraph->myData->myCurrentShapes.Bind(theNode, aReconstructed);
      myGraph->myData->myTShapeToDefId.Bind(aReconstructed.TShape().get(), theNode);
    }
  }
  return aReconstructed;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasOriginal(BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myOriginalShapes.IsBound(theNode);
}

//=================================================================================================

const TopoDS_Shape& BRepGraph::ShapesView::OriginalOf(BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = myGraph->myData->myOriginalShapes.Seek(theNode);
  if (aShape == nullptr)
    throw Standard_ProgramError("BRepGraph::ShapesView::OriginalOf() -- no original shape.");
  return *aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Reconstruct(BRepGraph_NodeId theRoot) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theRoot);
  if (aDef != nullptr && aDef->IsModified)
    return BRepGraph_Reconstruct::Node(*myGraph, theRoot);
  return BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theRoot);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFace(int theFaceDefIdx) const
{
  const BRepGraph_TopoNode::BaseDef* aDef =
    myGraph->TopoDef(BRepGraph_NodeId::Face(theFaceDefIdx));
  if (aDef != nullptr && aDef->IsModified)
    return BRepGraph_Reconstruct::Node(
      *myGraph, BRepGraph_NodeId::Face(theFaceDefIdx));
  BRepGraphInc_Reconstruct::Cache aCache;
  return BRepGraphInc_Reconstruct::FaceWithCache(
    myGraph->myData->myIncStorage, theFaceDefIdx, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFromUsage(BRepGraph_UsageId theRoot) const
{
  return BRepGraph_Reconstruct::Usage(*myGraph, theRoot);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::FindNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return BRepGraph_NodeId();

  const BRepGraph_NodeId* aNodeId = myGraph->myData->myTShapeToDefId.Seek(theShape.TShape().get());
  if (aNodeId == nullptr)
    return BRepGraph_NodeId();

  return *aNodeId;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return false;

  return myGraph->myData->myTShapeToDefId.IsBound(theShape.TShape().get());
}
