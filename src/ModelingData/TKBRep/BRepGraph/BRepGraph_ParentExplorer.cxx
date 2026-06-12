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

#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>

#include <algorithm>

namespace
{
static bool parentExplorerKindCanContainDescendant(const BRepGraph_NodeId::Kind theParentKind,
                                                   const BRepGraph_NodeId::Kind theTargetKind)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (theParentKind)
  {
    case Kind::Product:
      return theTargetKind == Kind::Occurrence || theTargetKind == Kind::Product
             || BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::Occurrence:
      return theTargetKind == Kind::Occurrence || theTargetKind == Kind::Product
             || BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::Compound:
      return BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::CompSolid:
      return theTargetKind == Kind::Solid || theTargetKind == Kind::Shell
             || theTargetKind == Kind::Face || theTargetKind == Kind::Wire
             || theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::Solid:
      return theTargetKind == Kind::Shell || theTargetKind == Kind::Face
             || theTargetKind == Kind::Wire || theTargetKind == Kind::CoEdge
             || theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Shell:
      return theTargetKind == Kind::Face || theTargetKind == Kind::Wire
             || theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::Face:
      return theTargetKind == Kind::Wire || theTargetKind == Kind::CoEdge
             || theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Wire:
      return theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::CoEdge:
      return theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Edge:
      return theTargetKind == Kind::Vertex;
    case Kind::Vertex:
      return false;
  }
  return false;
}

template <typename IteratorT, typename IdT>
static bool nthParentId(IteratorT theIterator, uint32_t& theIndex, IdT& theId)
{
  for (; theIterator.More(); theIterator.Next())
  {
    if (theIndex == 0u)
    {
      theId = theIterator.CurrentId();
      return true;
    }
    --theIndex;
  }
  return false;
}

} // namespace

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   const Config&          theConfig)
    : myGraph(&theGraph),
      myNode(theNode),
      myConfig(theConfig)
{
  myConfig.AvoidKind = normalizeAvoidKind(theNode, theConfig.TargetKind, theConfig.AvoidKind);
  startTraversal();
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode)
    : BRepGraph_ParentExplorer(theGraph, theNode, Config{})
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   const TraversalMode    theMode)
    : BRepGraph_ParentExplorer(theGraph, theNode, Config{theMode, {}, {}, false})
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theNode,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ParentExplorer(theGraph,
                               theNode,
                               Config{theMode, {}, theAvoidKind, theEmitAvoidKind})
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ParentExplorer(theGraph, theNode, theTargetKind, TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   BRepGraph_NodeId::Kind theTargetKind,
                                                   const TraversalMode    theMode)
    : BRepGraph_ParentExplorer(theGraph, theNode, Config{theMode, theTargetKind, {}, false})
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theNode,
  BRepGraph_NodeId::Kind                       theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ParentExplorer(theGraph,
                               theNode,
                               Config{theMode, theTargetKind, theAvoidKind, theEmitAvoidKind})
{
}

//=================================================================================================

void BRepGraph_ParentExplorer::startTraversal()
{
  myStackTop     = -1;
  myEmitIndex    = -1;
  myCurrentFrame = -1;
  myCurrent      = BRepGraph_NodeId();
  myLocation     = TopLoc_Location();
  myOrientation  = TopAbs_FORWARD;
  myHasMore      = false;
  if (myNode.IsValid())
  {
    StackFrame aStartFrame;
    aStartFrame.Node = myNode;
    pushFrame(aStartFrame);
  }
  advance();
}

//=================================================================================================

void BRepGraph_ParentExplorer::Next()
{
  advance();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ParentExplorer::CurrentChild() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return BRepGraph_NodeId();
  }

  return myStack[myCurrentFrame - 1].Node;
}

//=================================================================================================

BRepGraph_ParentExplorer::LinkKind BRepGraph_ParentExplorer::CurrentLinkKind() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return LinkKind::None;
  }

  return myStack[myCurrentFrame].StepToChild >= 0 ? LinkKind::Reference : LinkKind::Structural;
}

