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

#include <BRepGraph_ChildExplorer.hxx>

#include <BRepGraph_ChildIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <TopAbs.hxx>

#include <algorithm>
#include <utility>

namespace
{
static bool childExplorerKindCanContainDescendant(const BRepGraph_NodeId::Kind theParentKind,
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

static BRepGraph_ChildExplorer::Config childExplorerConfig(
  const BRepGraph_ChildExplorer::TraversalMode theMode,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind         = std::nullopt,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind          = std::nullopt,
  const bool                                   theEmitAvoidKind      = false,
  const bool                                   theAccumulateLocation = true,
  const bool                                   theAccumulateOri      = true,
  const TopLoc_Location&                       theStartLoc           = TopLoc_Location(),
  const TopAbs_Orientation                     theStartOri           = TopAbs_FORWARD)
{
  BRepGraph_ChildExplorer::Config aConfig;
  aConfig.Mode                  = theMode;
  aConfig.TargetKind            = theTargetKind;
  aConfig.AvoidKind             = theAvoidKind;
  aConfig.EmitAvoidKind         = theEmitAvoidKind;
  aConfig.AccumulateLocation    = theAccumulateLocation;
  aConfig.AccumulateOrientation = theAccumulateOri;
  aConfig.StartLoc              = theStartLoc;
  aConfig.StartOri              = theStartOri;
  return aConfig;
}

} // namespace

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 const Config&          theConfig)
    : myGraph(&theGraph),
      myRoot(theRoot),
      myConfig(theConfig)
{
  myConfig.AvoidKind = normalizeAvoidKind(theConfig.AvoidKind, theConfig.TargetKind);
  startTraversal(myConfig.StartLoc, myConfig.StartOri);
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot)
    : BRepGraph_ChildExplorer(theGraph, theRoot, Config{})
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph, theRoot, childExplorerConfig(theMode))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theRoot,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, std::nullopt, theAvoidKind, theEmitAvoidKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_NodeId       theRoot,
                                                 const BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              childExplorerConfig(TraversalMode::Recursive, theTargetKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_ProductId    theProduct,
                                                 const BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theProduct), theTargetKind)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_NodeId       theRoot,
                                                 const BRepGraph_NodeId::Kind theTargetKind,
                                                 const TraversalMode          theMode)
    : BRepGraph_ChildExplorer(theGraph, theRoot, childExplorerConfig(theMode, theTargetKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theRoot,
  const BRepGraph_NodeId::Kind                 theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, theTargetKind, theAvoidKind, theEmitAvoidKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_ProductId    theProduct,
                                                 const BRepGraph_NodeId::Kind theTargetKind,
                                                 const TraversalMode          theMode)
    : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_NodeId       theRoot,
                                                 const BRepGraph_NodeId::Kind theTargetKind,
                                                 const bool                   theCumLoc,
                                                 const bool                   theCumOri,
                                                 const TraversalMode          theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, theTargetKind, std::nullopt, false, theCumLoc, theCumOri))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_ProductId    theProduct,
                                                 const BRepGraph_NodeId::Kind theTargetKind,
                                                 const bool                   theCumLoc,
                                                 const bool                   theCumOri,
                                                 const TraversalMode          theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              BRepGraph_NodeId(theProduct),
                              theTargetKind,
                              theCumLoc,
                              theCumOri,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                                 const BRepGraph_NodeId       theRoot,
                                                 const BRepGraph_NodeId::Kind theTargetKind,
                                                 const TopLoc_Location&       theStartLoc,
                                                 const TopAbs_Orientation     theStartOri,
                                                 const TraversalMode          theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              childExplorerConfig(theMode,
                                                  theTargetKind,
                                                  std::nullopt,
                                                  false,
                                                  true,
                                                  true,
                                                  theStartLoc,
                                                  theStartOri))
{
}

//=================================================================================================

