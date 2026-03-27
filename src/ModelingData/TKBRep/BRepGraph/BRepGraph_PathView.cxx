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

#include <BRepGraph_PathView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_Map.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>

// No anonymous namespace helpers needed: entity RefId vectors provide O(1) lookup.

//=================================================================================================

bool BRepGraph::PathView::is1to1(const BRepGraph_NodeId::Kind theKind)
{
  return theKind == BRepGraph_NodeId::Kind::Occurrence || theKind == BRepGraph_NodeId::Kind::CoEdge;
  // Product(part) is also 1:1 but requires runtime check (ShapeRootId.IsValid()).
}

//=================================================================================================

TopLoc_Location BRepGraph::PathView::transitLocation(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::Occurrence && theNode.Index >= 0
      && theNode.Index < aStorage.NbOccurrences())
  {
    return aStorage.Occurrence(BRepGraph_OccurrenceId(theNode.Index)).Placement;
  }
  // CoEdge -> Edge: no location.
  return TopLoc_Location();
}

//=================================================================================================

TopAbs_Orientation BRepGraph::PathView::transitOrientation(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::CoEdge && theNode.Index >= 0
      && theNode.Index < aStorage.NbCoEdges())
  {
    return aStorage.CoEdge(BRepGraph_CoEdgeId(theNode.Index)).Sense;
  }
  return TopAbs_FORWARD;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PathView::transitChild(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::Occurrence && theNode.Index >= 0
      && theNode.Index < aStorage.NbOccurrences())
  {
    return aStorage.Occurrence(BRepGraph_OccurrenceId(theNode.Index)).ProductDefId;
  }
  if (theNode.NodeKind == Kind::CoEdge && theNode.Index >= 0
      && theNode.Index < aStorage.NbCoEdges())
  {
    return aStorage.CoEdge(BRepGraph_CoEdgeId(theNode.Index)).EdgeDefId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph::PathView::stepLocation(const BRepGraph_NodeId theParent,
                                                  const int              theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theRefIdx < 0)
  {
    // Product(part) -> ShapeRoot: RootLocation.
    if (theParent.NodeKind == Kind::Product && theParent.Index >= 0
        && theParent.Index < aStorage.NbProducts())
      return aStorage.Product(BRepGraph_ProductId(theParent.Index)).RootLocation;
    return TopLoc_Location();
  }

  switch (theParent.NodeKind)
  {
    case Kind::Product:
      return TopLoc_Location(); // Assembly -> Occurrence: identity.
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx < aComp.ChildRefIds.Length())
          return aStorage.ChildRefEntry(aComp.ChildRefIds.Value(theRefIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx < aCS.SolidRefIds.Length())
          return aStorage.SolidRefEntry(aCS.SolidRefIds.Value(theRefIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx < aSolid.ShellRefIds.Length())
          return aStorage.ShellRefEntry(aSolid.ShellRefIds.Value(theRefIdx)).LocalLocation;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aSolid.FreeChildRefIds.Value(aFreeIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx < aShell.FaceRefIds.Length())
          return aStorage.FaceRefEntry(aShell.FaceRefIds.Value(theRefIdx)).LocalLocation;
        const int aFreeIdx = theRefIdx - aShell.FaceRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aShell.FreeChildRefIds.Value(aFreeIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace =
          aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx < aFace.WireRefIds.Length())
          return aStorage.WireRefEntry(aFace.WireRefIds.Value(theRefIdx)).LocalLocation;
        const int aVertexIdx = theRefIdx - aFace.WireRefIds.Length();
        if (aVertexIdx >= 0 && aVertexIdx < aFace.VertexRefIds.Length())
          return aStorage.VertexRefEntry(aFace.VertexRefIds.Value(aVertexIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Wire: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire =
          aStorage.Wire(BRepGraph_WireId(theParent.Index));
        if (theRefIdx < aWire.CoEdgeRefIds.Length())
          return aStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(theRefIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        // Ordinal: 0=Start, 1=End, 2+=Internal
        if (theRefIdx == 0 && anEdge.StartVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.StartVertexRefId).LocalLocation;
        if (theRefIdx == 1 && anEdge.EndVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.EndVertexRefId).LocalLocation;
        const int aInternalIdx = theRefIdx - 2;
        if (aInternalIdx >= 0 && aInternalIdx < anEdge.InternalVertexRefIds.Length())
          return aStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(aInternalIdx)).LocalLocation;
      }
      return TopLoc_Location();
    }
    default:
      return TopLoc_Location();
  }
}

//=================================================================================================

TopAbs_Orientation BRepGraph::PathView::stepOrientation(const BRepGraph_NodeId theParent,
                                                        const int              theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theRefIdx < 0)
  {
    if (theParent.NodeKind == Kind::Product && theParent.Index >= 0
        && theParent.Index < aStorage.NbProducts())
      return aStorage.Product(BRepGraph_ProductId(theParent.Index)).RootOrientation;
    return TopAbs_FORWARD;
  }

  switch (theParent.NodeKind)
  {
    case Kind::Product:
      return TopAbs_FORWARD;
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx < aComp.ChildRefIds.Length())
          return aStorage.ChildRefEntry(aComp.ChildRefIds.Value(theRefIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx < aCS.SolidRefIds.Length())
          return aStorage.SolidRefEntry(aCS.SolidRefIds.Value(theRefIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx < aSolid.ShellRefIds.Length())
          return aStorage.ShellRefEntry(aSolid.ShellRefIds.Value(theRefIdx)).Orientation;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aSolid.FreeChildRefIds.Value(aFreeIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx < aShell.FaceRefIds.Length())
          return aStorage.FaceRefEntry(aShell.FaceRefIds.Value(theRefIdx)).Orientation;
        const int aFreeIdx = theRefIdx - aShell.FaceRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aShell.FreeChildRefIds.Value(aFreeIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace =
          aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx < aFace.WireRefIds.Length())
          return aStorage.WireRefEntry(aFace.WireRefIds.Value(theRefIdx)).Orientation;
        const int aVertexIdx = theRefIdx - aFace.WireRefIds.Length();
        if (aVertexIdx >= 0 && aVertexIdx < aFace.VertexRefIds.Length())
          return aStorage.VertexRefEntry(aFace.VertexRefIds.Value(aVertexIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Wire:
      return TopAbs_FORWARD; // CoEdgeRef has no orientation; Sense is on CoEdgeEntity.
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        if (theRefIdx == 0 && anEdge.StartVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.StartVertexRefId).Orientation;
        if (theRefIdx == 1 && anEdge.EndVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.EndVertexRefId).Orientation;
        const int aInternalIdx = theRefIdx - 2;
        if (aInternalIdx >= 0 && aInternalIdx < anEdge.InternalVertexRefIds.Length())
          return aStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(aInternalIdx)).Orientation;
      }
      return TopAbs_FORWARD;
    }
    default:
      return TopAbs_FORWARD;
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PathView::resolveChild(const BRepGraph_NodeId theParent,
                                                   const int              theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
      {
        const BRepGraphInc::ProductEntity& aProd =
          aStorage.Product(BRepGraph_ProductId(theParent.Index));
        if (theRefIdx < 0 && aProd.ShapeRootId.IsValid())
          return aProd.ShapeRootId; // Part -> shape root
        if (theRefIdx >= 0 && theRefIdx < aProd.OccurrenceRefIds.Length())
          return aStorage.OccurrenceRefEntry(aProd.OccurrenceRefIds.Value(theRefIdx)).OccurrenceDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefIds.Length())
          return aStorage.ChildRefEntry(aComp.ChildRefIds.Value(theRefIdx)).ChildDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefIds.Length())
          return aStorage.SolidRefEntry(aCS.SolidRefIds.Value(theRefIdx)).SolidDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aSolid.ShellRefIds.Length())
          return aStorage.ShellRefEntry(aSolid.ShellRefIds.Value(theRefIdx)).ShellDefId;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aSolid.FreeChildRefIds.Value(aFreeIdx)).ChildDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aShell.FaceRefIds.Length())
          return aStorage.FaceRefEntry(aShell.FaceRefIds.Value(theRefIdx)).FaceDefId;
        const int aFreeIdx = theRefIdx - aShell.FaceRefIds.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefIds.Length())
          return aStorage.ChildRefEntry(aShell.FreeChildRefIds.Value(aFreeIdx)).ChildDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace =
          aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aFace.WireRefIds.Length())
          return aStorage.WireRefEntry(aFace.WireRefIds.Value(theRefIdx)).WireDefId;
        const int aVertexIdx = theRefIdx - aFace.WireRefIds.Length();
        if (aVertexIdx >= 0 && aVertexIdx < aFace.VertexRefIds.Length())
          return aStorage.VertexRefEntry(aFace.VertexRefIds.Value(aVertexIdx)).VertexDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Wire: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire =
          aStorage.Wire(BRepGraph_WireId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aWire.CoEdgeRefIds.Length())
          return aStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(theRefIdx)).CoEdgeDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        if (theRefIdx == 0 && anEdge.StartVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.StartVertexRefId).VertexDefId;
        if (theRefIdx == 1 && anEdge.EndVertexRefId.IsValid())
          return aStorage.VertexRefEntry(anEdge.EndVertexRefId).VertexDefId;
        const int aInternalIdx = theRefIdx - 2;
        if (aInternalIdx >= 0 && aInternalIdx < anEdge.InternalVertexRefIds.Length())
          return aStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(aInternalIdx)).VertexDefId;
      }
      return BRepGraph_NodeId();
    }
    default:
      return BRepGraph_NodeId();
  }
}

