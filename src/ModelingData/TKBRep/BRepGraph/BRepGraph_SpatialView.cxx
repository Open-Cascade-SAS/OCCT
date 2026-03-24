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

#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Explorer.hxx>
#include <BRepGraph_TypedAttribute.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_PackedMap.hxx>
#include <Standard_Assert.hxx>
#include <Standard_GUID.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>

//! GUID for precomputed locations cache attribute.
static const Standard_GUID THE_PRECOMPUTED_LOCATIONS_GUID("b7e3a1f2-9c04-4d6b-8a5e-2f1d3c4b5a60");

//! Data stored in the precomputed locations attribute.
struct PrecomputedLocationsData
{
  BRepGraph_NodeId::Kind                                      TargetKind;
  NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> Entries;
};

//=================================================================================================

bool BRepGraph::SpatialView::is1to1(const BRepGraph_NodeId::Kind theKind)
{
  return theKind == BRepGraph_NodeId::Kind::Occurrence || theKind == BRepGraph_NodeId::Kind::CoEdge;
  // Product(part) is also 1:1 but requires runtime check (ShapeRootId.IsValid()).
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::transitLocation(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::Occurrence && theNode.Index >= 0
      && theNode.Index < aStorage.NbOccurrences())
  {
    return aStorage.Occurrence(theNode.Index).Placement;
  }
  // CoEdge -> Edge: no location.
  return TopLoc_Location();
}

//=================================================================================================

