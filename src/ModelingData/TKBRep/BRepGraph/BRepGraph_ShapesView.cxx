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

  // Reconstruct outside the lock to avoid holding it during expensive computation.
  TopoDS_Shape aReconstructed = BRepGraph_Reconstruct::Node(*myGraph, theNode);

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    if (!myGraph->myData->myCurrentShapes.IsBound(theNode))
      myGraph->myData->myCurrentShapes.Bind(theNode, aReconstructed);
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
  return BRepGraph_Reconstruct::Node(*myGraph, theRoot);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFace(int theFaceDefIdx) const
{
  return BRepGraph_Reconstruct::Node(
    *myGraph, BRepGraph_NodeId(BRepGraph_NodeKind::Face, theFaceDefIdx));
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFromUsage(BRepGraph_UsageId theRoot) const
{
  return BRepGraph_Reconstruct::Usage(*myGraph, theRoot);
}
