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

#include <BRepGraph_ParentExplorer.hxx>

#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <TopAbs.hxx>

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode)
{
  Init(theGraph, theNode);
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   BRepGraph_NodeId::Kind theTargetKind)
{
  Init(theGraph, theNode, theTargetKind);
}

//=================================================================================================

void BRepGraph_ParentExplorer::Init(const BRepGraph&       theGraph,
                                    const BRepGraph_NodeId theNode)
{
  myGraph        = &theGraph;
  myNode         = theNode;
  myFilterByKind = false;
  myMatchIndex   = 0;
  myMatches.Clear();
  collectMatches();
}

//=================================================================================================

void BRepGraph_ParentExplorer::Init(const BRepGraph&       theGraph,
                                    const BRepGraph_NodeId theNode,
                                    BRepGraph_NodeId::Kind theTargetKind)
{
  myGraph        = &theGraph;
  myNode         = theNode;
  myFilterByKind = true;
  myTargetKind   = theTargetKind;
  myMatchIndex   = 0;
  myMatches.Clear();
  collectMatches();
}

//=================================================================================================

void BRepGraph_ParentExplorer::Next()
{
  if (myMatchIndex < myMatches.Length())
  {
    ++myMatchIndex;
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ParentExplorer::Current() const
{
  return More() ? myMatches.Value(myMatchIndex).Node : BRepGraph_NodeId();
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ParentExplorer::Location() const
{
  static const TopLoc_Location THE_EMPTY_LOCATION;
  return More() ? myMatches.Value(myMatchIndex).Location : THE_EMPTY_LOCATION;
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::Orientation() const
{
  return More() ? myMatches.Value(myMatchIndex).Orientation : TopAbs_FORWARD;
}

//=================================================================================================

BRepGraph_TopologyPath BRepGraph_ParentExplorer::CurrentPath(
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  if (!More())
  {
    return BRepGraph_TopologyPath();
  }
  return myMatches.Value(myMatchIndex).Path.Truncated(myMatches.Value(myMatchIndex).ExplicitDepth,
                                                      theAllocator);
}

//=================================================================================================

void BRepGraph_ParentExplorer::collectMatches()
{
  if (myGraph == nullptr || !myNode.IsValid())
  {
    return;
  }

  NCollection_Vector<BRepGraph_TopologyPath> aPaths(8, myGraph->Allocator());
  collectPathsToNode(aPaths);

  for (int aPathIdx = 0; aPathIdx < aPaths.Length(); ++aPathIdx)
  {
    NCollection_Vector<ExpandedState> aStates(8, myGraph->Allocator());
    expandPathStates(aPaths.Value(aPathIdx), aStates);
    int aStartStateIdx = -1;
    for (int aStateIdx = aStates.Length() - 1; aStateIdx >= 0; --aStateIdx)
    {
      if (aStates.Value(aStateIdx).Node == myNode)
      {
        aStartStateIdx = aStateIdx;
        break;
      }
    }
    if (aStartStateIdx <= 0)
    {
      continue;
    }

    for (int aStateIdx = aStartStateIdx - 1; aStateIdx >= 0; --aStateIdx)
    {
      const ExpandedState& aState = aStates.Value(aStateIdx);
      if (myFilterByKind && aState.Node.NodeKind != myTargetKind)
      {
        continue;
      }

      MatchRecord& aMatch  = myMatches.Appended();
      aMatch.Path          = BRepGraph_TopologyPath(aPaths.Value(aPathIdx), myGraph->Allocator());
      aMatch.Node          = aState.Node;
      aMatch.Location      = aState.Location;
      aMatch.Orientation   = aState.Orientation;
      aMatch.ExplicitDepth = aState.ExplicitDepth;
    }
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::collectPathsToNode(
  NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const
{
  switch (myNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::CoEdge:
      appendPathsToCoEdge(BRepGraph_CoEdgeId(myNode.Index), thePaths);
      return;
    case BRepGraph_NodeId::Kind::Occurrence:
      appendPathsToOccurrence(BRepGraph_OccurrenceId(myNode.Index), thePaths);
      return;
    case BRepGraph_NodeId::Kind::Product:
      appendPathsToProduct(BRepGraph_ProductId(myNode.Index), thePaths);
      return;
    default: {
      const BRepGraph::TopoView& aTopo = myGraph->Topo();
      NCollection_Vector<BRepGraph_TopologyPath> aPaths =
        myGraph->Paths().PathsTo(myNode, myGraph->Allocator());
      for (int i = 0; i < aPaths.Length(); ++i)
      {
        const BRepGraph_TopologyPath& aTopoPath = aPaths.Value(i);
        bool                          hasProductWrapper = false;
        for (int aProdIdx = 0; aProdIdx < aTopo.NbProducts(); ++aProdIdx)
        {
          const BRepGraph_ProductId      aProductId(aProdIdx);
          const BRepGraphInc::ProductDef& aProduct = aTopo.Products().Definition(aProductId);
          if (aProduct.IsRemoved || aProduct.ShapeRootId != aTopoPath.Root())
          {
            continue;
          }

          NCollection_Vector<BRepGraph_TopologyPath> aProductPaths(4, myGraph->Allocator());
          appendPathsToProduct(aProductId, aProductPaths);
          for (int aProductPathIdx = 0; aProductPathIdx < aProductPaths.Length(); ++aProductPathIdx)
          {
            BRepGraph_TopologyPath aWrappedPath(aProductPaths.Value(aProductPathIdx),
                                                myGraph->Allocator());
            for (int aStepIdx = 0; aStepIdx < aTopoPath.Depth(); ++aStepIdx)
            {
              aWrappedPath.pushStep(aTopoPath.stepAt(aStepIdx));
            }
            thePaths.Append(aWrappedPath);
            hasProductWrapper = true;
          }
        }

        if (!hasProductWrapper)
        {
          thePaths.Append(BRepGraph_TopologyPath(aTopoPath, myGraph->Allocator()));
        }
      }
      return;
    }
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::appendPathsToOccurrence(
  const BRepGraph_OccurrenceId                  theOccurrence,
  NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  if (!theOccurrence.IsValid() || theOccurrence.Index >= aTopo.NbOccurrences())
  {
    return;
  }

  const BRepGraphInc::OccurrenceDef& anOcc = aTopo.Occurrences().Definition(theOccurrence);
  if (anOcc.IsRemoved || !anOcc.ParentProductDefId.IsValid())
  {
    return;
  }

  const NCollection_Vector<BRepGraph_OccurrenceRefId>& anOccRefIds =
    aRefs.OccurrenceRefIdsOf(anOcc.ParentProductDefId);
  int anOccRefIdx = -1;
  for (int i = 0; i < anOccRefIds.Length(); ++i)
  {
    const BRepGraphInc::OccurrenceRef& anOccRef = aRefs.Occurrence(anOccRefIds.Value(i));
    if (!anOccRef.IsRemoved && anOccRef.OccurrenceDefId == theOccurrence)
    {
      anOccRefIdx = i;
      break;
    }
  }
  if (anOccRefIdx < 0)
  {
    return;
  }

  if (anOcc.ParentOccurrenceDefId.IsValid())
  {
    NCollection_Vector<BRepGraph_TopologyPath> aParentPaths(4, myGraph->Allocator());
    appendPathsToOccurrence(anOcc.ParentOccurrenceDefId, aParentPaths);
    for (int i = 0; i < aParentPaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath(aParentPaths.Value(i), myGraph->Allocator());
      aPath.pushStep(anOccRefIdx);
      thePaths.Append(aPath);
    }
    return;
  }

  BRepGraph_TopologyPath aPath(anOcc.ParentProductDefId, myGraph->Allocator());
  aPath.pushStep(anOccRefIdx);
  thePaths.Append(aPath);
}

//=================================================================================================

void BRepGraph_ParentExplorer::appendPathsToProduct(
  const BRepGraph_ProductId                     theProduct,
  NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  if (!theProduct.IsValid() || theProduct.Index >= aTopo.NbProducts())
  {
    return;
  }

  const BRepGraphInc::ProductDef& aProduct = aTopo.Products().Definition(theProduct);
  if (aProduct.IsRemoved)
  {
    return;
  }

  bool isReferenced = false;
  for (int i = 0; i < aTopo.NbOccurrences(); ++i)
  {
    const BRepGraph_OccurrenceId anOccId(i);
    const BRepGraphInc::OccurrenceDef& anOcc = aTopo.Occurrences().Definition(anOccId);
    if (anOcc.IsRemoved || anOcc.ProductDefId != theProduct)
    {
      continue;
    }
    isReferenced = true;
    appendPathsToOccurrence(anOccId, thePaths);
  }

  if (!isReferenced)
  {
    thePaths.Append(BRepGraph_TopologyPath(theProduct, myGraph->Allocator()));
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::appendPathsToCoEdge(
  const BRepGraph_CoEdgeId                      theCoEdge,
  NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  if (!theCoEdge.IsValid() || theCoEdge.Index >= aTopo.NbCoEdges())
  {
    return;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aTopo.CoEdges().Definition(theCoEdge);
  if (aCoEdge.IsRemoved)
  {
    return;
  }

  for (int aWireIdx = 0; aWireIdx < aTopo.NbWires(); ++aWireIdx)
  {
    const BRepGraph_WireId aWireId(aWireIdx);
    const BRepGraphInc::WireDef& aWire = aTopo.Wires().Definition(aWireId);
    if (aWire.IsRemoved)
    {
      continue;
    }

    const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefIds = aRefs.CoEdgeRefIdsOf(aWireId);
    int aCoEdgeRefIdx = -1;
    for (int i = 0; i < aCoEdgeRefIds.Length(); ++i)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = aRefs.CoEdge(aCoEdgeRefIds.Value(i));
      if (!aCoEdgeRef.IsRemoved && aCoEdgeRef.CoEdgeDefId == theCoEdge)
      {
        aCoEdgeRefIdx = i;
        break;
      }
    }
    if (aCoEdgeRefIdx < 0)
    {
      continue;
    }

    NCollection_Vector<BRepGraph_TopologyPath> aWirePaths =
      myGraph->Paths().PathsTo(aWireId, myGraph->Allocator());
    for (int i = 0; i < aWirePaths.Length(); ++i)
    {
      BRepGraph_TopologyPath aPath(aWirePaths.Value(i), myGraph->Allocator());
      aPath.pushStep(aCoEdgeRefIdx);
      thePaths.Append(aPath);
    }
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::expandPathStates(const BRepGraph_TopologyPath&     thePath,
                                                NCollection_Vector<ExpandedState>& theStates) const
{
  theStates.Clear();
  if (!thePath.IsValid())
  {
    return;
  }

  BRepGraph_NodeId   aCurrent     = thePath.Root();
  TopLoc_Location    aLocation;
  TopAbs_Orientation anOrientation = TopAbs_FORWARD;
  int                anExpDepth    = 0;

  ExpandedState& aRootState = theStates.Appended();
  aRootState.Node           = aCurrent;
  aRootState.Location       = aLocation;
  aRootState.Orientation    = anOrientation;
  aRootState.ExplicitDepth  = 0;

  int aStepIdx = 0;
  while (aCurrent.IsValid() && aStepIdx < thePath.Depth())
  {
    while (appendOneToOneChild(aCurrent, aLocation, anOrientation, anExpDepth, theStates))
    {
    }

    if (!aCurrent.IsValid())
    {
      return;
    }

    const int aRefIdx = thePath.stepAt(aStepIdx);
    aLocation         = aLocation * stepLocation(aCurrent, aRefIdx);
    anOrientation     = TopAbs::Compose(anOrientation, stepOrientation(aCurrent, aRefIdx));
    aCurrent          = resolveChild(aCurrent, aRefIdx);
    ++anExpDepth;
    if (!aCurrent.IsValid())
    {
      return;
    }

    ExpandedState& aState = theStates.Appended();
    aState.Node           = aCurrent;
    aState.Location       = aLocation;
    aState.Orientation    = anOrientation;
    aState.ExplicitDepth  = anExpDepth;
    ++aStepIdx;
  }

  while (appendOneToOneChild(aCurrent, aLocation, anOrientation, anExpDepth, theStates))
  {
  }
}

//=================================================================================================

bool BRepGraph_ParentExplorer::appendOneToOneChild(BRepGraph_NodeId&              theCurrent,
                                                    TopLoc_Location&               theLocation,
                                                    TopAbs_Orientation&            theOrientation,
                                                    const int                      theExplicitDepth,
                                                    NCollection_Vector<ExpandedState>& theStates) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  if (!theCurrent.IsValid())
  {
    return false;
  }

  switch (theCurrent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraphInc::OccurrenceDef& anOcc =
        aTopo.Occurrences().Definition(BRepGraph_OccurrenceId(theCurrent.Index));
      if (anOcc.IsRemoved || !anOcc.ProductDefId.IsValid())
      {
        theCurrent = BRepGraph_NodeId();
        return false;
      }
      theLocation = theLocation * anOcc.Placement;
      theCurrent  = anOcc.ProductDefId;
      break;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId(theCurrent.Index));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid())
      {
        theCurrent = BRepGraph_NodeId();
        return false;
      }
      theOrientation = TopAbs::Compose(theOrientation, aCoEdge.Sense);
      theCurrent     = aCoEdge.EdgeDefId;
      break;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraphInc::ProductDef& aProduct =
        aTopo.Products().Definition(BRepGraph_ProductId(theCurrent.Index));
      if (aProduct.IsRemoved || !aProduct.ShapeRootId.IsValid())
      {
        return false;
      }
      theLocation    = theLocation * aProduct.RootLocation;
      theOrientation = TopAbs::Compose(theOrientation, aProduct.RootOrientation);
      theCurrent     = aProduct.ShapeRootId;
      break;
    }
    default:
      return false;
  }

  if (!theCurrent.IsValid())
  {
    return false;
  }

  ExpandedState& aState = theStates.Appended();
  aState.Node           = theCurrent;
  aState.Location       = theLocation;
  aState.Orientation    = theOrientation;
  aState.ExplicitDepth  = theExplicitDepth;
  return true;
}

//=================================================================================================

TopLoc_Location BRepGraph_ParentExplorer::stepLocation(const BRepGraph_NodeId theParent,
                                                       const int              theRefIdx) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return TopLoc_Location();
    case BRepGraph_NodeId::Kind::Compound: {
      return aRefs.Child(aRefs.ChildRefIdsOf(BRepGraph_CompoundId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      return aRefs.Solid(aRefs.SolidRefIdsOf(BRepGraph_CompSolidId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid = aTopo.Solids().Definition(BRepGraph_SolidId(theParent.Index));
      if (theRefIdx < aSolid.ShellRefIds.Length())
      {
        return aRefs.Shell(aSolid.ShellRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Child(aSolid.FreeChildRefIds.Value(theRefIdx - aSolid.ShellRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell = aTopo.Shells().Definition(BRepGraph_ShellId(theParent.Index));
      if (theRefIdx < aShell.FaceRefIds.Length())
      {
        return aRefs.Face(aShell.FaceRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Child(aShell.FreeChildRefIds.Value(theRefIdx - aShell.FaceRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace = aTopo.Faces().Definition(BRepGraph_FaceId(theParent.Index));
      if (theRefIdx < aFace.WireRefIds.Length())
      {
        return aRefs.Wire(aFace.WireRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Vertex(aFace.VertexRefIds.Value(theRefIdx - aFace.WireRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Wire:
      return aRefs.CoEdge(aRefs.CoEdgeRefIdsOf(BRepGraph_WireId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    case BRepGraph_NodeId::Kind::Edge: {
      NCollection_Vector<BRepGraph_VertexRefId> aVertexRefIds =
        aRefs.VertexRefIdsOf(BRepGraph_EdgeId(theParent.Index), myGraph->Allocator());
      return aRefs.Vertex(aVertexRefIds.Value(theRefIdx)).LocalLocation;
    }
    default:
      return TopLoc_Location();
  }
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::stepOrientation(const BRepGraph_NodeId theParent,
                                                             const int              theRefIdx) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return TopAbs_FORWARD;
    case BRepGraph_NodeId::Kind::Compound:
      return aRefs.Child(aRefs.ChildRefIdsOf(BRepGraph_CompoundId(theParent.Index)).Value(theRefIdx))
        .Orientation;
    case BRepGraph_NodeId::Kind::CompSolid:
      return aRefs.Solid(aRefs.SolidRefIdsOf(BRepGraph_CompSolidId(theParent.Index)).Value(theRefIdx))
        .Orientation;
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid = aTopo.Solids().Definition(BRepGraph_SolidId(theParent.Index));
      if (theRefIdx < aSolid.ShellRefIds.Length())
      {
        return aRefs.Shell(aSolid.ShellRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Child(aSolid.FreeChildRefIds.Value(theRefIdx - aSolid.ShellRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell = aTopo.Shells().Definition(BRepGraph_ShellId(theParent.Index));
      if (theRefIdx < aShell.FaceRefIds.Length())
      {
        return aRefs.Face(aShell.FaceRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Child(aShell.FreeChildRefIds.Value(theRefIdx - aShell.FaceRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace = aTopo.Faces().Definition(BRepGraph_FaceId(theParent.Index));
      if (theRefIdx < aFace.WireRefIds.Length())
      {
        return aRefs.Wire(aFace.WireRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Vertex(aFace.VertexRefIds.Value(theRefIdx - aFace.WireRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Wire:
      return TopAbs_FORWARD;
    case BRepGraph_NodeId::Kind::Edge: {
      NCollection_Vector<BRepGraph_VertexRefId> aVertexRefIds =
        aRefs.VertexRefIdsOf(BRepGraph_EdgeId(theParent.Index), myGraph->Allocator());
      return aRefs.Vertex(aVertexRefIds.Value(theRefIdx)).Orientation;
    }
    default:
      return TopAbs_FORWARD;
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ParentExplorer::resolveChild(const BRepGraph_NodeId theParent,
                                                        const int              theRefIdx) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return aRefs.Occurrence(aRefs.OccurrenceRefIdsOf(BRepGraph_ProductId(theParent.Index)).Value(theRefIdx))
        .OccurrenceDefId;
    case BRepGraph_NodeId::Kind::Compound:
      return aRefs.Child(aRefs.ChildRefIdsOf(BRepGraph_CompoundId(theParent.Index)).Value(theRefIdx))
        .ChildDefId;
    case BRepGraph_NodeId::Kind::CompSolid:
      return aRefs.Solid(aRefs.SolidRefIdsOf(BRepGraph_CompSolidId(theParent.Index)).Value(theRefIdx))
        .SolidDefId;
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid = aTopo.Solids().Definition(BRepGraph_SolidId(theParent.Index));
      if (theRefIdx < aSolid.ShellRefIds.Length())
      {
        return aRefs.Shell(aSolid.ShellRefIds.Value(theRefIdx)).ShellDefId;
      }
      return aRefs.Child(aSolid.FreeChildRefIds.Value(theRefIdx - aSolid.ShellRefIds.Length()))
        .ChildDefId;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell = aTopo.Shells().Definition(BRepGraph_ShellId(theParent.Index));
      if (theRefIdx < aShell.FaceRefIds.Length())
      {
        return aRefs.Face(aShell.FaceRefIds.Value(theRefIdx)).FaceDefId;
      }
      return aRefs.Child(aShell.FreeChildRefIds.Value(theRefIdx - aShell.FaceRefIds.Length()))
        .ChildDefId;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace = aTopo.Faces().Definition(BRepGraph_FaceId(theParent.Index));
      if (theRefIdx < aFace.WireRefIds.Length())
      {
        return aRefs.Wire(aFace.WireRefIds.Value(theRefIdx)).WireDefId;
      }
      return aRefs.Vertex(aFace.VertexRefIds.Value(theRefIdx - aFace.WireRefIds.Length()))
        .VertexDefId;
    }
    case BRepGraph_NodeId::Kind::Wire:
      return aRefs.CoEdge(aRefs.CoEdgeRefIdsOf(BRepGraph_WireId(theParent.Index)).Value(theRefIdx))
        .CoEdgeDefId;
    case BRepGraph_NodeId::Kind::Edge: {
      NCollection_Vector<BRepGraph_VertexRefId> aVertexRefIds =
        aRefs.VertexRefIdsOf(BRepGraph_EdgeId(theParent.Index), myGraph->Allocator());
      return aRefs.Vertex(aVertexRefIds.Value(theRefIdx)).VertexDefId;
    }
    default:
      return BRepGraph_NodeId();
  }
}