void BRepGraph_ChildExplorer::startTraversal(const TopLoc_Location&   theStartLoc,
                                             const TopAbs_Orientation theStartOri)
{
  myStackTop     = -1;
  myHasMore      = false;
  myCurrent      = BRepGraph_NodeId();
  myCurrentFrame = -1;
  myLocationTop  = -1;
  myMaxDepth     = myGraph->Topo().Gen().NbNodes();

  if (!myRoot.IsValid())
  {
    return;
  }

  const auto pushRootFrame = [&]() {
    const int aLocationIndex = pushLocation(theStartLoc);
    if (pushFrame(StackFrame{myRoot,
                             0,
                             BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX,
                             BRepGraph_RefId(),
                             aLocationIndex,
                             aLocationIndex >= 0,
                             theStartOri}))
    {
      return true;
    }
    if (aLocationIndex >= 0)
    {
      --myLocationTop;
    }
    return false;
  };

  if (matchesAvoid(myRoot))
  {
    const BRepGraphInc::BaseDef* aRootDef = myGraph->Topo().Gen().TopoEntity(myRoot);
    if (myConfig.TargetKind.has_value() && myConfig.EmitAvoidKind && aRootDef != nullptr
        && !myRoot.IsRemoved(*myGraph) && pushRootFrame())
    {
      setCurrentFromFrame(myStackTop);
    }
    return;
  }

  // Check if root is valid and not removed.
  const BRepGraphInc::BaseDef* aBaseDef = myGraph->Topo().Gen().TopoEntity(myRoot);
  if (aBaseDef == nullptr || myRoot.IsRemoved(*myGraph))
  {
    return;
  }

  // Check if root itself matches the target kind (e.g., root=Edge, target=Edge).
  if (myConfig.TargetKind.has_value() && matchesTarget(myRoot))
  {
    if (pushRootFrame())
    {
      setCurrentFromFrame(myStackTop);
    }
    return;
  }

  // Push root frame and find first matching descendant.
  if (pushRootFrame())
  {
    advance();
  }
}

//=================================================================================================

void BRepGraph_ChildExplorer::Next()
{
  advance();
}

//=================================================================================================