//=================================================================================================

BRepGraph_RefId BRepGraph_ParentExplorer::CurrentRef() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return BRepGraph_RefId();
  }

  const StackFrame& aFrame = myStack[myCurrentFrame];
  if (aFrame.StepToChild < 0)
  {
    return BRepGraph_RefId();
  }

  if (aFrame.RefToChild.IsValid())
  {
    return aFrame.RefToChild;
  }

  return myGraph->Refs().Gen().RefAtStep(aFrame.Node, aFrame.StepToChild);
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ParentExplorer::LeafLocation() const
{
  static const TopLoc_Location THE_EMPTY_LOCATION;
  return myHasMore ? myStack[0].AccLocation : THE_EMPTY_LOCATION;
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::LeafOrientation() const
{
  return myHasMore ? myStack[0].AccOrientation : TopAbs_FORWARD;
}

//=================================================================================================

bool BRepGraph_ParentExplorer::IsCurrentBranchRoot() const
{
  return myHasMore && myCurrentFrame == branchRootFrame();
}

//=================================================================================================

void BRepGraph_ParentExplorer::advance()
{
  myHasMore      = false;
  myCurrent      = BRepGraph_NodeId();
  myCurrentFrame = -1;

  if (myConfig.Mode == TraversalMode::DirectParents)
  {
    if (myStackTop > 0)
    {
      popFrame();
    }

    while (myStackTop >= 0)
    {
      StackFrame aParentFrame;
      if (!nextParentFrame(topFrame(), aParentFrame))
      {
        popFrame();
        break;
      }

      pushFrame(aParentFrame);
      prepareCurrentBranch();

      if (!shouldEmit(topFrame().Node))
      {
        popFrame();
        continue;
      }

      myCurrentFrame = myStackTop;
      myCurrent      = topFrame().Node;
      myLocation     = topFrame().AccLocation;
      myOrientation  = topFrame().AccOrientation;
      myHasMore      = true;
      return;
    }
    return;
  }

  if (myEmitIndex >= 1)
  {
    if (emitNextFromCurrentBranch())
    {
      return;
    }
    backtrackAfterBranchEmission();
  }

  while (myStackTop >= 0)
  {
    StackFrame aParentFrame;
    if (nextParentFrame(topFrame(), aParentFrame))
    {
      if (matchesAvoid(aParentFrame.Node))
      {
        pushFrame(aParentFrame);
        prepareCurrentBranch();
        myEmitIndex = 1;
        if (emitNextFromCurrentBranch())
        {
          return;
        }
        popFrame();
        myEmitIndex = -1;
        continue;
      }

      pushFrame(aParentFrame);
      continue;
    }

    if (myStackTop > 0)
    {
      prepareCurrentBranch();
      myEmitIndex = 1;
      if (emitNextFromCurrentBranch())
      {
        return;
      }
      popFrame();
      myEmitIndex = -1;
      continue;
    }

    popFrame();
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::backtrackAfterBranchEmission()
{
  myEmitIndex = -1;
  while (myStackTop >= 0)
  {
    popFrame();
    if (myStackTop < 0)
    {
      return;
    }

    StackFrame aProbe = topFrame();
    StackFrame aNextFrame;
    if (nextParentFrame(aProbe, aNextFrame))
    {
      return;
    }
  }
}

//=================================================================================================

bool BRepGraph_ParentExplorer::emitNextFromCurrentBranch()
{
  while (myEmitIndex >= 1 && myEmitIndex <= myStackTop)
  {
    const int         aFrameIdx = myEmitIndex++;
    const StackFrame& aFrame    = myStack[aFrameIdx];
    if (!shouldEmit(aFrame.Node))
    {
      continue;
    }

    myCurrentFrame = aFrameIdx;
    myCurrent      = aFrame.Node;
    myLocation     = aFrame.AccLocation;
    myOrientation  = aFrame.AccOrientation;
    myHasMore      = true;
    return true;
  }
  return false;
}

//=================================================================================================

std::optional<BRepGraph_NodeId::Kind> BRepGraph_ParentExplorer::normalizeAvoidKind(
  const BRepGraph_NodeId                       theNode,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind)
{
  if (!theAvoidKind.has_value() || !theNode.IsValid())
  {
    return theAvoidKind;
  }

  if (!theTargetKind.has_value())
  {
    return theAvoidKind;
  }

  if (*theAvoidKind == *theTargetKind)
  {
    return std::nullopt;
  }

  if (!canContainTarget(*theTargetKind, *theAvoidKind))
  {
    return std::nullopt;
  }

  return (theNode.NodeKind == *theAvoidKind || canContainTarget(*theAvoidKind, theNode.NodeKind))
           ? theAvoidKind
           : std::nullopt;
}

//=================================================================================================

bool BRepGraph_ParentExplorer::canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                                                const BRepGraph_NodeId::Kind theTargetKind)
{
  return parentExplorerKindCanContainDescendant(theParentKind, theTargetKind);
}

//=================================================================================================

bool BRepGraph_ParentExplorer::nextParentFrame(StackFrame& theChild, StackFrame& theParent) const
{
  using Kind = BRepGraph_NodeId::Kind;

  const BRepGraph::TopoView& aTopo = myGraph->Topo();

  for (;;)
  {
    const uint32_t aParentIdx = theChild.NextParentIdx++;

    switch (theChild.Node.NodeKind)
    {
      case Kind::Vertex: {
        const BRepGraph_VertexId      aVertexId(theChild.Node);
        const BRepGraph_EdgesOfVertex anEdges(*myGraph, aTopo.Vertices().Edges(aVertexId));
        const uint32_t                aNbEdges = static_cast<uint32_t>(anEdges.Size());
        if (aParentIdx < aNbEdges)
        {
          const BRepGraph_EdgeId anEdgeId = anEdges.Value(aParentIdx);
          if (anEdgeId.IsRemoved(*myGraph))
          {
            continue;
          }
          const int aStepToChild = findEdgeVertexStep(anEdgeId, aVertexId);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = anEdgeId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aParentIdx - aNbEdges, theParent);
      }

      case Kind::Edge: {
        const BRepGraph_EdgeId        aEdgeId(theChild.Node);
        const BRepGraph_CoEdgesOfEdge aCoEdges(*myGraph, aTopo.Edges().CoEdges(aEdgeId));
        const uint32_t                aNbCoEdges = static_cast<uint32_t>(aCoEdges.Size());
        if (aParentIdx < aNbCoEdges)
        {
          const BRepGraph_CoEdgeId aCoEdgeId = aCoEdges.Value(aParentIdx);
          if (BRepGraph_NodeId(aCoEdgeId).IsRemoved(*myGraph))
          {
            continue;
          }
          theParent.Node          = aCoEdgeId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aParentIdx - aNbCoEdges, theParent);
      }

      case Kind::Wire: {
        const BRepGraph_WireId      aWireId(theChild.Node);
        const BRepGraph_FacesOfWire aFaces(*myGraph,
                                           aTopo.Wires().Relations(aWireId).ParentWireRefIds);
        uint32_t                    aRemainingParentIdx = aParentIdx;
        BRepGraph_FaceId            aFaceId;
        if (nthParentId(aFaces, aRemainingParentIdx, aFaceId))
        {
          const int aStepToChild = findFaceChildStep(aFaceId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = aFaceId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aRemainingParentIdx, theParent);
      }

      case Kind::Face: {
        const BRepGraph_FaceId       aFaceId(theChild.Node);
        const BRepGraph_ShellsOfFace aShells(*myGraph,
                                             aTopo.Faces().Relations(aFaceId).ParentFaceRefIds);
        uint32_t                     aRemainingParentIdx = aParentIdx;
        BRepGraph_ShellId            aShellId;
        if (nthParentId(aShells, aRemainingParentIdx, aShellId))
        {
          const int aStepToChild = findShellChildStep(aShellId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = aShellId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aRemainingParentIdx, theParent);
      }

      case Kind::Shell: {
        const BRepGraph_ShellId       aShellId = BRepGraph_ShellId(theChild.Node);
        const BRepGraph_SolidsOfShell aSolids(*myGraph,
                                              aTopo.Shells().Relations(aShellId).ParentShellRefIds);
        uint32_t                      aRemainingParentIdx = aParentIdx;
        BRepGraph_SolidId             aSolidId;
        if (nthParentId(aSolids, aRemainingParentIdx, aSolidId))
        {
          const int aStepToChild = findSolidChildStep(aSolidId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = aSolidId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aRemainingParentIdx, theParent);
      }

      case Kind::Solid: {
        const BRepGraph_SolidId           aSolidId = BRepGraph_SolidId(theChild.Node);
        const BRepGraph_CompSolidsOfSolid aCompSolids(
          *myGraph,
          aTopo.Solids().Relations(aSolidId).ParentSolidRefIds);
        uint32_t              aRemainingParentIdx = aParentIdx;
        BRepGraph_CompSolidId aCompSolidId;
        if (nthParentId(aCompSolids, aRemainingParentIdx, aCompSolidId))
        {
          const int aStepToChild = findCompSolidSolidStep(aCompSolidId, aSolidId);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = aCompSolidId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aRemainingParentIdx, theParent);
      }

      case Kind::Compound:
        return nextCompoundOrOccurrenceParent(theChild.Node, aParentIdx, theParent);

      case Kind::CompSolid:
        return nextCompoundOrOccurrenceParent(theChild.Node, aParentIdx, theParent);

      case Kind::CoEdge: {
        const BRepGraph_CoEdgeId aCoEdgeId(theChild.Node);
        const BRepGraph_WireId   aWireId  = aTopo.CoEdges().Wire(aCoEdgeId);
        const uint32_t           aNbWires = aWireId.IsValid() ? 1u : 0u;
        if (aParentIdx < aNbWires)
        {
          if (aWireId.IsRemoved(*myGraph))
          {
            continue;
          }
          const int aStepToChild = findWireCoEdgeStep(aWireId, aCoEdgeId);
          if (aStepToChild < 0)
          {
            continue;
          }
          theParent.Node          = aWireId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }
        return nextCompoundOrOccurrenceParent(theChild.Node, aParentIdx - aNbWires, theParent);
      }

      case Kind::Product: {
        const BRepGraph_ProductId aProductId(theChild.Node);
        if (!aTopo.Gen().HasOccurrenceParents(BRepGraph_NodeId(aProductId)))
        {
          return false;
        }
        const BRepGraph_OccurrencesOfProduct anOccurrences(
          *myGraph,
          aTopo.Gen().OccurrenceRefIds(BRepGraph_NodeId(aProductId)));
        uint32_t               aRemainingParentIdx = aParentIdx;
        BRepGraph_OccurrenceId anOccurrenceId;
        if (!nthParentId(anOccurrences, aRemainingParentIdx, anOccurrenceId))
        {
          return false;
        }
        theParent.Node          = anOccurrenceId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Occurrence: {
        const BRepGraph_OccurrenceId         aOccurrenceId(theChild.Node);
        const BRepGraph_ProductsOfOccurrence aParents(
          *myGraph,
          aTopo.Occurrences().Relations(aOccurrenceId).ParentOccurrenceRefIds);
        uint32_t            aRemainingParentIdx = aParentIdx;
        BRepGraph_ProductId aProductId;
        if (!nthParentId(aParents, aRemainingParentIdx, aProductId))
        {
          return false;
        }
        BRepGraph_OccurrenceRefId anOccurrenceRefId;
        const int aStepToChild = findOccurrenceStep(aProductId, aOccurrenceId, &anOccurrenceRefId);
        if (aStepToChild < 0)
        {
          continue;
        }
        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = aStepToChild;
        theParent.RefToChild    = anOccurrenceRefId;
        return true;
      }

      default:
        return false;
    }
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::prepareCurrentBranch()
{
  if (myStackTop < 0)
  {
    return;
  }

  myStack[myStackTop].AccLocation    = TopLoc_Location();
  myStack[myStackTop].AccOrientation = TopAbs_FORWARD;
  for (int aFrameIdx = myStackTop - 1; aFrameIdx >= 0; --aFrameIdx)
  {
    myStack[aFrameIdx].AccLocation    = myStack[aFrameIdx + 1].AccLocation;
    myStack[aFrameIdx].AccOrientation = myStack[aFrameIdx + 1].AccOrientation;
    applyTransition(myStack[aFrameIdx + 1].Node,
                    myStack[aFrameIdx].Node,
                    myStack[aFrameIdx + 1].StepToChild,
                    myStack[aFrameIdx + 1].RefToChild,
                    myStack[aFrameIdx].AccLocation,
                    myStack[aFrameIdx].AccOrientation);
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::applyTransition(const BRepGraph_NodeId theParent,
                                               const BRepGraph_NodeId theChild,
                                               const int              theStepToChild,
                                               const BRepGraph_RefId  theRefToChild,
                                               TopLoc_Location&       theLocation,
                                               TopAbs_Orientation&    theOrientation) const
{
  if (theRefToChild.IsValid())
  {
    const BRepGraph::RefsView& aRefs = myGraph->Refs();
    theLocation                      = theLocation * aRefs.Gen().LocalLocation(theRefToChild);
    if (theRefToChild.RefKind != BRepGraph_RefId::Kind::Occurrence)
    {
      theOrientation = TopAbs::Compose(theOrientation, aRefs.Gen().Orientation(theRefToChild));
    }
    return;
  }

  if (theStepToChild >= 0)
  {
    theLocation    = theLocation * stepLocation(theParent, theStepToChild);
    theOrientation = TopAbs::Compose(theOrientation, stepOrientation(theParent, theStepToChild));
    return;
  }

  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Occurrence: {
      if (theChild.NodeKind != BRepGraph_NodeId::Kind::Product
          && !BRepGraph_NodeId::IsTopologyKind(theChild.NodeKind))
      {
        Standard_ASSERT_VOID(false, "ParentExplorer: invalid Occurrence structural child");
      }
      // The placement is owned by the parent Product -> OccurrenceRef edge.
      // Occurrence -> Product is structural and must not compose it again.
      return;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      if (theChild.NodeKind != BRepGraph_NodeId::Kind::CoEdge)
      {
        Standard_ASSERT_VOID(false, "ParentExplorer: invalid Wire structural child");
      }
      return;
    }

    case BRepGraph_NodeId::Kind::CoEdge: {
      if (theChild.NodeKind != BRepGraph_NodeId::Kind::Edge)
      {
        Standard_ASSERT_VOID(false, "ParentExplorer: invalid CoEdge structural child");
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId(theParent));
      if (!BRepGraph_CoEdgeId(theParent).IsRemoved(*myGraph))
      {
        theOrientation = TopAbs::Compose(theOrientation, aCoEdge.Orientation);
      }
      return;
    }

    default:
      Standard_ASSERT_VOID(false, "ParentExplorer: missing structural transition");
      return;
  }
}

//=================================================================================================

int BRepGraph_ParentExplorer::branchRootFrame() const
{
  if (myStackTop < 0)
  {
    return -1;
  }

  int aFrameIdx = myStackTop;
  while (aFrameIdx > 0 && myStack[aFrameIdx].StepToChild < 0)
  {
    --aFrameIdx;
  }
  return aFrameIdx;
}

//=================================================================================================

bool BRepGraph_ParentExplorer::findNthOccurrenceWrapper(
  const BRepGraph_NodeId     theNode,
  const uint32_t             theOrdinal,
  BRepGraph_OccurrenceId&    theOccurrence,
  BRepGraph_OccurrenceRefId& theOccurrenceRef) const
{
  const BRepGraph::TopoView&                                 aTopo  = myGraph->Topo();
  const BRepGraph::RefsView&                                 aRefs  = myGraph->Refs();
  uint32_t                                                   aCount = 0;
  const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& anOccurrenceRefs =
    aTopo.Gen().OccurrenceRefIds(theNode);
  for (const BRepGraph_OccurrenceRefId& anOccurrenceRefId : anOccurrenceRefs)
  {
    if (anOccurrenceRefId.IsRemoved(*myGraph))
    {
      continue;
    }
    const BRepGraph_OccurrenceId anOccurrenceId =
      aRefs.Occurrences().Entry(anOccurrenceRefId).ChildOccurrenceId;
    const BRepGraphInc::OccurrenceDef& anOccDef = aTopo.Occurrences().Definition(anOccurrenceId);
    if (anOccurrenceId.IsRemoved(*myGraph) || anOccDef.ChildNodeId != theNode)
    {
      continue;
    }

    if (aCount == theOrdinal)
    {
      theOccurrence    = anOccurrenceId;
      theOccurrenceRef = anOccurrenceRefId;
      return true;
    }
    ++aCount;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_ParentExplorer::nextCompoundOrOccurrenceParent(const BRepGraph_NodeId theNode,
                                                              const uint32_t theRemainingIdx,
                                                              StackFrame&    theParent) const
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();

  uint32_t aConsumedIdx = 0;
  if (aTopo.Gen().HasCompoundParents(theNode))
  {
    for (BRepGraph_ReverseIterator::IdsOfRefs<BRepGraph_ReverseIterator::CompoundFromChildRefTraits>
           aCompounds(*myGraph, aTopo.Gen().CompoundRefIds(theNode));
         aCompounds.More();
         aCompounds.Next())
    {
      const BRepGraph_CompoundId aCompoundId = aCompounds.CurrentId();
      if (aCompoundId.IsRemoved(*myGraph))
      {
        continue;
      }
      if (aConsumedIdx < theRemainingIdx)
      {
        ++aConsumedIdx;
        continue;
      }
      const int aStepToChild = findCompoundChildStep(aCompoundId, theNode);
      if (aStepToChild < 0)
      {
        continue;
      }
      theParent.Node          = aCompoundId;
      theParent.NextParentIdx = 0;
      theParent.StepToChild   = aStepToChild;
      return true;
    }
  }

  if (!aTopo.Gen().HasOccurrenceParents(theNode))
  {
    return false;
  }
  BRepGraph_OccurrenceId    anOccurrenceId;
  BRepGraph_OccurrenceRefId anOccurrenceRefId;
  if (!findNthOccurrenceWrapper(theNode,
                                theRemainingIdx - aConsumedIdx,
                                anOccurrenceId,
                                anOccurrenceRefId))
  {
    return false;
  }

  theParent.Node          = anOccurrenceId;
  theParent.NextParentIdx = 0;
  theParent.StepToChild   = -1;
  theParent.RefToChild    = anOccurrenceRefId;
  return true;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findOccurrenceStep(const BRepGraph_ProductId    theParentProduct,
                                                 const BRepGraph_OccurrenceId theOccurrence,
                                                 BRepGraph_OccurrenceRefId* theOccurrenceRef) const
{
  int aStep = 0;
  for (BRepGraph_RefsOccurrenceOfProduct aRefIt(*myGraph, theParentProduct); aRefIt.More();
       aRefIt.Next())
  {
    if (myGraph->Refs().Gen().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theOccurrence))
    {
      if (theOccurrenceRef != nullptr)
      {
        *theOccurrenceRef = aRefIt.CurrentId();
      }
      return aStep;
    }
    ++aStep;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findCompoundChildStep(const BRepGraph_CompoundId theParent,
                                                    const BRepGraph_NodeId     theChild) const
{
  int aStep = 0;
  for (BRepGraph_RefsChildOfCompound aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    if (myGraph->Refs().Gen().ChildNode(aRefIt.CurrentId()) == theChild)
    {
      return aStep;
    }
    ++aStep;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findCompSolidSolidStep(const BRepGraph_CompSolidId theParent,
                                                     const BRepGraph_SolidId     theChild) const
{
  int aStep = 0;
  for (BRepGraph_RefsSolidOfCompSolid aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    if (myGraph->Refs().Gen().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theChild))
    {
      return aStep;
    }
    ++aStep;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findSolidChildStep(const BRepGraph_SolidId theParent,
                                                 const BRepGraph_NodeId  theChild) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Shell)
  {
    int aStep = 0;
    for (BRepGraph_RefsShellOfSolid aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.Gen().ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findShellChildStep(const BRepGraph_ShellId theParent,
                                                 const BRepGraph_NodeId  theChild) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Face)
  {
    int aStep = 0;
    for (BRepGraph_RefsFaceOfShell aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.Gen().ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findFaceChildStep(const BRepGraph_FaceId theParent,
                                                const BRepGraph_NodeId theChild) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Wire)
  {
    int aStep = 0;
    for (BRepGraph_RefsWireOfFace aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.Gen().ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findWireCoEdgeStep(const BRepGraph_WireId   theParent,
                                                 const BRepGraph_CoEdgeId theChild) const
{
  int aStep = 0;
  for (BRepGraph_CoEdgesOfWire aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    if (aRefIt.CurrentId() == theChild)
    {
      return aStep;
    }
    ++aStep;
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findEdgeVertexStep(const BRepGraph_EdgeId   theParent,
                                                 const BRepGraph_VertexId theChild) const
{
  const BRepGraphInc::EdgeDef& anEdge = myGraph->Topo().Edges().Definition(theParent);
  if (anEdge.StartVertexRefId.IsValid()
      && myGraph->Refs().Gen().ChildNode(anEdge.StartVertexRefId) == BRepGraph_NodeId(theChild))
  {
    return 0;
  }
  if (anEdge.EndVertexRefId.IsValid()
      && myGraph->Refs().Gen().ChildNode(anEdge.EndVertexRefId) == BRepGraph_NodeId(theChild))
  {
    return 1;
  }
  return -1;
}

//=================================================================================================

void BRepGraph_ParentExplorer::pushFrame(const StackFrame& theFrame)
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const uint32_t aMaxDepth = aTopo.Compounds().Nb() + aTopo.CompSolids().Nb() + aTopo.Solids().Nb()
                             + aTopo.Shells().Nb() + aTopo.Faces().Nb() + aTopo.Wires().Nb()
                             + aTopo.Edges().Nb() + aTopo.Vertices().Nb() + aTopo.Products().Nb()
                             + aTopo.Occurrences().Nb() + aTopo.CoEdges().Nb();
  if (myStackTop >= 0 && static_cast<uint32_t>(myStackTop) >= aMaxDepth)
  {
    return;
  }

  ++myStackTop;
  if (static_cast<size_t>(myStackTop) >= myStack.Size())
  {
    const size_t aNewSize =
      std::max<size_t>(myStack.Size() * 2, static_cast<size_t>(THE_INLINE_STACK_SIZE));
    myStack.Reallocate(aNewSize, true);
  }
  myStack[myStackTop] = theFrame;
}

//=================================================================================================

void BRepGraph_ParentExplorer::popFrame()
{
  if (myStackTop >= 0)
  {
    --myStackTop;
  }
}

//=================================================================================================

TopLoc_Location BRepGraph_ParentExplorer::stepLocation(const BRepGraph_NodeId theParent,
                                                       const int              theRefIdx) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  return aRefs.Gen().LocalLocation(aRefs.Gen().RefAtStep(theParent, theRefIdx));
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::stepOrientation(const BRepGraph_NodeId theParent,
                                                             const int              theRefIdx) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  return aRefs.Gen().Orientation(aRefs.Gen().RefAtStep(theParent, theRefIdx));
}