TopAbs_Orientation BRepGraph::SpatialView::transitOrientation(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::CoEdge && theNode.Index >= 0
      && theNode.Index < aStorage.NbCoEdges())
  {
    return aStorage.CoEdge(theNode.Index).Sense;
  }
  return TopAbs_FORWARD;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SpatialView::transitChild(const BRepGraph_NodeId theNode) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (theNode.NodeKind == Kind::Occurrence && theNode.Index >= 0
      && theNode.Index < aStorage.NbOccurrences())
  {
    return BRepGraph_NodeId::Product(aStorage.Occurrence(theNode.Index).ProductIdx);
  }
  if (theNode.NodeKind == Kind::CoEdge && theNode.Index >= 0
      && theNode.Index < aStorage.NbCoEdges())
  {
    return BRepGraph_NodeId::Edge(aStorage.CoEdge(theNode.Index).EdgeIdx);
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::stepLocation(const BRepGraph_NodeId theParent,
                                                     int                    theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      // Product(part) -> ShapeRoot: RootLocation. Product(assembly) -> Occurrence: identity.
      if (theRefIdx < 0 && theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
        return aStorage.Product(theParent.Index).RootLocation;
      return TopLoc_Location();
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
          return aComp.ChildRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS = aStorage.CompSolid(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return aCS.SolidRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid = aStorage.Solid(theParent.Index);
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
        const BRepGraphInc::ShellEntity& aShell = aStorage.Shell(theParent.Index);
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
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(theParent.Index);
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
        const BRepGraphInc::WireEntity& aWire = aStorage.Wire(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aWire.CoEdgeRefs.Length())
          return aWire.CoEdgeRefs.Value(theRefIdx).LocalLocation;
      }
      return TopLoc_Location();
    }
    case Kind::Edge: {
      // RefIdx convention: 0=StartVertex, 1=EndVertex, 2+=InternalVertices[RefIdx-2].
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(theParent.Index);
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

TopAbs_Orientation BRepGraph::SpatialView::stepOrientation(const BRepGraph_NodeId theParent,
                                                           int                    theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      if (theRefIdx < 0 && theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
        return aStorage.Product(theParent.Index).RootOrientation;
      return TopAbs_FORWARD;
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
          return aComp.ChildRefs.Value(theRefIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS = aStorage.CompSolid(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return aCS.SolidRefs.Value(theRefIdx).Orientation;
      }
      return TopAbs_FORWARD;
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid = aStorage.Solid(theParent.Index);
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
        const BRepGraphInc::ShellEntity& aShell = aStorage.Shell(theParent.Index);
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
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(theParent.Index);
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
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(theParent.Index);
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

BRepGraph_NodeId BRepGraph::SpatialView::resolveChild(const BRepGraph_NodeId theParent,
                                                      int                    theRefIdx) const
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  switch (theParent.NodeKind)
  {
    case Kind::Product: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbProducts())
      {
        const BRepGraphInc::ProductEntity& aProd = aStorage.Product(theParent.Index);
        if (theRefIdx < 0 && aProd.ShapeRootId.IsValid())
          return aProd.ShapeRootId; // Part -> shape root
        if (theRefIdx >= 0 && theRefIdx < aProd.OccurrenceRefs.Length())
          return BRepGraph_NodeId::Occurrence(aProd.OccurrenceRefs.Value(theRefIdx).OccurrenceIdx);
      }
      return BRepGraph_NodeId();
    }
    case Kind::Compound: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aComp.ChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(theRefIdx);
          return BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx);
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::CompSolid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCS = aStorage.CompSolid(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aCS.SolidRefs.Length())
          return BRepGraph_NodeId::Solid(aCS.SolidRefs.Value(theRefIdx).SolidIdx);
      }
      return BRepGraph_NodeId();
    }
    case Kind::Solid: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid = aStorage.Solid(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aSolid.ShellRefs.Length())
          return BRepGraph_NodeId::Shell(aSolid.ShellRefs.Value(theRefIdx).ShellIdx);
        const int aFreeIdx = theRefIdx - aSolid.ShellRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aSolid.FreeChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aSolid.FreeChildRefs.Value(aFreeIdx);
          return BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx);
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::Shell: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell = aStorage.Shell(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aShell.FaceRefs.Length())
          return BRepGraph_NodeId::Face(aShell.FaceRefs.Value(theRefIdx).FaceIdx);
        const int aFreeIdx = theRefIdx - aShell.FaceRefs.Length();
        if (aFreeIdx >= 0 && aFreeIdx < aShell.FreeChildRefs.Length())
        {
          const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(aFreeIdx);
          return BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx);
        }
      }
      return BRepGraph_NodeId();
    }
    case Kind::Face: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace = aStorage.Face(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aFace.WireRefs.Length())
          return BRepGraph_NodeId::Wire(aFace.WireRefs.Value(theRefIdx).WireIdx);
        const int aVtxIdx = theRefIdx - aFace.WireRefs.Length();
        if (aVtxIdx >= 0 && aVtxIdx < aFace.VertexRefs.Length())
          return BRepGraph_NodeId::Vertex(aFace.VertexRefs.Value(aVtxIdx).VertexIdx);
      }
      return BRepGraph_NodeId();
    }
    case Kind::Wire: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire = aStorage.Wire(theParent.Index);
        if (theRefIdx >= 0 && theRefIdx < aWire.CoEdgeRefs.Length())
          return BRepGraph_NodeId::CoEdge(aWire.CoEdgeRefs.Value(theRefIdx).CoEdgeIdx);
      }
      return BRepGraph_NodeId();
    }
    case Kind::Edge: {
      if (theParent.Index >= 0 && theParent.Index < aStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(theParent.Index);
        if (theRefIdx == 0 && anEdge.StartVertex.VertexIdx >= 0)
          return BRepGraph_NodeId::Vertex(anEdge.StartVertex.VertexIdx);
        if (theRefIdx == 1 && anEdge.EndVertex.VertexIdx >= 0)
          return BRepGraph_NodeId::Vertex(anEdge.EndVertex.VertexIdx);
        const int anIntIdx = theRefIdx - 2;
        if (anIntIdx >= 0 && anIntIdx < anEdge.InternalVertices.Length())
          return BRepGraph_NodeId::Vertex(anEdge.InternalVertices.Value(anIntIdx).VertexIdx);
      }
      return BRepGraph_NodeId();
    }
    default:
      return BRepGraph_NodeId();
  }
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::composeToLevel(const BRepGraph_TopologyPath& thePath,
                                                       int theMaxSteps) const
{
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  Standard_ASSERT_VOID(theMaxSteps >= 0 && theMaxSteps <= thePath.Depth(),
                       "theMaxSteps out of range");
  if (!thePath.IsValid())
    return TopLoc_Location();

  TopLoc_Location  aResult;
  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < theMaxSteps)
  {
    // Handle 1:1 transitions (Occurrence->Product, CoEdge->Edge).
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aResult                = aResult * transitLocation(aCurrent);
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
          && aStorage.Product(aCurrent.Index).ShapeRootId.IsValid())
      {
        aResult  = aResult * aStorage.Product(aCurrent.Index).RootLocation;
        aCurrent = aStorage.Product(aCurrent.Index).ShapeRootId;
        continue;
      }
    }
    // Normal step: consume one RefIdx.
    const int aRefIdx = thePath.stepAt(aStepIdx);
    aResult           = aResult * stepLocation(aCurrent, aRefIdx);
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    ++aStepIdx;
  }

  // Handle trailing 1:1 transitions after last step.
  while (aCurrent.IsValid()
         && (is1to1(aCurrent.NodeKind)
             || (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product
                 && myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId.IsValid())))
  {
    BRepGraph_NodeId aPrev = aCurrent;
    if (is1to1(aCurrent.NodeKind))
    {
      aResult  = aResult * transitLocation(aCurrent);
      aCurrent = transitChild(aCurrent);
    }
    else
    {
      aResult  = aResult * myGraph->myData->myIncStorage.Product(aCurrent.Index).RootLocation;
      aCurrent = myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId;
    }
    if (aCurrent == aPrev)
      break;
  }

  // Per-node transform at the resolved leaf.
  if (aCurrent.IsValid())
  {
    const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(aCurrent);
    if (aNodeLoc != nullptr)
      aResult = *aNodeLoc * aResult;
  }

  return aResult;
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::GlobalLocation(const BRepGraph_TopologyPath& thePath) const
{
  return composeToLevel(thePath, thePath.Depth());
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::LocationAt(const BRepGraph_TopologyPath& thePath,
                                                   int                           theLevel) const
{
  return composeToLevel(thePath, theLevel + 1);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SpatialView::resolveAtLevel(const BRepGraph_TopologyPath& thePath,
                                                        int theMaxSteps) const
{
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  Standard_ASSERT_VOID(theMaxSteps >= 0 && theMaxSteps <= thePath.Depth(),
                       "theMaxSteps out of range");
  if (!thePath.IsValid())
    return BRepGraph_NodeId();

  BRepGraph_NodeId aCurrent = thePath.Root();
  int              aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < theMaxSteps)
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
      if (aCurrent.Index >= 0 && aCurrent.Index < aStorage.NbProducts()
          && aStorage.Product(aCurrent.Index).ShapeRootId.IsValid())
      {
        aCurrent = aStorage.Product(aCurrent.Index).ShapeRootId;
        continue;
      }
    }
    const int aRefIdx = thePath.stepAt(aStepIdx);
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    ++aStepIdx;
  }

  // Trailing 1:1 transitions.
  while (aCurrent.IsValid()
         && (is1to1(aCurrent.NodeKind)
             || (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
                 && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
                 && myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId.IsValid())))
  {
    BRepGraph_NodeId aPrev = aCurrent;
    if (is1to1(aCurrent.NodeKind))
      aCurrent = transitChild(aCurrent);
    else
      aCurrent = myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId;
    if (aCurrent == aPrev)
      break;
  }

  return aCurrent;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SpatialView::LeafNode(const BRepGraph_TopologyPath& thePath) const
{
  return resolveAtLevel(thePath, thePath.Depth());
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SpatialView::NodeAt(const BRepGraph_TopologyPath& thePath,
                                                int                           theLevel) const
{
  return resolveAtLevel(thePath, theLevel + 1);
}

//=================================================================================================

TopAbs_Orientation BRepGraph::SpatialView::composeOrientationToLevel(
  const BRepGraph_TopologyPath& thePath,
  int                           theMaxSteps) const
{
  Standard_ASSERT_VOID(thePath.IsValid(), "invalid path");
  Standard_ASSERT_VOID(theMaxSteps >= 0 && theMaxSteps <= thePath.Depth(),
                       "theMaxSteps out of range");
  if (!thePath.IsValid())
    return TopAbs_FORWARD;

  TopAbs_Orientation aResult  = TopAbs_FORWARD;
  BRepGraph_NodeId   aCurrent = thePath.Root();
  int                aStepIdx = 0;

  while (aCurrent.IsValid() && aStepIdx < theMaxSteps)
  {
    if (is1to1(aCurrent.NodeKind))
    {
      BRepGraph_NodeId aPrev = aCurrent;
      aResult                = TopAbs::Compose(aResult, transitOrientation(aCurrent));
      aCurrent               = transitChild(aCurrent);
      if (aCurrent == aPrev)
        break;
      continue;
    }
    if (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
      if (aCurrent.Index >= 0 && aCurrent.Index < aStorage.NbProducts()
          && aStorage.Product(aCurrent.Index).ShapeRootId.IsValid())
      {
        aResult  = TopAbs::Compose(aResult, aStorage.Product(aCurrent.Index).RootOrientation);
        aCurrent = aStorage.Product(aCurrent.Index).ShapeRootId;
        continue;
      }
    }
    const int aRefIdx = thePath.stepAt(aStepIdx);
    aResult           = TopAbs::Compose(aResult, stepOrientation(aCurrent, aRefIdx));
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    ++aStepIdx;
  }

  // Trailing 1:1 transitions (including Product(part) -> ShapeRoot).
  while (aCurrent.IsValid()
         && (is1to1(aCurrent.NodeKind)
             || (aCurrent.NodeKind == BRepGraph_NodeId::Kind::Product && aCurrent.Index >= 0
                 && aCurrent.Index < myGraph->myData->myIncStorage.NbProducts()
                 && myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId.IsValid())))
  {
    BRepGraph_NodeId aPrev = aCurrent;
    if (is1to1(aCurrent.NodeKind))
    {
      aResult  = TopAbs::Compose(aResult, transitOrientation(aCurrent));
      aCurrent = transitChild(aCurrent);
    }
    else
    {
      aResult =
        TopAbs::Compose(aResult,
                        myGraph->myData->myIncStorage.Product(aCurrent.Index).RootOrientation);
      aCurrent = myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId;
    }
    if (aCurrent == aPrev)
      break;
  }

  return aResult;
}

//=================================================================================================

TopAbs_Orientation BRepGraph::SpatialView::GlobalOrientation(
  const BRepGraph_TopologyPath& thePath) const
{
  return composeOrientationToLevel(thePath, thePath.Depth());
}

//=================================================================================================

TopAbs_Orientation BRepGraph::SpatialView::OrientationAt(const BRepGraph_TopologyPath& thePath,
                                                         int theLevel) const
{
  return composeOrientationToLevel(thePath, theLevel + 1);
}

//=================================================================================================

int BRepGraph::SpatialView::FindLevel(const BRepGraph_TopologyPath& thePath,
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
        && myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId.IsValid())
    {
      aCurrent = myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId;
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

BRepGraph_TopologyPath BRepGraph::SpatialView::Truncated(const BRepGraph_TopologyPath& thePath,
                                                         int theLevel) const
{
  BRepGraph_TopologyPath aResult(thePath.Root());
  const int              aLimit = theLevel < thePath.Depth() ? theLevel : thePath.Depth();
  for (int i = 0; i < aLimit; ++i)
    aResult.pushStep(thePath.stepAt(i));
  return aResult;
}

//=================================================================================================

int BRepGraph::SpatialView::PrecomputeCacheKey()
{
  static const int THE_KEY = BRepGraph_UserAttribute::AllocateKey(THE_PRECOMPUTED_LOCATIONS_GUID);
  return THE_KEY;
}

//=================================================================================================

void BRepGraph::SpatialView::PrecomputeLocations(BRepGraph&             theGraph,
                                                 BRepGraph_NodeId       theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind)
{
  const int aKey = PrecomputeCacheKey();

  // Compute all occurrence entries via Explorer.
  PrecomputedLocationsData aData;
  aData.TargetKind = theTargetKind;
  BRepGraph_Explorer anExp(theGraph, theRoot, theTargetKind);
  for (; anExp.More(); anExp.Next())
  {
    OccurrenceEntry anEntry;
    anEntry.Path     = anExp.CurrentPath();
    anEntry.Location = theGraph.Spatial().composeToLevel(anEntry.Path, anEntry.Path.Depth());
    anEntry.Orientation =
      theGraph.Spatial().composeOrientationToLevel(anEntry.Path, anEntry.Path.Depth());
    aData.Entries.Append(anEntry);
  }

  // Store as TypedAttribute on root node's cache.
  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theRoot);
  if (aCache == nullptr)
    return;

  occ::handle<BRepGraph_UserAttribute> anExisting = aCache->GetUserAttribute(aKey);
  if (anExisting)
  {
    // Update existing attribute.
    occ::handle<BRepGraph_TypedAttribute<PrecomputedLocationsData>> anAttr =
      occ::down_cast<BRepGraph_TypedAttribute<PrecomputedLocationsData>>(anExisting);
    if (!anAttr.IsNull())
    {
      anAttr->Set(aData);
      return;
    }
  }
  // Create new attribute.
  occ::handle<BRepGraph_TypedAttribute<PrecomputedLocationsData>> aNewAttr =
    new BRepGraph_TypedAttribute<PrecomputedLocationsData>(aData);
  aCache->SetUserAttribute(aKey, aNewAttr);
}

//=================================================================================================

NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> BRepGraph::SpatialView::GetPrecomputed(
  BRepGraph_NodeId theRoot) const
{
  const int aKey = PrecomputeCacheKey();

  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theRoot);
  if (aDef == nullptr)
    return NCollection_Vector<OccurrenceEntry>();

  occ::handle<BRepGraph_UserAttribute> anAttr = aDef->Cache.GetUserAttribute(aKey);
  if (anAttr.IsNull() || anAttr->IsDirty())
    return NCollection_Vector<OccurrenceEntry>();

  occ::handle<BRepGraph_TypedAttribute<PrecomputedLocationsData>> aTyped =
    occ::down_cast<BRepGraph_TypedAttribute<PrecomputedLocationsData>>(anAttr);
  if (aTyped.IsNull())
    return NCollection_Vector<OccurrenceEntry>();

  return aTyped->UncheckedValue().Entries;
}

