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

TopoDS_Shape BRepGraph::ShapesView::Shape(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: check unmodified originals in Storage.
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef != nullptr && !aDef->IsModified)
  {
    const TopoDS_Shape* anOrig = myGraph->myData->myIncStorage.FindOriginal(theNode);
    if (anOrig != nullptr)
      return *anOrig;
  }

  // Check mutable cache under shared lock.
  {
    std::shared_lock<std::shared_mutex> aReadLock(myGraph->myData->myCurrentShapesMutex);
    const TopoDS_Shape*                 aCached = myGraph->myData->myCurrentShapes.Seek(theNode);
    if (aCached != nullptr)
      return *aCached;
  }

  // Reconstruct from incidence storage.
  TopoDS_Shape aReconstructed =
    BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    if (!myGraph->myData->myCurrentShapes.IsBound(theNode))
    {
      myGraph->myData->myCurrentShapes.Bind(theNode, aReconstructed);
      myGraph->myData->myIncStorage.BindTShapeToNode(aReconstructed.TShape().get(), theNode);
    }
  }
  return aReconstructed;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasOriginal(const BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myIncStorage.HasOriginal(theNode);
}

//=================================================================================================

const TopoDS_Shape& BRepGraph::ShapesView::OriginalOf(const BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = myGraph->myData->myIncStorage.FindOriginal(theNode);
  if (aShape == nullptr)
    throw Standard_ProgramError("BRepGraph::ShapesView::OriginalOf() - no original shape.");
  return *aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Reconstruct(const BRepGraph_NodeId theRoot) const
{
  TopoDS_Shape aShape = BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theRoot);

  return aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFace(const BRepGraph_FaceId theFace) const
{
  BRepGraphInc_Reconstruct::Cache aCache;
  return BRepGraphInc_Reconstruct::FaceWithCache(myGraph->myData->myIncStorage,
                                                 theFace.Index,
                                                 aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFromNode(const BRepGraph_NodeId theNode) const
{
  return BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::FindNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return BRepGraph_NodeId();

  const BRepGraph_NodeId* aNodeId =
    myGraph->myData->myIncStorage.FindNodeByTShape(theShape.TShape().get());
  if (aNodeId != nullptr)
    return *aNodeId;
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::ShapesView::HasNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return false;

  return myGraph->myData->myIncStorage.HasTShapeBinding(theShape.TShape().get());
}
