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

static BRepGraph_VertexRefId edgeVertexRefIdAt(const BRepGraphInc::EdgeDef& theEdge,
                                               const int                    theRefIdx)
{
  if (theRefIdx == 0)
  {
    return theEdge.StartVertexRefId;
  }
  if (theRefIdx == 1)
  {
    return theEdge.EndVertexRefId;
  }

  const int anInternalIdx = theRefIdx - 2;
  if (anInternalIdx < 0 || anInternalIdx >= theEdge.InternalVertexRefIds.Length())
  {
    return BRepGraph_VertexRefId();
  }
  return theEdge.InternalVertexRefIds.Value(anInternalIdx);
}
} // namespace

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode)
    : BRepGraph_ParentExplorer(theGraph, theNode, TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theNode,
                                                   const TraversalMode    theMode)
    : myGraph(&theGraph),
      myNode(theNode),
      myMode(theMode),
      myTargetKind(std::nullopt),
      myAvoidKind(std::nullopt),
      myEmitAvoidKind(false)
{
  startTraversal();
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theNode,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : myGraph(&theGraph),
      myNode(theNode),
      myMode(theMode),
      myTargetKind(std::nullopt),
      myAvoidKind(normalizeAvoidKind(theNode, std::nullopt, theAvoidKind)),
      myEmitAvoidKind(theEmitAvoidKind)
{
  startTraversal();
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
    : myGraph(&theGraph),
      myNode(theNode),
      myMode(theMode),
      myTargetKind(theTargetKind),
      myAvoidKind(normalizeAvoidKind(theNode, theTargetKind, std::nullopt)),
      myEmitAvoidKind(false)
{
  startTraversal();
}

//=================================================================================================

BRepGraph_ParentExplorer::BRepGraph_ParentExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theNode,
  BRepGraph_NodeId::Kind                       theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : myGraph(&theGraph),
      myNode(theNode),
      myMode(theMode),
      myTargetKind(theTargetKind),
      myAvoidKind(normalizeAvoidKind(theNode, theTargetKind, theAvoidKind)),
      myEmitAvoidKind(theEmitAvoidKind)
{
  startTraversal();
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

  if (myMode == TraversalMode::DirectParents)
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
    const int aParentIdx = theChild.NextParentIdx++;

    switch (theChild.Node.NodeKind)
    {
      case Kind::Vertex: {
        const BRepGraph_VertexId                    aVertexId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_EdgeId>& aEdges = aTopo.Vertices().Edges(aVertexId);
        if (aParentIdx >= aEdges.Length())
        {
          return false;
        }

        const BRepGraph_EdgeId       anEdgeId = aEdges.Value(aParentIdx);
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
        const BRepGraph_EdgeId                        aEdgeId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = aTopo.Edges().CoEdges(aEdgeId);
        if (aParentIdx >= aCoEdges.Length())
        {
          BRepGraph_ProductId aProductId;
          if (!findNthProductWrapper(theChild.Node, aParentIdx - aCoEdges.Length(), aProductId))
          {
            return false;
          }

          theParent.Node          = aProductId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }

        const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdges.Value(aParentIdx);
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
        const BRepGraph_WireId                      aWireId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_FaceId>& aFaces = aTopo.Wires().Faces(aWireId);
        if (aParentIdx >= aFaces.Length())
        {
          BRepGraph_ProductId aProductId;
          if (!findNthProductWrapper(theChild.Node, aParentIdx - aFaces.Length(), aProductId))
          {
            return false;
          }

          theParent.Node          = aProductId;
          theParent.NextParentIdx = 0;
          theParent.StepToChild   = -1;
          return true;
        }

        const BRepGraph_FaceId       aFaceId = aFaces.Value(aParentIdx);
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
        const BRepGraph_FaceId                          aFaceId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_ShellId>&    aShells = aTopo.Faces().Shells(aFaceId);
        const NCollection_Vector<BRepGraph_CompoundId>& aCompounds =
          aTopo.Faces().Compounds(aFaceId);
        const int aNbShells    = aShells.Length();
        const int aNbCompounds = aCompounds.Length();
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
        const BRepGraph_ShellId aShellId = BRepGraph_ShellId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_SolidId>&    aSolids = aTopo.Shells().Solids(aShellId);
        const NCollection_Vector<BRepGraph_CompoundId>& aCompounds =
          aTopo.Shells().Compounds(aShellId);
        const int aNbSolids    = aSolids.Length();
        const int aNbCompounds = aCompounds.Length();
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
        const BRepGraph_SolidId aSolidId = BRepGraph_SolidId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_CompSolidId>& aCompSolids =
          aTopo.Solids().CompSolids(aSolidId);
        const NCollection_Vector<BRepGraph_CompoundId>& aCompounds =
          aTopo.Solids().Compounds(aSolidId);
        const int aNbCompSolids = aCompSolids.Length();
        const int aNbCompounds  = aCompounds.Length();
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
        const BRepGraph_CompoundId                      aCompoundId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_CompoundId>& aParents =
          aTopo.Compounds().ParentCompounds(aCompoundId);
        const int aNbParents = aParents.Length();
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
        const BRepGraph_CompSolidId                     aCompSolidId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_CompoundId>& aParents =
          aTopo.CompSolids().Compounds(aCompSolidId);
        const int aNbParents = aParents.Length();
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
        const BRepGraph_CoEdgeId                    aCoEdgeId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_WireId>& aWires   = aTopo.CoEdges().Wires(aCoEdgeId);
        const int                                   aNbWires = aWires.Length();
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
        const BRepGraph_ProductId                         aProductId(theChild.Node.Index);
        const NCollection_Vector<BRepGraph_OccurrenceId>& anOccurrences =
          aTopo.Products().Instances(aProductId);
        const int aNbOccurrences = anOccurrences.Length();
        if (aParentIdx >= aNbOccurrences)
        {
          return false;
        }

        const BRepGraph_OccurrenceId       anOccurrenceId = anOccurrences.Value(aParentIdx);
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

        const BRepGraph_OccurrenceId       anOccurrenceId(theChild.Node.Index);
        const BRepGraphInc::OccurrenceDef& anOccurrence =
          aTopo.Occurrences().Definition(anOccurrenceId);
        if (anOccurrence.IsRemoved || !anOccurrence.ParentProductDefId.IsValid())
        {
          return false;
        }

        const int aStepToChild =
          findOccurrenceStep(anOccurrence.ParentProductDefId, anOccurrenceId);
        if (aStepToChild < 0)
        {
          return false;
        }

        theParent.Node          = anOccurrence.ParentProductDefId;
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
                    myStack[aFrameIdx + 1].StepToChild,
                    myStack[aFrameIdx].AccLocation,
                    myStack[aFrameIdx].AccOrientation);
  }
}

//=================================================================================================

void BRepGraph_ParentExplorer::applyTransition(const BRepGraph_NodeId theParent,
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
      const BRepGraphInc::OccurrenceDef& anOcc =
        aTopo.Occurrences().Definition(BRepGraph_OccurrenceId(theParent.Index));
      if (!anOcc.IsRemoved)
      {
        theLocation = theLocation * anOcc.Placement;
      }
      return;
    }

    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId(theParent.Index));
      if (!aCoEdge.IsRemoved)
      {
        theOrientation = TopAbs::Compose(theOrientation, aCoEdge.Sense);
      }
      return;
    }

    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraphInc::ProductDef& aProduct =
        aTopo.Products().Definition(BRepGraph_ProductId(theParent.Index));
      if (!aProduct.IsRemoved && aProduct.ShapeRootId.IsValid())
      {
        theLocation    = theLocation * aProduct.RootLocation;
        theOrientation = TopAbs::Compose(theOrientation, aProduct.RootOrientation);
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
                                                     const int              theOrdinal,
                                                     BRepGraph_ProductId&   theProduct) const
{
  if (theOrdinal < 0)
  {
    return false;
  }

  const BRepGraph::TopoView& aTopo  = myGraph->Topo();
  int                        aCount = 0;
  for (int aPass = 0; aPass < 2; ++aPass)
  {
    for (BRepGraph_ProductIterator aProdIt(*myGraph); aProdIt.More(); aProdIt.Next())
    {
      const BRepGraph_ProductId       aProductId  = aProdIt.CurrentId();
      const BRepGraphInc::ProductDef& aProductDef = aProdIt.Current();
      if (aProductDef.ShapeRootId != theNode)
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

int BRepGraph_ParentExplorer::findOccurrenceStep(const BRepGraph_ProductId    theParentProduct,
                                                 const BRepGraph_OccurrenceId theOccurrence) const
{
  for (BRepGraph_RefsOccurrenceOfProduct aRefIt(*myGraph, theParentProduct); aRefIt.More();
       aRefIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& aRef =
      myGraph->Refs().Occurrences().Entry(aRefIt.CurrentId());
    if (aRef.OccurrenceDefId == theOccurrence)
    {
      return aRefIt.Index();
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findCompoundChildStep(const BRepGraph_CompoundId theParent,
                                                    const BRepGraph_NodeId     theChild) const
{
  for (BRepGraph_RefsChildOfCompound aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraphInc::ChildRef& aRef = myGraph->Refs().Children().Entry(aRefIt.CurrentId());
    if (aRef.ChildDefId == theChild)
    {
      return aRefIt.Index();
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findCompSolidSolidStep(const BRepGraph_CompSolidId theParent,
                                                     const BRepGraph_SolidId     theChild) const
{
  for (BRepGraph_RefsSolidOfCompSolid aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraphInc::SolidRef& aRef = myGraph->Refs().Solids().Entry(aRefIt.CurrentId());
    if (aRef.SolidDefId == theChild)
    {
      return aRefIt.Index();
    }
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
    for (BRepGraph_RefsShellOfSolid aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::ShellRef& aRef = aRefs.Shells().Entry(aRefIt.CurrentId());
      if (aRef.ShellDefId == BRepGraph_ShellId(theChild.Index))
      {
        return aRefIt.Index();
      }
    }
    return -1;
  }

  for (int aRefIdx = 0; aRefIdx < aSolid.FreeChildRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::ChildRef& aRef =
      aRefs.Children().Entry(aSolid.FreeChildRefIds.Value(aRefIdx));
    if (!aRef.IsRemoved && aRef.ChildDefId == theChild)
    {
      return aSolid.ShellRefIds.Length() + aRefIdx;
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
    for (BRepGraph_RefsFaceOfShell aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::FaceRef& aRef = aRefs.Faces().Entry(aRefIt.CurrentId());
      if (aRef.FaceDefId == BRepGraph_FaceId(theChild.Index))
      {
        return aRefIt.Index();
      }
    }
    return -1;
  }

  for (int aRefIdx = 0; aRefIdx < aShell.FreeChildRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::ChildRef& aRef =
      aRefs.Children().Entry(aShell.FreeChildRefIds.Value(aRefIdx));
    if (!aRef.IsRemoved && aRef.ChildDefId == theChild)
    {
      return aShell.FaceRefIds.Length() + aRefIdx;
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
    for (BRepGraph_RefsWireOfFace aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::WireRef& aRef = aRefs.Wires().Entry(aRefIt.CurrentId());
      if (aRef.WireDefId == BRepGraph_WireId(theChild.Index))
      {
        return aRefIt.Index();
      }
    }
    return -1;
  }

  if (theChild.NodeKind == BRepGraph_NodeId::Kind::Vertex)
  {
    for (BRepGraph_RefsVertexOfFace aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraphInc::VertexRef& aRef = aRefs.Vertices().Entry(aRefIt.CurrentId());
      if (aRef.VertexDefId == BRepGraph_VertexId(theChild.Index))
      {
        return aFace.WireRefIds.Length() + aRefIt.Index();
      }
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findWireCoEdgeStep(const BRepGraph_WireId   theParent,
                                                 const BRepGraph_CoEdgeId theChild) const
{
  for (BRepGraph_RefsCoEdgeOfWire aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraphInc::CoEdgeRef& aRef = myGraph->Refs().CoEdges().Entry(aRefIt.CurrentId());
    if (aRef.CoEdgeDefId == theChild)
    {
      return aRefIt.Index();
    }
  }
  return -1;
}

//=================================================================================================

int BRepGraph_ParentExplorer::findEdgeVertexStep(const BRepGraph_EdgeId   theParent,
                                                 const BRepGraph_VertexId theChild) const
{
  for (BRepGraph_RefsVertexOfEdge aRefIt(*myGraph, theParent); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraphInc::VertexRef& aRef = myGraph->Refs().Vertices().Entry(aRefIt.CurrentId());
    if (aRef.VertexDefId == theChild)
    {
      return aRefIt.Index();
    }
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
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return TopLoc_Location();
    case BRepGraph_NodeId::Kind::Compound: {
      return aRefs.Children()
        .Entry(aRefs.Children().IdsOf(BRepGraph_CompoundId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      return aRefs.Solids()
        .Entry(aRefs.Solids().IdsOf(BRepGraph_CompSolidId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid =
        aTopo.Solids().Definition(BRepGraph_SolidId(theParent.Index));
      if (theRefIdx < aSolid.ShellRefIds.Length())
      {
        return aRefs.Shells().Entry(aSolid.ShellRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Children()
        .Entry(aSolid.FreeChildRefIds.Value(theRefIdx - aSolid.ShellRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell =
        aTopo.Shells().Definition(BRepGraph_ShellId(theParent.Index));
      if (theRefIdx < aShell.FaceRefIds.Length())
      {
        return aRefs.Faces().Entry(aShell.FaceRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Children()
        .Entry(aShell.FreeChildRefIds.Value(theRefIdx - aShell.FaceRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace =
        aTopo.Faces().Definition(BRepGraph_FaceId(theParent.Index));
      if (theRefIdx < aFace.WireRefIds.Length())
      {
        return aRefs.Wires().Entry(aFace.WireRefIds.Value(theRefIdx)).LocalLocation;
      }
      return aRefs.Vertices()
        .Entry(aFace.VertexRefIds.Value(theRefIdx - aFace.WireRefIds.Length()))
        .LocalLocation;
    }
    case BRepGraph_NodeId::Kind::Wire:
      return aRefs.CoEdges()
        .Entry(aRefs.CoEdges().IdsOf(BRepGraph_WireId(theParent.Index)).Value(theRefIdx))
        .LocalLocation;
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge =
        aTopo.Edges().Definition(BRepGraph_EdgeId(theParent.Index));
      const BRepGraph_VertexRefId aVertexRefId = edgeVertexRefIdAt(anEdge, theRefIdx);
      if (!aVertexRefId.IsValid())
      {
        return TopLoc_Location();
      }
      return aRefs.Vertices().Entry(aVertexRefId).LocalLocation;
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
      return aRefs.Children()
        .Entry(aRefs.Children().IdsOf(BRepGraph_CompoundId(theParent.Index)).Value(theRefIdx))
        .Orientation;
    case BRepGraph_NodeId::Kind::CompSolid:
      return aRefs.Solids()
        .Entry(aRefs.Solids().IdsOf(BRepGraph_CompSolidId(theParent.Index)).Value(theRefIdx))
        .Orientation;
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid =
        aTopo.Solids().Definition(BRepGraph_SolidId(theParent.Index));
      if (theRefIdx < aSolid.ShellRefIds.Length())
      {
        return aRefs.Shells().Entry(aSolid.ShellRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Children()
        .Entry(aSolid.FreeChildRefIds.Value(theRefIdx - aSolid.ShellRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell =
        aTopo.Shells().Definition(BRepGraph_ShellId(theParent.Index));
      if (theRefIdx < aShell.FaceRefIds.Length())
      {
        return aRefs.Faces().Entry(aShell.FaceRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Children()
        .Entry(aShell.FreeChildRefIds.Value(theRefIdx - aShell.FaceRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraphInc::FaceDef& aFace =
        aTopo.Faces().Definition(BRepGraph_FaceId(theParent.Index));
      if (theRefIdx < aFace.WireRefIds.Length())
      {
        return aRefs.Wires().Entry(aFace.WireRefIds.Value(theRefIdx)).Orientation;
      }
      return aRefs.Vertices()
        .Entry(aFace.VertexRefIds.Value(theRefIdx - aFace.WireRefIds.Length()))
        .Orientation;
    }
    case BRepGraph_NodeId::Kind::Wire:
      return TopAbs_FORWARD;
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge =
        aTopo.Edges().Definition(BRepGraph_EdgeId(theParent.Index));
      const BRepGraph_VertexRefId aVertexRefId = edgeVertexRefIdAt(anEdge, theRefIdx);
      if (!aVertexRefId.IsValid())
      {
        return TopAbs_FORWARD;
      }
      return aRefs.Vertices().Entry(aVertexRefId).Orientation;
    }
    default:
      return TopAbs_FORWARD;
  }
}