//=================================================================================================

BRepGraph::PathView::WalkResult BRepGraph::PathView::walkToLevel(
  const BRepGraph_TopologyPath& thePath,
  const int                     theMaxSteps) const
{
  WalkResult aResult;
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  Standard_ASSERT_VOID(theMaxSteps >= 0 && theMaxSteps <= thePath.Depth(),
                       "theMaxSteps out of range");
  if (!thePath.IsValid())
    return aResult;

  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < theMaxSteps)
  {
    // Handle 1:1 transitions (Occurrence->Product, CoEdge->Edge).
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aResult.Location       = aResult.Location * transitLocation(aCurrent);
      aResult.Orientation    = TopAbs::Compose(aResult.Orientation, transitOrientation(aCurrent));
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      continue;
    }
    // Product(part) is also 1:1 but needs runtime check.
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
      if (aCurrent.Index >= 0 && aCurrent.Index < aStorage.NbProducts()
          && aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId.IsValid())
      {
        aResult.Location =
          aResult.Location * aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).RootLocation;
        aResult.Orientation =
          TopAbs::Compose(aResult.Orientation,
                          aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).RootOrientation);
        aCurrent = aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
        continue;
      }
    }
    // Normal step: consume one RefIdx.
    const int aRefIdx   = thePath.stepAt(aStepIdx);
    aResult.Location    = aResult.Location * stepLocation(aCurrent, aRefIdx);
    aResult.Orientation = TopAbs::Compose(aResult.Orientation, stepOrientation(aCurrent, aRefIdx));
    aCurrent            = resolveChild(aCurrent, aRefIdx);
    ++aStepIdx;
  }

  // Trailing 1:1 transitions (including Product(part) -> ShapeRoot).
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  while (aCurrent.IsValid()
         && (is1to1(aCurrent.NodeKind)
             || (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
                 && aCurrent.Index < aStorage.NbProducts()
                 && aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId.IsValid())))
  {
    BRepGraph_NodeId aPrev = aCurrent;
    if (is1to1(aCurrent.NodeKind))
    {
      aResult.Location    = aResult.Location * transitLocation(aCurrent);
      aResult.Orientation = TopAbs::Compose(aResult.Orientation, transitOrientation(aCurrent));
      aCurrent            = transitChild(aCurrent);
    }
    else
    {
      aResult.Location =
        aResult.Location * aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).RootLocation;
      aResult.Orientation =
        TopAbs::Compose(aResult.Orientation,
                        aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).RootOrientation);
      aCurrent = aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
    }
    if (aCurrent == aPrev)
      break;
  }

  aResult.LeafNode = aCurrent;
  return aResult;
}

