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

  // Reconstruct from incidence storage (sole source of truth for all entities).
  TopoDS_Shape aReconstructed =
    BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theNode);

  // For modified nodes, apply usage-specific orientation and location
  // (e.g. from Transform). Transitional: will be removed when orientation/location
  // data migrates from Usage structs to incidence entities.
  // Uses first usage only — Shape() returns a single representative shape.
  // Multi-usage scenarios (shared sub-shapes) use ReconstructFromUsage() with a specific UsageId.
  if (aDef != nullptr && aDef->IsModified && !aReconstructed.IsNull() && !aDef->Usages.IsEmpty())
  {
    const BRepGraph_UsageId& aFirstUsage = aDef->Usages.Value(0);
    auto applyFirstUsageLoc = [&](const auto& theUsageVec) {
      if (aFirstUsage.Index >= 0 && aFirstUsage.Index < theUsageVec.Length())
      {
        const auto& aUsage = theUsageVec.Value(aFirstUsage.Index);
        aReconstructed.Orientation(aUsage.Orientation);
        if (!aUsage.GlobalLocation.IsIdentity())
          aReconstructed.Location(aUsage.GlobalLocation);
      }
    };
    switch (theNode.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Solid:     applyFirstUsageLoc(myGraph->myData->mySolids.Usages); break;
      case BRepGraph_NodeId::Kind::Shell:     applyFirstUsageLoc(myGraph->myData->myShells.Usages); break;
      case BRepGraph_NodeId::Kind::Face:      applyFirstUsageLoc(myGraph->myData->myFaces.Usages); break;
      case BRepGraph_NodeId::Kind::Wire:      applyFirstUsageLoc(myGraph->myData->myWires.Usages); break;
      case BRepGraph_NodeId::Kind::Edge:      applyFirstUsageLoc(myGraph->myData->myEdges.Usages); break;
      case BRepGraph_NodeId::Kind::Vertex:    applyFirstUsageLoc(myGraph->myData->myVertices.Usages); break;
      case BRepGraph_NodeId::Kind::Compound:  applyFirstUsageLoc(myGraph->myData->myCompounds.Usages); break;
      case BRepGraph_NodeId::Kind::CompSolid: applyFirstUsageLoc(myGraph->myData->myCompSolids.Usages); break;
      default: break;
    }
  }

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
  return BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, theRoot);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFace(int theFaceDefIdx) const
{
  BRepGraphInc_Reconstruct::Cache aCache;
  return BRepGraphInc_Reconstruct::FaceWithCache(
    myGraph->myData->myIncStorage, theFaceDefIdx, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::ReconstructFromUsage(BRepGraph_UsageId theRoot) const
{
  // Reconstruct from incidence, then apply usage-specific orientation and location.
  const BRepGraph_NodeId aDefId = myGraph->DefOf(theRoot);
  TopoDS_Shape aShape = BRepGraphInc_Reconstruct::Node(myGraph->myData->myIncStorage, aDefId);
  if (aShape.IsNull() || !theRoot.IsValid())
    return aShape;

  // Apply usage orientation and location.
  auto applyUsage = [&](const auto& theUsageVec) {
    if (theRoot.Index >= 0 && theRoot.Index < theUsageVec.Length())
    {
      const auto& aUsage = theUsageVec.Value(theRoot.Index);
      aShape.Orientation(aUsage.Orientation);
      if (!aUsage.GlobalLocation.IsIdentity())
        aShape.Location(aUsage.GlobalLocation);
    }
  };

  switch (theRoot.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     applyUsage(myGraph->myData->mySolids.Usages); break;
    case BRepGraph_NodeId::Kind::Shell:     applyUsage(myGraph->myData->myShells.Usages); break;
    case BRepGraph_NodeId::Kind::Face:      applyUsage(myGraph->myData->myFaces.Usages); break;
    case BRepGraph_NodeId::Kind::Wire:      applyUsage(myGraph->myData->myWires.Usages); break;
    case BRepGraph_NodeId::Kind::Edge:      applyUsage(myGraph->myData->myEdges.Usages); break;
    case BRepGraph_NodeId::Kind::Vertex:    applyUsage(myGraph->myData->myVertices.Usages); break;
    case BRepGraph_NodeId::Kind::Compound:  applyUsage(myGraph->myData->myCompounds.Usages); break;
    case BRepGraph_NodeId::Kind::CompSolid: applyUsage(myGraph->myData->myCompSolids.Usages); break;
    default: break;
  }
  return aShape;
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
