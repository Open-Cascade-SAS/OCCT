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

#include <TopAbs.hxx>

#include <algorithm>

namespace
{
static int parentExplorerKindDepth(const BRepGraph_NodeId::Kind theKind)
{
  static constexpr int THE_DEPTH[] = {
    2,  // Kind::Solid=0
    3,  // Kind::Shell=1
    4,  // Kind::Face=2
    5,  // Kind::Wire=3
    7,  // Kind::Edge=4
    8,  // Kind::Vertex=5
    0,  // Kind::Compound=6
    1,  // Kind::CompSolid=7
    6,  // Kind::CoEdge=8
    99, // gap=9
    0,  // Kind::Product=10
    1,  // Kind::Occurrence=11
  };
  return THE_DEPTH[static_cast<int>(theKind)];
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

  return myGraph->Refs().RefAtStep(myStack[myCurrentFrame].Node,
                                   myStack[myCurrentFrame].StepToChild);
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
  return parentExplorerKindDepth(theParentKind) < parentExplorerKindDepth(theTargetKind);
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
        const BRepGraph_EdgesOfVertex aParents(*myGraph, aTopo.Vertices().Edges(aVertexId));
        if (aParentIdx >= aParents.Size())
        {
          return false;
        }

        const BRepGraph_EdgeId       anEdgeId = aParents.Value(aParentIdx);
        const BRepGraphInc::EdgeDef& anEdge   = aTopo.Edges().Definition(anEdgeId);
        if (anEdge.IsRemoved)
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

      case Kind::Edge: {
        const BRepGraph_EdgeId        aEdgeId(theChild.Node);
        const BRepGraph_CoEdgesOfEdge aParents(*myGraph, aTopo.Edges().CoEdges(aEdgeId));
        if (aParentIdx >= static_cast<uint32_t>(aParents.Size()))
        {
          BRepGraph_ProductId aProductId;
          if (!findNthProductWrapper(theChild.Node,
                                     aParentIdx - static_cast<uint32_t>(aParents.Size()),
                                     aProductId))
          {
            return false;
          }

          theParent.Node          = aProductId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }

        const BRepGraph_CoEdgeId       aCoEdgeId = aParents.Value(aParentIdx);
        const BRepGraphInc::CoEdgeDef& aCoEdge   = aTopo.CoEdges().Definition(aCoEdgeId);
        if (aCoEdge.IsRemoved)
        {
          continue;
        }

        theParent.Node          = aCoEdgeId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Wire: {
        const BRepGraph_WireId      aWireId(theChild.Node);
        const BRepGraph_FacesOfWire aParents(*myGraph, aTopo.Wires().Faces(aWireId));
        if (aParentIdx >= static_cast<uint32_t>(aParents.Size()))
        {
          BRepGraph_ProductId aProductId;
          if (!findNthProductWrapper(theChild.Node,
                                     aParentIdx - static_cast<uint32_t>(aParents.Size()),
                                     aProductId))
          {
            return false;
          }

          theParent.Node          = aProductId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }

        const BRepGraph_FaceId       aFaceId = aParents.Value(aParentIdx);
        const BRepGraphInc::FaceDef& aFace   = aTopo.Faces().Definition(aFaceId);
        if (aFace.IsRemoved)
        {
          continue;
        }

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

      case Kind::Face: {
        const BRepGraph_FaceId          aFaceId(theChild.Node);
        const BRepGraph_ShellsOfFace    aShells(*myGraph, aTopo.Faces().Shells(aFaceId));
        const BRepGraph_CompoundsOfFace aCompounds(*myGraph, aTopo.Faces().Compounds(aFaceId));
        const uint32_t                  aNbShells    = static_cast<uint32_t>(aShells.Size());
        const uint32_t                  aNbCompounds = static_cast<uint32_t>(aCompounds.Size());
        if (aParentIdx < aNbShells)
        {
          const BRepGraph_ShellId       aShellId = aShells.Value(aParentIdx);
          const BRepGraphInc::ShellDef& aShell   = aTopo.Shells().Definition(aShellId);
          if (aShell.IsRemoved)
          {
            continue;
          }

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
        if (aParentIdx < aNbShells + aNbCompounds)
        {
          const BRepGraph_CompoundId       aCompoundId = aCompounds.Value(aParentIdx - aNbShells);
          const BRepGraphInc::CompoundDef& aCompound   = aTopo.Compounds().Definition(aCompoundId);
          if (aCompound.IsRemoved)
          {
            continue;
          }

          const int aStepToChild = findCompoundChildStep(aCompoundId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }

          theParent.Node          = aCompoundId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }

        BRepGraph_ProductId aProductId;
        if (!findNthProductWrapper(theChild.Node,
                                   aParentIdx - aNbShells - aNbCompounds,
                                   aProductId))
        {
          return false;
        }

        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Shell: {
        const BRepGraph_ShellId          aShellId = BRepGraph_ShellId(theChild.Node);
        const BRepGraph_SolidsOfShell    aSolids(*myGraph, aTopo.Shells().Solids(aShellId));
        const BRepGraph_CompoundsOfShell aCompounds(*myGraph, aTopo.Shells().Compounds(aShellId));
        const uint32_t                   aNbSolids    = static_cast<uint32_t>(aSolids.Size());
        const uint32_t                   aNbCompounds = static_cast<uint32_t>(aCompounds.Size());
        if (aParentIdx < aNbSolids)
        {
          const BRepGraph_SolidId       aSolidId = aSolids.Value(aParentIdx);
          const BRepGraphInc::SolidDef& aSolid   = aTopo.Solids().Definition(aSolidId);
          if (aSolid.IsRemoved)
          {
            continue;
          }

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
        if (aParentIdx < aNbSolids + aNbCompounds)
        {
          const BRepGraph_CompoundId       aCompoundId = aCompounds.Value(aParentIdx - aNbSolids);
          const BRepGraphInc::CompoundDef& aCompound   = aTopo.Compounds().Definition(aCompoundId);
          if (aCompound.IsRemoved)
          {
            continue;
          }

          const int aStepToChild = findCompoundChildStep(aCompoundId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }

          theParent.Node          = aCompoundId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }

        BRepGraph_ProductId aProductId;
        if (!findNthProductWrapper(theChild.Node,
                                   aParentIdx - aNbSolids - aNbCompounds,
                                   aProductId))
        {
          return false;
        }

        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Solid: {
        const BRepGraph_SolidId           aSolidId = BRepGraph_SolidId(theChild.Node);
        const BRepGraph_CompSolidsOfSolid aCompSolids(*myGraph,
                                                      aTopo.Solids().CompSolids(aSolidId));
        const BRepGraph_CompoundsOfSolid  aCompounds(*myGraph, aTopo.Solids().Compounds(aSolidId));
        const uint32_t                    aNbCompSolids = static_cast<uint32_t>(aCompSolids.Size());
        const uint32_t                    aNbCompounds  = static_cast<uint32_t>(aCompounds.Size());
        if (aParentIdx < aNbCompSolids)
        {
          const BRepGraph_CompSolidId       aCompSolidId = aCompSolids.Value(aParentIdx);
          const BRepGraphInc::CompSolidDef& aCompSolid =
            aTopo.CompSolids().Definition(aCompSolidId);
          if (aCompSolid.IsRemoved)
          {
            continue;
          }

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
        if (aParentIdx < aNbCompSolids + aNbCompounds)
        {
          const BRepGraph_CompoundId aCompoundId     = aCompounds.Value(aParentIdx - aNbCompSolids);
          const BRepGraphInc::CompoundDef& aCompound = aTopo.Compounds().Definition(aCompoundId);
          if (aCompound.IsRemoved)
          {
            continue;
          }

          const int aStepToChild = findCompoundChildStep(aCompoundId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }

          theParent.Node          = aCompoundId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }

        BRepGraph_ProductId aProductId;
        if (!findNthProductWrapper(theChild.Node,
                                   aParentIdx - aNbCompSolids - aNbCompounds,
                                   aProductId))
        {
          return false;
        }

        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Compound: {
        const BRepGraph_CompoundId          aCompoundId(theChild.Node);
        const BRepGraph_CompoundsOfCompound aParents(
          *myGraph,
          aTopo.Compounds().ParentCompounds(aCompoundId));
        const uint32_t aNbParents = static_cast<uint32_t>(aParents.Size());
        if (aParentIdx < aNbParents)
        {
          const BRepGraph_CompoundId       aParentCompoundId = aParents.Value(aParentIdx);
          const BRepGraphInc::CompoundDef& aParentCompound =
            aTopo.Compounds().Definition(aParentCompoundId);
          if (aParentCompound.IsRemoved)
          {
            continue;
          }

          const int aStepToChild = findCompoundChildStep(aParentCompoundId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }

          theParent.Node          = aParentCompoundId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }

        BRepGraph_ProductId aProductId;
        if (!findNthProductWrapper(theChild.Node, aParentIdx - aNbParents, aProductId))
        {
          return false;
        }

        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::CompSolid: {
        const BRepGraph_CompSolidId          aCompSolidId(theChild.Node);
        const BRepGraph_CompoundsOfCompSolid aParents(*myGraph,
                                                      aTopo.CompSolids().Compounds(aCompSolidId));
        const uint32_t                       aNbParents = static_cast<uint32_t>(aParents.Size());
        if (aParentIdx < aNbParents)
        {
          const BRepGraph_CompoundId       aParentCompoundId = aParents.Value(aParentIdx);
          const BRepGraphInc::CompoundDef& aParentCompound =
            aTopo.Compounds().Definition(aParentCompoundId);
          if (aParentCompound.IsRemoved)
          {
            continue;
          }

          const int aStepToChild = findCompoundChildStep(aParentCompoundId, theChild.Node);
          if (aStepToChild < 0)
          {
            continue;
          }

          theParent.Node          = aParentCompoundId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = aStepToChild;
          return true;
        }

        BRepGraph_ProductId aProductId;
        if (!findNthProductWrapper(theChild.Node, aParentIdx - aNbParents, aProductId))
        {
          return false;
        }

        theParent.Node          = aProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::CoEdge: {
        const BRepGraph_CoEdgeId      aCoEdgeId(theChild.Node);
        const BRepGraph_WiresOfCoEdge aWires(*myGraph, aTopo.CoEdges().Wires(aCoEdgeId));
        const uint32_t                aNbWires = static_cast<uint32_t>(aWires.Size());
        if (aParentIdx >= aNbWires)
        {
          return false;
        }

        const BRepGraph_WireId       aWireId = aWires.Value(aParentIdx);
        const BRepGraphInc::WireDef& aWire   = aTopo.Wires().Definition(aWireId);
        if (aWire.IsRemoved)
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
        theParent.StepToChild   = aStepToChild;
        return true;
      }

      case Kind::Product: {
        const BRepGraph_ProductId                         aProductId(theChild.Node);
        const NCollection_Vector<BRepGraph_OccurrenceId>& anOccurrences =
          aTopo.Products().Instances(aProductId);
        const uint32_t aNbOccurrences = static_cast<uint32_t>(anOccurrences.Size());
        if (aParentIdx >= aNbOccurrences)
        {
          return false;
        }

        const BRepGraph_OccurrenceId       anOccurrenceId = anOccurrences.Value(static_cast<size_t>(aParentIdx));
        const BRepGraphInc::OccurrenceDef& anOccurrence =
          aTopo.Occurrences().Definition(anOccurrenceId);
        if (anOccurrence.IsRemoved)
        {
          continue;
        }

        theParent.Node          = anOccurrenceId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = -1;
        return true;
      }

      case Kind::Occurrence: {
        if (aParentIdx > 0)
        {
          return false;
        }

        const BRepGraph_OccurrenceId       anOccurrenceId(theChild.Node);
        const BRepGraphInc::OccurrenceDef& anOccurrence =
          aTopo.Occurrences().Definition(anOccurrenceId);
        if (anOccurrence.IsRemoved)
        {
          return false;
        }

        BRepGraph_ProductId aParentProductId;
        if (!findParentProduct(anOccurrenceId, aParentProductId))
        {
          return false;
        }

        const int aStepToChild = findOccurrenceStep(aParentProductId, anOccurrenceId);
        if (aStepToChild < 0)
        {
          return false;
        }

        theParent.Node          = aParentProductId;
        theParent.NextParentIdx = 0;
        theParent.StepToChild   = aStepToChild;
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
                    myStack[aFrameIdx].AccLocation,
                    myStack[aFrameIdx].AccOrientation);
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::applyTransition(const BRepGraph_NodeId theParent,
                                               const BRepGraph_NodeId theChild,
                                               const int              theStepToChild,
                                               TopLoc_Location&       theLocation,
                                               TopAbs_Orientation&    theOrientation) const
{
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
      const BRepGraph_OccurrenceId       anOccId(theParent);
      const BRepGraphInc::OccurrenceDef& anOcc = aTopo.Occurrences().Definition(anOccId);
      if (anOcc.IsRemoved)
      {
        return;
      }
      BRepGraph_ProductId aParentProductId;
      if (!findParentProduct(anOccId, aParentProductId))
      {
        return;
      }
      const BRepGraph::RefsView& aRefs = myGraph->Refs();
      for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, aParentProductId); anOccIt.More();
           anOccIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(anOccIt.CurrentId());
        if (aRef.OccurrenceDefId == anOccId)
        {
          theLocation = theLocation * aRef.LocalLocation;
          return;
        }
      }
      return;
    }

    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId(theParent));
      if (!aCoEdge.IsRemoved)
      {
        theOrientation = TopAbs::Compose(theOrientation, aCoEdge.Orientation);
      }
      return;
    }

    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId       aProductId(theParent);
      const BRepGraphInc::ProductDef& aProduct = aTopo.Products().Definition(aProductId);
      if (aProduct.IsRemoved)
      {
        return;
      }
      const BRepGraph::RefsView& aRefs = myGraph->Refs();
      for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, aProductId); anOccIt.More();
           anOccIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(anOccIt.CurrentId());
        const BRepGraphInc::OccurrenceDef& anOccDef =
          aTopo.Occurrences().Definition(aRef.OccurrenceDefId);
        if (anOccDef.ChildDefId == theChild)
        {
          theLocation = theLocation * aRef.LocalLocation;
          return;
        }
      }
      return;
    }

    default:
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

bool BRepGraph_ParentExplorer::findNthProductWrapper(const BRepGraph_NodeId theNode,
                                                     const uint32_t         theOrdinal,
                                                     BRepGraph_ProductId&   theProduct) const
{
  const BRepGraph::TopoView& aTopo  = myGraph->Topo();
  const BRepGraph::RefsView& aRefs  = myGraph->Refs();
  uint32_t                   aCount = 0;
  for (int aPass = 0; aPass < 2; ++aPass)
  {
    for (BRepGraph_ProductIterator aProdIt(*myGraph); aProdIt.More(); aProdIt.Next())
    {
      const BRepGraph_ProductId aProductId = aProdIt.CurrentId();

      bool aHasChild = false;
      for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, aProductId); anOccIt.More();
           anOccIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(anOccIt.CurrentId());
        const BRepGraphInc::OccurrenceDef& anOccDef =
          aTopo.Occurrences().Definition(aRef.OccurrenceDefId);
        if (anOccDef.ChildDefId == theNode)
        {
          aHasChild = true;
          break;
        }
      }
      if (!aHasChild)
      {
        continue;
      }

      const bool hasInstances = !aTopo.Products().Instances(aProductId).IsEmpty();
      if ((aPass == 0 && !hasInstances) || (aPass == 1 && hasInstances))
      {
        continue;
      }

      if (aCount == theOrdinal)
      {
        theProduct = aProductId;
        return true;
      }
      ++aCount;
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraph_ParentExplorer::findParentProduct(const BRepGraph_OccurrenceId theOccurrence,
                                                 BRepGraph_ProductId&         theProduct) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  for (BRepGraph_ProductIterator aProdIt(*myGraph); aProdIt.More(); aProdIt.Next())
  {
    const BRepGraph_ProductId                            aProductId = aProdIt.CurrentId();
    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, aProductId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(anOccIt.CurrentId());
      if (aRef.OccurrenceDefId == theOccurrence)
      {
        theProduct = aProductId;
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findOccurrenceStep(const BRepGraph_ProductId    theParentProduct,
                                                 const BRepGraph_OccurrenceId theOccurrence) const
{
  int aStep = 0;
  for (BRepGraph_RefsOccurrenceOfProduct aRefIt(*myGraph, theParentProduct); aRefIt.More();
       aRefIt.Next())
  {
    if (myGraph->Refs().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theOccurrence))
    {
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
    if (myGraph->Refs().ChildNode(aRefIt.CurrentId()) == theChild)
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
    if (myGraph->Refs().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theChild))
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
  const BRepGraph::TopoView&    aTopo  = myGraph->Topo();
  const BRepGraph::RefsView&    aRefs  = myGraph->Refs();
  const BRepGraphInc::SolidDef& aSolid = aTopo.Solids().Definition(theParent);
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Shell)
  {
    int aStep = 0;
    for (BRepGraph_RefsShellOfSolid aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }

  {
    uint32_t aRefIdx = 0;
    for (const BRepGraph_ChildRefId& aRefId : aSolid.AuxChildRefIds)
    {
      if (!aRefs.IsRemoved(aRefId) && aRefs.ChildNode(aRefId) == theChild)
        return aSolid.ShellRefIds.Length() + static_cast<int>(aRefIdx);
      ++aRefIdx;
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findShellChildStep(const BRepGraph_ShellId theParent,
                                                 const BRepGraph_NodeId  theChild) const
{
  const BRepGraph::TopoView&    aTopo  = myGraph->Topo();
  const BRepGraph::RefsView&    aRefs  = myGraph->Refs();
  const BRepGraphInc::ShellDef& aShell = aTopo.Shells().Definition(theParent);
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Face)
  {
    int aStep = 0;
    for (BRepGraph_RefsFaceOfShell aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }

  {
    uint32_t aRefIdx = 0;
    for (const BRepGraph_ChildRefId& aRefId : aShell.AuxChildRefIds)
    {
      if (!aRefs.IsRemoved(aRefId) && aRefs.ChildNode(aRefId) == theChild)
        return aShell.FaceRefIds.Length() + static_cast<int>(aRefIdx);
      ++aRefIdx;
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findFaceChildStep(const BRepGraph_FaceId theParent,
                                                const BRepGraph_NodeId theChild) const
{
  const BRepGraph::TopoView&   aTopo = myGraph->Topo();
  const BRepGraph::RefsView&   aRefs = myGraph->Refs();
  const BRepGraphInc::FaceDef& aFace = aTopo.Faces().Definition(theParent);
  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Wire)
  {
    int aStep = 0;
    for (BRepGraph_RefsWireOfFace aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aStep;
      }
      ++aStep;
    }
    return -1;
  }

  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Vertex)
  {
    int aStep = 0;
    for (BRepGraph_RefsVertexOfFace aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      if (aRefs.ChildNode(aRefIt.CurrentId()) == theChild)
      {
        return aFace.WireRefIds.Length() + aStep;
      }
      ++aStep;
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findWireCoEdgeStep(const BRepGraph_WireId   theParent,
                                                 const BRepGraph_CoEdgeId theChild) const
{
  int aStep = 0;
  for (BRepGraph_RefsCoEdgeOfWire aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    if (myGraph->Refs().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theChild))
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
  int aStep = 0;
  for (BRepGraph_RefsVertexOfEdge aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    if (myGraph->Refs().ChildNode(aRefIt.CurrentId()) == BRepGraph_NodeId(theChild))
    {
      return aStep;
    }
    ++aStep;
  }
  return -1;
}

//=================================================================================================

void BRepGraph_ParentExplorer::pushFrame(const StackFrame& theFrame)
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const int aMaxDepth = aTopo.Compounds().Nb() + aTopo.CompSolids().Nb() + aTopo.Solids().Nb()
                        + aTopo.Shells().Nb() + aTopo.Faces().Nb() + aTopo.Wires().Nb()
                        + aTopo.Edges().Nb() + aTopo.Vertices().Nb() + aTopo.Products().Nb()
                        + aTopo.Occurrences().Nb() + aTopo.CoEdges().Nb();
  if (myStackTop >= aMaxDepth)
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
  return aRefs.LocalLocation(aRefs.RefAtStep(theParent, theRefIdx));
}

//=================================================================================================

TopAbs_Orientation BRepGraph_ParentExplorer::stepOrientation(const BRepGraph_NodeId theParent,
                                                             const int              theRefIdx) const
{
  const BRepGraph::RefsView& aRefs = myGraph->Refs();
  return aRefs.Orientation(aRefs.RefAtStep(theParent, theRefIdx));
}