//=================================================================================================

TopLoc_Location BRepGraph::PathView::GlobalLocation(const BRepGraph_TopologyPath& thePath) const
{
  return walkToLevel(thePath, thePath.Depth()).Location;
}

//=================================================================================================

TopLoc_Location BRepGraph::PathView::LocationAt(const BRepGraph_TopologyPath& thePath,
                                                const int                     theLevel) const
{
  return walkToLevel(thePath, theLevel + 1).Location;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PathView::LeafNode(const BRepGraph_TopologyPath& thePath) const
{
  return walkToLevel(thePath, thePath.Depth()).LeafNode;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PathView::NodeAt(const BRepGraph_TopologyPath& thePath,
                                             const int                     theLevel) const
{
  return walkToLevel(thePath, theLevel + 1).LeafNode;
}

//=================================================================================================

TopAbs_Orientation BRepGraph::PathView::GlobalOrientation(
  const BRepGraph_TopologyPath& thePath) const
{
  return walkToLevel(thePath, thePath.Depth()).Orientation;
}

//=================================================================================================

TopAbs_Orientation BRepGraph::PathView::OrientationAt(const BRepGraph_TopologyPath& thePath,
                                                      const int                     theLevel) const
{
  return walkToLevel(thePath, theLevel + 1).Orientation;
}

//=================================================================================================

int BRepGraph::PathView::FindLevel(const BRepGraph_TopologyPath& thePath,
                                   BRepGraph_NodeId::Kind        theKind) const
{
  if (!thePath.IsValid())
    return -1;

  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < thePath.Depth())
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
        && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
        && myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index))
             .ShapeRootId.IsValid())
    {
      aCurrent =
        myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
      continue;
    }
    const int aRefIdx = thePath.stepAt(aStepIdx);
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    if (aCurrent.IsValid() && aCurrent.NodeKind == theKind)
      return aStepIdx;
    ++aStepIdx;
  }
  return -1;
}

//=================================================================================================

BRepGraph_TopologyPath BRepGraph::PathView::Truncated(const BRepGraph_TopologyPath& thePath,
                                                      const int                     theLevel) const
{
  return thePath.Truncated(theLevel);
}

//=================================================================================================

