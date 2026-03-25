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

#include <Standard_Assert.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>

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

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      // Product(part) -> ShapeRoot: RootLocation. Product(assembly) -> Occurrence: identity.
      if (theRefIdx < 0 && theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
        return aStorage.Product(BRepGraph_ProductId(theParent.Index)).RootLocation;
      return TopLoc_Location();
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
          return aComp.ChildRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return aCS.SolidRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aSolid.ShellRefs.Length())
          return aSolid.ShellRefs.Value(theRefIdx).LocalLocation;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefs.Length())
          return aSolid.FreeChildRefs.Value(aFreeIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aShell.FaceRefs.Length())
          return aShell.FaceRefs.Value(theRefIdx).LocalLocation;
        const int aFreeIdx = theRefIdx - aShell.FaceRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefs.Length())
          return aShell.FreeChildRefs.Value(aFreeIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aFace.WireRefs.Length())
          return aFace.WireRefs.Value(theRefIdx).LocalLocation;
        const int aVtxIdx = theRefIdx - aFace.WireRefs.Length();
        if (aVtxIdx >= 0 && aVtxIdx < aFace.VertexRefs.Length())
          return aFace.VertexRefs.Value(aVtxIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Wire: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire = aStorage.Wire(BRepGraph_WireId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aWire.CoEdgeRefs.Length())
          return aWire.CoEdgeRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Edge: {
      // RefIdx convention: 0=StartVertex, 1=EndVertex, 2+=InternalVertices[RefIdx-2].
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        if (theRefIdx == 0)
          return anEdge.StartVertex.LocalLocation;
        if (theRefIdx == 1)
          return anEdge.EndVertex.LocalLocation;
        const int anIntIdx = theRefIdx - 2;
        if (anIntIdx >= 0 && anIntIdx < anEdge.InternalVertices.Length())
          return anEdge.InternalVertices.Value(anIntIdx).LocalLocation;
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

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      if (theRefIdx < 0 && theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
        return aStorage.Product(BRepGraph_ProductId(theParent.Index)).RootOrientation;
      return TopAbs_FORWARD;
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
          return aComp.ChildRefs.Value(theRefIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return aCS.SolidRefs.Value(theRefIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aSolid.ShellRefs.Length())
          return aSolid.ShellRefs.Value(theRefIdx).Orientation;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefs.Length())
          return aSolid.FreeChildRefs.Value(aFreeIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aShell.FaceRefs.Length())
          return aShell.FaceRefs.Value(theRefIdx).Orientation;
        const int aFreeIdx = theRefIdx - aShell.FaceRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefs.Length())
          return aShell.FreeChildRefs.Value(aFreeIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aFace.WireRefs.Length())
          return aFace.WireRefs.Value(theRefIdx).Orientation;
        const int aVtxIdx = theRefIdx - aFace.WireRefs.Length();
        if (aVtxIdx >= 0 && aVtxIdx < aFace.VertexRefs.Length())
          return aFace.VertexRefs.Value(aVtxIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Wire:
      return TopAbs_FORWARD; // CoEdgeRef has no orientation; Sense is on CoEdgeEntity.
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        if (theRefIdx == 0)
          return anEdge.StartVertex.Orientation;
        if (theRefIdx == 1)
          return anEdge.EndVertex.Orientation;
        const int anIntIdx = theRefIdx - 2;
        if (anIntIdx >= 0 && anIntIdx < anEdge.InternalVertices.Length())
          return anEdge.InternalVertices.Value(anIntIdx).Orientation;
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
        if (theRefIdx >= 0 && theRefIdx < aProd.OccurrenceRefs.Length())
          return aProd.OccurrenceRefs.Value(theRefIdx).OccurrenceDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(theRefIdx);
          return aRef.ChildDefId;
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return aCS.SolidRefs.Value(theRefIdx).SolidDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          aStorage.Solid(BRepGraph_SolidId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aSolid.ShellRefs.Length())
          return aSolid.ShellRefs.Value(theRefIdx).ShellDefId;
        const int aFreeIdx = theRefIdx - aSolid.ShellRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aSolid.FreeChildRefs.Value(aFreeIdx);
          return aRef.ChildDefId;
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          aStorage.Shell(BRepGraph_ShellId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aShell.FaceRefs.Length())
          return aShell.FaceRefs.Value(theRefIdx).FaceDefId;
        const int aFreeIdx = theRefIdx - aShell.FaceRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(aFreeIdx);
          return aRef.ChildDefId;
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(BRepGraph_FaceId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aFace.WireRefs.Length())
          return aFace.WireRefs.Value(theRefIdx).WireDefId;
        const int aVtxIdx = theRefIdx - aFace.WireRefs.Length();
        if (aVtxIdx >= 0 && aVtxIdx < aFace.VertexRefs.Length())
          return aFace.VertexRefs.Value(aVtxIdx).VertexDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Wire: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire = aStorage.Wire(BRepGraph_WireId(theParent.Index));
        if (theRefIdx >= 0 && theRefIdx < aWire.CoEdgeRefs.Length())
          return aWire.CoEdgeRefs.Value(theRefIdx).CoEdgeDefId;
      }
      return BRepGraph_NodeId();
    }
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(BRepGraph_EdgeId(theParent.Index));
        if (theRefIdx == 0 && anEdge.StartVertex.VertexDefId.IsValid())
          return anEdge.StartVertex.VertexDefId;
        if (theRefIdx == 1 && anEdge.EndVertex.VertexDefId.IsValid())
          return anEdge.EndVertex.VertexDefId;
        const int anIntIdx = theRefIdx - 2;
        if (anIntIdx >= 0 && anIntIdx < anEdge.InternalVertices.Length())
          return anEdge.InternalVertices.Value(anIntIdx).VertexDefId;
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
                                       BRepGraph_NodeId              theNode) const
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
  BRepGraph_NodeId                                  theNode) const
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
  BRepGraph_NodeId                                  theNode) const
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
  BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return NCollection_Vector<BRepGraph_TopologyPath>();

  return reverseWalkPaths(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::PathsFromTo(
  BRepGraph_NodeId theRoot,
  BRepGraph_NodeId theLeaf) const
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

int BRepGraph::PathView::findShellRefIdx(const BRepGraphInc::SolidEntity& theSolid, int theShellIdx)
{
  for (int i = 0; i < theSolid.ShellRefs.Length(); ++i)
    if (theSolid.ShellRefs.Value(i).ShellDefId.Index == theShellIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findFaceRefIdx(const BRepGraphInc::ShellEntity& theShell, int theFaceIdx)
{
  for (int i = 0; i < theShell.FaceRefs.Length(); ++i)
    if (theShell.FaceRefs.Value(i).FaceDefId.Index == theFaceIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findWireRefIdx(const BRepGraphInc::FaceEntity& theFace,
                                        const int                       theWireIdx)
{
  for (int i = 0; i < theFace.WireRefs.Length(); ++i)
    if (theFace.WireRefs.Value(i).WireDefId.Index == theWireIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findCoEdgeRefIdx(const BRepGraphInc::WireEntity& theWire,
                                          const int                       theCoEdgeIdx)
{
  for (int i = 0; i < theWire.CoEdgeRefs.Length(); ++i)
    if (theWire.CoEdgeRefs.Value(i).CoEdgeDefId.Index == theCoEdgeIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findChildRefIdx(const BRepGraphInc::CompoundEntity& theCompound,
                                         BRepGraph_NodeId::Kind              theKind,
                                         const int                           theChildIdx)
{
  for (int i = 0; i < theCompound.ChildRefs.Length(); ++i)
  {
    const BRepGraphInc::ChildRef& aRef = theCompound.ChildRefs.Value(i);
    if (aRef.ChildDefId.NodeKind == theKind && aRef.ChildDefId.Index == theChildIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::PathView::findSolidRefIdx(const BRepGraphInc::CompSolidEntity& theCS,
                                         int                                  theSolidIdx)
{
  for (int i = 0; i < theCS.SolidRefs.Length(); ++i)
    if (theCS.SolidRefs.Value(i).SolidDefId.Index == theSolidIdx)
      return i;
  return -1;
}

//=================================================================================================

NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> BRepGraph::PathView::NodeLocations(
  BRepGraph_NodeId theNode) const
{
  return reverseWalkEntries(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::PathView::reverseWalkPaths(
  BRepGraph_NodeId theNode) const
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
  BRepGraph_NodeId theNode,
  const int        theDepthBudget) const
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
          const int aRefIdx = findSolidRefIdx(aCS, theNode.Index);
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
          const int aRefIdx = findChildRefIdx(aComp, Kind::Solid, theNode.Index);
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
          const int aRefIdx = findChildRefIdx(aParentComp, Kind::Compound, theNode.Index);
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
          const int aRefIdx = findChildRefIdx(aComp, Kind::CompSolid, theNode.Index);
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
  BRepGraph_NodeId theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aPaths = reverseWalkPaths(theNode);
  NCollection_Vector<OccurrenceEntry>        aResult;
  for (int i = 0; i < aPaths.Length(); ++i)
    aResult.Append(buildEntry(aPaths.Value(i)));
  return aResult;
}

//=================================================================================================

void BRepGraph::PathView::reverseWalkFromVertex(
  int                                         theVertexIdx,
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
    if (anEdge.StartVertex.VertexDefId.Index == theVertexIdx)
      aVertexRefIdx = 0;
    else if (anEdge.EndVertex.VertexDefId.Index == theVertexIdx)
      aVertexRefIdx = 1;
    else
    {
      for (int iv = 0; iv < anEdge.InternalVertices.Length(); ++iv)
      {
        if (anEdge.InternalVertices.Value(iv).VertexDefId.Index == theVertexIdx)
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

void BRepGraph::PathView::reverseWalkFromEdge(int theEdgeIdx,
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

      const int aCoEdgeRefIdx = findCoEdgeRefIdx(aWire, aCoEdgeId.Index);
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

void BRepGraph::PathView::reverseWalkFromWire(int theWireIdx,
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

    const int aWireRefIdx = findWireRefIdx(aFace, theWireIdx);
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

void BRepGraph::PathView::reverseWalkFromFace(int theFaceIdx,
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

      const int aFaceRefIdx = findFaceRefIdx(aShell, theFaceIdx);
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
      const int aRefIdx = findChildRefIdx(aComp, BRepGraph_NodeId::Kind::Face, theFaceIdx);
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
  int                                         theShellIdx,
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

      const int aShellRefIdx = findShellRefIdx(aSolid, theShellIdx);
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
      const int aRefIdx = findChildRefIdx(aComp, BRepGraph_NodeId::Kind::Shell, theShellIdx);
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
