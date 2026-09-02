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

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>

#include <algorithm>
#include <utility>

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

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&             theGraph,
                                                   const BRepGraph_NodeId       theNode,
                                                   const BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ParentExplorer(theGraph, theNode, theTargetKind, TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&             theGraph,
                                                   const BRepGraph_NodeId       theNode,
                                                   const BRepGraph_NodeId::Kind theTargetKind,
                                                   const TraversalMode          theMode)
    : BRepGraph_ParentExplorer(theGraph, theNode, Config{theMode, theTargetKind, {}, false})
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theNode,
  const BRepGraph_NodeId::Kind                 theTargetKind,
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
  myLocationTop  = -1;
  myHasMore      = false;
  myMaxDepth     = myGraph->Topo().Gen().NbNodes();
  if (myNode.IsValid())
  {
    (void)pushFrame(StackFrame{myNode,
                               BRepGraph_ParentIterator::Position(),
                               -1,
                               BRepGraph_RefId(),
                               -1,
                               TopAbs_FORWARD});
  }
  advance();
}

//=================================================================================================

void BRepGraph_ParentExplorer::Next()
{
  advance();
}

//=================================================================================================

BRepGraphInc::NodeInstance BRepGraph_ParentExplorer::Current() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return BRepGraphInc::NodeInstance();
  }
  const StackFrame& aFrame = myStack[myCurrentFrame];
  return {aFrame.Node, frameLocation(aFrame), aFrame.AccOrientation};
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ParentExplorer::CurrentLocation() const
{
  static const TopLoc_Location THE_IDENTITY;
  return myHasMore && myCurrentFrame >= 0 ? frameLocation(myStack[myCurrentFrame]) : THE_IDENTITY;
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::CurrentOrientation() const
{
  return myHasMore && myCurrentFrame >= 0 ? myStack[myCurrentFrame].AccOrientation : TopAbs_FORWARD;
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
  return myHasMore ? frameLocation(myStack[0]) : THE_EMPTY_LOCATION;
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

      if (!pushFrame(std::move(aParentFrame)))
      {
        continue;
      }
      prepareCurrentBranch();

      if (!shouldEmit(topFrame().Node))
      {
        popFrame();
        continue;
      }

      myCurrentFrame = myStackTop;
      myCurrent      = topFrame().Node;
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
        if (!pushFrame(std::move(aParentFrame)))
        {
          continue;
        }
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

      (void)pushFrame(std::move(aParentFrame));
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
  BRepGraph_ParentIterator anIt(*myGraph, theChild.Node, theChild.NextParentPosition);
  if (!anIt.More())
  {
    return false;
  }

  const BRepGraph_ParentIterator::ValueType& aTransition = anIt.Current();
  theChild.NextParentPosition                            = anIt.CurrentPosition();
  theParent = StackFrame{aTransition.Parent,
                         BRepGraph_ParentIterator::Position(),
                         aTransition.Link == BRepGraph_ParentIterator::LinkKind::Reference
                           ? static_cast<int>(aTransition.StepIndex)
                           : -1,
                         aTransition.Ref};
  return true;
}

//=================================================================================================

void BRepGraph_ParentExplorer::prepareCurrentBranch()
{
  if (myStackTop < 0)
  {
    return;
  }

  myLocationTop                      = -1;
  myStack[myStackTop].LocationIndex  = pushLocation(TopLoc_Location());
  myStack[myStackTop].AccOrientation = TopAbs_FORWARD;
  for (int aFrameIdx = myStackTop - 1; aFrameIdx >= 0; --aFrameIdx)
  {
    myStack[aFrameIdx].LocationIndex  = myStack[aFrameIdx + 1].LocationIndex;
    myStack[aFrameIdx].AccOrientation = myStack[aFrameIdx + 1].AccOrientation;
    applyTransition(myStack[aFrameIdx + 1].Node,
                    myStack[aFrameIdx].Node,
                    myStack[aFrameIdx + 1].StepToChild,
                    myStack[aFrameIdx + 1].RefToChild,
                    myStack[aFrameIdx].LocationIndex,
                    myStack[aFrameIdx].AccOrientation);
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::applyTransition(const BRepGraph_NodeId theParent,
                                               const BRepGraph_NodeId theChild,
                                               const int              theStepToChild,
                                               const BRepGraph_RefId  theRefToChild,
                                               int&                   theLocationIndex,
                                               TopAbs_Orientation&    theOrientation)
{
  const auto applyReference = [&](const BRepGraph_RefId theRef) {
    if (!theRef.IsValid())
    {
      return;
    }

    const BRepGraph::RefsView& aRefs = myGraph->Refs();
    if (theRef.RefKind == BRepGraph_RefId::Kind::Child
        || theRef.RefKind == BRepGraph_RefId::Kind::Occurrence)
    {
      const TopLoc_Location& aLocalLocation = aRefs.Gen().LocalLocation(theRef);
      if (!aLocalLocation.IsIdentity())
      {
        const TopLoc_Location anAccumulated =
          theLocationIndex >= 0 ? myLocations[theLocationIndex].Multiplied(aLocalLocation)
                                : aLocalLocation;
        theLocationIndex = pushLocation(anAccumulated);
      }
    }
    if (theRef.RefKind != BRepGraph_RefId::Kind::Occurrence)
    {
      theOrientation = TopAbs::Compose(theOrientation, aRefs.Gen().Orientation(theRef));
    }
  };

  if (theRefToChild.IsValid())
  {
    applyReference(theRefToChild);
    return;
  }

  if (theStepToChild >= 0)
  {
    applyReference(myGraph->Refs().Gen().RefAtStep(theParent, theStepToChild));
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

bool BRepGraph_ParentExplorer::pushFrame(StackFrame&& theFrame)
{
  if (myMaxDepth == 0 || static_cast<uint32_t>(myStackTop + 1) >= myMaxDepth)
  {
    return false;
  }

  ++myStackTop;
  if (static_cast<size_t>(myStackTop) >= myStack.Size())
  {
    const size_t aNewSize =
      std::max<size_t>(myStack.Size() * 2, static_cast<size_t>(THE_INLINE_STACK_SIZE));
    myStack.Reallocate(aNewSize, true);
  }
  myStack[myStackTop] = std::move(theFrame);
  return true;
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

int BRepGraph_ParentExplorer::pushLocation(TopLoc_Location theLocation)
{
  if (theLocation.IsIdentity())
  {
    return -1;
  }
  ++myLocationTop;
  if (static_cast<size_t>(myLocationTop) >= myLocations.Size())
  {
    const size_t aNewSize =
      std::max<size_t>(myLocations.Size() * 2, static_cast<size_t>(THE_INLINE_LOCATION_SIZE));
    myLocations.Reallocate(aNewSize, true);
  }
  myLocations[myLocationTop] = std::move(theLocation);
  return myLocationTop;
}