bool BRepGraph::PathView::PathContains(const BRepGraph_TopologyPath& thePath,
                                       const BRepGraph_NodeId        theNode) const
{
  if (!thePath.IsValid() || !theNode.IsValid())
    return false;

  if (thePath.Root() == theNode)
    return true;

  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < thePath.Depth())
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break; // no progress - malformed graph cycle
      if (aCurrent == theNode)
        return true;
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
        && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
        && myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index))
             .ShapeRootId.IsValid())
    {
      aCurrent =
        myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
      if (aCurrent == theNode)
        return true;
      continue;
    }
    const int aRefIdx = thePath.stepAt(aStepIdx);
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    if (aCurrent == theNode)
      return true;
    ++aStepIdx;
  }
  return false;
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::FilterByInclude(
  const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
  const BRepGraph_NodeId                            theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aResult;
  for (int i = 0; i < thePaths.Length(); ++i)
  {
    if (PathContains(thePaths.Value(i), theNode))
      aResult.Append(thePaths.Value(i));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::FilterByExclude(
  const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
  const BRepGraph_NodeId                            theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aResult;
  for (int i = 0; i < thePaths.Length(); ++i)
  {
    if (!PathContains(thePaths.Value(i), theNode))
      aResult.Append(thePaths.Value(i));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::PathsTo(
  const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return NCollection_Vector<BRepGraph_TopologyPath>();

  return reverseWalkPaths(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::PathsFromTo(
  const BRepGraph_NodeId theRoot,
  const BRepGraph_NodeId theLeaf) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aResult;
  if (!theRoot.IsValid() || !theLeaf.IsValid())
    return aResult;

  if (theRoot == theLeaf)
  {
    aResult.Append(BRepGraph_TopologyPath::FromRoot(theRoot));
    return aResult;
  }

  NCollection_Vector<BRepGraph_TopologyPath> aPaths = reverseWalkPaths(theLeaf);
  for (int i = 0; i < aPaths.Length(); ++i)
  {
    if (aPaths.Value(i).Root() == theRoot)
      aResult.Append(aPaths.Value(i));
  }
  return aResult;
}

//=================================================================================================

BRepGraph::PathView::OccurrenceEntry BRepGraph::PathView::buildEntry(
  const BRepGraph_TopologyPath& thePath) const
{
  const WalkResult aWalk = walkToLevel(thePath, thePath.Depth());
  OccurrenceEntry  anEntry;
  anEntry.Path        = thePath;
  anEntry.Location    = aWalk.Location;
  anEntry.Orientation = aWalk.Orientation;
  return anEntry;
}

//=================================================================================================

int BRepGraph::PathView::findShellRefIdx(const BRepGraphInc_Storage& theStorage,
                                         const BRepGraph_SolidId     theSolidId,
                                         const int                   theShellIdx)
{
  const BRepGraphInc::SolidEntity& aSolid = theStorage.Solid(theSolidId);
  for (int i = 0; i < aSolid.ShellRefIds.Length(); ++i)
  {
    const BRepGraphInc::ShellRefEntry& aRef = theStorage.ShellRefEntry(aSolid.ShellRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.ShellDefId.Index == theShellIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findFaceRefIdx(const BRepGraphInc_Storage& theStorage,
                                        const BRepGraph_ShellId     theShellId,
                                        const int                   theFaceIdx)
{
  const BRepGraphInc::ShellEntity& aShell = theStorage.Shell(theShellId);
  for (int i = 0; i < aShell.FaceRefIds.Length(); ++i)
  {
    const BRepGraphInc::FaceRefEntry& aRef = theStorage.FaceRefEntry(aShell.FaceRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.FaceDefId.Index == theFaceIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findWireRefIdx(const BRepGraphInc_Storage& theStorage,
                                        const BRepGraph_FaceId      theFaceId,
                                        const int                   theWireIdx)
{
  const BRepGraphInc::FaceEntity& aFace = theStorage.Face(theFaceId);
  for (int i = 0; i < aFace.WireRefIds.Length(); ++i)
  {
    const BRepGraphInc::WireRefEntry& aRef = theStorage.WireRefEntry(aFace.WireRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.WireDefId.Index == theWireIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findCoEdgeRefIdx(const BRepGraphInc_Storage& theStorage,
                                          const BRepGraph_WireId      theWireId,
                                          const int                   theCoEdgeIdx)
{
  const BRepGraphInc::WireEntity& aWire = theStorage.Wire(theWireId);
  for (int i = 0; i < aWire.CoEdgeRefIds.Length(); ++i)
  {
    const BRepGraphInc::CoEdgeRefEntry& aRef = theStorage.CoEdgeRefEntry(aWire.CoEdgeRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.CoEdgeDefId.Index == theCoEdgeIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findChildRefIdx(const BRepGraphInc_Storage& theStorage,
                                         const BRepGraph_CompoundId  theCompoundId,
                                         BRepGraph_NodeId::Kind      theKind,
                                         const int                   theChildIdx)
{
  const BRepGraphInc::CompoundEntity& aComp = theStorage.Compound(theCompoundId);
  for (int i = 0; i < aComp.ChildRefIds.Length(); ++i)
  {
    const BRepGraphInc::ChildRefEntry& aRef = theStorage.ChildRefEntry(aComp.ChildRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.ChildDefId.NodeKind == theKind && aRef.ChildDefId.Index == theChildIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findSolidRefIdx(const BRepGraphInc_Storage& theStorage,
                                         const BRepGraph_CompSolidId theCompSolidId,
                                         const int                   theSolidIdx)
{
  const BRepGraphInc::CompSolidEntity& aCS = theStorage.CompSolid(theCompSolidId);
  for (int i = 0; i < aCS.SolidRefIds.Length(); ++i)
  {
    const BRepGraphInc::SolidRefEntry& aRef = theStorage.SolidRefEntry(aCS.SolidRefIds.Value(i));
    if (!aRef.IsRemoved && aRef.SolidDefId.Index == theSolidIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> BRepGraph::PathView::NodeLocations(
  const BRepGraph_NodeId theNode) const
{
  return reverseWalkEntries(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::reverseWalkPaths(
  const BRepGraph_NodeId theNode) const
{
  // Conservative budget: worst-case path length cannot exceed total entity count.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  const int aBudget = aStorage.NbCompounds() + aStorage.NbCompSolids() + aStorage.NbSolids()
                      + aStorage.NbShells() + aStorage.NbFaces() + aStorage.NbWires()
                      + aStorage.NbEdges() + aStorage.NbVertices() + aStorage.NbCoEdges();
  return reverseWalkPaths(theNode, aBudget);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::reverseWalkPaths(
  const BRepGraph_NodeId theNode,
  const int              theDepthBudget) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aResult;
  if (!theNode.IsValid() || theDepthBudget <= 0)
    return aResult;

  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  switch (theNode.NodeKind)
  {
    case Kind::Vertex:
      reverseWalkFromVertex(theNode.Index, aResult, theDepthBudget - 1);
      break;
    case Kind::Edge:
      reverseWalkFromEdge(theNode.Index, aResult, theDepthBudget - 1);
      break;
    case Kind::Wire:
      reverseWalkFromWire(theNode.Index, aResult, theDepthBudget - 1);
      break;
    case Kind::Face:
      reverseWalkFromFace(theNode.Index, aResult, theDepthBudget - 1);
      break;
    case Kind::Shell:
      reverseWalkFromShell(theNode.Index, aResult, theDepthBudget - 1);
      break;
    case Kind::Solid: {
      const BRepGraphInc_ReverseIndex& aRevIdx    = aStorage.ReverseIndex();
      bool                             aHasParent = false;
      // Walk up to CompSolid parents.
      const NCollection_Vector<BRepGraph_CompSolidId>* aCompSolids =
        aRevIdx.CompSolidsOfSolid(BRepGraph_SolidId(theNode.Index));
      if (aCompSolids != nullptr)
      {
        for (int cs = 0; cs < aCompSolids->Length(); ++cs)
        {
          const int aCSIdx = aCompSolids->Value(cs).Index;
          if (aCSIdx < 0 || aCSIdx >= aStorage.NbCompSolids())
            continue;
          const BRepGraphInc::CompSolidEntity& aCS =
            aStorage.CompSolid(BRepGraph_CompSolidId(aCSIdx));
          if (aCS.IsRemoved)
            continue;
          const int aRefIdx = findSolidRefIdx(aStorage, BRepGraph_CompSolidId(aCSIdx), theNode.Index);
          if (aRefIdx < 0)
            continue;
          aHasParent = true;
          // Recurse: CompSolid may itself be inside a Compound.
          NCollection_Vector<BRepGraph_TopologyPath> aParentPaths =
            reverseWalkPaths(BRepGraph_NodeId::CompSolid(aCSIdx), theDepthBudget - 1);
          for (int p = 0; p < aParentPaths.Length(); ++p)
          {
            BRepGraph_TopologyPath aPath = aParentPaths.Value(p);
            aPath.pushStep(aRefIdx);
            aResult.Append(aPath);
          }
        }
      }
      // Walk up to Compound parents.
      const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
        aRevIdx.CompoundsOfSolid(BRepGraph_SolidId(theNode.Index));
      if (aCompounds != nullptr)
      {
        for (int c = 0; c < aCompounds->Length(); ++c)
        {
          const int aCompIdx = aCompounds->Value(c).Index;
          if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aComp =
            aStorage.Compound(BRepGraph_CompoundId(aCompIdx));
          if (aComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aStorage,
                                              BRepGraph_CompoundId(aCompIdx),
                                              Kind::Solid,
                                              theNode.Index);
          if (aRefIdx < 0)
            continue;
          aHasParent = true;
          NCollection_Vector<BRepGraph_TopologyPath> aParentPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx), theDepthBudget - 1);
          for (int p = 0; p < aParentPaths.Length(); ++p)
          {
            BRepGraph_TopologyPath aPath = aParentPaths.Value(p);
            aPath.pushStep(aRefIdx);
            aResult.Append(aPath);
          }
        }
      }
      if (!aHasParent)
        aResult.Append(BRepGraph_TopologyPath(theNode));
      break;
    }
    case Kind::Compound: {
      const BRepGraphInc_ReverseIndex&                aRevIdx = aStorage.ReverseIndex();
      const NCollection_Vector<BRepGraph_CompoundId>* aParents =
        aRevIdx.CompoundsOfCompound(BRepGraph_CompoundId(theNode.Index));
      if (aParents != nullptr && !aParents->IsEmpty())
      {
        for (int p = 0; p < aParents->Length(); ++p)
        {
          const int aParentIdx = aParents->Value(p).Index;
          if (aParentIdx < 0 || aParentIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aParentComp = aStorage.Compound(aParents->Value(p));
          if (aParentComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aStorage,
                                              BRepGraph_CompoundId(aParentIdx),
                                              Kind::Compound,
                                              theNode.Index);
          if (aRefIdx < 0)
            continue;
          NCollection_Vector<BRepGraph_TopologyPath> aGrandPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aParentIdx), theDepthBudget - 1);
          for (int g = 0; g < aGrandPaths.Length(); ++g)
          {
            BRepGraph_TopologyPath aPath = aGrandPaths.Value(g);
            aPath.pushStep(aRefIdx);
            aResult.Append(aPath);
          }
        }
      }
      else
      {
        aResult.Append(BRepGraph_TopologyPath(theNode));
      }
      break;
    }
    case Kind::CompSolid: {
      const BRepGraphInc_ReverseIndex&                aRevIdx = aStorage.ReverseIndex();
      const NCollection_Vector<BRepGraph_CompoundId>* aParents =
        aRevIdx.CompoundsOfCompSolid(BRepGraph_CompSolidId(theNode.Index));
      if (aParents != nullptr && !aParents->IsEmpty())
      {
        for (int p = 0; p < aParents->Length(); ++p)
        {
          const int aCompIdx = aParents->Value(p).Index;
          if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aParents->Value(p));
          if (aComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aStorage,
                                              BRepGraph_CompoundId(aCompIdx),
                                              Kind::CompSolid,
                                              theNode.Index);
          if (aRefIdx < 0)
            continue;
          NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx), theDepthBudget - 1);
          for (int g = 0; g < aCompPaths.Length(); ++g)
          {
            BRepGraph_TopologyPath aPath = aCompPaths.Value(g);
            aPath.pushStep(aRefIdx);
            aResult.Append(aPath);
          }
        }
      }
      else
      {
        aResult.Append(BRepGraph_TopologyPath(theNode));
      }
      break;
    }
    default:
      break;
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> BRepGraph::PathView::reverseWalkEntries(
  const BRepGraph_NodeId theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aPaths = reverseWalkPaths(theNode);
  NCollection_Vector<OccurrenceEntry>        aResult;
  for (int i = 0; i < aPaths.Length(); ++i)
    aResult.Append(buildEntry(aPaths.Value(i)));
  return aResult;
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromVertex(
  const int                                   theVertexIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult,
  const int                                   theDepthBudget) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theVertexIdx < 0 || theVertexIdx >= aStorage.NbVertices())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Vertex -> Edges (via reverse index)
  const NCollection_Vector<BRepGraph_EdgeId>* aEdges =
    aRevIdx.EdgesOfVertex(BRepGraph_VertexId(theVertexIdx));
  if (aEdges == nullptr)
    return;

  for (int e = 0; e < aEdges->Length(); ++e)
  {
    const int                       anEdgeIdx = aEdges->Value(e).Index;
    const BRepGraphInc::EdgeEntity& anEdge    = aStorage.Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.IsRemoved)
      continue;

    // Determine VertexSel: 0=Start, 1=End, 2+=Internal
    int aVertexRefIdx = -1;
    if (anEdge.StartVertexRefId.IsValid()
        && aStorage.VertexRefEntry(anEdge.StartVertexRefId).VertexDefId.Index == theVertexIdx)
      aVertexRefIdx = 0;
    else if (anEdge.EndVertexRefId.IsValid()
             && aStorage.VertexRefEntry(anEdge.EndVertexRefId).VertexDefId.Index == theVertexIdx)
      aVertexRefIdx = 1;
    else
    {
      for (int iv = 0; iv < anEdge.InternalVertexRefIds.Length(); ++iv)
      {
        if (aStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(iv)).VertexDefId.Index
            == theVertexIdx)
        {
          aVertexRefIdx = 2 + iv;
          break;
        }
      }
    }
    if (aVertexRefIdx < 0)
      continue;

    // Get all paths to the edge, then append vertex step.
    NCollection_Vector<BRepGraph_TopologyPath> aEdgePaths;
    reverseWalkFromEdge(anEdgeIdx, aEdgePaths, theDepthBudget - 1);
    for (int i = 0; i < aEdgePaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath = aEdgePaths.Value(i);
      aPath.pushStep(aVertexRefIdx);
      theResult.Append(aPath);
    }
  }
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromEdge(const int theEdgeIdx,
                                               NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                                               const int theDepthBudget) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeIdx < 0 || theEdgeIdx >= aStorage.NbEdges())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Edge -> CoEdges (via reverse index)
  const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdges =
    aRevIdx.CoEdgesOfEdge(BRepGraph_EdgeId(theEdgeIdx));
  if (aCoEdges == nullptr)
    return;

  for (int ce = 0; ce < aCoEdges->Length(); ++ce)
  {
    const BRepGraph_CoEdgeId          aCoEdgeId = aCoEdges->Value(ce);
    const BRepGraphInc::CoEdgeEntity& aCoEdge   = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.IsRemoved)
      continue;

    // CoEdge -> Wire: direct lookup via WiresOfCoEdge index.
    const NCollection_Vector<BRepGraph_WireId>* aWires = aRevIdx.WiresOfCoEdge(aCoEdgeId);
    if (aWires == nullptr)
      continue;

    for (int w = 0; w < aWires->Length(); ++w)
    {
      const int                       aWireIdx = aWires->Value(w).Index;
      const BRepGraphInc::WireEntity& aWire    = aStorage.Wire(BRepGraph_WireId(aWireIdx));
      if (aWire.IsRemoved)
        continue;

      const int aCoEdgeRefIdx = findCoEdgeRefIdx(aStorage, BRepGraph_WireId(aWireIdx), aCoEdgeId.Index);
      if (aCoEdgeRefIdx < 0)
        continue;

      // Get all paths to the wire, then append coedge step.
      // (CoEdge->Edge is 1:1, no step for it in the path)
      NCollection_Vector<BRepGraph_TopologyPath> aWirePaths;
      reverseWalkFromWire(aWireIdx, aWirePaths, theDepthBudget - 1);
      for (int i = 0; i < aWirePaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aWirePaths.Value(i);
        aPath.pushStep(aCoEdgeRefIdx); // Wire -> CoEdge (CoEdge->Edge is 1:1)
        theResult.Append(aPath);
      }
    }
  }
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromWire(const int theWireIdx,
                                               NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                                               const int theDepthBudget) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theWireIdx < 0 || theWireIdx >= aStorage.NbWires())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Wire -> Faces
  const NCollection_Vector<BRepGraph_FaceId>* aFaces =
    aRevIdx.FacesOfWire(BRepGraph_WireId(theWireIdx));
  if (aFaces == nullptr)
    return;

  for (int f = 0; f < aFaces->Length(); ++f)
  {
    const int                       aFaceIdx = aFaces->Value(f).Index;
    const BRepGraphInc::FaceEntity& aFace    = aStorage.Face(BRepGraph_FaceId(aFaceIdx));
    if (aFace.IsRemoved)
      continue;

    const int aWireRefIdx = findWireRefIdx(aStorage, BRepGraph_FaceId(aFaceIdx), theWireIdx);
    if (aWireRefIdx < 0)
      continue;

    NCollection_Vector<BRepGraph_TopologyPath> aFacePaths;
    reverseWalkFromFace(aFaceIdx, aFacePaths, theDepthBudget - 1);
    for (int i = 0; i < aFacePaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath = aFacePaths.Value(i);
      aPath.pushStep(aWireRefIdx);
      theResult.Append(aPath);
    }
  }
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromFace(const int theFaceIdx,
                                               NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                                               const int theDepthBudget) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theFaceIdx < 0 || theFaceIdx >= aStorage.NbFaces())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Face -> Shells
  const NCollection_Vector<BRepGraph_ShellId>* aShells =
    aRevIdx.ShellsOfFace(BRepGraph_FaceId(theFaceIdx));
  if (aShells != nullptr)
  {
    for (int s = 0; s < aShells->Length(); ++s)
    {
      const int                        aShellIdx = aShells->Value(s).Index;
      const BRepGraphInc::ShellEntity& aShell    = aStorage.Shell(BRepGraph_ShellId(aShellIdx));
      if (aShell.IsRemoved)
        continue;

      const int aFaceRefIdx = findFaceRefIdx(aStorage, BRepGraph_ShellId(aShellIdx), theFaceIdx);
      if (aFaceRefIdx < 0)
        continue;

      NCollection_Vector<BRepGraph_TopologyPath> aShellPaths;
      reverseWalkFromShell(aShellIdx, aShellPaths, theDepthBudget - 1);
      for (int i = 0; i < aShellPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aShellPaths.Value(i);
        aPath.pushStep(aFaceRefIdx);
        theResult.Append(aPath);
      }
    }
  }

  // Face -> Compounds (direct ChildRef).
  const NCollection_Vector<BRepGraph_CompoundId>* aCompFaces =
    aRevIdx.CompoundsOfFace(BRepGraph_FaceId(theFaceIdx));
  if (aCompFaces != nullptr)
  {
    for (int c = 0; c < aCompFaces->Length(); ++c)
    {
      const int aCompIdx = aCompFaces->Value(c).Index;
      if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
        continue;
      const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(BRepGraph_CompoundId(aCompIdx));
      if (aComp.IsRemoved)
        continue;
      const int aRefIdx = findChildRefIdx(aStorage,
                                          BRepGraph_CompoundId(aCompIdx),
                                          BRepGraph_NodeId::Kind::Face,
                                          theFaceIdx);
      if (aRefIdx < 0)
        continue;
      NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
        reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx), theDepthBudget - 1);
      for (int i = 0; i < aCompPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aCompPaths.Value(i);
        aPath.pushStep(aRefIdx);
        theResult.Append(aPath);
      }
    }
  }
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromShell(
  const int                                   theShellIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult,
  const int                                   theDepthBudget) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theShellIdx < 0 || theShellIdx >= aStorage.NbShells())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  bool aHasParent = false;

  // Shell -> Solids
  const NCollection_Vector<BRepGraph_SolidId>* aSolids =
    aRevIdx.SolidsOfShell(BRepGraph_ShellId(theShellIdx));
  if (aSolids != nullptr)
  {
    for (int s = 0; s < aSolids->Length(); ++s)
    {
      const int                        aSolidIdx = aSolids->Value(s).Index;
      const BRepGraphInc::SolidEntity& aSolid    = aStorage.Solid(BRepGraph_SolidId(aSolidIdx));
      if (aSolid.IsRemoved)
        continue;

      const int aShellRefIdx = findShellRefIdx(aStorage, BRepGraph_SolidId(aSolidIdx), theShellIdx);
      if (aShellRefIdx < 0)
        continue;

      aHasParent = true;
      // Solid may have Compound/CompSolid parents - recurse.
      NCollection_Vector<BRepGraph_TopologyPath> aSolidPaths =
        reverseWalkPaths(BRepGraph_NodeId::Solid(aSolidIdx), theDepthBudget - 1);
      for (int i = 0; i < aSolidPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aSolidPaths.Value(i);
        aPath.pushStep(aShellRefIdx);
        theResult.Append(aPath);
      }
    }
  }

  // Shell -> Compounds (direct ChildRef).
  const NCollection_Vector<BRepGraph_CompoundId>* aCompShells =
    aRevIdx.CompoundsOfShell(BRepGraph_ShellId(theShellIdx));
  if (aCompShells != nullptr)
  {
    for (int c = 0; c < aCompShells->Length(); ++c)
    {
      const int aCompIdx = aCompShells->Value(c).Index;
      if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
        continue;
      const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(BRepGraph_CompoundId(aCompIdx));
      if (aComp.IsRemoved)
        continue;
      const int aRefIdx = findChildRefIdx(aStorage,
                                          BRepGraph_CompoundId(aCompIdx),
                                          BRepGraph_NodeId::Kind::Shell,
                                          theShellIdx);
      if (aRefIdx < 0)
        continue;
      aHasParent = true;
      NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
        reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx), theDepthBudget - 1);
      for (int i = 0; i < aCompPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aCompPaths.Value(i);
        aPath.pushStep(aRefIdx);
        theResult.Append(aPath);
      }
    }
  }

  // Standalone root if no parents found.
  if (!aHasParent)
  {
    theResult.Append(BRepGraph_TopologyPath(BRepGraph_NodeId::Shell(theShellIdx)));
  }
}

//=================================================================================================

TopLoc_Location BRepGraph::PathView::OccurrenceLocation(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
    return TopLoc_Location();

  TopLoc_Location        aGlobal      = aStorage.Occurrence(theOccurrence).Placement;
  BRepGraph_OccurrenceId aParentOccId = aStorage.Occurrence(theOccurrence).ParentOccurrenceDefId;

  const int aNbOccurrences = aStorage.NbOccurrences();
  int       aSteps         = 0;

  while (aParentOccId.IsValid(aNbOccurrences) && aSteps < aNbOccurrences)
  {
    ++aSteps;
    const BRepGraphInc::OccurrenceEntity& aParentOcc = aStorage.Occurrence(aParentOccId);
    if (aParentOcc.IsRemoved)
      break;
    aGlobal      = aParentOcc.Placement * aGlobal;
    aParentOccId = aParentOcc.ParentOccurrenceDefId;
  }

  return aGlobal;
}

//=================================================================================================

int BRepGraph::PathView::DepthOfKind(const BRepGraph_TopologyPath& thePath,
                                     BRepGraph_NodeId::Kind        theKind) const
{
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  if (!thePath.IsValid())
    return 0;

  int              aCount   = 0;
  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  if (aCurrent.NodeKind == theKind)
    ++aCount;

  while (aCurrent.IsValid() && aStepIdx < thePath.Depth())
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      if (aCurrent.IsValid() && aCurrent.NodeKind == theKind)
        ++aCount;
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
        && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
        && myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index))
             .ShapeRootId.IsValid())
    {
      aCurrent =
        myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
      if (aCurrent.IsValid() && aCurrent.NodeKind == theKind)
        ++aCount;
      continue;
    }
    aCurrent = resolveChild(aCurrent, thePath.stepAt(aStepIdx));
    if (aCurrent.IsValid() && aCurrent.NodeKind == theKind)
      ++aCount;
    ++aStepIdx;
  }
  return aCount;
}

//=================================================================================================

BRepGraph_TopologyPath BRepGraph::PathView::CommonAncestor(
  const BRepGraph_TopologyPath& thePath1,
  const BRepGraph_TopologyPath& thePath2) const
{
  if (!thePath1.IsValid() || !thePath2.IsValid() || thePath1.Root() != thePath2.Root())
    return BRepGraph_TopologyPath();

  BRepGraph_TopologyPath aResult(thePath1.Root());
  const int aMinDepth = thePath1.Depth() < thePath2.Depth() ? thePath1.Depth() : thePath2.Depth();
  for (int i = 0; i < aMinDepth; ++i)
  {
    if (thePath1.stepAt(i) != thePath2.stepAt(i))
      break;
    aResult.pushStep(thePath1.stepAt(i));
  }
  return aResult;
}

//=================================================================================================

bool BRepGraph::PathView::IsAncestorOf(const BRepGraph_TopologyPath& theAncestorPath,
                                       const BRepGraph_TopologyPath& theDescendantPath) const
{
  if (!theAncestorPath.IsValid() || !theDescendantPath.IsValid())
    return false;
  if (theAncestorPath.Root() != theDescendantPath.Root())
    return false;
  if (theAncestorPath.Depth() > theDescendantPath.Depth())
    return false;
  for (int i = 0; i < theAncestorPath.Depth(); ++i)
  {
    if (theAncestorPath.stepAt(i) != theDescendantPath.stepAt(i))
      return false;
  }
  return true;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::PathView::AllNodesOnPath(
  const BRepGraph_TopologyPath& thePath) const
{
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (!thePath.IsValid())
    return aResult;

  BRepGraph_NodeId aCurrent = thePath.Root();
  aResult.Append(aCurrent);
  int aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < thePath.Depth())
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      if (aCurrent.IsValid())
        aResult.Append(aCurrent);
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
        && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
        && myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index))
             .ShapeRootId.IsValid())
    {
      aCurrent =
        myGraph->myData->myIncStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
      if (aCurrent.IsValid())
        aResult.Append(aCurrent);
      continue;
    }
    aCurrent = resolveChild(aCurrent, thePath.stepAt(aStepIdx));
    if (aCurrent.IsValid())
      aResult.Append(aCurrent);
    ++aStepIdx;
  }

  // Trailing 1:1 transitions.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  while (aCurrent.IsValid()
         && (is1to1(aCurrent.NodeKind)
             || (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
                 && aCurrent.Index < aStorage.NbProducts()
                 && aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId.IsValid())))
  {
    BRepGraph_NodeId aPrev = aCurrent;
    if (is1to1(aCurrent.NodeKind))
      aCurrent = transitChild(aCurrent);
    else
      aCurrent = aStorage.Product(BRepGraph_ProductId(aCurrent.Index)).ShapeRootId;
    if (aCurrent == aPrev)
      break;
    if (aCurrent.IsValid())
      aResult.Append(aCurrent);
  }

  return aResult;
}