void BRepGraph_ChildExplorer::advance()
{
  const BRepGraph::TopoView& aDefs = myGraph->Topo();

  if (myCurrentFrame >= 0)
  {
    if (myCurrentFrame == myStackTop && !shouldDescendFromCurrent())
    {
      popFrame();
    }
    myCurrentFrame = -1;
  }

  myHasMore = false;

  while (myStackTop >= 0)
  {
    StackFrame& aFrame = topFrame();

    BRepGraph_ChildIterator aChildIt(*myGraph, aFrame.Node, aFrame.NextChildIdx);
    if (!aChildIt.More())
    {
      popFrame();
      continue;
    }

    const BRepGraph_ChildIterator::ValueType& aStep = aChildIt.Current();
    aFrame.NextChildIdx                             = aChildIt.CurrentPosition();

    const BRepGraph_NodeId aChildNode = aStep.Child;
    const uint32_t         aStepIdx   = aStep.Link == BRepGraph_ChildIterator::LinkKind::Reference
                                          ? aStep.StepIndex
                                          : BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX;
    const BRepGraph_RefId  aCachedRef = aStep.Ref;

    TopAbs_Orientation aChildOri = aFrame.AccOrientation;
    if (myConfig.AccumulateOrientation)
    {
      aChildOri = TopAbs::Compose(aFrame.AccOrientation, aStep.LocalOrientation);
    }

    // If no child was found, this frame is exhausted - backtrack.
    if (!aChildNode.IsValid())
    {
      popFrame();
      continue;
    }

    if (matchesAvoid(aChildNode))
    {
      const BRepGraphInc::BaseDef* anAvoidDef = aDefs.Gen().TopoEntity(aChildNode);
      if (myConfig.EmitAvoidKind && anAvoidDef != nullptr && !aChildNode.IsRemoved(*myGraph))
      {
        if (pushChildFrame(aChildNode,
                           aStepIdx,
                           aCachedRef,
                           aChildIt.CurrentLocalLocation(),
                           aChildOri))
        {
          setCurrentFromFrame(myStackTop);
          return;
        }
      }
      continue;
    }

    // Target check, or all-descendant emission when no target is configured.
    if (shouldEmit(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPostDef = aDefs.Gen().TopoEntity(aChildNode);
      if (aPostDef == nullptr || aChildNode.IsRemoved(*myGraph))
      {
        continue;
      }

      if (pushChildFrame(aChildNode,
                         aStepIdx,
                         aCachedRef,
                         aChildIt.CurrentLocalLocation(),
                         aChildOri))
      {
        setCurrentFromFrame(myStackTop);
        return;
      }
      continue;
    }

    // Check if resolved child is valid and not removed before descending.
    const BRepGraphInc::BaseDef* aBaseDef = aDefs.Gen().TopoEntity(aChildNode);
    if (aBaseDef == nullptr || aChildNode.IsRemoved(*myGraph))
    {
      continue;
    }

    // Descend if this kind can contain the target.
    if (myConfig.Mode == TraversalMode::Recursive
        && ((myConfig.TargetKind.has_value()
             && canContainTarget(aChildNode.NodeKind, *myConfig.TargetKind))
            || (!myConfig.TargetKind.has_value() && canHaveChildren(aChildNode.NodeKind))))
    {
      (void)pushChildFrame(aChildNode,
                           aStepIdx,
                           aCachedRef,
                           aChildIt.CurrentLocalLocation(),
                           aChildOri);
    }
    // Otherwise skip this child (loop continues to next sibling or backtrack).
  }
}

//=================================================================================================

void BRepGraph_ChildExplorer::setCurrentFromFrame(const int theFrameIndex)
{
  myCurrentFrame = theFrameIndex;
  myCurrent      = myStack[theFrameIndex].Node;
  myHasMore      = true;
}

//=================================================================================================

BRepGraphInc::NodeInstance BRepGraph_ChildExplorer::Current() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return BRepGraphInc::NodeInstance();
  }
  const StackFrame& aFrame = myStack[myCurrentFrame];
  return {aFrame.Node, frameLocation(aFrame), aFrame.AccOrientation};
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ChildExplorer::CurrentLocation() const
{
  static const TopLoc_Location THE_IDENTITY;
  return myHasMore && myCurrentFrame >= 0 ? frameLocation(myStack[myCurrentFrame]) : THE_IDENTITY;
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ChildExplorer::CurrentOrientation() const
{
  return myHasMore && myCurrentFrame >= 0 ? myStack[myCurrentFrame].AccOrientation : TopAbs_FORWARD;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::CurrentParent() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return BRepGraph_NodeId();
  }

  return myStack[myCurrentFrame - 1].Node;
}

//=================================================================================================

BRepGraph_ChildExplorer::LinkKind BRepGraph_ChildExplorer::CurrentLinkKind() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return LinkKind::None;
  }

  return myStack[myCurrentFrame].StepFromParent != BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX
           ? LinkKind::Reference
           : LinkKind::Structural;
}

//=================================================================================================

BRepGraph_RefId BRepGraph_ChildExplorer::CurrentRef() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return BRepGraph_RefId();
  }

  return myStack[myCurrentFrame].Ref;
}

//=================================================================================================

uint32_t BRepGraph_ChildExplorer::CurrentStepIndex() const
{
  return myHasMore && myCurrentFrame >= 0 ? myStack[myCurrentFrame].StepFromParent
                                          : BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX;
}

//=================================================================================================

BRepGraph_UsagePath BRepGraph_ChildExplorer::CurrentUsagePath() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return BRepGraph_UsagePath();
  }
  const size_t        aFrameCount = static_cast<size_t>(myCurrentFrame + 1);
  BRepGraph_UsagePath aPath(aFrameCount);
  for (size_t aFrameIdx = 0; aFrameIdx < aFrameCount; ++aFrameIdx)
  {
    const StackFrame& aFrame = myStack[aFrameIdx];
    aPath.Append(BRepGraph_UsagePath::Step{aFrame.Node, aFrame.Ref, aFrame.StepFromParent});
  }
  return aPath;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::shouldDescendFromCurrent() const
{
  if (myConfig.Mode != TraversalMode::Recursive || myCurrentFrame < 0)
  {
    return false;
  }

  const BRepGraph_NodeId aNode = myStack[myCurrentFrame].Node;
  if (matchesAvoid(aNode))
  {
    return false;
  }

  if (myConfig.TargetKind.has_value())
  {
    return canContainTarget(aNode.NodeKind, *myConfig.TargetKind);
  }

  return canHaveChildren(aNode.NodeKind);
}