//=================================================================================================

bool BRepGraph::SpatialView::HasPrecomputed(BRepGraph_NodeId theRoot) const
{
  const int                          aKey = PrecomputeCacheKey();
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theRoot);
  if (aDef == nullptr)
    return false;
  occ::handle<BRepGraph_UserAttribute> anAttr = aDef->Cache.GetUserAttribute(aKey);
  return !anAttr.IsNull() && !anAttr->IsDirty();
}

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::NodeTransform(const BRepGraph_NodeId theDefId) const
{
  if (!theDefId.IsValid())
    return gp_Trsf();

  const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(theDefId);
  if (aNodeLoc != nullptr)
    return aNodeLoc->Transformation();

  return gp_Trsf();
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SameDomainFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);
  if (aFaceDef.SurfaceRepIdx < 0)
    return aResult;

  for (int aFaceIdx = 0; aFaceIdx < myGraph->myData->myIncStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aOtherFace = myGraph->myData->myIncStorage.Face(aFaceIdx);
    if (aOtherFace.SurfaceRepIdx == aFaceDef.SurfaceRepIdx && aOtherFace.Id != theFaceDef)
      aResult.Append(aOtherFace.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::FacesOfEdge(
  const BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<int>*       aFaces =
    myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdge(theEdgeDef.Index);
  if (aFaces != nullptr)
  {
    for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      aResult.Append(BRepGraph_NodeId::Face(aFaces->Value(aFIdx)));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SharedEdges(
  const BRepGraph_NodeId theFaceA,
  const BRepGraph_NodeId theFaceB) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDefA = myGraph->myData->myIncStorage.Face(theFaceA.Index);

  NCollection_PackedMap<int> aEdgesA;
  for (int aWIdx = 0; aWIdx < aFaceDefA.WireRefs.Length(); ++aWIdx)
  {
    const int                          aWireDefIdx = aFaceDefA.WireRefs.Value(aWIdx).WireIdx;
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myIncStorage.Wire(aWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      aEdgesA.Add(aCoEdge.EdgeIdx);
    }
  }

  const BRepGraph_TopoNode::FaceDef& aFaceDefB = myGraph->myData->myIncStorage.Face(theFaceB.Index);

  NCollection_PackedMap<int> aAdded;
  for (int aWIdx = 0; aWIdx < aFaceDefB.WireRefs.Length(); ++aWIdx)
  {
    const int                          aWireDefIdx = aFaceDefB.WireRefs.Value(aWIdx).WireIdx;
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myIncStorage.Wire(aWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      const int anEdgeDefIdx = aCoEdge.EdgeIdx;
      if (aEdgesA.Contains(anEdgeDefIdx) && aAdded.Add(anEdgeDefIdx))
        aResult.Append(myGraph->myData->myIncStorage.Edge(anEdgeDefIdx).Id);
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);

  const BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ReverseIndex();
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const int                          aWireDefIdx = aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx;
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myIncStorage.Wire(aWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      const int                      anEdgeDefIdx = aCoEdge.EdgeIdx;
      const NCollection_Vector<int>* aFaces       = aRevIdx.FacesOfEdge(anEdgeDefIdx);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const int aFaceIdx = aFaces->Value(aFIdx);
          if (aFaceIdx != theFaceDef.Index && aFaceSet.Add(aFaceIdx))
            aResult.Append(BRepGraph_NodeId::Face(aFaceIdx));
        }
      }
    }
  }

  return aResult;
}

//=================================================================================================

bool BRepGraph::SpatialView::PathContains(const BRepGraph_TopologyPath& thePath,
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
        && myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId.IsValid())
    {
      aCurrent = myGraph->myData->myIncStorage.Product(aCurrent.Index).ShapeRootId;
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

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::SpatialView::FilterByInclude(
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

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::SpatialView::FilterByExclude(
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

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::SpatialView::PathsTo(
  BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return NCollection_Vector<BRepGraph_TopologyPath>();

  return reverseWalkPaths(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::SpatialView::PathsFromTo(
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

BRepGraph::SpatialView::OccurrenceEntry BRepGraph::SpatialView::buildEntry(
  const BRepGraph_TopologyPath& thePath) const
{
  OccurrenceEntry anEntry;
  anEntry.Path        = thePath;
  anEntry.Location    = composeToLevel(thePath, thePath.Depth());
  anEntry.Orientation = composeOrientationToLevel(thePath, thePath.Depth());
  return anEntry;
}

//=================================================================================================

int BRepGraph::SpatialView::findShellRefIdx(const BRepGraphInc::SolidEntity& theSolid,
                                            int                              theShellIdx)
{
  for (int i = 0; i < theSolid.ShellRefs.Length(); ++i)
    if (theSolid.ShellRefs.Value(i).ShellIdx == theShellIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::SpatialView::findFaceRefIdx(const BRepGraphInc::ShellEntity& theShell,
                                           int                              theFaceIdx)
{
  for (int i = 0; i < theShell.FaceRefs.Length(); ++i)
    if (theShell.FaceRefs.Value(i).FaceIdx == theFaceIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::SpatialView::findWireRefIdx(const BRepGraphInc::FaceEntity& theFace, int theWireIdx)
{
  for (int i = 0; i < theFace.WireRefs.Length(); ++i)
    if (theFace.WireRefs.Value(i).WireIdx == theWireIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::SpatialView::findCoEdgeRefIdx(const BRepGraphInc::WireEntity& theWire,
                                             int                             theCoEdgeIdx)
{
  for (int i = 0; i < theWire.CoEdgeRefs.Length(); ++i)
    if (theWire.CoEdgeRefs.Value(i).CoEdgeIdx == theCoEdgeIdx)
      return i;
  return -1;
}

//=================================================================================================

int BRepGraph::SpatialView::findChildRefIdx(const BRepGraphInc::CompoundEntity& theCompound,
                                            BRepGraph_NodeId::Kind              theKind,
                                            int                                 theChildIdx)
{
  for (int i = 0; i < theCompound.ChildRefs.Length(); ++i)
  {
    const BRepGraphInc::ChildRef& aRef = theCompound.ChildRefs.Value(i);
    if (aRef.Kind == theKind && aRef.ChildIdx == theChildIdx)
      return i;
  }
  return -1;
}

//=================================================================================================

int BRepGraph::SpatialView::findSolidRefIdx(const BRepGraphInc::CompSolidEntity& theCS,
                                            int                                  theSolidIdx)
{
  for (int i = 0; i < theCS.SolidRefs.Length(); ++i)
    if (theCS.SolidRefs.Value(i).SolidIdx == theSolidIdx)
      return i;
  return -1;
}

//=================================================================================================

NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> BRepGraph::SpatialView::NodeLocations(
  BRepGraph_NodeId theNode) const
{
  return reverseWalkEntries(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraph_TopologyPath> BRepGraph::SpatialView::reverseWalkPaths(
  BRepGraph_NodeId theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aResult;
  if (!theNode.IsValid())
    return aResult;

  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  switch (theNode.NodeKind)
  {
    case Kind::Vertex:
      reverseWalkFromVertex(theNode.Index, aResult);
      break;
    case Kind::Edge:
      reverseWalkFromEdge(theNode.Index, aResult);
      break;
    case Kind::Wire:
      reverseWalkFromWire(theNode.Index, aResult);
      break;
    case Kind::Face:
      reverseWalkFromFace(theNode.Index, aResult);
      break;
    case Kind::Shell:
      reverseWalkFromShell(theNode.Index, aResult);
      break;
    case Kind::Solid: {
      const BRepGraphInc_ReverseIndex& aRevIdx    = aStorage.ReverseIndex();
      bool                             aHasParent = false;
      // Walk up to CompSolid parents.
      const NCollection_Vector<int>* aCompSolids = aRevIdx.CompSolidsOfSolid(theNode.Index);
      if (aCompSolids != nullptr)
      {
        for (int cs = 0; cs < aCompSolids->Length(); ++cs)
        {
          const int aCSIdx = aCompSolids->Value(cs);
          if (aCSIdx < 0 || aCSIdx >= aStorage.NbCompSolids())
            continue;
          const BRepGraphInc::CompSolidEntity& aCS = aStorage.CompSolid(aCSIdx);
          if (aCS.IsRemoved)
            continue;
          const int aRefIdx = findSolidRefIdx(aCS, theNode.Index);
          if (aRefIdx < 0)
            continue;
          aHasParent = true;
          // Recurse: CompSolid may itself be inside a Compound.
          NCollection_Vector<BRepGraph_TopologyPath> aParentPaths =
            reverseWalkPaths(BRepGraph_NodeId::CompSolid(aCSIdx));
          for (int p = 0; p < aParentPaths.Length(); ++p)
          {
            BRepGraph_TopologyPath aPath = aParentPaths.Value(p);
            aPath.pushStep(aRefIdx);
            aResult.Append(aPath);
          }
        }
      }
      // Walk up to Compound parents.
      const NCollection_Vector<int>* aCompounds = aRevIdx.CompoundsOfSolid(theNode.Index);
      if (aCompounds != nullptr)
      {
        for (int c = 0; c < aCompounds->Length(); ++c)
        {
          const int aCompIdx = aCompounds->Value(c);
          if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aCompIdx);
          if (aComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aComp, Kind::Solid, theNode.Index);
          if (aRefIdx < 0)
            continue;
          aHasParent = true;
          NCollection_Vector<BRepGraph_TopologyPath> aParentPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx));
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
      const BRepGraphInc_ReverseIndex& aRevIdx  = aStorage.ReverseIndex();
      const NCollection_Vector<int>*   aParents = aRevIdx.CompoundsOfCompound(theNode.Index);
      if (aParents != nullptr && !aParents->IsEmpty())
      {
        for (int p = 0; p < aParents->Length(); ++p)
        {
          const int aParentIdx = aParents->Value(p);
          if (aParentIdx < 0 || aParentIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aParentComp = aStorage.Compound(aParentIdx);
          if (aParentComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aParentComp, Kind::Compound, theNode.Index);
          if (aRefIdx < 0)
            continue;
          NCollection_Vector<BRepGraph_TopologyPath> aGrandPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aParentIdx));
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
      const BRepGraphInc_ReverseIndex& aRevIdx  = aStorage.ReverseIndex();
      const NCollection_Vector<int>*   aParents = aRevIdx.CompoundsOfCompSolid(theNode.Index);
      if (aParents != nullptr && !aParents->IsEmpty())
      {
        for (int p = 0; p < aParents->Length(); ++p)
        {
          const int aCompIdx = aParents->Value(p);
          if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
            continue;
          const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aCompIdx);
          if (aComp.IsRemoved)
            continue;
          const int aRefIdx = findChildRefIdx(aComp, Kind::CompSolid, theNode.Index);
          if (aRefIdx < 0)
            continue;
          NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
            reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx));
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

NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> BRepGraph::SpatialView::
  reverseWalkEntries(BRepGraph_NodeId theNode) const
{
  NCollection_Vector<BRepGraph_TopologyPath> aPaths = reverseWalkPaths(theNode);
  NCollection_Vector<OccurrenceEntry>        aResult;
  for (int i = 0; i < aPaths.Length(); ++i)
    aResult.Append(buildEntry(aPaths.Value(i)));
  return aResult;
}

//=================================================================================================

void BRepGraph::SpatialView::reverseWalkFromVertex(
  int                                         theVertexIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theVertexIdx < 0 || theVertexIdx >= aStorage.NbVertices())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Vertex -> Edges (via reverse index)
  const NCollection_Vector<int>* aEdges = aRevIdx.EdgesOfVertex(theVertexIdx);
  if (aEdges == nullptr)
    return;

  for (int e = 0; e < aEdges->Length(); ++e)
  {
    const int                       anEdgeIdx = aEdges->Value(e);
    const BRepGraphInc::EdgeEntity& anEdge    = aStorage.Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    // Determine VertexSel: 0=Start, 1=End, 2+=Internal
    int aVertexRefIdx = -1;
    if (anEdge.StartVertex.VertexIdx == theVertexIdx)
      aVertexRefIdx = 0;
    else if (anEdge.EndVertex.VertexIdx == theVertexIdx)
      aVertexRefIdx = 1;
    else
    {
      for (int iv = 0; iv < anEdge.InternalVertices.Length(); ++iv)
      {
        if (anEdge.InternalVertices.Value(iv).VertexIdx == theVertexIdx)
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
    reverseWalkFromEdge(anEdgeIdx, aEdgePaths);
    for (int i = 0; i < aEdgePaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath = aEdgePaths.Value(i);
      aPath.pushStep(aVertexRefIdx);
      theResult.Append(aPath);
    }
  }
}

//=================================================================================================

void BRepGraph::SpatialView::reverseWalkFromEdge(
  int                                         theEdgeIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeIdx < 0 || theEdgeIdx >= aStorage.NbEdges())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Edge -> CoEdges (via reverse index)
  const NCollection_Vector<int>* aCoEdges = aRevIdx.CoEdgesOfEdge(theEdgeIdx);
  if (aCoEdges == nullptr)
    return;

  for (int ce = 0; ce < aCoEdges->Length(); ++ce)
  {
    const int                         aCoEdgeIdx = aCoEdges->Value(ce);
    const BRepGraphInc::CoEdgeEntity& aCoEdge    = aStorage.CoEdge(aCoEdgeIdx);
    if (aCoEdge.IsRemoved)
      continue;

    // CoEdge -> Wire: direct lookup via WiresOfCoEdge index.
    const NCollection_Vector<int>* aWires = aRevIdx.WiresOfCoEdge(aCoEdgeIdx);
    if (aWires == nullptr)
      continue;

    for (int w = 0; w < aWires->Length(); ++w)
    {
      const int                       aWireIdx = aWires->Value(w);
      const BRepGraphInc::WireEntity& aWire    = aStorage.Wire(aWireIdx);
      if (aWire.IsRemoved)
        continue;

      const int aCoEdgeRefIdx = findCoEdgeRefIdx(aWire, aCoEdgeIdx);
      if (aCoEdgeRefIdx < 0)
        continue;

      // Get all paths to the wire, then append coedge step.
      // (CoEdge->Edge is 1:1, no step for it in the path)
      NCollection_Vector<BRepGraph_TopologyPath> aWirePaths;
      reverseWalkFromWire(aWireIdx, aWirePaths);
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

void BRepGraph::SpatialView::reverseWalkFromWire(
  int                                         theWireIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theWireIdx < 0 || theWireIdx >= aStorage.NbWires())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Wire -> Faces
  const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(theWireIdx);
  if (aFaces == nullptr)
    return;

  for (int f = 0; f < aFaces->Length(); ++f)
  {
    const int                       aFaceIdx = aFaces->Value(f);
    const BRepGraphInc::FaceEntity& aFace    = aStorage.Face(aFaceIdx);
    if (aFace.IsRemoved)
      continue;

    const int aWireRefIdx = findWireRefIdx(aFace, theWireIdx);
    if (aWireRefIdx < 0)
      continue;

    NCollection_Vector<BRepGraph_TopologyPath> aFacePaths;
    reverseWalkFromFace(aFaceIdx, aFacePaths);
    for (int i = 0; i < aFacePaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath = aFacePaths.Value(i);
      aPath.pushStep(aWireRefIdx);
      theResult.Append(aPath);
    }
  }
}

//=================================================================================================

void BRepGraph::SpatialView::reverseWalkFromFace(
  int                                         theFaceIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theFaceIdx < 0 || theFaceIdx >= aStorage.NbFaces())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  // Face -> Shells
  const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(theFaceIdx);
  if (aShells != nullptr)
  {
    for (int s = 0; s < aShells->Length(); ++s)
    {
      const int                        aShellIdx = aShells->Value(s);
      const BRepGraphInc::ShellEntity& aShell    = aStorage.Shell(aShellIdx);
      if (aShell.IsRemoved)
        continue;

      const int aFaceRefIdx = findFaceRefIdx(aShell, theFaceIdx);
      if (aFaceRefIdx < 0)
        continue;

      NCollection_Vector<BRepGraph_TopologyPath> aShellPaths;
      reverseWalkFromShell(aShellIdx, aShellPaths);
      for (int i = 0; i < aShellPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aShellPaths.Value(i);
        aPath.pushStep(aFaceRefIdx);
        theResult.Append(aPath);
      }
    }
  }

  // Face -> Compounds (direct ChildRef).
  const NCollection_Vector<int>* aCompFaces = aRevIdx.CompoundsOfFace(theFaceIdx);
  if (aCompFaces != nullptr)
  {
    for (int c = 0; c < aCompFaces->Length(); ++c)
    {
      const int aCompIdx = aCompFaces->Value(c);
      if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
        continue;
      const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aCompIdx);
      if (aComp.IsRemoved)
        continue;
      const int aRefIdx = findChildRefIdx(aComp, BRepGraph_NodeId::Kind::Face, theFaceIdx);
      if (aRefIdx < 0)
        continue;
      NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
        reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx));
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

void BRepGraph::SpatialView::reverseWalkFromShell(
  int                                         theShellIdx,
  NCollection_Vector<BRepGraph_TopologyPath>& theResult) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theShellIdx < 0 || theShellIdx >= aStorage.NbShells())
    return;
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();

  bool aHasParent = false;

  // Shell -> Solids
  const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(theShellIdx);
  if (aSolids != nullptr)
  {
    for (int s = 0; s < aSolids->Length(); ++s)
    {
      const int                        aSolidIdx = aSolids->Value(s);
      const BRepGraphInc::SolidEntity& aSolid    = aStorage.Solid(aSolidIdx);
      if (aSolid.IsRemoved)
        continue;

      const int aShellRefIdx = findShellRefIdx(aSolid, theShellIdx);
      if (aShellRefIdx < 0)
        continue;

      aHasParent = true;
      // Solid may have Compound/CompSolid parents - recurse.
      NCollection_Vector<BRepGraph_TopologyPath> aSolidPaths =
        reverseWalkPaths(BRepGraph_NodeId::Solid(aSolidIdx));
      for (int i = 0; i < aSolidPaths.Length(); ++i)
      {
        BRepGraph_TopologyPath aPath = aSolidPaths.Value(i);
        aPath.pushStep(aShellRefIdx);
        theResult.Append(aPath);
      }
    }
  }

  // Shell -> Compounds (direct ChildRef).
  const NCollection_Vector<int>* aCompShells = aRevIdx.CompoundsOfShell(theShellIdx);
  if (aCompShells != nullptr)
  {
    for (int c = 0; c < aCompShells->Length(); ++c)
    {
      const int aCompIdx = aCompShells->Value(c);
      if (aCompIdx < 0 || aCompIdx >= aStorage.NbCompounds())
        continue;
      const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aCompIdx);
      if (aComp.IsRemoved)
        continue;
      const int aRefIdx = findChildRefIdx(aComp, BRepGraph_NodeId::Kind::Shell, theShellIdx);
      if (aRefIdx < 0)
        continue;
      aHasParent = true;
      NCollection_Vector<BRepGraph_TopologyPath> aCompPaths =
        reverseWalkPaths(BRepGraph_NodeId::Compound(aCompIdx));
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

TopLoc_Location BRepGraph::SpatialView::GlobalPlacement(const int theOccurrenceIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theOccurrenceIdx < 0 || theOccurrenceIdx >= aStorage.NbOccurrences())
    return TopLoc_Location();

  TopLoc_Location aGlobal       = aStorage.Occurrence(theOccurrenceIdx).Placement;
  int             aParentOccIdx = aStorage.Occurrence(theOccurrenceIdx).ParentOccurrenceIdx;

  const int aNbOccurrences = aStorage.NbOccurrences();
  int       aSteps         = 0;

  while (aParentOccIdx >= 0 && aParentOccIdx < aNbOccurrences && aSteps < aNbOccurrences)
  {
    ++aSteps;
    const BRepGraphInc::OccurrenceEntity& aParentOcc = aStorage.Occurrence(aParentOccIdx);
    if (aParentOcc.IsRemoved)
      break;
    aGlobal       = aParentOcc.Placement * aGlobal;
    aParentOccIdx = aParentOcc.ParentOccurrenceIdx;
  }

  return aGlobal;
}