// ==========================================================================
// Assembly accessors
// ==========================================================================

//=================================================================================================

int BRepGraph::PathView::NbProducts() const
{
  return myGraph->myData->myIncStorage.NbProducts();
}

//=================================================================================================

int BRepGraph::PathView::NbOccurrences() const
{
  return myGraph->myData->myIncStorage.NbOccurrences();
}

//=================================================================================================

int BRepGraph::PathView::NbActiveProducts() const
{
  return myGraph->myData->myIncStorage.NbActiveProducts();
}

//=================================================================================================

int BRepGraph::PathView::NbActiveOccurrences() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrences();
}

//=================================================================================================

const BRepGraph_TopoNode::ProductDef& BRepGraph::PathView::Product(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.Product(theProduct);
}

//=================================================================================================

const BRepGraph_TopoNode::OccurrenceDef& BRepGraph::PathView::Occurrence(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  return myGraph->myData->myIncStorage.Occurrence(theOccurrence);
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::PathView::RootProducts() const
{
  const BRepGraphInc_Storage& aStorage    = myGraph->myData->myIncStorage;
  const int                   aNbProducts = aStorage.NbProducts();

  // Collect product indices that are referenced by at least one active occurrence.
  // Scan occurrences directly (always up to date, unlike the reverse index which
  // is only rebuilt during BuildReverseIndex).
  NCollection_Map<int> aReferencedProducts;
  for (int anOccIdx = 0; anOccIdx < aStorage.NbOccurrences(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceEntity& anOcc =
      aStorage.Occurrence(BRepGraph_OccurrenceId(anOccIdx));
    if (!anOcc.IsRemoved && anOcc.ProductDefId.IsValid())
      aReferencedProducts.Add(anOcc.ProductDefId.Index);
  }

  NCollection_Vector<BRepGraph_NodeId> aRoots;
  for (int aProdIdx = 0; aProdIdx < aNbProducts; ++aProdIdx)
  {
    const BRepGraphInc::ProductEntity& aProduct = aStorage.Product(BRepGraph_ProductId(aProdIdx));
    if (aProduct.IsRemoved)
      continue;
    if (!aReferencedProducts.Contains(aProdIdx))
      aRoots.Append(BRepGraph_NodeId::Product(aProdIdx));
  }
  return aRoots;
}

//=================================================================================================

bool BRepGraph::PathView::IsAssembly(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
    return false;
  const BRepGraphInc::ProductEntity& aProductEnt = aStorage.Product(theProduct);
  return !aProductEnt.ShapeRootId.IsValid();
}

//=================================================================================================

bool BRepGraph::PathView::IsPart(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
    return false;
  const BRepGraphInc::ProductEntity& aProductEnt = aStorage.Product(theProduct);
  return aProductEnt.ShapeRootId.IsValid();
}

//=================================================================================================

int BRepGraph::PathView::NbComponents(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
    return 0;
  return aStorage.Product(theProduct).OccurrenceRefIds.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PathView::Component(const BRepGraph_ProductId theProduct,
                                                const int                 theComponentIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
    return BRepGraph_NodeId();
  const BRepGraphInc::ProductEntity& aProductEnt = aStorage.Product(theProduct);
  if (theComponentIdx < 0 || theComponentIdx >= aProductEnt.OccurrenceRefIds.Length())
    return BRepGraph_NodeId();
  return aStorage.OccurrenceRefEntry(aProductEnt.OccurrenceRefIds.Value(theComponentIdx)).OccurrenceDefId;
}