//=================================================================================================

std::optional<BRepGraph_NodeId::Kind> BRepGraph_ChildExplorer::normalizeAvoidKind(
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind)
{
  if (!theAvoidKind.has_value() || !theTargetKind.has_value())
  {
    return theAvoidKind;
  }

  if (*theAvoidKind == *theTargetKind)
  {
    return std::nullopt;
  }

  return canContainTarget(*theAvoidKind, *theTargetKind) ? theAvoidKind : std::nullopt;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                                               const BRepGraph_NodeId::Kind theTargetKind)
{
  return childExplorerKindCanContainDescendant(theParentKind, theTargetKind);
}

//=================================================================================================

bool BRepGraph_ChildExplorer::canHaveChildren(const BRepGraph_NodeId::Kind theNodeKind)
{
  return theNodeKind != BRepGraph_NodeId::Kind::Vertex;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::pushFrame(StackFrame&& theFrame)
{
  // Guard against pathological cycles (e.g., self-referencing compounds).
  // A valid DFS path cannot exceed the total node count in the graph.
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

bool BRepGraph_ChildExplorer::pushChildFrame(const BRepGraph_NodeId   theNode,
                                             const uint32_t           theStepIndex,
                                             const BRepGraph_RefId    theRef,
                                             const TopLoc_Location&   theLocalLocation,
                                             const TopAbs_Orientation theOrientation)
{
  int  aLocationIndex = topFrame().LocationIndex;
  bool ownsLocation   = false;
  if (myConfig.AccumulateLocation && !theLocalLocation.IsIdentity())
  {
    const TopLoc_Location anAccumulated = aLocationIndex >= 0
                                            ? frameLocation(topFrame()).Multiplied(theLocalLocation)
                                            : theLocalLocation;
    aLocationIndex                      = pushLocation(anAccumulated);
    ownsLocation                        = aLocationIndex >= 0;
  }
  if (pushFrame(
        StackFrame{theNode, 0, theStepIndex, theRef, aLocationIndex, ownsLocation, theOrientation}))
  {
    return true;
  }
  if (ownsLocation)
  {
    --myLocationTop;
  }
  return false;
}

//=================================================================================================

void BRepGraph_ChildExplorer::popFrame()
{
  if (myStackTop >= 0)
  {
    if (myStack[myStackTop].OwnsLocation)
    {
      --myLocationTop;
    }
    --myStackTop;
  }
}

//=================================================================================================

int BRepGraph_ChildExplorer::pushLocation(TopLoc_Location theLocation)
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

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
  {
    if (myStack[i].Node.NodeKind == theKind)
    {
      return frameLocation(myStack[i]);
    }
  }
  // Check the current match.
  if (myHasMore && myCurrentFrame >= 0 && myCurrent.NodeKind == theKind)
  {
    return frameLocation(myStack[myCurrentFrame]);
  }
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::NodeOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
  {
    if (myStack[i].Node.NodeKind == theKind)
    {
      return myStack[i].Node;
    }
  }
  if (myHasMore && myCurrent.NodeKind == theKind)
  {
    return myCurrent;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationAt(const size_t theLevel) const
{
  if (myHasMore && myCurrentFrame >= 0 && theLevel <= static_cast<size_t>(myCurrentFrame))
  {
    return frameLocation(myStack[theLevel]);
  }
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::NodeAt(const size_t theLevel) const
{
  if (myHasMore && myCurrentFrame >= 0 && theLevel <= static_cast<size_t>(myCurrentFrame))
  {
    return myStack[theLevel].Node;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

BRepGraph_RefId BRepGraph_ChildExplorer::RefAt(const size_t theLevel) const
{
  if (myHasMore && myCurrentFrame >= 0 && theLevel <= static_cast<size_t>(myCurrentFrame))
  {
    return myStack[theLevel].Ref;
  }
  return BRepGraph_RefId();
}
